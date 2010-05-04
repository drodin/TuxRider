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

#ifndef _HIER_UTIL_H
#define _HIER_UTIL_H

void draw_sphere( int num_divisions );

void traverse_dag( scene_node_t *node, material_t *mat );

vector_t make_normal( polygon_t p, point_t *v );

bool_t intersect_polygon( polygon_t p, point_t *v );

bool_t intersect_polyhedron( polyhedron_t p );

polyhedron_t copy_polyhedron( polyhedron_t ph );

void free_polyhedron( polyhedron_t ph ) ;

void trans_polyhedron( matrixgl_t mat, polyhedron_t ph );

bool_t  check_polyhedron_collision_with_dag( 
    scene_node_t *node, matrixgl_t modelMatrix, matrixgl_t invModelMatrix,
    polyhedron_t ph );

#endif


#ifdef __cplusplus
} /* extern "C" */
#endif
