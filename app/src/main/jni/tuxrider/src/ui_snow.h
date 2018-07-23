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

#ifndef _UI_SNOW_H_
#define _UI_SNOW_H_

void init_ui_snow( void );
void update_ui_snow( scalar_t time_step, bool_t windy );
void push_ui_snow( point2d_t pos );
void draw_ui_snow( void );
void make_ui_snow( point2d_t pos );
void reset_ui_snow_cursor_pos( point2d_t pos );

#endif /* _UI_SNOW_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif
