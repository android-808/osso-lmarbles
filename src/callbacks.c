/**
   @file callbacks.c

   Osso Lmarbles callback functions.

    Copyright (c) 2004, 2005 Nokia Corporation.
	
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#include "callbacks.h"
#include "game.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <SDL/SDL.h>
#include <hgw/hgw.h>
#include <libintl.h>
#include <locale.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

extern SDL_TimerID escape_timeout;

extern int trm_gm;
extern HgwContext *sapp_context;
extern int exiting_needed;

extern gboolean written;

int
exit_callback(int errcode)
{
    FILE *pFile;
    exiting_needed = 1;

    gchar *file = g_build_filename( g_get_home_dir(), ".lmarbles_state",
		NULL);

    if(!written) {

    pFile = g_fopen(file, "wb");
    if (pFile)
    {
        char *state = StateDump();
        fprintf(pFile, "%s", state);
        free(state);
        fclose(pFile);
    } else {
    	g_warning( "exit_callback(%d), Couldn't open state file for write",
    		errcode );
    }
    
    g_free(file);
    }

    trm_gm = 1;

    return 0;
}

int
quit_callback(int errcode)
{
    FILE *dFile;
    char buf[1024];
    errcode = errcode;

    gchar *file = g_build_filename( g_get_home_dir(), ".lmarbles_state",
		NULL);
    
    if(!written) {
    dFile = g_fopen(file, "r");
    if (dFile)
    {
        buf[fread(buf, 1, 1023, dFile)] = '\0';
        fclose(dFile);
        buf[2] = '\0';
        dFile = g_fopen(file, "wb");
        if (dFile)
        {
            fprintf(dFile, "%s", buf);
            fclose(dFile);
        }
    } else {
    	g_warning( "Couldn't open state file for read" );    
    }
    }

    trm_gm = 1;

    return 0;
}

int
flush_callback(int errcode)
{
	FILE *dFile;
    errcode = errcode;
    char buf[1024];
    
    /* No need for this
    dFile = fopen("/tmp/sappdebug.txt", "a");    
    if (dFile)
    {
        fprintf(dFile, "flush callback called with exit code %d\n", errcode);
        fclose(dFile);
    }
    */

    gchar *file = g_build_filename(g_get_home_dir(), ".lmarbles_state",
		NULL);

    dFile = g_fopen(file, "r");
    if (dFile)
    {
        buf[fread(buf, 1, 1023, dFile)] = '\0';
        fclose(dFile);
        buf[2] = '\0';
        dFile = g_fopen(file, "wb");
        if (dFile)
        {
            fprintf(dFile, "%s", buf);
            fclose(dFile);
        } else {
        	g_warning( "Can't open state file to write" );
        }
    } else {
		g_warning( "Can't open state file to read" );
	}    
    

    return 0;
}

/* Shouldn't be used anymore
Uint32
app_escape_timeout(Uint32 interval, void *param)
{
    (void) param;

    static SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);

    app_remove_timeout();

    return interval;
}

void
app_remove_timeout(void)
{
    if (escape_timeout != NULL)
    {
        SDL_RemoveTimer(escape_timeout);
        escape_timeout = NULL;
    }
}
*/

/* 
 * Call this function in time consuming loops, because we want to run osso
 * (dbus) for hw-events. */
void
gmainloop(void)
{
    while (g_main_context_iteration(g_main_context_default(), FALSE)) ;
}
