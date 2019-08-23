/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Sam Aug  5 12:36:32 MEST 2000
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

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef _WIN32
#include <fcntl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <osso-log.h>
#include <hgw/hgw.h>
#include "menu.h"
#include "cfg.h"
#include "levels.h"
#include "timer.h"
#include "sdl.h"
#include "audio.h"
#include "profile.h"
#include "game.h"
#include "engine_main.h"

// sappdata -- main.c //
extern HgwContext *sapp_context;
// timer -- timer.c //
extern struct timeb t_c, t_lc;
// quit game quickly ? //
int trm_gm = 0;
// menu config -- cfg.c //
extern Cfg cfg;
// sdl struct //
extern Sdl sdl;
// levelset list and number of entries -- levels.c //
extern char **ls_lst;
extern int ls_n;
// profiles //
extern DLst prfs;
// string list for the menu -- profile.c //
extern char **prf_lst;
extern int prf_n;
// menu entry containing the profile list //
// MEnt *me_prf, *me_del, *me_clr;

// menu actions MUST BE GREATER THAN 0 //
#define MA_NONE 0
#define MA_QUT 1
#define MA_PLY 2
#define MA_HSC 3

void CB_Snd(void);
void CB_StV(void);
void CB_CrtP(void);
void CB_ClrP(void);
void CB_DelP(void);
void CB_SrtP(void);
void event_clear_sdl_queue(void);
int all_filter(const SDL_Event * event);
void H_Shw(void);
// void MM_CrtE(void);


/* 
 * ==================================================================== Clear 
 * the SDL event key (keydown events)
 * ==================================================================== */
int
all_filter(const SDL_Event * event)
{
    event = event;
    return 0;
}

void
event_clear_sdl_queue()
{
    SDL_Event event;
    SDL_SetEventFilter(all_filter);
    while (SDL_PollEvent(&event)) ;
    SDL_SetEventFilter(0);
}

/* 
 * previously defined hiscore.h but this file no longer needed */
typedef struct {
    char nm[12];
    int lvl;
    int lvs;
    int scr;
    int pct;
} H_E;


// menu callbacks //

/* 
 * activate / deactivate sound */
void
CB_Snd()
{
#ifdef SOUND
    sound_enable(cfg.s_on);
#endif
}

/* 
 * set sound volume */
void
CB_StV()
{
#ifdef SOUND
    sound_volume(cfg.s_vol * 16);
#endif
}

/* 
 * clear a profile */
void
CB_ClrP()
{
    Prf *p = DL_Get(&prfs, cfg.prf);
    DL_Clr(&p->sts);
    p->lvls = 0;
    p->scr = 0;
    p->pct = 0;
}

void
CB_SrtP()
{
    // Prf_Srt();
}

/* 
 * init sound, menu, profiles, game main loop for the menu */
int
lmarbles(int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    /* SDL_Event e; int go_on = 1; int ms; */
    char aux[64];
    char tmp;
#ifdef _WIN32
    /* Get Windows to open files in binary mode instead of default text mode */
    _fmode = _O_BINARY;
#endif

    // random init
    srand(time(NULL));

    // sdl init //
#ifdef SOUND
    Sdl_Ini(SDL_INIT_VIDEO | SDL_INIT_TIMER);
#else
    Sdl_Ini(SDL_INIT_VIDEO | SDL_INIT_TIMER);
#endif
    sprintf(aux, _("game_ap_marbles_name"));
    SDL_WM_SetCaption(aux, 0);
    tmp = 0;
    if (hgw_conf_request_bool
        (sapp_context, "/apps/osso/lmarbles/running", &tmp))
    {
        return 1;
    }
    if ((int) tmp != 1)
    {
        return 1;
    }

    // show logo //
    Sdl_StVdMd(800, 480, 16, SDL_SURFACE_TYPE | SDL_FULLSCREEN);

    // load config //
    C_StPth();
    C_Ld();

    // #define WORKAROUND
    // get difficulty from startup //
    cfg.diff = 0;
#ifndef WORKAROUND
    if (hgw_conf_request_int
        (sapp_context, "/apps/osso/lmarbles/difficulty_level", &cfg.diff))
    {
        cfg.diff = 0;
    }
#endif

    // get sound setting from startup //

#ifndef WORKAROUND
    if (hgw_conf_request_bool
        (sapp_context, "/apps/osso/lmarbles/enable_sound", &tmp))
    {
        tmp = 0;
    }
#endif
    cfg.s_on = (int) tmp;

    FILE *dFile;
    if ((dFile = fopen("/tmp/.gamewrapper/lmarbles.debug", "a")))
    {
        fprintf(dFile, "Sound: %d\n", cfg.s_on);
        fclose(dFile);
    }

    // load profiles //
    Prf_Ini();
    if (!Prf_Ld())
        cfg.prf = 0;

    // create levelset list and reset config's levelset index if nescessary
    // //
    L_CrtLst();
    if (cfg.ls >= ls_n)
        cfg.ls = 0;

#ifdef SOUND
    audio_open();
    sound_enable(cfg.s_on);
    sound_volume(cfg.s_vol * 16);
#endif

    // game init //
    G_Ini();
    if (hgw_conf_request_bool
        (sapp_context, "/apps/osso/lmarbles/running", &tmp))
    {
        return 1;
    }
    if ((int) tmp != 1)
    {
        Sdl_Qut();
        return 1;
    }
    if (G_Opn())
    {
        G_Run();
        G_Cls();
    }
    // terminate game //
    G_Trm();

    // close soundserver //
#ifdef SOUND
    audio_close();
#endif

    // save config //
    C_Sv();

    // free levelset list //
    L_DelLst();

    // save profiles //
    Prf_Sv();
    Prf_Trm();

    // free screen //
    Sdl_Qut();

    ULOG_DEBUG("Bye, bye!\n");
    return 0;
}
