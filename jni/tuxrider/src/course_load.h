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

#include "tuxracer.h"
#include "list.h"

#ifndef _COURSE_LOAD_H_
#define _COURSE_LOAD_H_

#define STRIDE_GL_ARRAY ( 8 * sizeof(GLfloat) + 4 * sizeof(GLubyte) )

/* Convenience macro for accessing terrain elevations */
#define ELEV(x,y) ( elevation[(x) + nx*(y)] )

/* Convenience acro to create a course vertex */
#define COURSE_VERTEX(x,y) make_point( (scalar_t)(x)/(nx-1.)*course_width, \
                       ELEV((x),(y)), -(scalar_t)(y)/(ny-1.)*course_length ) 

void load_course( char *course );

scalar_t     *get_course_elev_data();
terrain_t    *get_course_terrain_data();
scalar_t      get_course_angle();
void          get_course_dimensions( scalar_t *width, scalar_t *length );
void          get_play_dimensions( scalar_t *width, scalar_t *length );
scalar_t      get_terrain_base_height( scalar_t distance );
scalar_t      get_terrain_max_height( scalar_t distance );
void          get_course_divisions( int *nx, int *ny );
tree_t       *get_tree_locs();
int           get_num_trees();
void          set_start_pt( point2d_t p );
point2d_t     get_start_pt();
polyhedron_t  get_tree_polyhedron( int type );
char         *get_tree_name(int type);
char         *get_course_author();
char         *get_course_name(); 
light_t      *get_course_lights();
void          set_course_mirroring( bool_t state );
bool_t        get_course_mirroring( );
void          fill_gl_arrays();
void          get_gl_arrays( GLubyte **vertex_normal_arr );
#ifdef __APPLE__
    char		 *get_calculation_mode();
#endif

void          register_course_load_tcl_callbacks( Tcl_Interp *interp );

item_t       *get_item_locs();
int           get_num_items();
char         *get_item_name(int type);

typedef struct {
    char * name;
    scalar_t diam, height;
    scalar_t vary;
    polyhedron_t poly;
    char * texture;
    int  num_trees;
    unsigned char red, green, blue;
    list_t pos;
    list_elem_t insert_pos;
} tree_type_t;

typedef struct {
    char * name;
    char * texture;
    scalar_t diam, height;
    scalar_t above_ground;
    unsigned char red, green, blue;
    bool_t nocollision;
    bool_t reset_point;
    list_t pos;
    list_elem_t insert_pos;
    int  num_items;
    bool_t use_normal;
    vector_t normal;
} item_type_t;

item_type_t *  get_item_types();
int            get_num_item_types();

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
