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

#ifndef LISTBOX_H
#define LISTBOX_H 1

#include "tuxracer.h"
#include "list.h"

typedef struct listbox_ listbox_t; /* Opaque */

typedef char* (*listbox_list_elem_to_string_fptr_t)( list_elem_data_t elem );
typedef void (*listbox_item_change_event_cb_t)( listbox_t *listbox, void *userdata );

char * listbox_default_list_elem_to_string_func( list_elem_data_t data );
listbox_t* listbox_create( point2d_t pos, scalar_t w, scalar_t h, 
			   char *font_binding, list_t item_list,
			   listbox_list_elem_to_string_fptr_t func );
void listbox_delete( listbox_t *listbox );
void listbox_set_item_change_event_cb( listbox_t *listbox, 
				       listbox_item_change_event_cb_t cb,
				       void *userdata );
void listbox_set_item_list( listbox_t *listbox, 
			    list_t item_list,
			    listbox_list_elem_to_string_fptr_t func );
list_elem_t listbox_get_current_item( listbox_t *listbox );
void listbox_set_current_item( listbox_t *listbox, list_elem_t elem);
bool_t listbox_goto_next_item( listbox_t *listbox );
bool_t listbox_goto_prev_item( listbox_t *listbox );
void listbox_set_position( listbox_t *listbox, point2d_t pos );
scalar_t listbox_get_width( listbox_t *listbox );
scalar_t listbox_get_height( listbox_t *listbox );
void listbox_set_visible( listbox_t *listbox, bool_t visible );
bool_t listbox_is_visible( listbox_t *listbox );
void listbox_set_active( listbox_t *listbox, bool_t active );
bool_t listbox_is_active( listbox_t *listbox );
void listbox_draw( listbox_t *listbox );

/* EOF */


#endif /* LISTBOX_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
