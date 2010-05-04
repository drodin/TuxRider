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

#ifndef SSBUTTON_H
#define SSBUTTON_H 1

#include "tuxracer.h"

typedef struct ssbutton_ ssbutton_t;

void ssbutton_draw( ssbutton_t *ssbutton );
ssbutton_t* ssbutton_create( point2d_t pos, scalar_t w, scalar_t h, 
			     int num_states );
void ssbutton_delete( ssbutton_t *ssbutton );
int ssbutton_get_state( ssbutton_t *ssbutton );
void ssbutton_set_state( ssbutton_t *ssbutton, int state );
void ssbutton_set_state_image( ssbutton_t *ssbutton, int state,
			       char *binding,
			       point2d_t ll, point2d_t ur,
			       colour_t colour );
void ssbutton_set_hilit_image( ssbutton_t *ssbutton, 
			       char *binding, 
			       point2d_t ll, point2d_t ur, 
			       colour_t colour );
void ssbutton_set_position( ssbutton_t *ssbutton, point2d_t pos );
scalar_t ssbutton_get_width( ssbutton_t *ssbutton );
scalar_t ssbutton_get_height( ssbutton_t *ssbutton );
void ssbutton_set_enabled( ssbutton_t *ssbutton, bool_t enabled );
bool_t ssbutton_is_enabled( ssbutton_t *ssbutton );
void ssbutton_set_visible( ssbutton_t *ssbutton, bool_t visible );
bool_t ssbutton_is_visible( ssbutton_t *ssbutton );
void ssbutton_set_active( ssbutton_t *ssbutton, bool_t active );
bool_t ssbutton_is_active( ssbutton_t *ssbutton );
void ssbutton_simulate_mouse_click( ssbutton_t *ssbutton );

/* EOF */


#endif /* SSBUTTON_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
