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

#include "tuxracer.h"
#include "keyboard.h"
#include "keyboard_util.h"
#include "loop.h"

#define KEYMAP_SIZE 1000
#define KEYTABLE_SIZE WSK_LAST
#define SPECIAL_KEYTABLE_SIZE WSK_LAST 

static key_cb_t keytable[KEYTABLE_SIZE];
static key_cb_t special_keytable[SPECIAL_KEYTABLE_SIZE];

static keymap_t keymap[KEYMAP_SIZE];
static int num_keymap_entries = 0;

int add_keymap_entry( game_mode_t mode, keymap_class_t keymap_class,
		      char *keys, key_func_t key_func, key_cb_t key_cb )
{
    if ( num_keymap_entries == KEYMAP_SIZE ) {
	return 1; /* keymap full */
    }

    keymap[ num_keymap_entries ].mode = mode;
    keymap[ num_keymap_entries ].keymap_class = keymap_class;
    keymap[ num_keymap_entries ].keys = keys;
    keymap[ num_keymap_entries ].key_func = key_func;
    keymap[ num_keymap_entries ].key_cb = key_cb;

    num_keymap_entries += 1;

    return 0; /* success */
}

static void fill_keytable( key_cb_t value  )
{
    int i;

    for (i=0; i<KEYTABLE_SIZE; i++) {
	keytable[i] = value;
    }

    for (i=0; i<SPECIAL_KEYTABLE_SIZE; i++) {
	special_keytable[i] = value;
    }
}

static int insert_keytable_entries( char *keys, key_cb_t callback )
{
    key_desc_t *key_list;
    int num_keys;
    int i;

    num_keys = translate_key_string( keys, &key_list );

    if ( num_keys > 0 ) {
	for ( i=0; i<num_keys; i++ ) {
	    if ( key_list[i].special ) {
		special_keytable[ key_list[i].key ] = callback;
	    } else {
		keytable[ key_list[i].key ] = callback;
	    }
	} 

	free( key_list );
	return 1;

    } else {
	return 0;
    }
}

static void init_keytable( game_mode_t mode )
{
    int i;
    char *keys;
    fill_keytable( NULL );

    /* Handle default callbacks first */
    for (i=0; i<num_keymap_entries; i++) {
	if ( ( keymap[i].mode == mode || keymap[i].mode == ALL_MODES ) && 
            keymap[i].keymap_class == DEFAULT_CALLBACK ) 
        {
	    fill_keytable( keymap[i].key_cb );
	}
    }

    /* Handle other classes */
    for (i=0; i<num_keymap_entries; i++) {
	if ( keymap[i].mode == mode || keymap[i].mode == ALL_MODES ) {
	    switch ( keymap[i].keymap_class ) {
	    case FIXED_KEY:
		if ( ! insert_keytable_entries( keymap[i].keys, 
						keymap[i].key_cb ) )
		{
		    check_assertion( 0, "failed to insert keytable entries" );
		}

		break;

	    case CONFIGURABLE_KEY:
		check_assertion( keymap[i].key_func != NULL,
				 "No key_func for configurable key" );

		keys = keymap[i].key_func();

		check_assertion( keys != NULL,
				 "key_func returned NULL keys string" );

		if ( ! insert_keytable_entries( keys, keymap[i].key_cb ) )
		{
		    fprintf( stderr, "Tux Racer warning: key specification "
			     "'%s' is unrecognized; using '%s' instead.\n",
			     keys, keymap[i].keys );
		    if ( ! insert_keytable_entries( keymap[i].keys, 
						    keymap[i].key_cb ) )
		    {
			check_assertion( 0, "couldn't insert keytable entry" );
		    }
		}

		break;

	    case DEFAULT_CALLBACK:
		/* handled above */
		break;

            default:
		code_not_reached();
	    }

	}
    }
}

static void keyboard_handler( unsigned int key, bool_t special, 
			      bool_t release, int x, int y )
{
    TRDebugLog("key : %d ; special : %d ; release : %d ; x : %d ; y : %d \n",key,special,release,x,y);
    static game_mode_t last_mode = NO_MODE;
    key_cb_t *table;

    if ( is_mode_change_pending() ) {
	/* Don't process keyboard events until the mode change happens */
	return;
    }

    if ( last_mode != g_game.mode ) {
	last_mode = g_game.mode;
	init_keytable( last_mode );
    }

    if ( special ) {
	table = special_keytable;
    } else {
	table = keytable;
    }

    if ( isalpha( key ) ) {
	key = tolower( key );
    }

    if ( table[key] != NULL ) {
	(table[key])( key, special, release, x, y );
    }
}

void init_keyboard()
{
    winsys_set_keyboard_func( keyboard_handler );
}
