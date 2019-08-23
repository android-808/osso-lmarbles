/**
    @file callbacks.h

    Osso Lmarbles callback definitions.

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

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <SDL/SDL.h>


/* #define PAUSE_WITH_HW_KEY "/apps/osso/games_startup/pause_with_hw_key" */

int exit_callback(int errcode);
int quit_callback(int errcode);
int flush_callback(int errcode);

/* We don't need timeouts?
Uint32 app_escape_timeout(Uint32 interval, void *param);
void app_remove_timeout(void);
*/

/* 
 * Call this function in time consuming loops, because we want to run osso
 * (dbus) for hw-events. */
void gmainloop(void);

#endif /* CALLBACKS_H */
