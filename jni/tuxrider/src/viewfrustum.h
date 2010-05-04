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

#ifndef _VIEWFRUSTUM_H_
#define _VIEWFRUSTUM_H_

#include "tuxracer.h"

typedef enum {
    NoClip,
    SomeClip,
    NotVisible
} clip_result_t;

void setup_view_frustum( player_data_t *plyr, scalar_t near, scalar_t far );

clip_result_t clip_aabb_to_view_frustum( point_t min, point_t max );

plane_t get_far_clip_plane();
plane_t get_left_clip_plane();
plane_t get_right_clip_plane();
plane_t get_bottom_clip_plane();
 
#endif /* _VIEWFRUSTUM_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif
