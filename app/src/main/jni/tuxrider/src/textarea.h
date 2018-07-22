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

#ifndef TEXTAREA_H
#define TEXTAREA_H 1

#include "tuxracer.h"

typedef struct textarea_ textarea_t; /* Opaque */

void textarea_draw( textarea_t *ta );
textarea_t* textarea_create( point2d_t pos, scalar_t w, scalar_t h,
			     char *font_binding, char *text );
void textarea_delete( textarea_t *ta );
void textarea_set_text( textarea_t *ta, char *text );
void textarea_set_position( textarea_t *ta, point2d_t pos );
int textarea_get_height( textarea_t *ta );
int textarea_get_width( textarea_t *ta );
void textarea_set_visible( textarea_t *ta, bool_t visible );
bool_t textarea_get_visible( textarea_t *ta );
void textarea_set_active( textarea_t *ta, bool_t active );
bool_t textarea_get_active( textarea_t *ta );

#endif /* TEXTAREA_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
