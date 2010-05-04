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
#include "keyboard_util.h"

#define WHITESPACE " \t\n\r"

typedef struct {
    char *name;
    key_desc_t desc;
} key_aliases_t;

static key_aliases_t key_aliases[] = {
    { "f1",         { WSK_F1,            True } },
    { "f2",         { WSK_F2,            True } },
    { "f3",         { WSK_F3,            True } },
    { "f4",         { WSK_F4,            True } },
    { "f5",         { WSK_F5,            True } },
    { "f6",         { WSK_F6,            True } },
    { "f7",         { WSK_F7,            True } },
    { "f8",         { WSK_F8,            True } },
    { "f9",         { WSK_F9,            True } },
    { "f10",        { WSK_F10,           True } },
    { "f11",        { WSK_F11,           True } },
    { "f12",        { WSK_F12,           True } },
    { "left",       { WSK_LEFT,          True } },
    { "up",         { WSK_UP,            True } },
    { "right",      { WSK_RIGHT,         True } },
    { "down",       { WSK_DOWN,          True } },
    { "page_up",    { WSK_PAGEUP,        True } },
    { "page_down",  { WSK_PAGEDOWN,      True } },
    { "home",       { WSK_HOME,          True } },
    { "end",        { WSK_END,           True } },
    { "insert",     { WSK_INSERT,        True } },
    { "right_shift",{ WSK_RSHIFT,        True } },
    { "left_shift", { WSK_LSHIFT,        True } },
    { "right_ctrl", { WSK_RCTRL,         True } },
    { "left_ctrl",  { WSK_LCTRL,         True } },
    { "right_alt",  { WSK_RALT,          True } },
    { "left_alt",   { WSK_LALT,          True } },
    { "tab",        { '\t',              False } },
    { "space",      { ' ',               False } },
    { "backspace",  { '\010',            False } },
    { "enter",      { '\n',              False } },
    { "delete",     { '\177',            False } },
    { "escape",     { '\033',            False } }
};

bool_t translate_key( char *name, key_desc_t *desc ) 
{
    int i;
    int key;

    if ( strlen( name ) == 1 ) {
	key = name[0];
	if ( isalpha(key) ) {
	    key = tolower( key );
	}
	desc->key = key;
	desc->special = False;
	return True;
    }

    for (i=0; i<sizeof(key_aliases)/sizeof(key_aliases[0]); i++) {
	if ( string_cmp_no_case( name, key_aliases[i].name ) == 0 ) {
	    desc->key = key_aliases[i].desc.key;
	    desc->special = key_aliases[i].desc.special;
	    return True;
	}
    }

    return False;
}

int translate_key_string( char *keys, key_desc_t **key_list )
{
    char *keys_copy;
    char *key_ptr;
    int max_keys;
    int num_keys = 0;

    max_keys = strlen( keys );

    keys_copy = string_copy( keys );

    if ( strtok( keys_copy, WHITESPACE ) == NULL ) {
	free( keys_copy );
	return 0;
    } else {
	*key_list = (key_desc_t *) malloc( sizeof( key_desc_t ) * max_keys );

	if ( translate_key( keys_copy, *key_list ) ) {
	    num_keys++;
	}

	while ( ( key_ptr = strtok( NULL, WHITESPACE ) ) != NULL ) {
	    if ( translate_key( key_ptr, &(*key_list)[num_keys] ) ) {
		num_keys++;
	    } else {
		fprintf( stderr, "Tux Racer warning: Unrecognized key '%s'\n",
			 key_ptr );
	    }
	}

	free( keys_copy );

	if ( num_keys == 0 ) {
	    free( *key_list );
	    *key_list = NULL;
	}

	return num_keys;
    }
}

