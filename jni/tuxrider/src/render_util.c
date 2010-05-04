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
#include "gl_util.h"
#include "render_util.h"
#include "textures.h"
#include "ui_theme.h"

/*
 * Constants 
 */

/* Distance by which to push back far clip plane, to ensure that the
   fogging plane is drawn (m) */
#define FAR_CLIP_FUDGE_AMOUNT 5

static const colour_t text_colour = { 0.0, 0.0, 0.0, 1.0 };

const colour_t white = { 1.0, 1.0, 1.0, 1.0 };
const colour_t grey  = { 0.7, 0.7, 0.7, 1.0 };
const colour_t red   = { 1.0, 0. , 0., 1.0  };
const colour_t green = { 0. , 1.0, 0., 1.0  };
const colour_t blue  = { 0. , 0. , 1.0, 1.0 };
const colour_t light_blue = { 0.5, 0.5, 0.8, 1.0 };
const colour_t black = { 0., 0., 0., 1.0 };
const colour_t sky   = { 0.82, 0.86, 0.88, 1.0 };

#ifdef __APPLE__
const unsigned int PRECISION = 16; 
GLfixed ONE  = 1 << 16 /*PRECISION*/; 
const GLfixed ZERO = 0;

inline GLfixed FixedFromInt(int value) {return value << PRECISION;}; 
inline GLfixed FixedFromFloat(float value)  
{ return (GLfixed)value;}; 
inline GLfixed MultiplyFixed(GLfixed op1, GLfixed op2) 
{ return (op1 * op2) >> PRECISION;};

void glesPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
	GLfloat xmin, xmax, ymin, ymax;
	
	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	
	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}
#endif

void reshape( int w, int h )
{
    scalar_t far_clip_dist;

    setparam_x_resolution( w );
    setparam_y_resolution( h );
    glViewport( 0, 0, (GLint) w, (GLint) h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    far_clip_dist = getparam_forward_clip_distance() + FAR_CLIP_FUDGE_AMOUNT;

#ifdef __APPLE__
    glesPerspective( getparam_fov(), (scalar_t)w/h, NEAR_CLIP_DIST, 
		    far_clip_dist );
#else
    gluPerspective( getparam_fov(), (scalar_t)w/h, NEAR_CLIP_DIST, 
		    far_clip_dist );
#endif

    glMatrixMode( GL_MODELVIEW );
} 

void flat_mode()
{
    set_gl_options( TEXT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrthof( -0.5, 639.5, -0.5, 479.5, -1.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void draw_overlay() {
    glColor4f( 0.0, 0.0, 1.0, 0.1 );
#ifdef __APPLE__DISABLED__
    const GLfloat vertices []=
    {
       0, 0,
       640, 0,
       640, 480,
       0, 480
    };

    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (2, GL_FLOAT , 0, vertices);	
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
    glRecti( 0, 0, 640, 480 );
#endif
} 

void clear_rendering_context()
{
    glDepthMask( GL_TRUE );
    glClearColor( ui_background_colour.r,
		  ui_background_colour.g,
		  ui_background_colour.b,
		  ui_background_colour.a );
    glClearStencil( 0 );
    glClear( GL_COLOR_BUFFER_BIT 
	     | GL_DEPTH_BUFFER_BIT 
	     | GL_STENCIL_BUFFER_BIT );
}

/* 
 * Sets the material properties
 */
void set_material( colour_t diffuse_colour, colour_t specular_colour,
			 scalar_t specular_exp )
{
  GLfloat mat_amb_diff[4];
  GLfloat mat_specular[4];

  /* Set material colour (used when lighting is on) */
  mat_amb_diff[0] = diffuse_colour.r;
  mat_amb_diff[1] = diffuse_colour.g;
  mat_amb_diff[2] = diffuse_colour.b;
  mat_amb_diff[3] = diffuse_colour.a; 
  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff );

  mat_specular[0] = specular_colour.r;
  mat_specular[1] = specular_colour.g;
  mat_specular[2] = specular_colour.b;
  mat_specular[3] = specular_colour.a;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );

  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, specular_exp );

  /* Set standard colour */
  glColor4f( diffuse_colour.r, diffuse_colour.g, diffuse_colour.b, 
	     diffuse_colour.a);
} 


void draw_billboard( player_data_t *plyr, 
		     point_t center_pt, scalar_t width, scalar_t height, 
		     bool_t use_world_y_axis, 
		     point2d_t min_tex_coord, point2d_t max_tex_coord )
{
    point_t pt;
    vector_t x_vec;
    vector_t y_vec;
    vector_t z_vec;

    x_vec.x = plyr->view.inv_view_mat[0][0];
    x_vec.y = plyr->view.inv_view_mat[0][1];
    x_vec.z = plyr->view.inv_view_mat[0][2];

    if ( use_world_y_axis ) {
	y_vec = make_vector( 0, 1, 0 );
	x_vec = project_into_plane( y_vec, x_vec );
	normalize_vector( &x_vec );
	z_vec = cross_product( x_vec, y_vec );
    } else {
	y_vec.x = plyr->view.inv_view_mat[1][0];
	y_vec.y = plyr->view.inv_view_mat[1][1];
	y_vec.z = plyr->view.inv_view_mat[1][2];
	z_vec.x = plyr->view.inv_view_mat[2][0];
	z_vec.y = plyr->view.inv_view_mat[2][1];
	z_vec.z = plyr->view.inv_view_mat[2][2];
    }

#ifdef __APPLE__DISABLED__
    glNormal3f( z_vec.x, z_vec.y, z_vec.z );

    pt = move_point( center_pt, scale_vector( -width/2.0, x_vec ) );
    pt = move_point( pt, scale_vector( -height/2.0, y_vec ) );
    point_t pt2 = move_point( pt, scale_vector( width, x_vec ) );
    point_t pt3 = move_point( pt2, scale_vector( height, y_vec ) );
    point_t pt4 = move_point( pt3, scale_vector( -width, x_vec ) );

    const GLfloat vertices2 []=
    {
       pt.x, pt.y, pt.z,
       pt2.x, pt2.y, pt2.z,
       pt3.x, pt3.y, pt3.z,
       pt4.x, pt4.y, pt4.z
    };

    const GLshort texCoords2 []=
    {
       min_tex_coord.x, min_tex_coord.y,
       max_tex_coord.x, min_tex_coord.y,
       max_tex_coord.x, max_tex_coord.y,
       min_tex_coord.x, max_tex_coord.y,
    };

    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT , 0, vertices2);	
    glTexCoordPointer(2, GL_SHORT, 0, texCoords2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
    glBegin( GL_QUADS );
    {
	pt = move_point( center_pt, scale_vector( -width/2.0, x_vec ) );
	pt = move_point( pt, scale_vector( -height/2.0, y_vec ) );
	glNormal3f( z_vec.x, z_vec.y, z_vec.z );
	glTexCoord2f( min_tex_coord.x, min_tex_coord.y );
	glVertex3f( pt.x, pt.y, pt.z );

	pt = move_point( pt, scale_vector( width, x_vec ) );
	glTexCoord2f( max_tex_coord.x, min_tex_coord.y );
	glVertex3f( pt.x, pt.y, pt.z );

	pt = move_point( pt, scale_vector( height, y_vec ) );
	glTexCoord2f( max_tex_coord.x, max_tex_coord.y );
	glVertex3f( pt.x, pt.y, pt.z );

	pt = move_point( pt, scale_vector( -width, x_vec ) );
	glTexCoord2f( min_tex_coord.x, max_tex_coord.y );
	glVertex3f( pt.x, pt.y, pt.z );
    }
    glEnd();
#endif
}
