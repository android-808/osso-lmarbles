/**
    @file main.c

    Osso Lmarbles main.

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

#include "engine_main.h"
#include "callbacks.h"
#include "sdl.h"
#include "i18n.h"

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
#include <osso-log.h>
#include <libosso.h>
#include <signal.h>

/* Ugly global. */
HgwContext *sapp_context = NULL;
osso_context_t *osso;
int home_pressed = 0;

static void sig_handler(int signum);
void osso_hw_state(osso_hw_state_t * state, gpointer data);
void display_event_handler(osso_display_state_t state, gpointer data);

static void
sig_handler(int signum)
{
    if (signum == SIGTERM)
    {
        exit_callback(0);
    }
}

void
osso_hw_state(osso_hw_state_t * state, gpointer data)
{
    (void) data;
    static SDL_Event ev;
    if (state->memory_low_ind)
    {
        /* Memory low */
        quit_callback(1);
    }
    else if (state->save_unsaved_data_ind || state->system_inactivity_ind)
    {
        /* Battery low or system inactive */
        ev.type = SDL_USEREVENT;
        SDL_PushEvent(&ev);
    }
}

void display_event_handler(osso_display_state_t state, gpointer data)
{
	static SDL_Event ev;

	(void) data;

	if(OSSO_DISPLAY_ON == state) {
		/* Do Nothing */
	}
	else if( (OSSO_DISPLAY_OFF == state) || (OSSO_DISPLAY_DIMMED == state) ) {
		ev.type = SDL_USEREVENT;
		SDL_PushEvent(&ev);
	}

	return;
}

/**
  Application main.
  Initializes internationalization, liboss, libosso-wrapper and SDL.
  Calls user interface creation functions.

  @param argc Number of command line arguments
  @param argv Command line arguments
   @return 0 if successful; otherwise error code
*/
int
main(int argc, char *argv[])
{
    FILE *dFile;
    time_t rawtime;
    struct tm *timeinfo;
    char *printable;
    osso_hw_state_t hw_state;

    dFile = fopen("/tmp/.gamewrapper/lmarbles.debug", "a");
    if (dFile)
    {
        fprintf(dFile, "lmarbles launched!\n");
    }

    ULOG_OPEN(PACKAGE_NAME " " PACKAGE_VERSION);

    osso = osso_initialize(PACKAGE_NAME, PACKAGE_VERSION, FALSE, NULL);

    if (osso == NULL)
    {
        if (dFile)
            fprintf(dFile, "osso initialize failed!\n");
        return -1;
    }

    if (osso_hw_set_event_cb(osso, NULL, osso_hw_state, NULL) != OSSO_OK)
    {
        if (dFile)
            fprintf(dFile, "osso_hw_set_event_cb failes!\n");
        return -1;
    }
    if (osso_hw_set_display_event_cb(osso, display_event_handler, NULL) != OSSO_OK)
    {
        if (dFile)
            fprintf(dFile, "osso_hw_set_display_event_cb failes!\n");
        return -1;
    }
    if (dFile)
        fclose(dFile);

    // if startup kills us, we want to save the state
    signal(SIGTERM, sig_handler);

    hw_state.shutdown_ind = FALSE;
    hw_state.save_unsaved_data_ind = FALSE;
    hw_state.memory_low_ind = FALSE;
    hw_state.system_inactivity_ind = FALSE;
    hw_state.sig_device_mode_ind = FALSE;

#ifdef ENABLE_NLS
    /* Initialize localization */
    /* Getttext does not seem to work properly without the following function 
     * call */
    setlocale(LC_ALL, "");

    bindtextdomain(GETTEXT_PACKAGE, LMARBLESLOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif /* ENABLE_NLS */
#if SAPP_FUNC
    sapp_context = hgw_context_compat_init(argc, argv);
    if (sapp_context == NULL)
    {
        dFile = fopen("/tmp/.gamewrapper/debug/lmarbles.debug", "a");
        if (dFile)
        {
            rawtime = time(NULL);
            timeinfo = localtime(&rawtime);
            printable = asctime(timeinfo);
            fprintf(dFile, "%s\tsapp initiliazation failed\n", printable);
            fclose(dFile);
        }
        return 0;
    }
    hgw_compat_set_cb_exit(sapp_context, exit_callback);
    hgw_compat_set_cb_quit(sapp_context, quit_callback);
    hgw_compat_set_cb_flush(sapp_context, flush_callback);
    if (!hgw_context_compat_check(sapp_context))
    {
        return 0;
    }

    hgw_msg_compat_receive(sapp_context, 0);

#endif
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        ULOG_ERR("cannot init SDL\n");
        osso_deinitialize(osso);
        return -1;
    }

    if (SDL_ShowCursor(SDL_DISABLE) != SDL_DISABLE)
    {
        ULOG_ERR("Can not disable cursor\n");

    }
#if SAPP_FUNC
    /* Shadow app part */
    hgw_msg_compat_receive(sapp_context, 0);
    usleep(100);
#endif
    lmarbles(argc, argv);
    /* Free memory */
#if SAPP_FUNC
    hgw_context_compat_destroy_deinit(sapp_context);
#endif
    osso_deinitialize(osso);
    /* if (home_pressed) { sleep(1); } */
    /* Exit successfullt, regardless of any errors */
    return 0;

}

/* EOF */
