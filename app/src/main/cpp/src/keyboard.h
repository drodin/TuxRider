/* 
 * Tux Racer 
 * Copyright (C) 1999-2001 Jasmin F. Patry
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "tuxracer.h"

typedef enum {
    FIXED_KEY,
    CONFIGURABLE_KEY,
    DEFAULT_CALLBACK
} keymap_class_t;

typedef char* (*key_func_t)();
typedef void (*key_cb_t)( int key, bool_t special, bool_t release, 
			  int x, int y );

typedef struct {
    game_mode_t mode;
    keymap_class_t keymap_class;
    char *keys;
    key_func_t key_func;
    key_cb_t key_cb;
} keymap_t;

#define START_KEYBOARD_CB( name ) \
    void name ( int key, bool_t special, bool_t release, int x, int y ) { \
    player_data_t *plyr = get_player_data( local_player() ); \
    plyr = plyr + 1 - 1; /* to suppress warnings */


#define END_KEYBOARD_CB }

void init_keyboard();
int add_keymap_entry( game_mode_t mode, keymap_class_t keymap_class,
		      char *keys, key_func_t key_func, key_cb_t key_cb );

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
