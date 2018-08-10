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

#ifndef _HIER_H_
#define _HIER_H_

#define MIN_SPHERE_DIVISIONS 3
#define MAX_SPHERE_DIVISIONS 16

extern int get_scene_node( char *node_name, scene_node_t **node );

extern char* reset_scene_node(char *node);
extern char* rotate_scene_node(char *node, char axis, scalar_t angle);
extern char* translate_scene_node(char *node, vector_t trans);
extern char* scale_scene_node(char *node, point_t origin, scalar_t factor[3]);
extern char* transform_scene_node(char *node, matrixgl_t mat, matrixgl_t invMat);

extern char* set_scene_node_material(char *node, char *mat);
extern char* create_material(char *mat, colour_t d, colour_t s, scalar_t s_exp);

extern char* set_scene_resolution(char *resolution);

extern char* set_scene_node_shadow_state( char *node, char *state );
extern char* set_scene_node_eye( char *node, char *which_eye );

extern char* create_tranform_node(char *parent, char *name);
extern char* create_sphere_node( char *parent_name, char *child_name, scalar_t resolution );

extern void initialize_scene_graph();

extern void draw_scene_graph( char *node );
extern  bool_t collide( char *node, polyhedron_t ph );

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
