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

#ifndef UI_MGR_H
#define UI_MGR_H 1

#include "tuxracer.h"

typedef void (*mouse_motion_event_cb_t)( void *widget, int x, int y );
typedef void (*mouse_button_event_cb_t)( void *widget, 
					 winsys_mouse_button_t button,
					 int x, int y );
typedef void (*widget_draw_cb_t)( void *widget );

void init_ui_manager();

void ui_add_mouse_motion_callback( void* widget, mouse_motion_event_cb_t cb );
void ui_delete_mouse_motion_callback( void* widget );
void ui_add_mouse_down_callback( void* widget, mouse_button_event_cb_t cb );
void ui_delete_mouse_down_callback( void* widget );
void ui_add_mouse_up_callback( void* widget, mouse_button_event_cb_t cb );
void ui_delete_mouse_up_callback( void* widget );
void ui_add_widget_draw_callback( void* widget, widget_draw_cb_t cb );
void ui_delete_widget_draw_callback( void* widget );
void ui_set_dirty();
void ui_check_dirty();
void ui_setup_display();
void ui_draw();
point2d_t ui_get_mouse_position( void );

/* GLUT callbacks */
void ui_event_mouse_func( int button, int state, int x, int y );
void ui_event_motion_func( int x, int y );

#endif /* UI_MGR_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
