/***************************************************************************
                          sdl.c  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : (C) 2000 by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sdl.h"
#include "callbacks.h"
#ifdef USE_SDL_IMAGE
#include "SDL/SDL_image.h"
#endif
#include <osso-log.h>
#include <stdlib.h>
#include <string.h>

extern int trm_gm;

Sdl sdl;

// sdl surface //

/* 
 * load a surface from file putting it in soft or hardware mem */
SDL_Surface *
SS_Ld(char *fname, int f)
{
    SDL_Surface *buf;
    SDL_Surface *new_sur;

    f |= SDL_NONFATAL;
    char path[strlen(LMARBLESGFXDIR) + strlen(fname) + 2];

    sprintf(path, "%s", fname);

#ifndef USE_SDL_IMAGE
    buf = SDL_LoadBMP(path);
#else
    buf = IMG_Load(path);
#endif
    if (buf == 0)
    {
        ULOG_ERR("ERR: ssur_load: file '%s' not found or not enough memory\n",
                 path);
        if (f & SDL_NONFATAL)
            return 0;
        else
            exit(1);
    }
    new_sur = SS_Crt(buf->w, buf->h, f);
    SDL_BlitSurface(buf, 0, new_sur, 0);
    SDL_FreeSurface(buf);
    return new_sur;
}

/* 
 * create an surface MUST NOT BE USED IF NO SDLSCREEN IS SET */
SDL_Surface *
SS_Crt(int w, int h, int f)
{
    SDL_Surface *sur;
    SDL_PixelFormat *spf = SDL_GetVideoSurface()->format;
    if ((sur =
         SDL_CreateRGBSurface(f, w, h, spf->BitsPerPixel, spf->Rmask,
                              spf->Gmask, spf->Bmask, spf->Amask)) == 0)
    {
        ULOG_ERR
            ("ERR: ssur_create: not enough memory to create surface...\n");
        exit(1);
    }
    SDL_SetColorKey(sur, SDL_SRCCOLORKEY, SDL_MapRGB(sur->format, 0, 0, 0));
    SDL_SetAlpha(sur, 0, 0);    // no alpha //
    return sur;
}

/* 
 * return display format */
int
SS_DpyFmt(SDL_Surface * sur)
{
    if ((sur = SDL_DisplayFormat(sur)) == 0)
    {
        ULOG_ERR("ERR: ssur_displayformat: convertion failed\n");
        return 1;
    }
    return 0;
}

/* 
 * lock surface */
void
SS_Bgn(SDL_Surface * sur)
{
    if (SDL_MUSTLOCK(sur))
        SDL_LockSurface(sur);
}

/* 
 * unlock surface */
void
SS_End(SDL_Surface * sur)
{
    if (SDL_MUSTLOCK(sur))
        SDL_UnlockSurface(sur);
}

/* 
 * blit surface with destination D_DST and source D_SRC using it's actual
 * alpha and color key settings */
void
SS_Blt(void)
{
#ifdef SDL_1_1_5
    if (sdl.s.s->flags & SDL_SRCALPHA)
        SDL_SetAlpha(sdl.s.s, SDL_SRCALPHA, 255 - sdl.s.s->format->alpha);
#endif
    SDL_BlitSurface(sdl.s.s, &sdl.s.r, sdl.d.s, &sdl.d.r);
#ifdef SDL_1_1_5
    if (sdl.s.s->flags & SDL_SRCALPHA)
        SDL_SetAlpha(sdl.s.s, SDL_SRCALPHA, 255 - sdl.s.s->format->alpha);
#endif
}

/* 
 * Fill surface D_DST by tiling D_SRC */
void
SS_TlBlt(void)
{
    int t_w, t_h, i, j;
    SDL_Rect t_s_r;
    SDL_Rect t_d_r;
    t_s_r.x = sdl.s.r.x;
    t_s_r.y = sdl.s.r.y;
    t_w = sdl.s.s->w - sdl.s.r.x;
    t_h = sdl.s.s->h - sdl.s.r.y;
#ifdef SDL_1_1_5
    if (sdl.s.s->flags & SDL_SRCALPHA)
        SDL_SetAlpha(sdl.s.s, SDL_SRCALPHA, 255 - sdl.s.s->format->alpha);
#endif
    for (i = 0; i < sdl.d.r.w; i += t_w)
    {
        t_d_r.x = sdl.d.r.x + i;
        for (j = 0; j < sdl.d.r.h; j += t_h)
        {
            t_d_r.y = sdl.d.r.y + j;
            t_s_r.w = sdl.d.r.w - i;
            t_s_r.h = sdl.d.r.h - j;
            SDL_BlitSurface(sdl.s.s, &t_s_r, sdl.d.s, &t_d_r);
        }
    }
#ifdef SDL_1_1_5
    if (sdl.s.s->flags & SDL_SRCALPHA)
        SDL_SetAlpha(sdl.s.s, SDL_SRCALPHA, 255 - sdl.s.s->format->alpha);
#endif
}

/* 
 * do an alpha blit */
void
SS_ABlt(int alpha)
{
#ifdef SDL_1_1_5
    SDL_SetAlpha(sdl.s.s, SDL_SRCALPHA, 255 - alpha);
#else
    SDL_SetAlpha(sdl.s.s, SDL_SRCALPHA, alpha);
#endif
    SDL_BlitSurface(sdl.s.s, &sdl.s.r, sdl.d.s, &sdl.d.r);
    SDL_SetAlpha(sdl.s.s, 0, 0);
}

/* 
 * fill surface with color c */
void
SS_Fill(int c)
{
    SDL_FillRect(sdl.d.s, &sdl.d.r,
                 SDL_MapRGB(sdl.d.s->format, c >> 16, (c >> 8) & 0xFF,
                            c & 0xFF));
}

/* get pixel */
Uint32
get_pixel(SDL_Surface * surf, int x, int y)
{
    int pos = 0;
    Uint32 pixel = 0;

    pos = y * surf->pitch + x * surf->format->BytesPerPixel;
    memcpy(&pixel, (void *) ((char *) surf->pixels + pos),
           surf->format->BytesPerPixel);
    return pixel;
}

// sdl //

/* 
 * initialize sdl */
void
Sdl_Ini(int f)
{
    sdl.scr = 0;
    if (SDL_Init(f) < 0)
    {
        ULOG_ERR("ERR: sdl_init: %s", SDL_GetError());
        exit(1);
    }
    SDL_EnableUNICODE(1);
    atexit(SDL_Quit);
}

/* 
 * free screen */
void
Sdl_Qut()
{
    if (sdl.scr)
        SDL_FreeSurface(sdl.scr);
}

/* 
 * set video mode and give information about hardware capabilities */
int
Sdl_StVdMd(int w, int h, int d, int f)
{
    int depth;

#ifdef DEBUG
    SDL_PixelFormat *fmt;
#endif
    if (sdl.scr)
    {
        SDL_FreeSurface(sdl.scr);
    }

    // is this mode is supported
    depth = SDL_VideoModeOK(w, h, d, f);
    if (depth == 0)
    {
        ULOG_ERR
            ("ERR: SDL_VideoModeOK says mode %ix%ix%i is invalid...\ntrying to emulate with 16 bits depth\n",
             w, h, d);
        depth = 16;
    }

    // set video mode
    if ((sdl.scr = SDL_SetVideoMode(w, h, depth, f)) == 0)
    {
        ULOG_ERR("ERR: sdl_setvideomode: %s", SDL_GetError());
        return 1;
    }

#ifdef DEBUG
    if (f & SDL_HWSURFACE && !(sdl.scr->flags & SDL_HWSURFACE))
        ULOG_ERR("unable to create screen in hardware memory...\n");
    if (f & SDL_DOUBLEBUF && !(sdl.scr->flags & SDL_DOUBLEBUF))
        ULOG_ERR("unable to create double buffered screen...\n");
    if (f & SDL_FULLSCREEN && !(sdl.scr->flags & SDL_FULLSCREEN))
        ULOG_ERR("unable to switch to fullscreen...\n");

    fmt = sdl.scr->format;
    ULOG_DEBUG("video mode format:\n");
    ULOG_DEBUG("Masks: R=%i, G=%i, B=%i\n", fmt->Rmask, fmt->Gmask,
               fmt->Bmask);
    ULOG_DEBUG("LShft: R=%i, G=%i, B=%i\n", fmt->Rshift, fmt->Gshift,
               fmt->Bshift);
    ULOG_DEBUG("RShft: R=%i, G=%i, B=%i\n", fmt->Rloss, fmt->Gloss,
               fmt->Bloss);
    ULOG_DEBUG("BBP: %i\n", fmt->BitsPerPixel);
    ULOG_DEBUG("-----\n");
#endif

    return 0;
}

/* 
 * update just one rect */
void
Sdl_Upd(int x, int y, int w, int h)
{
    SDL_UpdateRect(sdl.scr, x, y, w, h);
}

/* 
 * do a full update */
void
Sdl_FUpd()
{
    SDL_UpdateRect(sdl.scr, 0, 0, 0, 0);
}

/* 
 * draw all update regions */
void
Sdl_UpdR()
{
    if (sdl.rnum == SDL_MAX_RGNS)
        SDL_UpdateRect(sdl.scr, 0, 0, sdl.scr->w, sdl.scr->h);
    else
        SDL_UpdateRects(sdl.scr, sdl.rnum, sdl.rgns);
    sdl.rnum = 0;
}

/* 
 * add update region */
void
Sdl_AddR(int x, int y, int w, int h)
{
    if (sdl.rnum == SDL_MAX_RGNS)
        return;
    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }
    if (x + w > sdl.scr->w)
        w = sdl.scr->w - x;
    if (y + h > sdl.scr->h)
        h = sdl.scr->h - y;
    if (w <= 0 || h <= 0)
        return;
    sdl.rgns[sdl.rnum].x = x;
    sdl.rgns[sdl.rnum].y = y;
    sdl.rgns[sdl.rnum].w = w;
    sdl.rgns[sdl.rnum].h = h;
    sdl.rnum++;
}

/* 
 * fade screen to black */
void
Sdl_Dim(int steps, int delay, int trp)
{
#ifndef NODIM
    SDL_Surface *buffer;
    int per_step = trp / steps;
    int i;
    if (trm_gm)
        return;
    buffer = SS_Crt(sdl.scr->w, sdl.scr->h, SDL_SURFACE_TYPE);
    SDL_SetColorKey(buffer, 0, 0);
    D_FDST(buffer);
    D_FSRC(sdl.scr);
    SS_Blt();
    for (i = trp; i >= 0; i -= per_step)
    {
        D_FDST(sdl.scr);
        SS_Fill(0x0);
        D_FSRC(buffer);
        SS_ABlt(i);
        Sdl_FUpd();
        SDL_Delay(delay);
    }
    if (trp == 255)
    {
        D_FDST(sdl.scr);
        SS_Fill(0x0);
        Sdl_FUpd();
    }
    SDL_FreeSurface(buffer);
#else
    Sdl_FUpd();
#endif
}

/* 
 * undim screen */
void
Sdl_UnDim(int steps, int delay, int trp)
{
#ifndef NODIM
    SDL_Surface *buffer;
    int per_step = trp / steps;
    int i;
    if (trm_gm)
        return;
    buffer = SS_Crt(sdl.scr->w, sdl.scr->h, SDL_SURFACE_TYPE);
    SDL_SetColorKey(buffer, 0, 0);
    D_FDST(buffer);
    D_FSRC(sdl.scr);
    SS_Blt();
    for (i = 0; i <= trp; i += per_step)
    {
        D_FDST(sdl.scr);
        SS_Fill(0x0);
        D_FSRC(buffer);
        SS_ABlt(i);
        Sdl_FUpd();
        SDL_Delay(delay);
    }
    D_FDST(sdl.scr);
    D_FSRC(buffer);
    SS_Blt();
    Sdl_FUpd();
    SDL_FreeSurface(buffer);
#else
    Sdl_FUpd();
#endif
}

/* 
 * wait for a key */
int
Sdl_WtFrKy()
{
    // wait for key
    SDL_Event event;
    while (1)
    {
        while (!SDL_PollEvent(&event))
        {
            gmainloop();
            SDL_Delay(10);
        }
        // SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
        {
            trm_gm = 1;
            return 0;
        }
        if (event.type == SDL_USEREVENT)
        {
            trm_gm = 1;
            exit_callback(0);
            return 0;
        }
        if (event.type == SDL_KEYUP)
        {
            return event.key.keysym.sym;
        }
    }
}

/* 
 * wait for a key or mouse click */
void
Sdl_WtFrClk()
{
    // wait for key or button
    SDL_Event event;
    while (1)
    {
        // SDL_WaitEvent(&event);
        while (!SDL_PollEvent(&event))
        {
            gmainloop();
            SDL_Delay(10);
        }
        if (event.type == SDL_QUIT)
        {
            trm_gm = 1;
            return;
        }
        if (event.type == SDL_USEREVENT)
        {
            trm_gm = 1;
            exit_callback(0);
            return;
        }
        if (event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP)
        {
            return;
        }
    }
}

/* 
 * lock surface */
void
Sdl_Bgn()
{
    if (SDL_MUSTLOCK(sdl.scr))
        SDL_LockSurface(sdl.scr);
}

/* 
 * unlock surface */
void
Sdl_End()
{
    if (SDL_MUSTLOCK(sdl.scr))
        SDL_UnlockSurface(sdl.scr);
}

/* 
 * flip hardware screens (double buffer) */
void
Sdl_Flp()
{
    SDL_Flip(sdl.scr);
}
