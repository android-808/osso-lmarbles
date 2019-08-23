/***************************************************************************
                          game.c  -  description
                             -------------------
    begin                : Mon Aug 14 2000
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

#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>             /* for unlink */
#include <string.h>
#include <hgw/hgw.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <osso-log.h>
#include "file.h"
#include "cfg.h"
#include "menu.h"
#include "timer.h"
#include "ft.h"
#include "callbacks.h"
#include <libosso.h>

#define KEY_ESCAPE_TIMEOUT 1500
SDL_TimerID escape_timeout = NULL;

extern HgwContext *sapp_context;
extern osso_context_t *osso;
Game gm;
/* levelset names & levelsets -- levels.h */
extern char **ls_lst;
extern int ls_n;
extern DLst l_sts;
/* line counter -- file.c */
extern int f_ln;
/* config -- cfg.c */
extern Cfg cfg;
/* Sdl -- sdl.c */
extern Sdl sdl;
/* profiles -- profile.c */
extern DLst prfs;
/* terminate game -- sdl.c */
extern int trm_gm;

int quit_gm = 0;
int exit_gm = 0;
int exiting_needed = 0;

/* This is needed to avoid the marbles exploding animation in the unlikely
 * case, when the game is started with a pattern completed */
int just_started = 1;

gboolean written = FALSE;

/* 
 * Helper function for user event handling */
static void
userevent_handler(void)
{
    /* app_remove_timeout(); */
    exit_callback(0);
}

void modify_score(int *b_lvl, int *b_tm);
char *StateDump(void);
int StateLoad(char *state);
// int G_Cngrt(void);

/* 
 * initialize game */
void
G_Ini()
{
    FILE *f;
    int i;
    char str[256];
    int ok = 0;
    // cursors //
    char data[32], mask[32];
    char csr[6][256] = {
        {
         0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 2, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 2, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 2, 1, 2, 1, 1, 2, 1, 2, 0, 0, 0, 0,
         0, 0, 0, 0, 2, 1, 2, 1, 1, 2, 1, 2, 0, 0, 0, 0,
         0, 0, 0, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 0, 0, 0,
         0, 0, 0, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 0, 0, 0,
         0, 0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0, 0,
         0, 0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0, 0,
         0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0,
         0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0,
         2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
         2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         },
        {
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
         2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
         0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0,
         0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0,
         0, 0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0, 0,
         0, 0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0, 0,
         0, 0, 0, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 0, 0, 0,
         0, 0, 0, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 0, 0, 0,
         0, 0, 0, 0, 2, 1, 2, 1, 1, 2, 1, 2, 0, 0, 0, 0,
         0, 0, 0, 0, 2, 1, 2, 1, 1, 2, 1, 2, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 2, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 2, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0,
         },
        {
         2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         2, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         2, 1, 2, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         2, 1, 2, 2, 2, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0,
         2, 1, 2, 1, 1, 2, 2, 1, 1, 2, 2, 0, 0, 0, 0, 0,
         2, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 0, 0, 0,
         2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 0,
         2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 2,
         2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 2,
         2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 0,
         2, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 0, 0, 0,
         2, 1, 2, 1, 1, 2, 2, 1, 1, 2, 2, 0, 0, 0, 0, 0,
         2, 1, 2, 2, 2, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0,
         2, 1, 2, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         2, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         },
        {
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 2,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 2, 1, 2,
         0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 2, 2, 2, 1, 2,
         0, 0, 0, 0, 0, 2, 2, 1, 1, 2, 2, 1, 1, 2, 1, 2,
         0, 0, 0, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 1, 2,
         0, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2,
         2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2,
         2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2,
         0, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2,
         0, 0, 0, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 1, 2,
         0, 0, 0, 0, 0, 2, 2, 1, 1, 2, 2, 1, 1, 2, 1, 2,
         0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 2, 2, 2, 1, 2,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 2, 1, 2,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 2,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2,
         },
        {
         2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
         2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2,
         2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2,
         2, 1, 1, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 1, 1, 2,
         2, 1, 1, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 1, 2, 0,
         0, 2, 1, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 2, 0, 0,
         0, 0, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0,
         0, 0, 2, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 2, 0, 0,
         0, 2, 1, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 1, 2, 0,
         2, 1, 1, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 1, 1, 2,
         2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2,
         2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2,
         2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
         },
        {
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
         2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
         0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 2, 0,
         0, 2, 1, 1, 2, 2, 2, 0, 0, 2, 2, 1, 1, 1, 2, 0,
         0, 0, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 0, 0,
         0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0,
         0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0,
         0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0,
         0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0,
         0, 0, 2, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 2, 0, 0,
         0, 2, 1, 2, 0, 2, 1, 1, 1, 1, 2, 0, 2, 1, 2, 0,
         0, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2, 0,
         2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2,
         2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         }
    };

    // initialize dyn list l_sts
    DL_Ini(&l_sts);
    l_sts.flgs = DL_AUTODEL;
    l_sts.cb = L_DelSt;
    // load level sets
    ULOG_DEBUG("loading levelsets...\n");
    for (i = 0; i < ls_n; i++)
    {
        ULOG_DEBUG("%s... ", ls_lst[i]);

        sprintf(str, "%s/%s", LMARBLESLEVELSDIR, ls_lst[i]);

        f = fopen(str, "r");
        if (f != 0)
        {
            f_ln = 1;
            if (L_LdSt(f))
            {
                ok = 1;
                strcpy(((LSet *) l_sts.tl.p->d)->nm, ls_lst[i]);
            }
            else
                L_AddInvSt(ls_lst[i]);
            fclose(f);
        }
        else
        {
            L_AddInvSt(ls_lst[i]);
            ULOG_WARN("WARNING: levelset not found...\n");
        }
    }

    // found something ? //
    if (!ok)
    {
        ULOG_ERR
            ("ERROR: no valid level set found; need at least one level set...\n");
        exit(1);
    }

    /* clear gm struct */
    // memset( &gm, 0, sizeof( gm ) );

    // initialize gfx set
    DL_Ini(&gm.g_sts);
    gm.g_sts.flgs = DL_AUTODEL;
    gm.g_sts.cb = G_DelGSt;
    // load gfx sets
    G_LdGSts();

    // load board
    gm.s_brd = SS_Ld(LMARBLESGFXDIR "/board.jpg", SDL_SURFACE_TYPE);
    SDL_SetColorKey(gm.s_brd, 0, 0);
    // load lights
    gm.s_lghts = SS_Ld(LMARBLESGFXDIR "/lights.bmp", SDL_SURFACE_TYPE);
    // load marbles
    gm.s_mrb = SS_Ld(LMARBLESGFXDIR "/marbles.jpg", SDL_SURFACE_TYPE);
    // load undo button
    gm.s_undo = SS_Ld(LMARBLESGFXDIR "/undo-button.bmp", SDL_SURFACE_TYPE);
    // load pressed undo button
    gm.s_undop =
        SS_Ld(LMARBLESGFXDIR "/undo-button_pressed.bmp", SDL_SURFACE_TYPE);
    // load dimmed undo button
    gm.s_undod =
        SS_Ld(LMARBLESGFXDIR "/undo-button_dimmed.bmp", SDL_SURFACE_TYPE);
    // load overlay button
    gm.s_ret =
        SS_Ld("/usr/share/icons/hicolor/scalable/hildon/general_overlay_back.png", SDL_SURFACE_TYPE);
    // load figure
    gm.s_fig = SS_Ld(LMARBLESGFXDIR "/figure.bmp", SDL_SURFACE_TYPE);
    // load marble frame
    gm.s_mf = SS_Ld(LMARBLESGFXDIR "/m_frame.bmp", SDL_SURFACE_TYPE);
    // load marble select frame
    gm.s_msf = SS_Ld(LMARBLESGFXDIR "/m_sframe.bmp", SDL_SURFACE_TYPE);
    // background //
    gm.s_bkgd = 0;

    // load dialog graphics //
    gm.s_dtr =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_top_right.bmp",
              SDL_SURFACE_TYPE);
    gm.s_dtl =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_top_left.bmp", SDL_SURFACE_TYPE);
    gm.s_dbr =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_bottom_right.bmp",
              SDL_SURFACE_TYPE);
    gm.s_dbl =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_bottom_left.bmp",
              SDL_SURFACE_TYPE);
    gm.s_dto =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_top_tile.bmp", SDL_SURFACE_TYPE);
    gm.s_dbo =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_bottom_tile.bmp",
              SDL_SURFACE_TYPE);
    gm.s_dle =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_left_tile.bmp",
              SDL_SURFACE_TYPE);
    gm.s_drg =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_right_tile.bmp",
              SDL_SURFACE_TYPE);
    gm.s_dbg =
        SS_Ld(LMARBLESGFXDIR "/dialog/dialog_background_tile.bmp",
              SDL_SURFACE_TYPE);
    gm.s_dyes =
        SS_Ld(LMARBLESGFXDIR "/dialog/Yes-button.bmp", SDL_SURFACE_TYPE);
    gm.s_dno =
        SS_Ld(LMARBLESGFXDIR "/dialog/No-button.bmp", SDL_SURFACE_TYPE);

    // load font
    if (FT_Init_FreeType(&gm.ft_lib))
    {
        ULOG_WARN("cannot init FreeType\n");
    }

    if (FT_New_Face
        (gm.ft_lib, "/usr/share/fonts/nokia/nosnr.ttf", 0, &gm.ft_fnt))
    {
        ULOG_WARN("cannot load font\n");
    }

    // font size
    gm.ft_uis = 24;

    // undo stack
    gm.ms_us = NULL;

    // level
    gm.c_lvl = 0;

    // marble
    gm.m_v = 0.15;
    gm.m_a.p = 0;
    gm.m_a.f = 4;
    gm.m_a.c = 0.016 + 0.008;
    gm.m_a.w = gm.m_a.h = 32;
    gm.m_act = M_EMPTY;
    gm.m_o_x = gm.m_o_y = -1;
    gm.m_warp = 0;

    // marble select frame
    gm.msf_a.f = 4;
    gm.msf_a.w = gm.msf_a.h = 40;
    gm.msf_a.c = 0.02;
    gm.msf_a.p = 0;

    // marble frame
    gm.mf_a.f = 4;
    gm.mf_a.w = gm.mf_a.h = 40;
    gm.mf_a.c = 0.02;
    gm.mf_a.p = 0;

    // layout
    gm.b_x = 800 - 360;
    gm.f_x = 171;
    gm.f_y = 162;
    gm.i_x = 102;
    gm.i_y = 10;
    gm.t_x = 172;
    gm.t_y = 104;
    gm.s_x = 72;
    gm.s_y = 318;

    // geometry
    gm.t_w = gm.t_h = 32;
    gm.f_w = gm.f_h = 16;
    gm.i_w = 258;
    gm.i_h = 70;
    gm.f_fw = 156;
    gm.f_fh = 116;

    gm.scr_w = 800;
    gm.scr_h = 480;
    gm.brd_w = 360;
    gm.s_w = 264;
    gm.s_h = 120;
    gm.c_off = 12;

    // cursors
    memset(data, 0, sizeof(data));
    gm.c_u = gm.c_d = gm.c_l = gm.c_r = gm.c_s = gm.c_w = 0;
    gm.c_n = SDL_GetCursor();
    Cr_Ld(csr[0], data, mask);
    gm.c_u = SDL_CreateCursor((Uint8 *)data,(Uint8 *) mask, 16, 16, 8, 8);
    Cr_Ld(csr[1], data, mask);
    gm.c_d = SDL_CreateCursor((Uint8 *)data, (Uint8 *)mask, 16, 16, 8, 8);
    Cr_Ld(csr[2], data, mask);
    gm.c_r = SDL_CreateCursor((Uint8 *)data, (Uint8 *)mask, 16, 16, 8, 8);
    Cr_Ld(csr[3], data, mask);
    gm.c_l = SDL_CreateCursor((Uint8 *)data, (Uint8 *)mask, 16, 16, 8, 8);
    Cr_Ld(csr[4], data, mask);
    gm.c_s = SDL_CreateCursor((Uint8 *)data, (Uint8 *)mask, 16, 16, 8, 8);
    Cr_Ld(csr[5], data, mask);
    gm.c_w = SDL_CreateCursor((Uint8 *)data, (Uint8 *)mask, 16, 16, 8, 8);

    // map animations
    gm.m_ani = 0;

    // oneway animation info
    gm.ma_ow_a.f = 4;
    gm.ma_ow_a.c = 0.008;
    gm.ma_ow_a.p = 0;
    gm.ma_ow_a.w = gm.t_w;
    gm.ma_ow_a.h = gm.t_h;

    // teleport information
    gm.ma_tlp_a.f = 4;
    gm.ma_tlp_a.c = 0.008;
    gm.ma_tlp_a.p = 0;
    gm.ma_tlp_a.w = gm.t_w;
    gm.ma_tlp_a.h = gm.t_h;
    gm.tlp_a = 128;

    // sounds
#ifdef SOUND
    gm.wv_tlp = sound_chunk_load("teleport.wav");
    gm.wv_sel = sound_chunk_load("select.wav");
    gm.wv_stp = sound_chunk_load("stop.wav");
    gm.wv_clk = sound_chunk_load("click.wav");
    gm.wv_exp = sound_chunk_load("explode.wav");
    gm.wv_alm = sound_chunk_load("alarm.wav");
    gm.wv_arw = sound_chunk_load("arrow.wav");
    gm.wv_scr = sound_chunk_load("score.wav");
#endif

    // shrapnells
    DL_Ini(&gm.shr);
    gm.shr.flgs = DL_AUTODEL;
    gm.shr.cb = Shr_Del;
    gm.shr_a_c = 0.1;

    // shnapshot
    gm.snap = 0;
}

/* 
 * terminate game */
void
G_Trm()
{
    // release gfx
    if (gm.s_brd)
        SDL_FreeSurface(gm.s_brd);
    if (gm.s_undo)
        SDL_FreeSurface(gm.s_undo);
    if (gm.s_lghts)
        SDL_FreeSurface(gm.s_lghts);
    if (gm.s_mrb)
        SDL_FreeSurface(gm.s_mrb);
    if (gm.s_fig)
        SDL_FreeSurface(gm.s_fig);
    if (gm.s_mf)
        SDL_FreeSurface(gm.s_mf);
    if (gm.s_msf)
        SDL_FreeSurface(gm.s_msf);

    if (gm.s_dtr)
        SDL_FreeSurface(gm.s_dtr);
    if (gm.s_dtl)
        SDL_FreeSurface(gm.s_dtl);
    if (gm.s_dbr)
        SDL_FreeSurface(gm.s_dbr);
    if (gm.s_dbl)
        SDL_FreeSurface(gm.s_dbl);
    if (gm.s_dto)
        SDL_FreeSurface(gm.s_dto);
    if (gm.s_dbo)
        SDL_FreeSurface(gm.s_dbo);
    if (gm.s_dle)
        SDL_FreeSurface(gm.s_dle);
    if (gm.s_drg)
        SDL_FreeSurface(gm.s_drg);
    if (gm.s_dbg)
        SDL_FreeSurface(gm.s_dbg);
    if (gm.s_dyes)
        SDL_FreeSurface(gm.s_dyes);
    if (gm.s_dno)
        SDL_FreeSurface(gm.s_dno);

    // release dynlists
    DL_Clr(&gm.g_sts);
    DL_Clr(&l_sts);
    DL_Clr(&gm.shr);
    // cursors //
    if (gm.c_u)
        SDL_FreeCursor(gm.c_u);
    if (gm.c_d)
        SDL_FreeCursor(gm.c_d);
    if (gm.c_l)
        SDL_FreeCursor(gm.c_l);
    if (gm.c_r)
        SDL_FreeCursor(gm.c_r);
    if (gm.c_s)
        SDL_FreeCursor(gm.c_s);
    if (gm.c_w)
        SDL_FreeCursor(gm.c_w);
    // free map animations //
    if (gm.m_ani)
        free(gm.m_ani);
    // sounds
#ifdef SOUND
    if (gm.wv_tlp)
        sound_chunk_free(&gm.wv_tlp);
    if (gm.wv_sel)
        sound_chunk_free(&gm.wv_sel);
    if (gm.wv_stp)
        sound_chunk_free(&gm.wv_stp);
    if (gm.wv_clk)
        sound_chunk_free(&gm.wv_clk);
    if (gm.wv_exp)
        sound_chunk_free(&gm.wv_exp);
    if (gm.wv_alm)
        sound_chunk_free(&gm.wv_alm);
    if (gm.wv_arw)
        sound_chunk_free(&gm.wv_arw);
    if (gm.wv_scr)
        sound_chunk_free(&gm.wv_scr);
#endif
}

/* 
 * open a new game */
int
G_Opn()
{
    Prf *p;
    DL_E *e;
    FILE *sFile;
    int flgs = SDL_SURFACE_TYPE;
    char buf[1024];

    // get current level set
    gm.c_l_st = (LSet *) DL_Get(&l_sts, cfg.ls);

    // check if current level set is valid
    if (!G_CkLSt())
    {
        return 0;
    }

    // current profile
    gm.c_prf = (Prf *) DL_Get(&prfs, cfg.prf);

    // current set info
    gm.c_s_inf = Prf_RegLS(gm.c_prf, gm.c_l_st);

    // dim & resize
    // if (cfg.dim)
    // SDL_DIM();
    // if (cfg.fscr)
    flgs = flgs | SDL_FULLSCREEN;
    // Sdl_StVdMd(gm.scr_w, gm.scr_h, 16, flgs);

    // create background
    gm.s_bkgd = SS_Crt(gm.scr_w, gm.scr_h, SDL_SURFACE_TYPE);
    SDL_SetColorKey(gm.s_bkgd, 0, 0);

    // get highest score
    gm.hi_scr = 0;
    e = prfs.hd.n;
    while (e != &prfs.tl)
    {
        p = (Prf *) e->d;
        if (p->scr > gm.hi_scr)
            gm.hi_scr = p->scr;
        e = e->n;
    }

    // clear old chapter
    gm.o_ch = -1;

    // stateload
    gchar *file = g_build_filename(g_get_home_dir(),
                                   ".lmarbles_state",
                                   NULL);
    if (g_file_test(file, G_FILE_TEST_IS_REGULAR)) {
	    sFile = g_fopen(file, "r");

	    /* Rama - Check File size */
	    struct stat statbuf;
	    memset(&statbuf, 0, sizeof(struct stat));

	    g_stat(file, &statbuf);

	    if (sFile && (0 != (int)statbuf.st_size))
	    {
		    buf[fread(buf, 1, 1023, sFile)] = '\0';
		    StateLoad(buf);
		    fclose(sFile);
		    g_unlink(file);
	    }
	    else {
		    // init first level
		    L_Ini(0, 0, L_DATA | L_GFX);
	    }
    }
    else
	    // init first level
	    L_Ini(0, 0, L_DATA | L_GFX);
    
    g_free(file);

    return 1;
}

/* void just_jump_to_next_level(void) {
 * 
 * } */


/* 
 * close game */
void
G_Cls()
{
    // dim
    if (!trm_gm && cfg.dim)
        SDL_DIM();

    // free background
    if (gm.s_bkgd)
        SDL_FreeSurface(gm.s_bkgd);

    // free level
    free(gm.c_lvl);
    gm.c_lvl = 0;

    // restore cursor
    SDL_SetCursor(gm.c_n);
}

/* 
 * game's main loop */
void
G_Run()
{

    int leave = 0;
    int restart = (gm.c_lvl->tm <= 0);
    int ms;
    SDL_Event ev;
    int restore_pos;
    int tm_rel = 0;
    int ign_c_stat = 0;
    int bonus_level, bonus_moves;   /* bonus for level completion and
                                     * remaining moves */
    int undop = -1;
    char level_aux[64] = "";

    gboolean draw_move = TRUE;
    gboolean begin_level = TRUE;
    int level_completed = 0;

    while (!trm_gm)
    {
        Tm_Hd();
        if (!gm.l_done) Tm_Shw();
        Inf_Hd();
        Inf_Shw();
        strcpy(level_aux, gm.inf_str);
        gmainloop();
        // clear input and check for some events.
        while (!trm_gm && SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
                case SDL_USEREVENT:
                    userevent_handler();
                    break;
                case SDL_KEYDOWN:
                    if (ev.key.keysym.sym == SDLK_F5)
                    {
                        exit_callback(0);
                    }
                    break;
                case SDL_ACTIVEEVENT:
                    if (ev.active.gain == 0)
                    {
                        trm_gm = leave = 1;
                        exit_callback(0);
                    }
                    break;
            }
        }

        // main loop
        begin_level = TRUE;
        while (!leave && !trm_gm && !gm.l_done && !restart)
        {
            just_started = 0;
            gmainloop();

            // don't consume all cpu time
            if (draw_move)
                /* Sleep when marble is moving, this will keep smooth enoungh 
                 */
                SDL_Delay(16);
            else
                /* Sleep when marble is stoping, this will reduce CPU eating */
                SDL_Delay(45);


            restore_pos = 0;    // do not restore old position
            ign_c_stat = 0;     // do not ignore cursor state
            // get input
            while (SDL_PollEvent(&ev))
            {

                switch (ev.type)
                {
                    case SDL_ACTIVEEVENT:
                        if (ev.active.gain == 0)    /* Lost focus, event */
                        {
                            exit_callback(0);
                        }
                        break;
                    case SDL_QUIT:
                        quit_callback(0);
                        break;
                    case SDL_KEYDOWN:
                        /* if (ev.key.keysym.sym == SDLK_ESCAPE) { if
                         * (escape_timeout == NULL) { escape_timeout =
                         * SDL_AddTimer(KEY_ESCAPE_TIMEOUT,
                         * (SDL_NewTimerCallback)app_escape_timeout, (void
                         * *)0); } } */
#if 0
			if( ev.key.keysym.sym == SDLK_q &&
                            ev.key.keysym.mod | ( KMOD_RCTRL & KMOD_LCTRL ) ) {
                        	printf( "Close whole game\n" );
                        	/* Add code here to send message to startup */
						    SDL_Event ev;
						    ev.type = SDL_USEREVENT;
						    SDL_PushEvent(&ev);                         
                            exiting_needed = 1;
                            exit_gm = 1;
                            gm.m_warp = 1;    
                            break;                    	
                        }
#endif
                        
                        if ((ev.key.keysym.sym == SDLK_ESCAPE)
                            || (ev.key.keysym.sym == SDLK_F6)
                            || (ev.key.keysym.sym == SDLK_F5))
                        {
				SDL_Event ev;
				ev.type = SDL_USEREVENT;
				SDL_PushEvent(&ev);                         
                            exiting_needed = 1;
                            exit_gm = 1;
                            gm.m_warp = 1;
                            
                        }
                        break;
                    case SDL_USEREVENT:
                        userevent_handler();
                        return;
                    case SDL_KEYUP:
                    	/* app_remove_timeout(); */

                        switch (ev.key.keysym.sym)
                        {
                            case SDLK_TAB:
                            case SDLK_KP_PLUS:
                            case SDLK_KP_MINUS:
                            case SDLK_PLUS:
                            case SDLK_MINUS:
                            case SDLK_F4:
                            case SDLK_F5:
                            case SDLK_F6:
                            case SDLK_ESCAPE:

                                exit_callback(0);
                                break;
                            case SDLK_RETURN:
                                if (gm.m_sel && !gm.m_mv)
                                    Mr_Rel( /* ev.button.x, ev.button.y */ );
                                break;

                            default:
                                /* if no marble is selected we don't have to
                                 * check anything */
                                if (!gm.m_sel)
                                    break;
                                /* warp? */
                                if (gm.m_mv
                                    && (ev.key.keysym.sym == cfg.k_right
                                        || ev.key.keysym.sym == cfg.k_left
                                        || ev.key.keysym.sym == cfg.k_up
                                        || ev.key.keysym.sym == cfg.k_down))
                                {
                                    gm.m_warp = 1;
                                    break;
                                }
                                // undo key
                                if ((ev.key.keysym.sym == cfg.k_undo)
                                    && !gm.m_mv)
                                {
                                    restore_pos = 1;
                                }
				
				// return key
                                if ((ev.key.keysym.sym == cfg.k_ret)
                                    && !gm.m_mv)
                                {
                                	exit_callback(0);
                                }
				
                                // up key
                                if (ev.key.keysym.sym == cfg.k_up
                                    && (gm.m_vd & MD_U) && !gm.m_mv)
                                {
                                    ign_c_stat = 1;
                                    gm.c_stat = C_U;
                                    Mr_IniMv();
                                }
                                // down key
                                if (ev.key.keysym.sym == cfg.k_down
                                    && (gm.m_vd & MD_D) && !gm.m_mv)
                                {
                                    ign_c_stat = 1;
                                    gm.c_stat = C_D;
                                    Mr_IniMv();
                                }
                                // left key
                                if (ev.key.keysym.sym == cfg.k_left
                                    && (gm.m_vd & MD_L) && !gm.m_mv)
                                {
                                    ign_c_stat = 1;
                                    gm.c_stat = C_L;
                                    Mr_IniMv();
                                }
                                // right key
                                if (ev.key.keysym.sym == cfg.k_right
                                    && (gm.m_vd & MD_R) && !gm.m_mv)
                                {
                                    ign_c_stat = 1;
                                    gm.c_stat = C_R;
                                    Mr_IniMv();
                                }
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if (gm.ms_us && ev.button.button == 1 &&
                            ev.button.x > gm.b_x + 56 &&
                            ev.button.x < gm.b_x + 56 + gm.s_undo->w &&
                            ev.button.y > 110 &&
                            ev.button.y < 110 + gm.s_undo->h)
                        {
                            D_DST(sdl.scr, gm.b_x + 56, 110, gm.s_undop->w,
                                  gm.s_undop->h);
                            D_SRC(gm.s_undop, 0, 0);
                            SS_Blt();
                            Sdl_AddR(gm.b_x + 56, 110, gm.s_undop->w,
                                     gm.s_undop->h);
                            undop = 1;
                            /* break; */
                        }
                        if (ev.button.button == 1 &&
                            ev.button.x > gm.b_x + 230 &&
                            ev.button.x < gm.b_x + 230 + gm.s_ret->w &&
                            ev.button.y > 0 &&
                            ev.button.y < gm.s_ret->h)
                        {
				SDL_Event ev;
				ev.type = SDL_USEREVENT;
				SDL_PushEvent(&ev);                         
                            exiting_needed = 1;
                            exit_gm = 1;
                            gm.m_warp = 1;
				leave = 4;
				break;
			}

                        break;
                    case SDL_MOUSEBUTTONUP:
                        if (undop == 1 && ev.button.button == 1 &&
                            ev.button.x > gm.b_x + 56 &&
                            ev.button.x < gm.b_x + 56 + gm.s_undo->w &&
                            ev.button.y > 110 &&
                            ev.button.y < 110 + gm.s_undo->h)
                        {
                            undop = 0;
                            D_DST(sdl.scr, gm.b_x + 56, 110, gm.s_undo->w,
                                  gm.s_undo->h);
                            D_SRC(gm.s_undo, 0, 0);
                            SS_Blt();
                            Sdl_AddR(gm.b_x + 56, 110, gm.s_undo->w,
                                     gm.s_undo->h);
                            if (!gm.m_mv)
                                restore_pos = 1;
                            break;
                        }
			/* Sandy */
                        if (leave == 4 && ev.button.button == 1 &&
                            ev.button.x > gm.b_x + 230 &&
                            ev.button.x < gm.b_x + 230 + gm.s_ret->w &&
                            ev.button.y > 0 &&
                            ev.button.y < gm.s_ret->h)
                        {
                    		exit_callback(0);
				break;
			}
			/* Sandy -end */
                        if (ev.button.button == 1 && gm.m_mv &&
                            (gm.c_l_st->limit_type == MOVES))
                        {
                            gm.m_warp = 1;
                            break;
                        }
                        gm.bttn[ev.button.button] = 1;
                        if (ev.button.button == 1)
                        {
                            if (gm.c_stat == C_SEL)
                                Mr_Sel(ev.button.x, ev.button.y);
                            else if (gm.m_sel && !gm.m_mv)
                            {
                                // start movement of selected marble

                                Mr_IniMv();

                            }
                        }
                        else if (gm.m_sel && !gm.m_mv)
                            Mr_Rel( /* ev.button.x, ev.button.y */ );
                        break;
                    case SDL_MOUSEMOTION:
                        if (undop == 1 && (ev.motion.x < gm.b_x + 56 ||
                                           ev.motion.x >
                                           gm.b_x + 56 + gm.s_undo->w
                                           || ev.motion.y < 110
                                           || ev.motion.y >
                                           110 + gm.s_undo->h))
                        {
                            D_DST(sdl.scr, gm.b_x + 56, 110, gm.s_undo->w,
                                  gm.s_undo->h);
                            D_SRC(gm.s_undo, 0, 0);
                            SS_Blt();
                            Sdl_AddR(gm.b_x + 56, 110, gm.s_undo->w,
                                     gm.s_undo->h);
                            undop = 0;
                        }
                        if (!ign_c_stat)
                            Cr_Cng(ev.motion.x, ev.motion.y);
                        gm.o_mx = ev.motion.x;
                        gm.o_my = ev.motion.y;
                        break;
                    default:
                        break;
                }
            }

            if ((undop != -1) && (!gm.ms_us))
            {
                D_DST(sdl.scr, gm.b_x + 56, 110, gm.s_undod->w,
                      gm.s_undod->h);
                D_SRC(gm.s_undod, 0, 0);
                SS_Blt();
                Sdl_AddR(gm.b_x + 56, 110, gm.s_undod->w, gm.s_undod->h);
                undop = -1;
            }
            else if ((undop == -1) && (gm.ms_us))
            {
                D_DST(sdl.scr, gm.b_x + 56, 110, gm.s_undo->w, gm.s_undo->h);
                D_SRC(gm.s_undo, 0, 0);
                SS_Blt();
                Sdl_AddR(gm.b_x + 56, 110, gm.s_undo->w, gm.s_undo->h);
                undop = 0;
            }

            draw_move = (gm.m_mv || restore_pos
                         || begin_level) ? TRUE : FALSE;

            // hide
            if (!leave && !restart)
            {
                Mr_Hd();
                MF_Hd();
                if (draw_move)
                {
                    Tm_Hd();
                    if (begin_level)
                    {
                        Inf_Hd();
                    }
                    Shr_Hd();
                }
                // Cr_Hd();
            }

            // update
            ms = T_Gt();
            MA_Upd(ms);
            if (restore_pos)
                Mr_ResPos();
            if (!Mr_Upd(ms))
                restart = 1;
            MF_Upd(ms);
            Shr_Upd(ms);
            if (!Tm_Upd(ms))
                restart = 1;
            if (Inf_Upd() && G_CfmWrp())
            {
                gm.c_ch = gm.w_c;
                gm.c_l_id = gm.w_l;
                restart = 1;
            }

            // show
            if (!leave && !restart)
            {
                MA_Shw();
                Mr_Shw();
                if( !gm.l_done ) {
	                MF_Shw();
	                if (draw_move)
	                {
	                    Tm_Shw();
	                    if (begin_level)
	                    {
	                        Inf_Shw();
	                    }
	                    Shr_Shw();
	                }

	                // refresh
	                Sdl_UpdR();
	            } else {
                	sdl.rnum = 0;   // reset redraw regions
                }
			}
			
            begin_level = FALSE;

            // reset buttonstate
            memset(gm.bttn, 0, sizeof(gm.bttn));

            // run shadowapp mainloop
            // hgw_msg_compat_receive(sapp_context, 0);
        }

        // leave ?
        if (leave)
            break;

        // init next level
        if (gm.l_done || restart)
        {
            // figure animation
            if (!just_started && !restart) FA_Run();


            if (gm.l_done)
            {

                if (!gm.c_s_inf->cmp[gm.c_ch * gm.c_s_inf->l_num + gm.c_l_id])
                    level_completed = 0;    // level wasn't completed until
                                            // now
                else
                    level_completed = 1;    // level was completed before

                // bonus summary
                if (!level_completed)
                {
                    /* level wasn't completed until now so gain score for it */
                    bonus_level = LB_COMPLETED;
                    bonus_moves = gm.c_lvl->tm * LB_PER_MOVE;
                    modify_score(&bonus_level, &bonus_moves);
                    if (exiting_needed == 0)
                    {
                        BS_Run(bonus_level, bonus_moves);
                    }
                    tm_rel =
                        (1000 * gm.c_lvl->tm) /
                        gm.c_l_st->ch[gm.c_ch].lvls[gm.c_l_id].tm;
                    Prf_Upd(gm.c_prf, gm.c_s_inf,
                            gm.c_ch * gm.c_l_st->l_num + gm.c_l_id, tm_rel,
                            bonus_level + bonus_moves);
                }
            }
            if (exiting_needed == 0)
            {
                if (cfg.dim)
                    SDL_DIM();
            }
            if (!restart)
                if (!L_FndNxt())
                {               // game finished ?
                    if (!level_completed && !trm_gm)
                        G_Cngrt();
                    // break;
                }
            if (exiting_needed == 0)
            {
                MS_Clear(&gm.ms_us);
                L_Ini(gm.c_ch, gm.c_l_id, L_DATA | L_GFX);
                restart = 0;
            }
            else
            {
                MS_Clear(&gm.ms_us);
                // L_Ini(gm.c_ch, gm.c_l_id, L_DATA | L_GFX);
            }
            if (quit_gm)
                quit_callback(0);
            if (exit_gm)
                exit_callback(0);
        }
    }
    // save profiles
    Prf_Sv();

    /* app_remove_timeout(); */
}

/* 
 * load all gfx sets */
void
G_LdGSts()
{
    char d_nm[256];
    char path[256 + 64];
    DIR *dir = 0;
    struct dirent *e;
    struct stat s;
    GSet *g_st;


    // create directory string //
    sprintf(d_nm, "%s", LMARBLESGFXDIR);

    // find and open directory //
    if ((dir = opendir(d_nm)) == 0)
    {
        ULOG_ERR("ERROR: can't find directory '%s'\n", d_nm);
        exit(1);
    }

    // well, let's check for directories //
    while ((e = readdir(dir)) != 0)
    {
        sprintf(path, "%s/%s", d_nm, e->d_name);
        stat(path, &s);
        if (S_ISDIR(s.st_mode) && e->d_name[0] != '.' && e->d_name[0] != 'd')
        {
            ULOG_DEBUG("'%s'... \n", e->d_name);
            // load gfx //
            g_st = (GSet *) malloc(sizeof(GSet));
            memset(g_st, 0, sizeof(GSet));
            strcpy(g_st->nm, e->d_name);
            g_st->ok = 1;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "background.jpg");
            g_st->s_bkgd = SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL);
            SDL_SetColorKey(g_st->s_bkgd, 0, 0);

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name, "wall.bmp");
            if ((g_st->s_wl =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name, "floor.bmp");
            if ((g_st->s_flr =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "arrow_r.bmp");
            if ((g_st->s_r_arw =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "arrow_l.bmp");
            if ((g_st->s_l_arw =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "arrow_u.bmp");
            if ((g_st->s_u_arw =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "arrow_d.bmp");
            if ((g_st->s_d_arw =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "barrier_lr.bmp");
            if ((g_st->s_lr_bar =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "barrier_ud.bmp");
            if ((g_st->s_ud_bar =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "teleport0.bmp");
            if ((g_st->s_tlp_0 =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "teleport1.bmp");
            if ((g_st->s_tlp_1 =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "teleport2.bmp");
            if ((g_st->s_tlp_2 =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "teleport3.bmp");
            if ((g_st->s_tlp_3 =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            sprintf(path, "%s/%s/%s", LMARBLESGFXDIR, e->d_name,
                    "wall_crumble.bmp");
            if ((g_st->s_crmbl =
                 SS_Ld(path, SDL_SURFACE_TYPE | SDL_NONFATAL)) == 0)
                g_st->ok = 0;

            if (g_st->ok)
            {
                ULOG_DEBUG("ok\n");
                // opaque
                SDL_SetColorKey(g_st->s_flr, 0, 0);
                SDL_SetColorKey(g_st->s_wl, 0, 0);
            }
            DL_Add(&gm.g_sts, g_st);
        }
    }
    closedir(dir);
}


/* 
 * delete a gset */
void
G_DelGSt(void *p)
{
    GSet *st = (GSet *) p;
    if (st->s_bkgd)
        SDL_FreeSurface(st->s_bkgd);
    if (st->s_wl)
        SDL_FreeSurface(st->s_wl);
    if (st->s_flr)
        SDL_FreeSurface(st->s_flr);
    if (st->s_u_arw)
        SDL_FreeSurface(st->s_u_arw);
    if (st->s_d_arw)
        SDL_FreeSurface(st->s_d_arw);
    if (st->s_r_arw)
        SDL_FreeSurface(st->s_r_arw);
    if (st->s_l_arw)
        SDL_FreeSurface(st->s_l_arw);
    if (st->s_lr_bar)
        SDL_FreeSurface(st->s_lr_bar);
    if (st->s_ud_bar)
        SDL_FreeSurface(st->s_ud_bar);
    if (st->s_tlp_0)
        SDL_FreeSurface(st->s_tlp_0);
    if (st->s_tlp_1)
        SDL_FreeSurface(st->s_tlp_1);
    if (st->s_tlp_2)
        SDL_FreeSurface(st->s_tlp_2);
    if (st->s_tlp_3)
        SDL_FreeSurface(st->s_tlp_3);
    if (st->s_crmbl)
        SDL_FreeSurface(st->s_crmbl);
    free(st);
}

/* 
 * pause game */
void
G_Ps()
{
    SDL_Surface *buf;
    SDL_Event e;
    int leave = 0;
    int mx = gm.o_mx, my = gm.o_my;

    // save screen //
    buf = SS_Crt(gm.scr_w, gm.scr_h, SDL_SURFACE_TYPE);
    SDL_SetColorKey(buf, 0, 0);
    D_FDST(buf);
    D_FSRC(sdl.scr);
    SS_Blt();

    // cursor
    SDL_SetCursor(gm.c_n);

    // fill with black
    D_FDST(sdl.scr);
    SS_Fill(0x0);

    FT_DrwTxtAlgn(gm.ft_fnt, sdl.scr, gm.scr_w / 2, gm.scr_h / 2,
                  FT_ALIGN_X_C | FT_ALIGN_Y_C, 30, "Game paused", COL_DLG);

    Sdl_FUpd();

    // wait for 'p'
    while (!leave)
    {
        // SDL_WaitEvent(&e);
        while (!SDL_PollEvent(&e))
        {
            gmainloop();
            SDL_Delay(45);
        }
        switch (e.type)
        {
            case SDL_USEREVENT:
                userevent_handler();
                leave = 1;
                trm_gm = 1;
                break;
            case SDL_QUIT:
                trm_gm = 1;
                leave = 1;
                break;
            case SDL_KEYUP:
                if (e.key.keysym.sym == SDLK_p)
                    leave = 1;
            case SDL_KEYDOWN:
                if ((e.key.keysym.sym == SDLK_ESCAPE)
                    || (e.key.keysym.sym == SDLK_F4)
                    || (e.key.keysym.sym == SDLK_F5)
                    || (e.key.keysym.sym == SDLK_F6))
                {
                    exit_gm = 1;
                    leave = 1;
                    exiting_needed = 1;
                    exit_callback(0);
                    return;
                }
                break;
            case SDL_MOUSEMOTION:
                mx = e.motion.x;
                my = e.motion.y;
                break;
        }
    }

    // restore screen
    D_FDST(sdl.scr);
    D_FSRC(buf);
    SS_Blt();
    Sdl_FUpd();
    SDL_FreeSurface(buf);

    // cursor
    Cr_Cng(mx, my);

    // reset time //
    T_Rst();
}



/* 
 * check if the figure has been completed */
void
G_CkFgr()
{
    int i, j, k, l;
    Lvl *lvl = gm.c_lvl;

    for (i = 0; i < lvl->m_w - lvl->f_w; i++)
        for (j = 0; j < lvl->m_h - lvl->f_h; j++)
        {
            gm.l_done = 1;
            for (k = 0; k < lvl->f_w; k++)
            {
                for (l = 0; l < lvl->f_h; l++)
                {
                    if (lvl->fgr[k][l] != -1)
                        if (lvl->map[i + k][j + l].m != lvl->fgr[k][l])
                        {
                            gm.l_done = 0;
                            break;
                        }
                }
                if (!gm.l_done)
                    break;
            }
            if (gm.l_done)
                return;
        }
}

int
G_DrwDlg(int fontsize, char *question,
         int fontsize2, char *question2, char *but1, char *but2)
{
    int dlg_w = gm.s_dyes->w + gm.s_dno->w;
    int dlg_h = gm.s_dyes->h + 10;
    int dlg_x, dlg_y;

    int dlg_tw = question ? FT_TxtWdth(gm.ft_fnt, fontsize, question) : 0;
    int dlg_tw2 = question2 ? FT_TxtWdth(gm.ft_fnt, fontsize2, question2) : 0;
    int dlg_th = 0;

    SDL_Surface *dlg;
    SDL_Surface *buf;
    SDL_Event e;

    int leave = 0, ret = 0;
    int mx = gm.o_mx, my = gm.o_my;

    // save screen //
    buf = SS_Crt(gm.scr_w, gm.scr_h, SDL_SURFACE_TYPE);
    SDL_SetColorKey(buf, 0, 0);
    D_FDST(buf);
    D_FSRC(sdl.scr);
    SS_Blt();

    dlg = SS_Crt(gm.scr_w, gm.scr_h, SDL_SURFACE_TYPE);
    SDL_SetColorKey(dlg, 0, 0);
    D_FDST(dlg);
    SS_Fill(0x0);
    D_FDST(sdl.scr);
    D_FSRC(dlg);
    SS_ABlt(128);
    SDL_FreeSurface(dlg);

    // cursor
    SDL_SetCursor(gm.c_n);

    if (dlg_tw > dlg_w)
        dlg_w = dlg_tw;
    if (dlg_tw2 > dlg_w)
        dlg_w = dlg_tw2;
    if (dlg_w > 640)
        dlg_w = 640;
    dlg_th = question ? (FT_CntLns(gm.ft_fnt, fontsize, question, dlg_w)
                         * FT_LnHght(gm.ft_fnt, fontsize)) : 0;
    dlg_h += (question2 ? FT_CntLns(gm.ft_fnt, fontsize2, question2, dlg_w)
              * FT_LnHght(gm.ft_fnt, fontsize) : 0) + dlg_th;

    dlg_w += gm.s_dle->w + gm.s_drg->w;
    dlg_h += gm.s_dto->h + gm.s_dbo->h;

    dlg_x = (gm.scr_w - dlg_w) / 2;
    dlg_y = (gm.scr_h - dlg_h) / 2;

    dlg = SS_Crt(dlg_w, dlg_h, SDL_SURFACE_TYPE);

    // Bottom right
    D_DST(dlg, dlg_w - gm.s_dbr->w, dlg_h - gm.s_dbr->h, gm.s_dbr->w,
          gm.s_dbr->h);
    D_SRC(gm.s_dbr, 0, 0);
    SS_Blt();

    // Bottom left
    D_DST(dlg, 0, dlg_h - gm.s_dtr->h, gm.s_dtr->w, gm.s_dtr->h);
    D_SRC(gm.s_dbl, 0, 0);
    SS_Blt();

    // Top right
    D_DST(dlg, dlg_w - gm.s_dbl->w, 0, gm.s_dbl->w, gm.s_dbl->h);
    D_SRC(gm.s_dtr, 0, 0);
    SS_Blt();

    // Top left
    D_DST(dlg, 0, 0, gm.s_dtl->w, gm.s_dtl->h);
    D_SRC(gm.s_dtl, 0, 0);
    SS_Blt();

    // Right
    D_DST(dlg, dlg_w - gm.s_drg->w, gm.s_dtr->h, gm.s_drg->w,
          dlg_h - gm.s_dto->h - gm.s_dbo->h);
    D_SRC(gm.s_drg, 0, 0);
    SS_TlBlt();

    // Left
    D_DST(dlg, 0, gm.s_dto->h, gm.s_dle->w,
          dlg_h - gm.s_dto->h - gm.s_dbo->h);
    D_SRC(gm.s_dle, 0, 0);
    SS_TlBlt();

    // Bottom
    D_DST(dlg, gm.s_dle->w, dlg_h - gm.s_dbl->h,
          dlg_w - gm.s_dle->w - gm.s_drg->w, gm.s_dbo->h);
    D_SRC(gm.s_dbo, 0, 0);
    SS_TlBlt();

    // Top
    D_DST(dlg, gm.s_dle->w, 0, dlg_w - gm.s_dle->w - gm.s_drg->w,
          gm.s_dto->h);
    D_SRC(gm.s_dto, 0, 0);
    SS_TlBlt();

    // Background
    D_DST(dlg, gm.s_dle->w, gm.s_dto->h, dlg_w - gm.s_dle->w - gm.s_drg->w,
          dlg_h - gm.s_dto->h - gm.s_dbo->h);
    D_SRC(gm.s_dbg, 0, 0);
    SS_TlBlt();

    if (but1)
    {
        // Yes button
        D_DST(dlg, gm.s_dle->w, dlg_h - gm.s_dbo->h - gm.s_dyes->h,
              gm.s_dyes->w, gm.s_dyes->h);
        D_SRC(gm.s_dyes, 0, 0);
        SS_Blt();
        FT_DrwTxtAlgn(gm.ft_fnt, dlg, gm.s_dle->w + 75,
                      dlg_h - gm.s_dbo->h - gm.s_dyes->h / 2, FT_ALIGN_Y_C,
                      32, but1, COL_MAIN);
    }

    if (but2)
    {
        // No button
        D_DST(dlg, dlg_w - gm.s_drg->w - gm.s_dno->w,
              dlg_h - gm.s_dbo->h - gm.s_dno->h, gm.s_dno->w, gm.s_dno->h);
        D_SRC(gm.s_dno, 0, 0);
        SS_Blt();
        FT_DrwTxtAlgn(gm.ft_fnt, dlg,
                      dlg_w - gm.s_drg->w - FT_TxtWdth(gm.ft_fnt, 32,
                                                       but2) - 75,
                      dlg_h - gm.s_dbo->h - gm.s_dyes->h / 2, FT_ALIGN_Y_C,
                      32, but2, COL_MAIN);
    }

    if (question)
    {
        FT_DrwTxtWrpAlgn(gm.ft_fnt, dlg, gm.s_dle->w, gm.s_dto->h, dlg_w,
                         FT_ALIGN_Y_T | FT_ALIGN_X_C, fontsize, question,
                         COL_MAIN);
    }

    if (question2)
    {
        FT_DrwTxtWrpAlgn(gm.ft_fnt, dlg, gm.s_dle->w, gm.s_dto->h + dlg_th,
                         dlg_w, FT_ALIGN_Y_T | FT_ALIGN_X_C, fontsize2,
                         question2, COL_MAIN);
    }

    D_DST(sdl.scr, dlg_x, dlg_y, dlg->w, dlg->h);
    D_SRC(dlg, 0, 0);
    SS_Blt();

    SDL_FreeSurface(dlg);

    Sdl_FUpd();

    while (!leave)
    {
        // SDL_WaitEvent(&e);
        while (!SDL_PollEvent(&e))
        {
            gmainloop();
            SDL_Delay(45);
        }
        switch (e.type)
        {
            case SDL_ACTIVEEVENT:
                if (e.active.gain == 0)
                {
                    trm_gm = leave = 1;
                    exit_callback(0);
                }
                break;
            case SDL_USEREVENT:
                leave = 1;
                trm_gm = 1;
                userevent_handler();
                break;
            case SDL_QUIT:
                trm_gm = leave = 1;
                quit_callback(0);
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {

                    case SDLK_F6:
                    case SDLK_F5:
                        trm_gm = leave = 1;
                        exit_callback(0);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym)
                {
                    case SDLK_TAB:
                    case SDLK_KP_PLUS:
                    case SDLK_KP_MINUS:
                    case SDLK_PLUS:
                    case SDLK_MINUS:
                    case SDLK_F4:


                        trm_gm = leave = 1;
                        exit_callback(0);
                        break;
                    case SDLK_RETURN:
                    case SDLK_y:
                        ret = 1;
                        leave = 1;
                        break;
                    case SDLK_n:
                    case SDLK_ESCAPE:
                        ret = 0;
                        leave = 1;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == 1)
                {
                    if (but1 && e.button.x > dlg_x + gm.s_dle->w &&
                        e.button.x < dlg_x + gm.s_dle->w + gm.s_dyes->w &&
                        e.button.y > dlg_y + dlg_h - gm.s_dbo->h - gm.s_dno->h
                        && e.button.y < dlg_y + dlg_h - gm.s_dbo->h)
                    {
                        ret = 1;
                        leave = 1;
                    }
                    else if (but2 &&
                             e.button.x >
                             dlg_x + dlg_w - gm.s_drg->w - gm.s_dno->w
                             && e.button.x < dlg_x + dlg_w - gm.s_drg->w
                             && e.button.y >
                             dlg_y + dlg_h - gm.s_dbo->h - gm.s_dno->h
                             && e.button.y < dlg_y + dlg_h - gm.s_dbo->h)
                    {
                        ret = 0;
                        leave = 1;
                    }
                }
                break;
        }
    }
    D_FDST(sdl.scr);
    D_FSRC(buf);
    SS_Blt();
    Sdl_FUpd();

    // cursor
    Cr_Cng(mx, my);

    SDL_FreeSurface(buf);

    T_Rst();

    return ret;
}

int
G_Cngrt()
{
    return G_DrwDlg(32, _("game_ti_lmarbles_congratulations_title"), 16,
                    _("game_fi_lmarbles_congratulations_description"),
		    dgettext ("hildon-libs", "wdgt_bd_done" ),
                    0);
}

/* 
 * confirm warp */
int
G_CfmWrp()
{
    char str[1024];
    char *tmpmsg = _("game_nc_change_level");
    snprintf(str, 1024, tmpmsg, gm.w_l + 1, gm.w_c + 1);
    // G_Tmpl(str, 1024, _("game_nc_change_level"), gm.w_c + 1, gm.w_l + 1);
    return G_DrwDlg(26, str, 0, 0, _("game_bd_change_level_ok"),
                    _("game_bd_change_level_cancel"));
}

/* 
 * confirm quit */
int
G_CfmQut()
{
    return G_DrwDlg(32, "Quit", 16, "Do you really want to quit?", "Yes",
                    "No");
}

/* 
 * confirm reset */
int
G_CfmRst()
{
    return G_DrwDlg(0, 0, 32, "Do you really want to restart?", "Yes", "No");
}

/* 
 * check if level set can be played and ask for confirmation if it got errors */
int
G_CkLSt()
{
    if (!gm.c_l_st->ok)
    {
        return 0;
    }
    return 1;
}

// marble //
/* 
 * hide current marble */
void
Mr_Hd()
{
    if (!gm.m_sel)
        return;
    D_DST(sdl.scr, (int) gm.m_x, (int) gm.m_y, gm.t_w, gm.t_h);
    D_SRC(gm.s_bkgd, (int) gm.m_x, (int) gm.m_y);
    SS_Blt();
    Sdl_AddR((int) gm.m_x, (int) gm.m_y, gm.t_w, gm.t_h);
}

/* 
 * show current marble */
void
Mr_Shw()
{
    if (!gm.m_sel)
        return;
    D_DST(sdl.scr, (int) gm.m_x, (int) gm.m_y, gm.t_w, gm.t_h);
    D_SRC(gm.s_mrb, (int) gm.m_a.p * gm.m_a.w, gm.m_id * gm.t_h);
    SS_Blt();
    Sdl_AddR((int) gm.m_x, (int) gm.m_y, gm.t_w, gm.t_h);
}

/* 
 * update current marble return 0 if times out( moves ) */
int
Mr_Upd(int ms)
{
    float c;                    // change
    int stp = 0;                // marble stopped ?

    if (!gm.m_sel || !gm.m_mv)
        return 1;

    // marble animation (frame)
    switch (gm.m_d)
    {
        case 1:
        case 2:
            gm.m_a.p += gm.m_a.c * ms;
            if (gm.m_a.p >= gm.m_a.f)
                gm.m_a.p = 0;
            break;
        case 0:
        case 3:
            gm.m_a.p -= gm.m_a.c * ms;
            if (gm.m_a.p <= 0)
                gm.m_a.p = gm.m_a.f;
            break;
    }

    c = ms * gm.m_v;
    switch (gm.m_d)
    {
        case 0:
            gm.m_y -= c;
            if (gm.m_y <= gm.m_ty)
            {
                Mr_Stp();
                stp = 1;
            }
            break;
        case 1:
            gm.m_x += c;
            if (gm.m_x >= gm.m_tx)
            {
                Mr_Stp();
                stp = 1;
            }
            break;
        case 2:
            gm.m_y += c;
            if (gm.m_y >= gm.m_ty)
            {
                Mr_Stp();
                stp = 1;
            }
            break;
        case 3:
            gm.m_x -= c;
            if (gm.m_x <= gm.m_tx)
            {
                Mr_Stp();
                stp = 1;
            }
            break;
    }

    // warp???
    if (gm.m_warp)
    {
        gm.m_x = gm.m_tx;
        gm.m_y = gm.m_ty;
        Mr_Stp();
        stp = 1;
        gm.m_warp = 0;
    }

    // check time if move limit //
    if (stp && gm.c_l_st->limit_type == MOVES &&
        gm.m_act != M_TLP_0 && gm.m_act != M_TLP_1 &&
        gm.m_act != M_TLP_2 && gm.m_act != M_TLP_3)
    {

        gm.c_lvl->tm--;
        if (gm.c_lvl->tm <= 0 && !gm.l_done /* completion with last move is
                                             * okay */ )
            return 0;

    }

    // stopped and awaiting action ?
    if (stp && gm.m_act != M_EMPTY)
        Mr_Act();
    else if (stp)
    {
#ifdef SOUND
        sound_play(gm.wv_stp);
#endif
    }

    return 1;
}

/* 
 * select a marble */
void
Mr_Sel(int x, int y)
{
    int mx, my;

    mx = (x - gm.l_x) / gm.t_w;
    my = (y - gm.l_y) / gm.t_h;

    if (gm.m_sel)
    {
        gm.mf_a.p = gm.msf_a.p = 0;
        Mr_Ins();
    }

    // set selected
    gm.m_sel = 1;
    // get map position
    gm.m_mx = mx;
    gm.m_my = my;
    // check valid moving directions
    Mr_CkVDir(mx, my);
    // delete marble from background
    L_DrwMpTl(mx, my);
    // get position in screen
    gm.m_x = mx * gm.t_w + gm.l_x;
    gm.m_y = my * gm.t_h + gm.l_y;
    // get id //
    gm.m_id = gm.c_lvl->map[mx][my].m;
    // delete marble from map
    gm.c_lvl->map[mx][my].m = -1;
    // save old position
    gm.m_o_x = (int) gm.m_x;
    gm.m_o_y = (int) gm.m_y;
    gm.m_o_move_count = gm.c_lvl->tm;

#ifdef SOUND
    sound_play(gm.wv_sel);
#endif
}

/* 
 * release a marble */
void
Mr_Rel( /* int x, int y */ )
{
    Mr_Ins();
    gm.m_sel = 0;
    SDL_SetCursor(gm.c_n);
    gm.m_o_x = gm.m_o_y = -1;
}

/* 
 * initialize movement */
void
Mr_IniMv()
{
    int x_a = 0, y_a = 0, tx, ty;
    int t_fnd = 0;

    // direction
    gm.m_d = gm.c_stat - 2;
    if (gm.m_d < 0 || gm.m_d > 4)
    {
        return;
    }
#ifdef SOUND
    sound_play(gm.wv_clk);
#endif

    // direction verified; activate movement
    gm.m_mv = 1;

    // store position if no action
    if (gm.m_act == M_EMPTY)
    {
        gm.m_o_x = (int) gm.m_x;
        gm.m_o_y = (int) gm.m_y;
        gm.m_o_move_count = gm.c_lvl->tm;
    }

    // clear previous action
    gm.m_act = M_EMPTY;

    // compute target position
    switch (gm.m_d)
    {
        case 0:
            y_a = -1;
            break;
        case 1:
            x_a = 1;
            break;
        case 2:
            y_a = 1;
            break;
        case 3:
            x_a = -1;
            break;
    }
    tx = gm.m_mx + x_a;
    ty = gm.m_my + y_a;
    while (                     // target already found
              !t_fnd &&
              // wall
              gm.c_lvl->map[tx][ty].t != M_WALL &&
              // crumbling wall
              gm.c_lvl->map[tx][ty].t != M_CRUMBLE &&
              // marble
              gm.c_lvl->map[tx][ty].m == -1 &&
              // up
              !((gm.c_lvl->map[tx][ty].t == M_OW_D
                 || gm.c_lvl->map[tx][ty].t == M_OW_D_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_L_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_R_C) && y_a == -1) &&
              // down
              !((gm.c_lvl->map[tx][ty].t == M_OW_U
                 || gm.c_lvl->map[tx][ty].t == M_OW_U_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_L_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_R_C) && y_a == 1) &&
              // right
              !((gm.c_lvl->map[tx][ty].t == M_OW_L
                 || gm.c_lvl->map[tx][ty].t == M_OW_L_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_U_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_D_C) && x_a == 1) &&
              // left
              !((gm.c_lvl->map[tx][ty].t == M_OW_R
                 || gm.c_lvl->map[tx][ty].t == M_OW_R_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_U_C
                 || gm.c_lvl->map[tx][ty].t == M_OW_D_C) && x_a == -1))
    {

        // check action
        switch (gm.c_lvl->map[tx][ty].t)
        {
            case M_TLP_0:
            case M_TLP_1:
            case M_TLP_2:
            case M_TLP_3:
                t_fnd = 1;
                gm.m_act = gm.c_lvl->map[tx][ty].t;
                break;
            case M_OW_U:
                if (y_a != -1)
                {
                    gm.m_act = M_OW_U;
                    t_fnd = 1;
                }
                break;
            case M_OW_D:
                if (y_a != 1)
                {
                    gm.m_act = M_OW_D;
                    t_fnd = 1;
                }
                break;
            case M_OW_L:
                if (x_a != -1)
                {
                    gm.m_act = M_OW_L;
                    t_fnd = 1;
                }
                break;
            case M_OW_R:
                if (x_a != 1)
                {
                    gm.m_act = M_OW_R;
                    t_fnd = 1;
                }
                break;
        }
        tx += x_a;
        ty += y_a;
    }

    // crumbling wall
    if (gm.c_lvl->map[tx][ty].t == M_CRUMBLE)
        gm.m_act = M_CRUMBLE;

    tx -= x_a;
    ty -= y_a;

    gm.m_tx = tx * gm.t_w + gm.l_x;
    gm.m_ty = ty * gm.t_h + gm.l_y;

    if (gm.m_act == M_EMPTY || gm.m_act == M_CRUMBLE)
        MS_Push(&gm.ms_us, gm.m_o_x, gm.m_o_y, gm.m_tx, gm.m_ty,
                gm.m_o_move_count - gm.c_lvl->tm + 1);

    D_DST(sdl.scr, gm.b_x + 56, 110, gm.s_undo->w, gm.s_undo->h);
    D_SRC(gm.s_undo, 0, 0);
    SS_Blt();
    Sdl_AddR(gm.b_x + 56, 110, gm.s_undo->w, gm.s_undo->h);

    // wait cursor
    SDL_SetCursor(gm.c_w);
}

/* 
 * stop a marble */
void
Mr_Stp()
{
    int mx, my;

    // position in screen
    gm.m_x = gm.m_tx;
    gm.m_y = gm.m_ty;

    mx = (gm.m_x - gm.l_x) / gm.t_w;
    my = (gm.m_y - gm.l_y) / gm.t_h;

    // position in map
    gm.m_mx = mx;
    gm.m_my = my;

    // check valid moving directions
    Mr_CkVDir(mx, my);

    gm.m_mv = 0;
    gm.m_a.p = 0;

    // check cursor //
    Cr_Cng(gm.o_mx, gm.o_my);

    // if no action check if the figure is completed
    if (gm.m_act == M_EMPTY || gm.m_act == M_CRUMBLE)
    {

        gm.c_lvl->map[mx][my].m = gm.m_id;
        G_CkFgr();
        gm.c_lvl->map[mx][my].m = -1;
    }
}

/* 
 * insert a marble into map */
void
Mr_Ins()
{
    int mx, my, x, y;

    mx = (gm.m_x - gm.l_x) / gm.t_w;
    my = (gm.m_y - gm.l_y) / gm.t_h;
    x = mx * gm.t_w + gm.l_x;
    y = my * gm.t_h + gm.l_y;

    // hide frame
    MF_Hd();
    // draw to background
    D_DST(gm.s_bkgd, x, y, gm.t_w, gm.t_h);
    D_SRC(gm.s_mrb, 0, gm.m_id * gm.t_h);
    SS_Blt();
    // to screen
    D_DST(sdl.scr, x, y, gm.t_w, gm.t_h);
    D_SRC(gm.s_mrb, 0, gm.m_id * gm.t_h);
    SS_Blt();
    // and to map
    gm.c_lvl->map[mx][my].m = gm.m_id;
}

/* 
 * check valid directions */
void
Mr_CkVDir(int mx, int my)
{
    gm.m_vd = 0;
    if (gm.c_lvl->map[mx][my].t != M_OW_D_C
        && gm.c_lvl->map[mx][my].t != M_OW_U_C
        && gm.c_lvl->map[mx - 1][my].t != M_WALL
        && gm.c_lvl->map[mx - 1][my].t != M_CRUMBLE
        && gm.c_lvl->map[mx - 1][my].m == -1
        && gm.c_lvl->map[mx - 1][my].t != M_OW_R
        && gm.c_lvl->map[mx - 1][my].t != M_OW_R_C
        && gm.c_lvl->map[mx - 1][my].t != M_OW_U_C
        && gm.c_lvl->map[mx - 1][my].t != M_OW_D_C)
        gm.m_vd = gm.m_vd | MD_L;
    if (gm.c_lvl->map[mx][my].t != M_OW_D_C
        && gm.c_lvl->map[mx][my].t != M_OW_U_C
        && gm.c_lvl->map[mx + 1][my].t != M_WALL
        && gm.c_lvl->map[mx + 1][my].t != M_CRUMBLE
        && gm.c_lvl->map[mx + 1][my].m == -1
        && gm.c_lvl->map[mx + 1][my].t != M_OW_L
        && gm.c_lvl->map[mx + 1][my].t != M_OW_L_C
        && gm.c_lvl->map[mx + 1][my].t != M_OW_U_C
        && gm.c_lvl->map[mx + 1][my].t != M_OW_D_C)
        gm.m_vd = gm.m_vd | MD_R;
    if (gm.c_lvl->map[mx][my].t != M_OW_L_C
        && gm.c_lvl->map[mx][my].t != M_OW_R_C
        && gm.c_lvl->map[mx][my - 1].t != M_WALL
        && gm.c_lvl->map[mx][my - 1].t != M_CRUMBLE
        && gm.c_lvl->map[mx][my - 1].m == -1
        && gm.c_lvl->map[mx][my - 1].t != M_OW_D
        && gm.c_lvl->map[mx][my - 1].t != M_OW_D_C
        && gm.c_lvl->map[mx][my - 1].t != M_OW_L_C
        && gm.c_lvl->map[mx][my - 1].t != M_OW_R_C)
        gm.m_vd = gm.m_vd | MD_U;
    if (gm.c_lvl->map[mx][my].t != M_OW_L_C
        && gm.c_lvl->map[mx][my].t != M_OW_R_C
        && gm.c_lvl->map[mx][my + 1].t != M_WALL
        && gm.c_lvl->map[mx][my + 1].t != M_CRUMBLE
        && gm.c_lvl->map[mx][my + 1].m == -1
        && gm.c_lvl->map[mx][my + 1].t != M_OW_U
        && gm.c_lvl->map[mx][my + 1].t != M_OW_U_C
        && gm.c_lvl->map[mx][my + 1].t != M_OW_L_C
        && gm.c_lvl->map[mx][my + 1].t != M_OW_R_C)
        gm.m_vd = gm.m_vd | MD_D;
}

/* 
 * handle actions! */
void
Mr_Act()
{
    int x_a = 0, y_a = 0, ow = 0, mx = gm.m_mx, my = gm.m_my, tx, ty;
    int i, j;

    // crumbling wall ?
    if (gm.m_act == M_CRUMBLE)
    {
        tx = mx + (gm.m_d == 1 ? 1 : gm.m_d == 3 ? -1 : 0);
        ty = my + (gm.m_d == 0 ? -1 : gm.m_d == 2 ? 1 : 0);
        if (gm.ms_us)
        {
            gm.ms_us->c_x = tx;
            gm.ms_us->c_y = ty;
        }
        if (gm.c_lvl->map[tx][ty].t == M_CRUMBLE)
        {
#ifdef SOUND
            sound_play(gm.wv_stp);
#endif
            if (gm.c_lvl->map[tx][ty].id > 0)
                gm.c_lvl->map[tx][ty].id--;
            else
            {
                Wl_Exp(gm.l_x + tx * gm.t_w, gm.l_y + ty * gm.t_h, gm.m_d);
                gm.c_lvl->map[tx][ty].t = M_FLOOR;
                // check moving direction
                Mr_CkVDir(gm.m_mx, gm.m_my);
                Cr_Cng(gm.o_mx, gm.o_my);
                // reset restore position
                gm.m_o_x = gm.m_tx;
                gm.m_o_y = gm.m_ty;
                gm.m_o_move_count = gm.c_lvl->tm;
#ifdef SOUND
                sound_play(gm.wv_exp);
#endif
            }
            // draw to background
            L_DrwMpTl(tx, ty);
            // draw to screen
            D_DST(sdl.scr, gm.l_x + tx * gm.t_w, gm.l_y + ty * gm.t_h, gm.t_w,
                  gm.t_h);
            D_SRC(gm.s_bkgd, gm.l_x + tx * gm.t_w, gm.l_y + ty * gm.t_h);
            SS_Blt();
            // add refresh rect
            Sdl_AddR(gm.l_x + tx * gm.t_w, gm.l_y + ty * gm.t_h, gm.t_w,
                     gm.t_h);
        }
        // no action
        // gm.m_act = M_EMPTY;
        return;
    }

    // oneway ?
    switch (gm.m_act)
    {
        case M_OW_U:
            y_a = -1;
            ow = 1;
            gm.c_stat = C_U;
            break;
        case M_OW_D:
            y_a = 1;
            ow = 1;
            gm.c_stat = C_D;
            break;
        case M_OW_R:
            x_a = 1;
            ow = 1;
            gm.c_stat = C_R;
            break;
        case M_OW_L:
            x_a = -1;
            ow = 1;
            gm.c_stat = C_L;
            break;
    }
    if (ow)
    {
        /* mx += x_a; my += y_a; while (gm.c_lvl->map[mx][my].m != -1) { mx
         * += x_a; my += y_a; } mx -= x_a; my -= y_a; if (mx != gm.m_mx || my 
         * != gm.m_my) { Mr_Ins(); Mr_Sel(gm.l_x + mx * gm.t_w, gm.l_y + my * 
         * gm.t_h); } */
#ifdef SOUND
        sound_play(gm.wv_arw);
#endif
        Mr_IniMv();
        return;
    }

    // teleport ?
    if (gm.m_act >= M_TLP_0 && gm.m_act <= M_TLP_3)
    {
        for (i = 0; i < gm.c_lvl->m_w; i++)
        {
            for (j = 0; j < gm.c_lvl->m_h; j++)
                if (gm.c_lvl->map[i][j].t == gm.m_act
                    && (i != gm.m_mx || j != gm.m_my))
                {
                    // only warp if destination is empty //
                    if (gm.c_lvl->map[i][j].m == -1)
                    {
                        gm.m_mx = i;
                        gm.m_my = j;
                        gm.m_x = gm.m_mx * gm.t_w + gm.l_x;
                        gm.m_y = gm.m_my * gm.t_h + gm.l_y;
#ifdef SOUND
                        sound_play(gm.wv_tlp);
#endif
                    }
                    gm.c_stat = gm.m_d + 2; // restore c_stat for movement
                                            // initialization
                    // initate movement
                    Mr_IniMv();

                    /* // check if the figure is completed
                     * gm.c_lvl->map[gm.m_mx][gm.m_my].m = gm.m_id;
                     * G_CkFgr(); gm.c_lvl->map[gm.m_mx][gm.m_my].m = -1; */

                    return;
                }
        }
    }

    // gm.m_act = M_EMPTY;
}

/* 
 * restore old position */
void
Mr_ResPos()
{
    int mx, my;
    if (!gm.ms_us)
        return;

    while (gm.m_act != M_EMPTY && gm.m_act != M_CRUMBLE)
        Mr_Upd(10000);
    Mr_Upd(10000);
    Mr_Rel();

    mx = (gm.ms_us->t_x - gm.l_x) / gm.t_w;
    my = (gm.ms_us->t_y - gm.l_y) / gm.t_h;

    Mr_Sel(gm.ms_us->t_x, gm.ms_us->t_y);
    // draw to background
    L_DrwMpTl(mx, my);
    // draw to screen
    D_DST(sdl.scr, gm.l_x + mx * gm.t_w, gm.l_y + my * gm.t_h, gm.t_w,
          gm.t_h);
    D_SRC(gm.s_bkgd, gm.l_x + mx * gm.t_w, gm.l_y + my * gm.t_h);
    SS_Blt();
    // add refresh rect
    Sdl_AddR(gm.l_x + mx * gm.t_w, gm.l_y + my * gm.t_h, gm.t_w, gm.t_h);

    gm.m_act = M_EMPTY;
    gm.m_tx = gm.ms_us->f_x;
    gm.m_ty = gm.ms_us->f_y;
    if (gm.ms_us->c_x != -1)
    {
        int cx = gm.ms_us->c_x;
        int cy = gm.ms_us->c_y;
        if (gm.c_lvl->map[cx][cy].t == M_FLOOR)
            gm.c_lvl->map[cx][cy].t = M_CRUMBLE;
        else
            gm.c_lvl->map[cx][cy].id++;
        // draw to background
        L_DrwMpTl(cx, cy);
        // draw to screen
        D_DST(sdl.scr, gm.l_x + cx * gm.t_w, gm.l_y + cy * gm.t_h, gm.t_w,
              gm.t_h);
        D_SRC(gm.s_bkgd, gm.l_x + cx * gm.t_w, gm.l_y + cy * gm.t_h);
        SS_Blt();
        // add refresh rect
        Sdl_AddR(gm.l_x + cx * gm.t_w, gm.l_y + cy * gm.t_h, gm.t_w, gm.t_h);
    }
    gm.m_x = gm.ms_us->t_x;
    gm.m_y = gm.ms_us->t_y;

    if (gm.c_l_st->limit_type == MOVES)
        gm.c_lvl->tm += gm.ms_us->moves;

    MS_Pop(&gm.ms_us);
    Mr_Stp();
}

// timer //
/* 
 * hide time */
void
Tm_Hd()
{
    int w = gm.brd_w - gm.t_x;
    int h = FT_FnHght(gm.ft_fnt, gm.ft_uis);

    D_DST(sdl.scr, gm.t_x + gm.b_x, gm.t_y, w, h);
    D_SRC(gm.s_bkgd, gm.t_x + gm.b_x, gm.t_y);
    SS_Blt();
    Sdl_AddR(gm.t_x + gm.b_x, gm.t_y, w, h);
}

/* 
 * show time */
void
Tm_Shw()
{
    // SFnt *ft;
    char str_tm[64];
    // char str_sec[4];
    int tm;

    // adjust time //
    if (gm.c_l_st->limit_type == TIME)
        tm = gm.c_lvl->tm / 1000;
    else
        tm = gm.c_lvl->tm;

    // compute time str
    G_Tmpl(str_tm, 63, _("game_fi_lmarbles_main_moves"), tm, 0);

    FT_DrwTxtAlgn(gm.ft_fnt, sdl.scr, gm.b_x + gm.t_x, gm.t_y, FT_ALIGN_Y_T,
                  gm.ft_uis, str_tm, COL_MAIN);

    Sdl_AddR(gm.t_x + gm.b_x, gm.t_y, gm.brd_w - gm.t_x,
             FT_FnHght(gm.ft_fnt, gm.ft_uis));
}

/* 
 * update time return 0 if time out */
int
Tm_Upd(int ms)
{
#ifdef SOUND
    int old_sec = gm.c_lvl->tm / 1000;
#endif

    gm.blink_time += ms;

    // if limit_type is MOVES, time is ignored //
    if (gm.c_l_st->limit_type == MOVES)
        return 1;

    gm.c_lvl->tm -= ms;

    // new second ?
#ifdef SOUND
    if (old_sec != gm.c_lvl->tm / 1000 && old_sec <= 30)
    {
        sound_play(gm.wv_alm);
    }
#endif

    if (gm.c_lvl->tm < 0)
    {
        gm.c_lvl->tm = 0;
        return 0;
    }
    return 1;
}

// level info //
/* 
 * hide level info */
void
Inf_Hd()
{
    D_DST(sdl.scr, gm.b_x, gm.s_y + gm.s_h, gm.brd_w,
          gm.scr_h - gm.s_y - gm.s_h);
    D_SRC(gm.s_bkgd, gm.b_x, gm.s_y + gm.s_h);
    SS_Blt();
    Sdl_AddR(gm.b_x, gm.s_y + gm.s_h, gm.brd_w, gm.scr_h - gm.s_y - gm.s_h);
}

/* 
 * update level info */
int
Inf_Upd()
{
    int x, y;

    x = (gm.o_mx - gm.c_x - gm.b_x) / L_SIZE;
    y = (gm.o_my - gm.c_y) / L_SIZE;

    if (gm.o_mx < gm.c_x + gm.b_x || gm.o_my < gm.c_y || x >= gm.c_l_st->l_num
        || y >= gm.c_l_st->c_num)
    {
        // G_Tmpl(gm.inf_str, 63, _("game_fi_lmarbles_main_chapter_level"),
        // gm.c_ch + 1, gm.c_l_id + 1);
        char *tmpmsg = _("game_fi_lmarbles_main_chapter_level");
        snprintf(gm.inf_str, 63, tmpmsg, gm.c_ch + 1, gm.c_l_id + 1);
        return 0;
    }
    if (!gm.c_s_inf->cmp[y * gm.c_s_inf->l_num + x] && !gm.c_s_inf->c_opn[y])
    {
        return 0;
    }
    // G_Tmpl(gm.inf_str, 63, _("game_fi_lmarbles_main_chapter_level"), y +
    // 1, x + 1);
    char *tmpmsg = _("game_fi_lmarbles_main_chapter_level");
    snprintf(gm.inf_str, 63, tmpmsg, y + 1, x + 1);
    if (gm.bttn[1])
    {
        gm.w_c = y;
        gm.w_l = x;
        return 1;
    }
    return 0;
}

/* 
 * show level info */
void
Inf_Shw()
{
    FT_DrwTxtAlgn(gm.ft_fnt, sdl.scr, gm.b_x + gm.s_x + gm.s_w / 2,
                  gm.s_y + gm.s_h, FT_ALIGN_X_C | FT_ALIGN_Y_T, gm.ft_uis,
                  gm.inf_str, COL_MAIN);
}

// cursor
/* 
 * load cursor */
void
Cr_Ld(char *src, char *d, char *m)
{
    int w = 16, h = 16;
    int i, j, k;
    char b_d, b_m;
    int p;

    k = 0;
    for (j = 0; j < w * h; j += 8, k++)
    {
        p = 1;
        b_d = b_m = 0;
        // create byte
        for (i = 7; i >= 0; i--)
        {
            switch (src[j + i])
            {
                case 2:
                    b_d += p;
                case 1:
                    b_m += p;
                    break;
            }
            p *= 2;
        }
        // add to mask
        d[k] = b_d;
        m[k] = b_m;
    }
}

/* 
 * change cursors appearing */
void
Cr_Cng(int x, int y)
{
    int mx, my, cx, cy;

    if (gm.m_mv)
    {
        if (x > gm.scr_w - gm.brd_w)
            SDL_SetCursor(gm.c_n);
        else
            SDL_SetCursor(gm.c_w);
        return;
    }

    mx = (x - gm.l_x) / gm.t_w;
    my = (y - gm.l_y) / gm.t_h;

    if (mx >= 0 && my >= 0 && mx < gm.c_lvl->m_w && my < gm.c_lvl->m_h
        && (gm.c_lvl->map[mx][my].m != -1
            || (gm.m_mx == mx && gm.m_my == my)))
    {
        // on marble
        SDL_SetCursor(gm.c_s);
        gm.c_stat = C_SEL;
    }
    else if (!gm.m_sel || x > gm.scr_w - gm.brd_w)
    {
        // nothing selected
        SDL_SetCursor(gm.c_n);
        gm.c_stat = C_NONE;
    }
    else
    {
        // up, left, right, down
        cx = x - (gm.m_mx * gm.t_w + gm.l_x + gm.t_w / 2);
        cy = y - (gm.m_my * gm.t_h + gm.l_y + gm.t_h / 2);
        if (abs(cx) > abs(cy))
        {
            if (cx > 0)
            {
                if (gm.m_vd & MD_R)
                {
                    SDL_SetCursor(gm.c_r);
                    gm.c_stat = C_R;
                }
                else
                {
                    SDL_SetCursor(gm.c_n);
                    gm.c_stat = C_NONE;
                }
            }
            else
            {
                if (gm.m_vd & MD_L)
                {
                    SDL_SetCursor(gm.c_l);
                    gm.c_stat = C_L;
                }
                else
                {
                    SDL_SetCursor(gm.c_n);
                    gm.c_stat = C_NONE;
                }
            }
        }
        else
        {
            if (cy > 0)
            {
                if (gm.m_vd & MD_D)
                {
                    SDL_SetCursor(gm.c_d);
                    gm.c_stat = C_D;
                }
                else
                {
                    SDL_SetCursor(gm.c_n);
                    gm.c_stat = C_NONE;
                }
            }
            else
            {
                if (gm.m_vd & MD_U)
                {
                    SDL_SetCursor(gm.c_u);
                    gm.c_stat = C_U;
                }
                else
                {
                    SDL_SetCursor(gm.c_n);
                    gm.c_stat = C_NONE;
                }
            }
        }
    }
}

// frame //
/* 
 * hide marble frame */
void
MF_Hd()
{
    int x, y, w, h;

    if (!gm.m_sel)
        return;

    // get size
    if (gm.msf_a.p == gm.msf_a.f)
    {
        w = gm.mf_a.w;
        h = gm.mf_a.h;
    }
    else
    {
        w = gm.msf_a.w;
        h = gm.msf_a.h;
    }

    // get position
    x = gm.m_x + (gm.t_w - w) / 2;
    y = gm.m_y + (gm.t_h - h) / 2;

    // hide
    D_DST(sdl.scr, x, y, w, h);
    D_SRC(gm.s_bkgd, x, y);
    SS_Blt();
    Sdl_AddR(x - 1, y - 1, w + 1, h + 1);
}

/* 
 * update marble frame */
void
MF_Upd(int ms)
{
    if (!gm.m_sel)
    {
        gm.mf_a.p = gm.msf_a.p = 0;
        return;
    }
    if (gm.msf_a.p != gm.msf_a.f)
    {
        // still select animation
        gm.msf_a.p += gm.msf_a.c * ms;
        if (gm.msf_a.p >= gm.msf_a.f)
            gm.msf_a.p = gm.msf_a.f;
    }
    else
    {
        gm.mf_a.p += gm.mf_a.c * ms;
        if (gm.mf_a.p >= gm.mf_a.f)
            gm.mf_a.p = 0;
    }
}

/* 
 * show marble frame */
void
MF_Shw()
{
    int x, y;
    AInf *a;
    SDL_Surface *s;

    if (!gm.m_sel)
        return;

    // get animation info
    if (gm.msf_a.p == gm.msf_a.f)
    {
        a = &gm.mf_a;
        s = gm.s_mf;
    }
    else
    {
        a = &gm.msf_a;
        s = gm.s_msf;
    }

    // get position
    x = gm.m_x + (gm.t_w - a->w) / 2;
    y = gm.m_y + (gm.t_h - a->h) / 2;

    // show
    D_DST(sdl.scr, x, y, a->w, a->h);
    D_SRC(s, (int) a->p * a->w, 0);
    SS_Blt();
    Sdl_AddR(x, y, a->w, a->h);
}

// map animations //
/* 
 * get position and type of all animations */
void
MA_Ini()
{
    int i, j;

    // free and reallocate m_ani
    if (gm.m_ani)
        free(gm.m_ani);
    gm.m_ani = (MAni *) malloc(sizeof(MAni) * gm.c_lvl->m_w * gm.c_lvl->m_h);
    gm.ma_num = 0;

    // parse level map
    for (i = 0; i < gm.c_lvl->m_w; i++)
        for (j = 0; j < gm.c_lvl->m_h; j++)
            switch (gm.c_lvl->map[i][j].t)
            {
                case M_OW_U:
                case M_OW_D:
                case M_OW_L:
                case M_OW_R:
                case M_OW_U_C:
                case M_OW_D_C:
                case M_OW_L_C:
                case M_OW_R_C:
                    gm.m_ani[gm.ma_num].x = i;
                    gm.m_ani[gm.ma_num].y = j;
                    gm.m_ani[gm.ma_num].t = gm.c_lvl->map[i][j].t;
                    gm.m_ani[gm.ma_num].a = &gm.ma_ow_a;
                    gm.ma_num++;
                    break;
                case M_TLP_0:
                case M_TLP_1:
                case M_TLP_2:
                case M_TLP_3:
                    gm.m_ani[gm.ma_num].x = i;
                    gm.m_ani[gm.ma_num].y = j;
                    gm.m_ani[gm.ma_num].t = gm.c_lvl->map[i][j].t;
                    gm.m_ani[gm.ma_num].a = &gm.ma_tlp_a;
                    gm.ma_num++;
                    break;
                default:
                    break;
            }
}

void
MA_Upd(int ms)
{
    if (!cfg.ani)
        return;

    gm.ma_ow_a.p += (float) ms *gm.ma_ow_a.c;
    if (gm.ma_ow_a.p >= gm.ma_ow_a.f)
        gm.ma_ow_a.p = 0;

    gm.ma_tlp_a.p += (float) ms *gm.ma_tlp_a.c;
    if (gm.ma_tlp_a.p >= gm.ma_tlp_a.f)
        gm.ma_tlp_a.p = 0;
}

/* 
 * show map animations */
void
MA_Shw()
{
    int i;
    int x, y;

    if (!cfg.ani)
        return;

    for (i = 0; i < gm.ma_num; i++)
    {
        // get position in screen
        x = gm.l_x + gm.m_ani[i].x * gm.t_w;
        y = gm.l_y + gm.m_ani[i].y * gm.t_h;

        // draw empty floor
        D_DST(sdl.scr, x, y, gm.t_w, gm.t_h);
        D_SRC(gm.c_g_st->s_flr, 0, 0);
        SS_Blt();

        // oneway
        D_DST(sdl.scr, x, y, gm.t_w, gm.t_h);
        switch (gm.m_ani[i].t)
        {
            case M_OW_R:
            case M_OW_R_C:
                D_SRC(gm.c_g_st->s_r_arw, (int) gm.m_ani[i].a->p * gm.t_w, 0);
                SS_Blt();
                break;
            case M_OW_L:
            case M_OW_L_C:
                D_SRC(gm.c_g_st->s_l_arw, (int) gm.m_ani[i].a->p * gm.t_w, 0);
                SS_Blt();
                break;
            case M_OW_U:
            case M_OW_U_C:
                D_SRC(gm.c_g_st->s_u_arw, 0, (int) gm.m_ani[i].a->p * gm.t_h);
                SS_Blt();
                break;
            case M_OW_D:
            case M_OW_D_C:
                D_SRC(gm.c_g_st->s_d_arw, 0, (int) gm.m_ani[i].a->p * gm.t_h);
                SS_Blt();
                break;
        }
        D_DST(sdl.scr, x, y, gm.t_w, gm.t_h);
        switch (gm.m_ani[i].t)
        {
            case M_OW_U_C:
            case M_OW_D_C:
                D_SRC(gm.c_g_st->s_lr_bar, 0, 0);
                SS_Blt();
                break;
            case M_OW_L_C:
            case M_OW_R_C:
                D_SRC(gm.c_g_st->s_ud_bar, 0, 0);
                SS_Blt();
                break;
        }

        // teleport
        D_DST(sdl.scr, x, y, gm.t_w, gm.t_h);
        switch (gm.m_ani[i].t)
        {
            case M_TLP_0:
                D_SRC(gm.c_g_st->s_tlp_0, (int) gm.m_ani[i].a->p * gm.t_w, 0);
                SS_ABlt(gm.tlp_a);
                break;
            case M_TLP_1:
                D_SRC(gm.c_g_st->s_tlp_1, (int) gm.m_ani[i].a->p * gm.t_w, 0);
                SS_ABlt(gm.tlp_a);
                break;
            case M_TLP_2:
                D_SRC(gm.c_g_st->s_tlp_2, (int) gm.m_ani[i].a->p * gm.t_w, 0);
                SS_ABlt(gm.tlp_a);
                break;
            case M_TLP_3:
                D_SRC(gm.c_g_st->s_tlp_3, (int) gm.m_ani[i].a->p * gm.t_w, 0);
                SS_ABlt(gm.tlp_a);
                break;
        }

        // marble on animation
        if (gm.c_lvl->map[gm.m_ani[i].x][gm.m_ani[i].y].m != -1 &&
            (((gm.blink_time / 250) & 1) || gm.l_done))
        {
            D_DST(sdl.scr, x, y, gm.t_w, gm.t_h);
            D_SRC(gm.s_mrb, 0,
                  gm.c_lvl->map[gm.m_ani[i].x][gm.m_ani[i].y].m * gm.t_w);
            SS_Blt();
        }

        // refresh rect
        Sdl_AddR(x, y, gm.t_w, gm.t_h);
    }
}

// shrapnells //
/* 
 * add new shrapnell */
void
Shr_Add(int x, int y, int w, int h, Vec d, SDL_Surface * s_shr)
{
    Shr *s;

    s = (Shr *) malloc(sizeof(Shr));
    s->d = d;
    s->x = x;
    s->y = y;
    s->w = w;
    s->h = h;
    s->a = 0;
    if (s_shr)
        s->s_shr = s_shr;
    else
    {

        s->s_shr = SS_Crt(w, h, SDL_SURFACE_TYPE);
        D_DST(s->s_shr, 0, 0, w, h);
        D_SRC(sdl.scr, x, y);
        SS_Blt();

    }

    DL_Add(&gm.shr, s);
}

/* 
 * delete shrapnell */
void
Shr_Del(void *p)
{
    Shr *s = (Shr *) p;
    if (s->s_shr)
        SDL_FreeSurface(s->s_shr);
    free(p);
}

/* 
 * hide shrapnell */
void
Shr_Hd()
{
    DL_E *e = gm.shr.hd.n;
    Shr *s;

    while (e != &gm.shr.tl)
    {

        s = (Shr *) e->d;

        D_DST(sdl.scr, (int) s->x, (int) s->y, s->w, s->h);
        D_SRC(gm.s_bkgd, (int) s->x, (int) s->y);
        SS_Blt();
        Sdl_AddR((int) s->x, (int) s->y, s->w, s->h);

        e = e->n;

    }

}

/* 
 * update shrapnell */
void
Shr_Upd(int ms)
{
    DL_E *n, *e = gm.shr.hd.n;
    Shr *s;

    while (e != &gm.shr.tl)
    {

        n = e->n;

        s = (Shr *) e->d;

        s->x += s->d.x * (float) ms;
        s->y += s->d.y * (float) ms;
        s->a += gm.shr_a_c * (float) ms;

        if (s->a >= 255)
            DL_DelE(&gm.shr, e);

        e = n;

    }
}

/* 
 * show shrapnell */
void
Shr_Shw()
{
    DL_E *e = gm.shr.hd.n;
    Shr *s;

    while (e != &gm.shr.tl)
    {

        s = (Shr *) e->d;

        D_DST(sdl.scr, (int) s->x, (int) s->y, s->w, s->h);
        D_SRC(s->s_shr, 0, 0);
        SS_ABlt(s->a);
        Sdl_AddR((int) s->x, (int) s->y, s->w, s->h);

        e = e->n;

    }

}

// wall //
/* 
 * explode crumble wall into lot of pieces */
void
Wl_Exp(int x, int y, int d)
{
    int i, j;
    int sz = 4;
    Vec v;
    int x_r, x_off, y_r, y_off; // direction values
    SDL_Surface *s_shr;

    if (!cfg.ani)
        return;

    x_r = y_r = 200;
    x_off = y_off = 100;

    // adjust direction values
    switch (d)
    {

        case 0:
            y_r = y_off = 100;
            break;

        case 1:
            x_r = 100;
            x_off = 0;
            break;

        case 2:
            y_r = 100;
            y_off = 0;
            break;

        case 3:
            x_r = x_off = 100;
            break;

    }

    for (i = 0; i < gm.t_w; i += sz)
        for (j = 0; j < gm.t_h; j += sz)
        {

            v.x = (float) ((rand() % x_r) - x_off) / 1000;
            v.y = (float) ((rand() % y_r) - y_off) / 1000;

            s_shr = SS_Crt(sz, sz, SDL_SURFACE_TYPE);
            D_DST(s_shr, 0, 0, sz, sz);
            D_SRC(gm.c_g_st->s_crmbl, i, j);
            SS_Blt();

            Shr_Add(x + i, y + j, sz, sz, v, s_shr);

        }
}

// figure animation //
/* 
 * main animation function; select and explode marbles one by one */
int
FA_Run()
{
    int i, j, k;
    int m_cnt = 0;              // marble count
    int m_pos[gm.c_lvl->f_w * gm.c_lvl->f_h][2], b;
    int ms;
    SDL_Event e;
    int leave = 0;
    int tm, c_tm;               // time in ms

    if (!cfg.ani)
        return 0;

    if (gm.m_sel)
        Mr_Ins();

    // count marbles and get position
    for (i = 0; i < gm.c_lvl->m_w; i++)
        for (j = 0; j < gm.c_lvl->m_h; j++)
            if (gm.c_lvl->map[i][j].m != -1)
            {

                m_pos[m_cnt][0] = i;
                m_pos[m_cnt][1] = j;
                m_cnt++;

            }

    // unsort positions
    for (k = 0; k < m_cnt * 5; k++)
    {

        i = rand() % m_cnt;
        j = rand() % m_cnt;

        b = m_pos[i][0];
        m_pos[i][0] = m_pos[j][0];
        m_pos[j][0] = b;
        b = m_pos[i][1];
        m_pos[i][1] = m_pos[j][1];
        m_pos[j][1] = b;

    }

    // explosions
    MF_Hd();
    Tm_Shw();
    Inf_Shw();
    T_Rst();
    tm = 250;
    c_tm = 0;
    m_cnt--;
    SDL_SetCursor(gm.c_w);
    while ((m_cnt >= 0 || gm.shr.cntr > 0) && !trm_gm && !leave)
    {
        gmainloop();
        // termination ?
        if (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_ACTIVEEVENT:
                    if (e.active.gain == 0)
                    {
                        trm_gm = leave = 1;
                        exit_gm = 1;
                        exiting_needed = 1;
                        return 1;
                        // exit_callback(0);
                    }
                    break;
                case SDL_USEREVENT:
                    userevent_handler();
                    break;
                case SDL_QUIT:
                    trm_gm = 1;
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_F5)
                    {
                        exit_gm = 1;
                        leave = 1;
                        exiting_needed = 1;
                        return 1;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                case SDL_KEYUP:
                    if (e.key.keysym.sym == SDLK_TAB)
                    {
                        SnapShot();
                    }
                    else
                    {
                        if ((e.key.keysym.sym == SDLK_ESCAPE)
                            || (e.key.keysym.sym == SDLK_F5)
                            || (e.key.keysym.sym == SDLK_F6))
                        {
                            exit_gm = 1;
                            leave = 1;
                            exiting_needed = 1;
                            return 1;
                        }
                        else
                        {
                            leave = 1;
                        }


                    }
                    break;
            }
        }

        // show shrapnells
        Shr_Hd();
        ms = T_Gt();
        c_tm -= ms;
        MA_Upd(ms);
        Shr_Upd(ms);
        MA_Shw();
        Shr_Shw();
        Sdl_UpdR();

        // add new shrapnells
        if (m_cnt >= 0 && c_tm <= 0)
        {

            FA_Add(m_pos[m_cnt][0],
                   m_pos[m_cnt][1],
                   gm.c_lvl->map[m_pos[m_cnt][0]][m_pos[m_cnt][1]].m);
            c_tm = tm;
            m_cnt--;
#ifdef SOUND
            sound_play(gm.wv_exp);
#endif

        }

    }
    return 0;
}

/* 
 * add shrapnells at x,y with marble-id m */
void
FA_Add(int mx, int my, int m)
{
    int x, y;
    int i, j;
    int sz = 4;                 // size
    SDL_Surface *s_shr;
    Vec v;

    x = gm.l_x + mx * gm.t_w;
    y = gm.l_y + my * gm.t_h;

    // delete from screne
    gm.c_lvl->map[mx][my].m = -1;
    L_DrwMpTl(mx, my);
    Sdl_AddR(x, y, gm.t_w, gm.t_h);

    // create shrapnells
    for (i = 0; i < gm.t_w; i += sz)
        for (j = 0; j < gm.t_h; j += sz)
        {

            v.x = (float) ((rand() % 200) - 100) / 1000;
            v.y = (float) ((rand() % 200) - 100) / 1000;

            s_shr = SS_Crt(sz, sz, SDL_SURFACE_TYPE);
            D_DST(s_shr, 0, 0, sz, sz);
            D_SRC(gm.s_mrb, i, gm.t_h * m + j);
            SS_Blt();

            Shr_Add(x + i, y + j, sz, sz, v, s_shr);

        }
}

// modify score //
void
modify_score(int *b_lvl, int *b_tm)
{
    /* modify score according to difficulty level */
    switch (cfg.diff)
    {
        case DIFF_EASY:
            *b_lvl /= 2;
            *b_tm /= 2;
            break;
        case DIFF_NORMAL:
            break;
        case DIFF_HARD:
            *b_lvl *= 2;
            *b_tm *= 2;
            break;
        case DIFF_BRAINSTORM:
            *b_tm *= 5;
            *b_lvl *= 5;
            break;
    }
}

// bonus summary //
/* 
 * give a bonus summary */
void
BS_Run(float b_lvl, float b_tm)
{
    SDL_Surface *buf;
    SDL_Event e;
    int leave = 0;
    int coff, cy;               // level completed
    int toff, ty;               // time bonus
    int soff, sy;               // score
    int ms;
    int sw = 80, sh = FT_FnHght(gm.ft_fnt, 32); // string width, height
    float b_c = 1.0;            // bonus change
    float scr = gm.c_prf->scr;
    int end_scr;
    int old_scr;

    end_scr = gm.c_prf->scr + (int) b_lvl + (int) b_tm;

    // normal cursor
    SDL_SetCursor(gm.c_n);

    // darken screen
    buf = SS_Crt(gm.scr_w, gm.scr_h, SDL_SURFACE_TYPE);
    SDL_SetColorKey(buf, 0, 0);
    D_FDST(buf);
    SS_Fill(0x0);
    D_FDST(sdl.scr);
    D_FSRC(buf);
    SS_ABlt(128);
    SDL_FreeSurface(buf);
    D_FDST(gm.s_bkgd);
    D_FSRC(sdl.scr);
    SS_Blt();

    // positions
    cy = 200;
    coff = 200;
    ty = 230;
    toff = 200;
    sy = 270;
    soff = 200;

    // info
    FT_DrwTxtAlgn(gm.ft_fnt, sdl.scr, coff, cy, FT_ALIGN_X_L | FT_ALIGN_Y_T,
                  32, _("game_fi_lmarbles_main_level_bonus"), COL_MAIN);
    FT_DrwTxtAlgn(gm.ft_fnt, sdl.scr, toff, ty, FT_ALIGN_X_L | FT_ALIGN_Y_T,
                  32, _("game_fi_lmarbles_main_move_bonus"), COL_MAIN);
    FT_DrwTxtAlgn(gm.ft_fnt, sdl.scr, soff, sy, FT_ALIGN_X_L | FT_ALIGN_Y_T,
                  32, _("game_fi_lmarbles_main_total_score"), COL_MAIN);
    Sdl_FUpd();

    // show bonus first time
    BS_Shw(gm.scr_w - soff, sy, (int) scr);
    Sdl_UpdR();
    SDL_Delay(500);
    BS_Shw(gm.scr_w - coff, cy, (int) b_lvl);
    Sdl_UpdR();
#ifdef SOUND
    sound_play(gm.wv_exp);
#endif
    SDL_Delay(500);
    BS_Shw(gm.scr_w - toff, ty, (int) b_tm);
    Sdl_UpdR();
#ifdef SOUND
    sound_play(gm.wv_exp);
#endif
    SDL_Delay(500);

    T_Rst();
    while (!leave)
    {
        // break?
        gmainloop();
        if (SDL_PollEvent(&e))
            switch (e.type)
            {
                case SDL_USEREVENT:
                    fprintf(stderr, "Got  SDL_USEREVENT\n");
                    userevent_handler();
                    leave = 1;
                    break;
                case SDL_ACTIVEEVENT:
                    fprintf(stderr, "Got  SDL_ACTIVEEVENT\n");
                    if (e.active.gain == 0)
                    {
                        trm_gm = leave = 1;
                        exiting_needed = 1;
                        exit_gm = 1;
                        // exit_callback(0);
                    }
                    break;
                case SDL_QUIT:
                    trm_gm = 1;
                    quit_gm = 1;
                    break;
                case SDL_KEYDOWN:
                    if ((e.key.keysym.sym == SDLK_F6)
                        || (e.key.keysym.sym == SDLK_F5))
                    {
                        trm_gm = 1;
                        exit_gm = 1;

                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                case SDL_KEYUP:
                    leave = 1;
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_TAB:
                        case SDLK_KP_PLUS:
                        case SDLK_KP_MINUS:
                        case SDLK_PLUS:
                        case SDLK_MINUS:
                        case SDLK_F4:
                        case SDLK_ESCAPE:
                            trm_gm = 1;
                            exit_gm = 1;
                            break;
                        default:
                            break;
                    }
                    break;
            }

        // time
        ms = T_Gt();

        // hide
        BS_Hd(gm.scr_w - coff - sw, cy, sw, sh);
        BS_Hd(gm.scr_w - toff - sw, ty, sw, sh);
        BS_Hd(gm.scr_w - soff - sw, sy, sw, sh);

        // update
        old_scr = (int) scr;
        if (b_lvl > 0)
        {

            b_lvl -= b_c * (float) ms;
            scr += b_c * (float) ms;
            if (b_lvl < 0)
                b_lvl = 0;

        }
        if (b_tm > 0)
        {

            b_tm -= b_c * (float) ms;
            scr += b_c * (float) ms;
            if (b_tm < 0)
                b_tm = 0;

        }
        if ((int) scr >= end_scr)
            scr = end_scr;
        if (b_lvl == 0 && b_tm == 0)
            scr = end_scr;
#ifdef SOUND
        if ((old_scr / 50) != (int) scr / 50)
        {
            sound_play(gm.wv_scr);
        }
#endif

        // show
        BS_Shw(gm.scr_w - coff, cy, (int) b_lvl);
        BS_Shw(gm.scr_w - toff, ty, (int) b_tm);
        BS_Shw(gm.scr_w - soff, sy, (int) scr);
        if (trm_gm == 0)
            SDL_Delay(50);

        Sdl_UpdR();
    }
}

/* 
 * hide number */
void
BS_Hd(int x, int y, int w, int h)
{
    D_DST(sdl.scr, x, y, w, h);
    D_SRC(gm.s_bkgd, x, y);
    SS_Blt();
    Sdl_AddR(x, y, w, h);
}

/* 
 * show number */
void
BS_Shw(int x, int y, int v)
{
    char str[10];

    snprintf(str, 9, "%d", v);
    FT_DrwTxtAlgn(gm.ft_fnt, sdl.scr, x, y, FT_ALIGN_X_R | FT_ALIGN_Y_T, 32,
                  str, COL_MAIN);
    Sdl_AddR(x - FT_TxtWdth(gm.ft_fnt, 32, str), y,
             FT_TxtWdth(gm.ft_fnt, 32, str), FT_FnHght(gm.ft_fnt, 32));
}

// snap shot //
/* 
 * take a screenshot */
void
SnapShot()
{
    char filename[32];
#ifdef SOUND
    sound_play(gm.wv_clk);
#endif
    sprintf(filename, "snapshot_%i.bmp", gm.snap++);
    SDL_SaveBMP(sdl.scr, filename);
}

char *
StateDump()
{
    char *state;
    int i = MS_Size(gm.ms_us) * 7 + 4;

    state = (char *) malloc(i--);

    state[0] = gm.c_ch + '0';
    state[1] = gm.c_l_id + '0';
    state[2] = cfg.diff + '0';
    state[i--] = '\0';

    while (gm.ms_us)
    {
        state[i--] = '0' + gm.ms_us->c_y;
        state[i--] = '0' + gm.ms_us->c_x;
        state[i--] = '0' + gm.ms_us->moves;
        state[i--] = '0' + (gm.ms_us->t_y - gm.l_y) / gm.t_h;
        state[i--] = '0' + (gm.ms_us->t_x - gm.l_x) / gm.t_w;
        state[i--] = '0' + (gm.ms_us->f_y - gm.l_y) / gm.t_h;
        state[i--] = '0' + (gm.ms_us->f_x - gm.l_x) / gm.t_w;
        MS_Pop(&gm.ms_us);
    }
    written = TRUE;
    return state;
}

int
StateLoad(char *state)
{
    int t_x, t_y, f_x, f_y, c_x, c_y, moves;

    gm.c_ch = *(state++) - '0';
    gm.c_l_id = *(state++) - '0';
    if (*state)
    {
        cfg.diff = *(state++) - '0';
    }

    L_Ini(gm.c_ch, gm.c_l_id, L_DATA);
    G_CkFgr();

    while (*state)
    {
        f_x = *(state++) - '0';
        f_y = *(state++) - '0';
        t_x = *(state++) - '0';
        t_y = *(state++) - '0';
        moves = *(state++) - '0';
        c_x = *(state++) - '0';
        c_y = *(state++) - '0';

        gm.c_lvl->map[t_x][t_y].m = gm.c_lvl->map[f_x][f_y].m;
        gm.c_lvl->map[f_x][f_y].m = -1;
        if (c_x != -1 && gm.c_lvl->map[c_x][c_y].id > 0)
            gm.c_lvl->map[c_x][c_y].id--;
        else
            gm.c_lvl->map[c_x][c_y].t = M_FLOOR;

        MS_Push(&gm.ms_us, f_x * gm.t_w + gm.l_x, f_y * gm.t_h + gm.l_y,
                t_x * gm.t_w + gm.l_x, t_y * gm.t_h + gm.l_y, moves);
        gm.ms_us->c_x = c_x;
        gm.ms_us->c_y = c_y;
        if (gm.c_l_st->limit_type == MOVES)
            gm.c_lvl->tm -= moves;
    }

    /* Checking if level is completed */
    G_CkFgr();
    L_Ini(gm.c_ch, gm.c_l_id, L_GFX);

    written = FALSE;
    return 1;
}


void
G_Tmpl(char *where, int wherelen, char *what, int val1, int val2)
{
    int tmp;

    if (!what || !where || !wherelen)
        return;

    while (*what && wherelen > 0)
    {
        if (*what == '%')
        {
            what++;
            if (*what == 'n')
            {
                tmp = snprintf(where, wherelen, "%d", val1);
                where += tmp;
                wherelen -= tmp;
            }
            else if (*what == 'm')
            {
                tmp = snprintf(where, wherelen, "%d", val2);
                where += tmp;
                wherelen -= tmp;
            }
            else if (*what == 'd')
            {
                tmp = snprintf(where, wherelen, "%d", val1);
                where += tmp;
                wherelen -= tmp;
            }
            else
            {
                *where = '%';
                where++;
                wherelen--;
            }
        }
        else
        {
            *where = *what;
            where++;
            wherelen--;
        }
        what++;
    }

    *where = 0;
}
