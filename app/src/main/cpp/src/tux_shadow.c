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

#include "tux_shadow.h"
#include "gl_util.h"
#include "tux.h"
#include "hier.h"
#include "phys_sim.h"
#include "textures.h"

#define SHADOW_HEIGHT 0.1


/* 
 * Make the shadow darker if the stencil buffer is active 
 */
#ifdef USE_STENCIL_BUFFER

static colour_t shadow_colour = { 0.0, 0.0, 0.0, 0.3 };

#else 

static colour_t shadow_colour = { 0.0, 0.0, 0.0, 0.1 };

#endif /* USE_STENCIL_BUFFER */

void draw_tux_shadow()
{
    matrixgl_t model_matrix;
    char *tux_root_node_name;
    scene_node_t *tux_root_node;

    if ( ! getparam_draw_tux_shadow() ) 
	return;

    set_gl_options( TUX_SHADOW ); 

    glColor4f( shadow_colour.r, shadow_colour.g, shadow_colour.b,
	       shadow_colour.a );

    make_identity_matrix( model_matrix );

    tux_root_node_name = get_tux_root_node();

    if ( get_scene_node( tux_root_node_name, &tux_root_node ) != TCL_OK ) {
	check_assertion( 0, "couldn't find tux's root node" );
    } 

    traverse_dag_for_shadow( tux_root_node, model_matrix );
}

void traverse_dag_for_shadow( scene_node_t *node, matrixgl_t model_matrix )
{
    matrixgl_t new_model_matrix;
    scene_node_t *child;

    check_assertion( node != NULL, "node is NULL" );

    multiply_matrices(new_model_matrix, model_matrix, node->trans);

    if ( node->geom == Sphere && node->render_shadow ) {
	draw_shadow_sphere( new_model_matrix );
    } 

    child = node->child;
    while (child != NULL) {

        traverse_dag_for_shadow(child, new_model_matrix);

        child = child->next;
    } 
}

void draw_shadow_sphere( matrixgl_t model_matrix )
{
    scalar_t theta, phi, d_theta, d_phi, eps, twopi;
    scalar_t x, y, z;
    int div = getparam_tux_shadow_sphere_divisions();
    
    eps = 1e-15;
    twopi = M_PI * 2.0;

    d_theta = d_phi = M_PI / div;

#ifndef __APPLE__DISABLED__
// FIXME
    for ( phi = 0.0; phi + eps < M_PI; phi += d_phi ) {
	scalar_t cos_theta, sin_theta;
	scalar_t sin_phi, cos_phi;
	scalar_t sin_phi_d_phi, cos_phi_d_phi;

	sin_phi = sin( phi );
	cos_phi = cos( phi );
	sin_phi_d_phi = sin( phi + d_phi );
	cos_phi_d_phi = cos( phi + d_phi );
        
        if ( phi <= eps ) {

            glBegin( GL_TRIANGLE_FAN );
		draw_shadow_vertex( 0., 0., 1., model_matrix );

                for ( theta = 0.0; theta + eps < twopi; theta += d_theta ) {
		    sin_theta = sin( theta );
		    cos_theta = cos( theta );

                    x = cos_theta * sin_phi_d_phi;
		    y = sin_theta * sin_phi_d_phi;
                    z = cos_phi_d_phi;
		    draw_shadow_vertex( x, y, z, model_matrix );
                } 

		x = sin_phi_d_phi;
		y = 0.0;
		z = cos_phi_d_phi;
		draw_shadow_vertex( x, y, z, model_matrix );
            glEnd();

        } else if ( phi + d_phi + eps >= M_PI ) {
            
            glBegin( GL_TRIANGLE_FAN );
		draw_shadow_vertex( 0., 0., -1., model_matrix );

                for ( theta = twopi; theta - eps > 0; theta -= d_theta ) {
		    sin_theta = sin( theta );
		    cos_theta = cos( theta );

                    x = cos_theta * sin_phi;
                    y = sin_theta * sin_phi;
                    z = cos_phi;
		    draw_shadow_vertex( x, y, z, model_matrix );
                } 
                x = sin_phi;
                y = 0.0;
                z = cos_phi;
		draw_shadow_vertex( x, y, z, model_matrix );
            glEnd();

        } else {
            
            glBegin( GL_TRIANGLE_STRIP );
                
                for ( theta = 0.0; theta + eps < twopi; theta += d_theta ) {
		    sin_theta = sin( theta );
		    cos_theta = cos( theta );

                    x = cos_theta * sin_phi;
                    y = sin_theta * sin_phi;
                    z = cos_phi;
		    draw_shadow_vertex( x, y, z, model_matrix );

                    x = cos_theta * sin_phi_d_phi;
                    y = sin_theta * sin_phi_d_phi;
                    z = cos_phi_d_phi;
		    draw_shadow_vertex( x, y, z, model_matrix );
                } 
                x = sin_phi;
                y = 0.0;
                z = cos_phi;
		draw_shadow_vertex( x, y, z, model_matrix );

                x = sin_phi_d_phi;
                y = 0.0;
                z = cos_phi_d_phi;
		draw_shadow_vertex( x, y, z, model_matrix );

            glEnd();

        } 
    } 
#endif
} 

void draw_shadow_vertex( scalar_t x, scalar_t y, scalar_t z, 
			 matrixgl_t model_matrix )
{
    point_t pt;
    scalar_t old_y;
    vector_t nml;

    pt = make_point( x, y, z );
    pt = transform_point( model_matrix, pt );

    old_y = pt.y;
    
    nml = find_course_normal( pt.x, pt.z );
    pt.y = find_y_coord( pt.x, pt.z ) + SHADOW_HEIGHT;

    if ( pt.y > old_y ) 
	pt.y = old_y;

#ifndef __APPLE__DISABLED__
//FIXME
    glNormal3f( nml.x, nml.y, nml.z );
    glVertex3f( pt.x, pt.y, pt.z );
#endif
}
