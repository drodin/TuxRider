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
#include "view.h"
#include "phys_sim.h"
#include "tux.h"
#include "hier.h"

/* This defines the camera height "target" for all cameras; 
   cameras can go below this because of interpolation (m) */
#define MIN_CAMERA_HEIGHT  1.5

/* Absolute lower bound on camera height above terrain (m) */
#define ABSOLUTE_MIN_CAMERA_HEIGHT  0.3

/* Distance of camera from player (m) */
#define CAMERA_DISTANCE 4.0

/* Angle that camera should look above the slope of the terrain (deg) */
#define CAMERA_ANGLE_ABOVE_SLOPE 10

/* Angle at which to position player in camera below the horizontal (deg) */
#define PLAYER_ANGLE_IN_CAMERA 20

/* Maximum downward pitch of camera (deg)  */
#define MAX_CAMERA_PITCH 40

/* Time constant for interpolation of camera position in "behind" mode (s) */
#define BEHIND_ORBIT_TIME_CONSTANT 0.06

/* Time constant for interpolation of camera orientation in 
   "behind" mode (s) */
#define BEHIND_ORIENT_TIME_CONSTANT 0.06

/* Time constant for interpolation of camera position in "follow" mode (s) */
#define FOLLOW_ORBIT_TIME_CONSTANT 0.06

/* Time constant for interpolation of orientation position in 
   "follow" mode (s) */
#define FOLLOW_ORIENT_TIME_CONSTANT 0.06

/* This places an upper limit on the interpolation, which prevent excessive
   "jumping" of the camera during the occaisonal slow frame */
#define MAX_INTERPOLATION_VALUE 0.3

/* Speed above which interpolation is "normal" (m/s) */
#define BASELINE_INTERPOLATION_SPEED 4.5

/* Speed below which no camera position interpolation occurs (m/s) */
#define NO_INTERPOLATION_SPEED 2.0

static point_t   tux_eye_pts[2];
static point_t   tux_view_pt;

void set_view_mode( player_data_t *plyr, view_mode_t mode )
{
    plyr->view.mode = mode;
    print_debug( DEBUG_VIEW, "View mode: %d", plyr->view.mode );
} 

view_mode_t get_view_mode( player_data_t *plyr )
{
    return plyr->view.mode;
} 

void traverse_dag_for_view_point( scene_node_t *node, matrixgl_t trans )
{
    matrixgl_t new_trans;
    scene_node_t *child;

    check_assertion( node != NULL, "node is NULL" );

    multiply_matrices( new_trans, trans, node->trans );

    if ( node->eye == True ) {
	set_tux_eye( node->which_eye,
		     transform_point( new_trans, make_point( 0., 0., 0. ) ) );
    }

    child = node->child;
    while (child != NULL) {
        traverse_dag_for_view_point( child, new_trans );
        child = child->next;
    } 
}

point_t get_tux_view_pt( player_data_t *plyr ) 
{ 
    matrixgl_t trans;
    char *tux_root_node_name;
    scene_node_t *tux_root_node;

    make_identity_matrix( trans );

    tux_root_node_name = get_tux_root_node();

    if ( get_scene_node( tux_root_node_name, &tux_root_node ) != TCL_OK ) {
	check_assertion(0, "couldn't load tux's root node" );
    } 

    traverse_dag_for_view_point( tux_root_node, trans );

    tux_view_pt = move_point( tux_view_pt, 
			      scale_vector( 0.2, plyr->plane_nml ) );

    return tux_view_pt; 
}

void set_tux_eye( tux_eye_t which_eye, point_t pt ) 
{
    tux_eye_pts[ which_eye ] = pt;

    tux_view_pt.x = ( tux_eye_pts[0].x + tux_eye_pts[1].x ) / 2.0; 
    tux_view_pt.y = ( tux_eye_pts[0].y + tux_eye_pts[1].y ) / 2.0; 
    tux_view_pt.z = ( tux_eye_pts[0].z + tux_eye_pts[1].z ) / 2.0; 
}


/*! 
  Interpolates between camera positions.
  \arg \c  plyr_pos1 \c pos1 is relative to this position
  \arg \c  plyr_pos2 \c pos2 is relative to this position
  \arg \c  max_vec_angle Maximum downward pitch of vector from camera to player
  \arg \c  pos1 original camera position
  \arg \c  pos2 target camera position
  \arg \c  dist distance of interpolated camera position from player
  \arg \c  dt time step size (s)
  \arg \c  time_constant time constant to use in interpolation (s)

  \return  Interpolated camera position
  \author  jfpatry
  \date    Created:  2000-08-26
  \date    Modified: 2000-08-26
*/
point_t interpolate_view_pos( point_t plyr_pos1, point_t plyr_pos2,
			      scalar_t max_vec_angle,
			      point_t pos1, point_t pos2,
			      scalar_t dist, scalar_t dt,
			      scalar_t time_constant )
{
    static vector_t y_vec = { 0.0, 1.0, 0.0 };

    quaternion_t q1, q2;
    vector_t vec1, vec2;
    scalar_t alpha;
    scalar_t theta;
    matrixgl_t rot_mat;
    vector_t axis;

    vec1 = subtract_points( pos1, plyr_pos1 );
    vec2 = subtract_points( pos2, plyr_pos2 );

    normalize_vector( &vec1 );
    normalize_vector( &vec2 );

    q1 = make_rotation_quaternion( y_vec, vec1 );
    q2 = make_rotation_quaternion( y_vec, vec2 );

    alpha = min( MAX_INTERPOLATION_VALUE,
		 1.0 - exp ( -dt / time_constant ) );

    q2 = interpolate_quaternions( q1, q2, alpha );

    vec2 = rotate_vector( q2, y_vec );

    /* Constrain angle with x-z plane */
    theta = RADIANS_TO_ANGLES( M_PI/2 - acos( dot_product( vec2, y_vec ) ) );

    if ( theta > max_vec_angle )
    {
	axis = cross_product( y_vec, vec2 );
	normalize_vector( &axis );

	make_rotation_about_vector_matrix( rot_mat, axis, 
					   theta-max_vec_angle );
	vec2 = transform_vector( rot_mat, vec2 );
    }

    return move_point( plyr_pos2, 
		       scale_vector( dist, vec2 ) );

}


/*! 
  Interpolates between camera orientations
  \pre     p_up2, p_dir2 != NULL; *p_up2 is the target up vector; 
	   *p_dir2 is the target view direction
  \arg \c  up1 original up vector
  \arg \c  dir1 original view direction
  \arg \c  p_up2 pointer to target up vector; is overwritten with 
           interpolated up vector
  \arg \c  p_dir2 pointer to target view direction; is overwritten with 
           interpolated view direction
  \arg \c  dt time step
  \arg \c  time_constant interpolation time constant

  \return  none
  \author  jfpatry
  \date    Created:  2000-08-26
  \date    Modified: 2000-08-26
*/
void interpolate_view_frame( vector_t up1, vector_t dir1,
			     vector_t *p_up2, vector_t *p_dir2,
			     scalar_t dt, scalar_t time_constant )
{
    quaternion_t q1, q2;
    scalar_t alpha;
    vector_t x1, y1, z1;
    vector_t x2, y2, z2;
    matrixgl_t cob_mat1, inv_cob_mat1;
    matrixgl_t cob_mat2, inv_cob_mat2;

    /* Now interpolate between camera orientations */
    z1 = scale_vector( -1.0, dir1 );
    normalize_vector( &z1 );

    y1 = project_into_plane( z1, up1 );
    normalize_vector( &y1 );

    x1 = cross_product( y1, z1 );

    make_change_of_basis_matrix( cob_mat1, inv_cob_mat1,
				 x1, y1, z1 );

    q1 = make_quaternion_from_matrix( cob_mat1 );

    z2 = scale_vector( -1.0, *p_dir2 );
    normalize_vector( &z2 );

    y2 = project_into_plane( z2, *p_up2 );
    normalize_vector( &y2 );

    x2 = cross_product( y2, z2 );

    make_change_of_basis_matrix( cob_mat2, inv_cob_mat2,
				 x2, y2, z2 );

    q2 = make_quaternion_from_matrix( cob_mat2 );

    alpha = min( MAX_INTERPOLATION_VALUE, 
		 1.0 - exp( -dt / time_constant ) );

    q2 = interpolate_quaternions( q1, q2, alpha );

    make_matrix_from_quaternion( cob_mat2, q2 );

    p_up2->x = cob_mat2[1][0];
    p_up2->y = cob_mat2[1][1];
    p_up2->z = cob_mat2[1][2];

    p_dir2->x = -cob_mat2[2][0];
    p_dir2->y = -cob_mat2[2][1];
    p_dir2->z = -cob_mat2[2][2];
}

void setup_view_matrix( player_data_t *plyr ) 
{
    vector_t view_x, view_y, view_z;
    matrixgl_t view_mat;
    point_t viewpt_in_view_frame;

    view_z = scale_vector( -1, plyr->view.dir );
    view_x = cross_product( plyr->view.up, view_z );
    view_y = cross_product( view_z, view_x );
    normalize_vector( &view_z );
    normalize_vector( &view_x );
    normalize_vector( &view_y );

    make_identity_matrix( plyr->view.inv_view_mat );

    plyr->view.inv_view_mat[0][0] = view_x.x;
    plyr->view.inv_view_mat[0][1] = view_x.y;
    plyr->view.inv_view_mat[0][2] = view_x.z;

    plyr->view.inv_view_mat[1][0] = view_y.x;
    plyr->view.inv_view_mat[1][1] = view_y.y;
    plyr->view.inv_view_mat[1][2] = view_y.z;

    plyr->view.inv_view_mat[2][0] = view_z.x;
    plyr->view.inv_view_mat[2][1] = view_z.y;
    plyr->view.inv_view_mat[2][2] = view_z.z;

    plyr->view.inv_view_mat[3][0] = plyr->view.pos.x;
    plyr->view.inv_view_mat[3][1] = plyr->view.pos.y;
    plyr->view.inv_view_mat[3][2] = plyr->view.pos.z;
    plyr->view.inv_view_mat[3][3] = 1;
    
    transpose_matrix( plyr->view.inv_view_mat, view_mat );

    view_mat[0][3] = 0;
    view_mat[1][3] = 0;
    view_mat[2][3] = 0;
    
    viewpt_in_view_frame = transform_point( view_mat, plyr->view.pos );
    
    view_mat[3][0] = -viewpt_in_view_frame.x;
    view_mat[3][1] = -viewpt_in_view_frame.y;
    view_mat[3][2] = -viewpt_in_view_frame.z;
    
    glLoadIdentity();
#ifdef __APPLE__DISABLED__
    GLfloat matrix[3][3];
    int i,j;
    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
            matrix[i][j] = view_mat[i][j];
    }
    glMultMatrixf( (GLfloat *) matrix );
#else
    glMultMatrixd( (scalar_t *) view_mat );
#endif

}

/*! 
  Updates camera and sets the view matrix
  \pre     plyr != NULL, plyr has been initialized with position & 
           velocity info., plyr->view.mode has been set
  \arg \c  plyr pointer to player data
  \arg \c  dt time step size  

  \return  none
  \author  jfpatry
  \date    Created:  2000-08-26
  \date    Modified: 2000-08-26
*/
void update_view( player_data_t *plyr, scalar_t dt )
{
    point_t view_pt;
    vector_t view_dir, up_dir, vel_dir, view_vec;
    scalar_t ycoord;
    scalar_t course_angle;
    vector_t axis;
    matrixgl_t rot_mat;
    vector_t y_vec;
    vector_t mz_vec;
    vector_t vel_proj;
    quaternion_t rot_quat;
    scalar_t speed;
    vector_t vel_cpy;
    scalar_t time_constant_mult;

    vel_cpy = plyr->vel;
    speed = normalize_vector( &vel_cpy );

    time_constant_mult = 1.0 /
	min( 1.0, 
	     max( 0.0, 
		  ( speed - NO_INTERPOLATION_SPEED ) /
		  ( BASELINE_INTERPOLATION_SPEED - NO_INTERPOLATION_SPEED )));

    up_dir = make_vector( 0, 1, 0 );

    vel_dir = plyr->vel;
    normalize_vector( &vel_dir );

    course_angle = get_course_angle();

    switch( plyr->view.mode ) {
    case TUXEYE:
    {
        view_pt = plyr->pos;
        up_dir = plyr->plane_nml;
        scalar_t f = 2;
        up_dir.x = (plyr->view.up.x * f +  up_dir.x) / (f + 1);
        up_dir.y = (plyr->view.up.y * f + up_dir.y) / (f + 1);
        up_dir.z = (plyr->view.up.z * f + up_dir.z) / (f + 1);

        vector_t v = plyr->plane_nml;
        scalar_t n = 1.;
        view_pt.x += v.x / n  * 0.3;
        view_pt.y += v.y / n * 0.3;
        view_pt.y += 0.1;
        view_pt.z += v.z / n * 0.3;

        view_dir = plyr->direction;
        view_dir.y += 0.1;

        view_dir.x = (plyr->view.dir.x * f +  view_dir.x) / (f + 1);
        view_dir.y = (plyr->view.dir.y * f + view_dir.y) / (f + 1);
        view_dir.z = (plyr->view.dir.z * f + view_dir.z) / (f + 1);

        break;
    }
    case BEHIND:
    {
	/* Camera-on-a-string mode */

	/* Construct vector from player to camera */
	view_vec = make_vector( 0, 
				sin( ANGLES_TO_RADIANS( 
				    course_angle -
				    CAMERA_ANGLE_ABOVE_SLOPE + 
				    PLAYER_ANGLE_IN_CAMERA ) ),
				cos( ANGLES_TO_RADIANS( 
				    course_angle -
				    CAMERA_ANGLE_ABOVE_SLOPE + 
				    PLAYER_ANGLE_IN_CAMERA ) ) );

	view_vec = scale_vector( CAMERA_DISTANCE, view_vec );

	y_vec = make_vector( 0.0, 1.0, 0.0 );
	mz_vec = make_vector( 0.0, 0.0, -1.0 );
	vel_proj = project_into_plane( y_vec, vel_dir );

	normalize_vector( &vel_proj );

	/* Rotate view_vec so that it places the camera behind player */
	rot_quat = make_rotation_quaternion( mz_vec, vel_proj );

	view_vec = rotate_vector( rot_quat, view_vec );


	/* Construct view point */
	view_pt = move_point( plyr->pos, view_vec );

	/* Make sure view point is above terrain */
        ycoord = find_y_coord( view_pt.x, view_pt.z );

        if ( view_pt.y < ycoord + MIN_CAMERA_HEIGHT ) {
            view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
        } 

	/* Interpolate view point */
	if ( plyr->view.initialized ) {
	    /* Interpolate twice to get a second-order filter */
	    int i;
	    for (i=0; i<2; i++) {
		view_pt = 
		    interpolate_view_pos( plyr->pos, plyr->pos, 
					  MAX_CAMERA_PITCH, plyr->view.pos, 
					  view_pt, CAMERA_DISTANCE, dt,
					  BEHIND_ORBIT_TIME_CONSTANT * 
					  time_constant_mult );
	    }
	}

	/* Make sure interpolated view point is above terrain */
        ycoord = find_y_coord( view_pt.x, view_pt.z );

        if ( view_pt.y < ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT ) {
            view_pt.y = ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT;
        } 

	/* Construct view direction */
	view_vec = subtract_points( view_pt, plyr->pos );
	
	axis = cross_product( y_vec, view_vec );
	normalize_vector( &axis );
	
	make_rotation_about_vector_matrix( rot_mat, axis,
					   PLAYER_ANGLE_IN_CAMERA );
	view_dir = scale_vector( -1.0, 
				 transform_vector( rot_mat, view_vec ) );

	/* Interpolate orientation of camera */
	if ( plyr->view.initialized ) {
	    /* Interpolate twice to get a second-order filter */
	    int i;
	    for (i=0; i<2; i++) {
		interpolate_view_frame( plyr->view.up, plyr->view.dir,
					&up_dir, &view_dir, dt,
					BEHIND_ORIENT_TIME_CONSTANT );
		up_dir = make_vector( 0.0, 1.0, 0.0 );
	    }
	}

        break;
    }

    case FOLLOW: 
    {
	/* Camera follows player (above and behind) */

	up_dir = make_vector( 0, 1, 0 );

	/* Construct vector from player to camera */
	view_vec = make_vector( 0, 
				sin( ANGLES_TO_RADIANS( 
				    course_angle -
				    CAMERA_ANGLE_ABOVE_SLOPE +
				    PLAYER_ANGLE_IN_CAMERA ) ),
				cos( ANGLES_TO_RADIANS( 
				    course_angle -
				    CAMERA_ANGLE_ABOVE_SLOPE + 
				    PLAYER_ANGLE_IN_CAMERA ) ) );
	view_vec = scale_vector( CAMERA_DISTANCE, view_vec );

	y_vec = make_vector( 0.0, 1.0, 0.0 );
	mz_vec = make_vector( 0.0, 0.0, -1.0 );
	vel_proj = project_into_plane( y_vec, vel_dir );

	normalize_vector( &vel_proj );

	/* Rotate view_vec so that it places the camera behind player */
	rot_quat = make_rotation_quaternion( mz_vec, vel_proj );

	view_vec = rotate_vector( rot_quat, view_vec );


	/* Construct view point */
	view_pt = move_point( plyr->pos, view_vec );


	/* Make sure view point is above terrain */
        ycoord = find_y_coord( view_pt.x, view_pt.z );

        if ( view_pt.y < ycoord + MIN_CAMERA_HEIGHT ) {
            view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
	}

	/* Interpolate view point */
	if ( plyr->view.initialized ) {
	    /* Interpolate twice to get a second-order filter */
	    int i;
	    for ( i=0; i<2; i++ ) {
		view_pt = 
		    interpolate_view_pos( plyr->view.plyr_pos, plyr->pos, 
					  MAX_CAMERA_PITCH, plyr->view.pos, 
					  view_pt, CAMERA_DISTANCE, dt,
					  FOLLOW_ORBIT_TIME_CONSTANT *
					  time_constant_mult );
	    }
	}

	/* Make sure interpolate view point is above terrain */
        ycoord = find_y_coord( view_pt.x, view_pt.z );

        if ( view_pt.y < ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT ) {
            view_pt.y = ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT;
        } 

	/* Construct view direction */
	view_vec = subtract_points( view_pt, plyr->pos );
	
	axis = cross_product( y_vec, view_vec );
	normalize_vector( &axis );
	
	make_rotation_about_vector_matrix( rot_mat, axis,
					   PLAYER_ANGLE_IN_CAMERA );
	view_dir = scale_vector( -1.0, 
				 transform_vector( rot_mat, view_vec ) );

	/* Interpolate orientation of camera */
	if ( plyr->view.initialized ) {
	    /* Interpolate twice to get a second-order filter */
	    int i;
	    for ( i=0; i<2; i++ ) {
		interpolate_view_frame( plyr->view.up, plyr->view.dir,
					&up_dir, &view_dir, dt,
					FOLLOW_ORIENT_TIME_CONSTANT );
		up_dir = make_vector( 0.0, 1.0, 0.0 );
	    }
	}

        break;
    }

    case ABOVE:
    {
	/* Camera always uphill of player */

	up_dir = make_vector( 0, 1, 0 );


	/* Construct vector from player to camera */
	view_vec = make_vector( 0, 
				sin( ANGLES_TO_RADIANS( 
				    course_angle - 
				    CAMERA_ANGLE_ABOVE_SLOPE+
				    PLAYER_ANGLE_IN_CAMERA ) ),
				cos( ANGLES_TO_RADIANS( 
				    course_angle - 
				    CAMERA_ANGLE_ABOVE_SLOPE+ 
				    PLAYER_ANGLE_IN_CAMERA ) ) );
	view_vec = scale_vector( CAMERA_DISTANCE, view_vec );

	
	/* Construct view point */
	view_pt = move_point( plyr->pos, view_vec );


	/* Make sure view point is above terrain */
        ycoord = find_y_coord( view_pt.x, view_pt.z );

        if ( view_pt.y < ycoord + MIN_CAMERA_HEIGHT ) {
            view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
	}

	/* Construct view direction */
	view_vec = subtract_points( view_pt, plyr->pos );

	make_rotation_matrix( rot_mat, PLAYER_ANGLE_IN_CAMERA, 'x' );
	view_dir = scale_vector( -1.0, 
				 transform_vector( rot_mat, view_vec ) );

        break;
    }

    default:
	code_not_reached();
    } 

    /* Create view matrix */
    plyr->view.pos = view_pt;
    plyr->view.dir = view_dir;
    plyr->view.up = up_dir;
    plyr->view.plyr_pos = plyr->pos;
    plyr->view.initialized = True;

    setup_view_matrix( plyr );
} 
