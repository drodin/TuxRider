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

#ifndef UI_THEME_H
#define UI_THEME_H 1

#include "tuxracer.h"

extern colour_t ui_background_colour;
extern colour_t ui_foreground_colour;
extern colour_t ui_highlight_colour;
extern colour_t ui_disabled_colour;
#ifdef __APPLE__
//in TRWC, we want condition button to appear like if it was enabled, but it is disabled
extern colour_t ui_enabled_but_disabled_colour;
#endif
void ui_draw_menu_decorations();

#endif /* UI_THEME_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
