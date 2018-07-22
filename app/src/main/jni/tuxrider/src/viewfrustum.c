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
#include "viewfrustum.h"

static plane_t frustum_planes[6];

/* This will be used as a bitfield to select the "n" and "p" vertices
of the bounding boxes wrt to each plane.  
*/

static char p_vertex_code[6];

void setup_view_frustum( player_data_t *plyr, scalar_t near_dist, 
			 scalar_t far_dist )
{
    scalar_t aspect = (scalar_t) getparam_x_resolution() /
	getparam_y_resolution();

    int i;
    point_t pt;
    point_t origin = { 0., 0., 0. };
    scalar_t half_fov = ANGLES_TO_RADIANS( getparam_fov() * 0.5 );
    scalar_t half_fov_horiz = atan( tan( half_fov ) * aspect ); 


    /* create frustum in viewing coordinates */

    /* near */
    frustum_planes[0] = make_plane( 0, 0, 1, near_dist );
    
    /* far */
    frustum_planes[1] = make_plane( 0, 0, -1, -far_dist );

    /* left */
    frustum_planes[2] = make_plane( -cos(half_fov_horiz), 0, 
				    sin(half_fov_horiz), 0 );

    /* right */
    frustum_planes[3] = make_plane( cos(half_fov_horiz), 0, 
				    sin(half_fov_horiz), 0 );

    /* top */
    frustum_planes[4] = make_plane( 0, cos(half_fov),  
				    sin(half_fov), 0 );

    /* bottom */
    frustum_planes[5] = make_plane( 0, -cos(half_fov),  
				    sin(half_fov), 0 );


    /* We now transform frustum to world coordinates */
    for (i=0; i<6; i++) {
	pt = transform_point( 
	    plyr->view.inv_view_mat, 
	    move_point( origin, scale_vector( 
		-frustum_planes[i].d, frustum_planes[i].nml ) ) );

	frustum_planes[i].nml = transform_vector( 
	    plyr->view.inv_view_mat, frustum_planes[i].nml );

	frustum_planes[i].d = -dot_product( 
	    frustum_planes[i].nml,
	    subtract_points( pt, origin ) );
    }

    for (i=0; i<6; i++) {
	p_vertex_code[i] = 0;

	if ( frustum_planes[i].nml.x > 0 ) {
	    p_vertex_code[i] |= 4;
	}
	if ( frustum_planes[i].nml.y > 0 ) {
	    p_vertex_code[i] |= 2;
	}
	if ( frustum_planes[i].nml.z > 0 ) {
	    p_vertex_code[i] |= 1;
	}
    }
}

/* View frustum clipping for AABB (axis-aligned bounding box). See
   Assarsson, Ulf and Tomas M\"oller, "Optimized View Frustum Culling
   Algorithms", unpublished, http://www.ce.chalmers.se/staff/uffe/ .  */
clip_result_t clip_aabb_to_view_frustum( point_t min, point_t max )
{
    vector_t n, p;
    int i;
    bool_t intersect = False;

    for (i=0; i<6; i++) {
	p = make_vector( min.x, min.y, min.z );
	n = make_vector( max.x, max.y, max.z );

	if ( p_vertex_code[i] & 4 ) {
	    p.x = max.x;
	    n.x = min.x;
	}

	if ( p_vertex_code[i] & 2 ) {
	    p.y = max.y;
	    n.y = min.y;
	}

	if ( p_vertex_code[i] & 1 ) {
	    p.z = max.z;
	    n.z = min.z;
	}

	if ( dot_product( n, frustum_planes[i].nml ) +
	     frustum_planes[i].d > 0 )
	{
	    return NotVisible;
	}

	if ( dot_product( p, frustum_planes[i].nml ) +
	     frustum_planes[i].d > 0 )
	{
	    intersect = True;
	}

    }

    if ( intersect ) {
	return SomeClip;
    }

    return NoClip;
}

plane_t get_far_clip_plane()
{
    return frustum_planes[1];
}

plane_t get_left_clip_plane()
{
    return frustum_planes[2];
}

plane_t get_right_clip_plane()
{
    return frustum_planes[3];
}

plane_t get_bottom_clip_plane()
{
    return frustum_planes[5];
}
