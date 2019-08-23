/***************************************************************************
                          game.h  -  description
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

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libintl.h>
#include "levels.h"
#include "dynlist.h"
#include "profile.h"
#include "sdl.h"
#include "ft.h"
#include "audio.h"
#include "stack.h"
#include "i18n.h"


// gfx set //
typedef struct {
    char nm[16];                // name
    int ok;                     // can be used
      SDL_Surface * s_bkgd,     // background
     *s_wl,                     // walls
     *s_flr,                    // floor
     *s_r_arw, *s_l_arw, *s_u_arw, *s_d_arw,    // arrows
     *s_ud_bar, *s_lr_bar,      // barriers
     *s_tlp_0, *s_tlp_1, *s_tlp_2, *s_tlp_3,    // teleporters
     *s_crmbl;                  // crumbling wall
} GSet;

// animation information //
typedef struct {
    int f;                      // frame limit //
    float c;                    // change per millisecond //
    float p;                    // current position //
    int w, h;                   // size of one sprite //
} AInf;

// map animation //
typedef struct {
    int x, y;                   // position in map
    int t;                      // type
    AInf *a;                    // animation info
} MAni;

// position
typedef struct {
    int x, y;
} Pos;

// vector
typedef struct {
    float x, y;
} Vec;

// shrapnell
typedef struct {
    SDL_Surface *s_shr;         // picture;
    Vec d;                      // direction
    float x, y;                 // position
    int w, h;                   // size
    float a;                    // alpha
} Shr;

// cursor states //
#define C_NONE  0
#define C_SEL   1
#define C_U     2
#define C_R     3
#define C_D     4
#define C_L     5

// valid marble directions //
#define MD_L    (1<<0)
#define MD_R    (1<<1)
#define MD_U    (1<<2)
#define MD_D    (1<<3)

#define L_GREEN     0
#define L_ORANGE    1
#define L_RED       2
#define L_WHITE     3
#define L_SIZE      32

#define COL_MAIN 0xff, 0xc8, 0x33
#define COL_DLG 0xff, 0xff, 0xff
typedef struct {
    // gfx sets //
    DLst g_sts;                 // gfx sets //
    // mouse button state //
    int bttn[4];
    // graphics //
    SDL_Surface *s_lghts;       // lamps displaying level progress //
    SDL_Surface *s_brd;         // side board //
    SDL_Surface *s_undo;        // undo button //
    SDL_Surface *s_undop;       // pressed undo button //
    SDL_Surface *s_undod;       // dimmed undo button //
    SDL_Surface *s_mrb;         // marbles //
    SDL_Surface *s_fig;         // figure //
    SDL_Surface *s_bkgd;        // background //
    SDL_Surface *s_mf;          // marble frame //
    SDL_Surface *s_msf;         // marble select frame //
    SDL_Surface *s_ret;         // return //

    SDL_Surface *s_dtr;
    SDL_Surface *s_dtl;
    SDL_Surface *s_dbr;
    SDL_Surface *s_dbl;
    SDL_Surface *s_dbo;
    SDL_Surface *s_dle;
    SDL_Surface *s_drg;
    SDL_Surface *s_dto;
    SDL_Surface *s_dyes;
    SDL_Surface *s_dno;
    SDL_Surface *s_dbg;

    struct MoveStack *ms_us;    // undo stack //

    // fonts //
    FT_Face ft_fnt;             // Freetype Face //
    FT_Library ft_lib;          // Freetype Library //
    int ft_uis;
    // references //
    Prf *c_prf;                 // current profile //
    SInf *c_s_inf;              // current profile level set info //
    LSet *c_l_st;               // current level set //
    GSet *c_g_st;               // current gfx set //
    Lvl *c_lvl;                 // current level //
    // player //
    int hi_scr;                 // last hiscore //
    int c_l_id;                 // current level //
    int c_ch;                   // current chapter //
    int o_ch;                   // old chapter //
    int l_done;                 // level finished //
    // marble //
    float m_v;                  // marble velocity per millisecond //
    int m_sel;                  // selected a marble ? //
    int m_mx, m_my;             // position in map //
    int m_o_x, m_o_y;           // old position //
    struct MoveStack *m_ms;
    int m_o_move_count;         // old move count //
    float m_x, m_y;             // position in screen //
    int m_mv;                   // moving ? //
    int m_d;                    // direction of moving //
    int m_vd;                   // valid directions //
    int m_tx, m_ty;             // target position in screen //
    int m_id;                   // current type of marble //
    AInf m_a;                   // animation information //
    int m_act;                  // action type //
    int m_warp;                 // warp marble to destination //
    // marble frame and marble select frame
    AInf mf_a;                  // animation info
    AInf msf_a;                 // animation info
    // layout //
    int l_x, l_y;               // offset of level relative to 0,0 -- in
                                // L_Ini()
    int f_x, f_y;               // figure offset relative to b_x, 0
    int b_x;                    // board x
    int i_x, i_y;               // info position relative to b_x,0
    int t_x, t_y;               // timer position relative to b_x,0
    int s_x, s_y;               // set info position relative to b_x,0
    int c_x, c_y;               // position of first chapter info relative to 
                                // b_x,0 -- in L_Ini()
    // geometry //
    int t_w, t_h;               // tile size //
    int f_w, f_h;               // figure marble size //
    int i_w, i_h;
    int f_fw, f_fh;             // figure frame size //
    int scr_w, scr_h;           // screen size //
    int brd_w;                  // board width
    int s_w, s_h;               // size of set info
    int c_off;                  // offset between chapter infos //
    // cursors //
    int c_stat;                 // cursor state //
    SDL_Cursor *c_u, *c_d, *c_l, *c_r, *c_s, *c_n, *c_w;    // cursors
    // old mouse pos //
    int o_mx, o_my;
    // level warp //
    char inf_str[64];           // warp info string
    int w_c, w_l;               // warp level and chapter
    // map animations //
    MAni *m_ani;                // animations
    int ma_num;                 // animation number
    AInf ma_ow_a;               // oneway info
    AInf ma_tlp_a;              // teleport info
    int tlp_a;                  // telporter alpha
    // sounds
#ifdef SOUND
    Sound_Chunk *wv_tlp;        // teleport
    Sound_Chunk *wv_sel;        // select
    Sound_Chunk *wv_stp;        // stop
    Sound_Chunk *wv_clk;        // click
    Sound_Chunk *wv_exp;        // explode
    Sound_Chunk *wv_alm;        // alarm
    Sound_Chunk *wv_arw;        // arrow
    Sound_Chunk *wv_scr;        // score
#endif
    DLst shr;                   // shrapnells
    float shr_a_c;              // alpha change per second
    // snapshot
    int snap;
    // blink time //
    int blink_time;
} Game;

void G_Ini(void);
void G_Trm(void);
int G_Opn(void);
void G_Cls(void);
void G_Run(void);
void G_LdGSts(void);
void G_DelGSt(void *p);
void G_Ps(void);
void G_CkFgr(void);
int G_CfmWrp(void);
int G_DrwDlg(int fontsize, char *question,
             int fontsize2, char *question2, char *but1, char *but2);
int G_CfmQut(void);
int G_CfmRst(void);
int G_CkLSt(void);

// marble //
void Mr_Hd(void);
void Mr_Shw(void);
int Mr_Upd(int ms);
void Mr_Sel(int x, int y);
// void Mr_Rel(int x, int y);
void Mr_Rel(void);
void Mr_IniMv(void);
void Mr_Stp(void);
void Mr_Ins(void);
void Mr_CkVDir(int mx, int my);
void Mr_Act(void);
void Mr_ResPos(void);

// timer //
void Tm_Hd(void);
void Tm_Shw(void);
int Tm_Upd(int ms);

// level info //
void Inf_Hd(void);
int Inf_Upd(void);
void Inf_Shw(void);

// cursor //
void Cr_Ld(char *src, char *d, char *m);
void Cr_Cng(int x, int y);

// frame //
void MF_Hd(void);
void MF_Upd(int ms);
void MF_Shw(void);

// map animations //
void MA_Ini(void);
void MA_Upd(int ms);
void MA_Shw(void);

// shrapnells //
void Shr_Add(int x, int y, int w, int h, Vec d, SDL_Surface * s_shr);
void Shr_Del(void *p);
void Shr_Hd(void);
void Shr_Upd(int ms);
void Shr_Shw(void);

// wall //
void Wl_Exp(int x, int y, int v);

// figure animation //
int FA_Run(void);
void FA_Add(int mx, int my, int m);

// bonus summary //
void BS_Run(float b_lvl, float b_tm);
void BS_Hd(int x, int y, int w, int h);
void BS_Shw(int x, int y, int v);

// shnapshot //
void SnapShot(void);

char *StateDump(void);
int StateLoad(char *state);

int G_Cngrt(void);

void G_Tmpl(char *where, int wherelen, char *what, int val1, int val2);

#endif
