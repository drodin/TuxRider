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

#ifndef BUTTON_H
#define BUTTON_H 1

#include "tuxracer.h"

typedef struct button_ button_t; /* Opaque */
typedef void (*button_click_event_cb_t)(button_t *button, void *userdata);

button_t* button_create( point2d_t pos, scalar_t w, scalar_t h,
			 char *font_binding, char *label );
void button_delete( button_t *button );
void button_set_click_event_cb( button_t *button, button_click_event_cb_t cb,
				void *userdata );
void button_set_hilit_font_binding( button_t *button, char *binding );
void button_set_disabled_font_binding( button_t *button, char *binding );
void button_set_image( button_t *button, char *texture_binding,
		       point2d_t p0, point2d_t p1, colour_t colour );
void button_set_hilit_image( button_t *button, char *texture_binding,
			     point2d_t p0, point2d_t p1, colour_t colour );
void button_set_clicked_image( button_t *button, char *texture_binding,
			       point2d_t p0, point2d_t p1, colour_t colour );
void button_set_disabled_image( button_t *button, char *texture_binding,
				point2d_t p0, point2d_t p1, colour_t colour_t );
void button_set_position( button_t *button, point2d_t pos );
scalar_t button_get_width( button_t *button );
scalar_t button_get_height( button_t *button );
void button_set_enabled( button_t *button, bool_t enabled );
bool_t button_is_enabled( button_t *button );
void button_set_visible( button_t *button, bool_t visible );
bool_t button_is_visible( button_t *button );
void button_set_active( button_t *button, bool_t active );
bool_t button_is_active( button_t *button );
void button_draw( button_t *button );
void button_set_highlight( button_t *button, bool_t highlight );
void button_simulate_mouse_click( button_t *button );
void button_perform_click_action( button_t *button );

/* EOF */


#endif /* BUTTON_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
