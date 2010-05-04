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

#ifndef _COURSE_RENDER_H_
#define _COURSE_RENDER_H_

vector_t* get_course_normals();
void reset_course_list();
void calc_normals();
void setup_course_tex_gen();
void setup_course_lighting();
void render_course();
void draw_background(scalar_t fov, scalar_t aspect );
void draw_sky(point_t pos);
void draw_trees() ;
void set_course_clipping( bool_t state );
void set_course_eye_point( point_t pt );
void set_course_fog( bool_t state);
void draw_fog_plane();

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
