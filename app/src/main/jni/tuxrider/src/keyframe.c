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
#include "tux.h"
#include "phys_sim.h"
#include "hier.h"
#include "loop.h"

#define MAX_NUM_KEY_FRAMES 128

static key_frame_t frames[MAX_NUM_KEY_FRAMES];
static int numFrames = 0;
static scalar_t keyTime;

void get_key_frame_data( key_frame_t **fp, int *n )
{
    *fp = frames;
    *n = numFrames;
}

void reset_key_frame()
{
    keyTime = 0;
    numFrames = 0;
} 

void init_key_frame()
{
    keyTime = frames[0].time;

    reset_scene_node( get_tux_head() );
    reset_scene_node( get_tux_neck() );
} 

scalar_t interp( scalar_t frac, scalar_t v1, scalar_t v2 ) 
{
    return frac*v1 + (1.-frac)*v2;
} 

void update_key_frame( player_data_t *plyr, scalar_t dt )
{
    int idx;
    scalar_t frac;
    point_t pos;
    scalar_t v;
    matrixgl_t cob_mat, rot_mat;

    char *root;
    char *lsh;
    char *rsh;
    char *lhp;
    char *rhp;
    char *lkn;
    char *rkn;
    char *lank;
    char *rank;
    char *head;
    char *neck;
    char *tail;

    root = get_tux_root_node();
    lsh  = get_tux_left_shoulder_joint();
    rsh  = get_tux_right_shoulder_joint();
    lhp  = get_tux_left_hip_joint();
    rhp  = get_tux_right_hip_joint();
    lkn  = get_tux_left_knee_joint();
    rkn  = get_tux_right_knee_joint();
    lank = get_tux_left_ankle_joint();
    rank = get_tux_right_ankle_joint();
    head = get_tux_head();
    neck = get_tux_neck();
    tail = get_tux_tail_joint();

    keyTime += dt;

    for (idx = 1; idx < numFrames; idx ++) {
        if ( keyTime < frames[idx].time )
            break;
    } 

    if ( idx == numFrames || numFrames == 0 ) {
        set_game_mode( RACING );
        return;
    } 

    reset_scene_node( root );
    reset_scene_node( lsh );
    reset_scene_node( rsh );
    reset_scene_node( lhp );
    reset_scene_node( rhp );
    reset_scene_node( lkn );
    reset_scene_node( rkn );
    reset_scene_node( lank );
    reset_scene_node( rank );
    reset_scene_node( head );
    reset_scene_node( neck );
    reset_scene_node( tail );

    check_assertion( idx > 0, "invalid keyframe index" );

    if ( fabs( frames[idx-1].time - frames[idx].time ) < EPS ) {
	frac = 1.;
    } else {
	frac = (keyTime - frames[idx].time) 
	    / ( frames[idx-1].time - frames[idx].time );
    }

    pos.x = interp( frac, frames[idx-1].pos.x, frames[idx].pos.x );
    pos.z = interp( frac, frames[idx-1].pos.z, frames[idx].pos.z );
    pos.y = interp( frac, frames[idx-1].pos.y, frames[idx].pos.y );
    pos.y += find_y_coord( pos.x, pos.z );

    set_tux_pos( plyr, pos );

    make_identity_matrix( cob_mat );

    v = interp( frac, frames[idx-1].yaw, frames[idx].yaw );
    rotate_scene_node( root, 'y', v );
    make_rotation_matrix( rot_mat, v, 'y' );
    multiply_matrices( cob_mat, cob_mat, rot_mat );

    v = interp( frac, frames[idx-1].pitch, frames[idx].pitch );
    rotate_scene_node( root, 'x', v );
    make_rotation_matrix( rot_mat, v, 'x' );
    multiply_matrices( cob_mat, cob_mat, rot_mat );

    v = interp( frac, frames[idx-1].l_shldr, frames[idx].l_shldr );
    rotate_scene_node( lsh, 'z', v );

    v = interp( frac, frames[idx-1].r_shldr, frames[idx].r_shldr );
    rotate_scene_node( rsh, 'z', v );

    v = interp( frac, frames[idx-1].l_hip, frames[idx].l_hip );
    rotate_scene_node( lhp, 'z', v );

    v = interp( frac, frames[idx-1].r_hip, frames[idx].r_hip );
    rotate_scene_node( rhp, 'z', v );

    /* Set orientation */
    plyr->orientation = make_quaternion_from_matrix( cob_mat );
    plyr->orientation_initialized = True;
} 

static int key_frame_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    double tmp;
    key_frame_t frame;
    point2d_t start_pt = get_start_pt();

    if (numFrames == MAX_NUM_KEY_FRAMES ) {
        print_warning( TCL_WARNING, 
		       "%s: max. num. of frames reached", argv[0] );
    } 

    if ( argc != 11 ) {
	print_warning( TCL_WARNING, "wrong number of args to %s", argv[0] );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.time = tmp;
    }

    if ( Tcl_GetDouble( ip, argv[2], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.pos.x = tmp + start_pt.x;
    }

    if ( Tcl_GetDouble( ip, argv[3], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.pos.z = -tmp + start_pt.y;
    }
    
    if ( Tcl_GetDouble( ip, argv[4], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.pos.y = tmp;
    }

    if ( Tcl_GetDouble( ip, argv[5], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.yaw = tmp;
    }

    if ( Tcl_GetDouble( ip, argv[6], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.pitch = tmp;
    }

    if ( Tcl_GetDouble( ip, argv[7], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.l_shldr = tmp;
    }

    if ( Tcl_GetDouble( ip, argv[8], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.r_shldr = tmp;
    }

    if ( Tcl_GetDouble( ip, argv[9], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.l_hip = tmp;
    }

    if ( Tcl_GetDouble( ip, argv[10], &tmp ) != TCL_OK ) {
        return TCL_ERROR;
    } else {
	frame.r_hip = tmp;
    }

    frames[numFrames] = frame;
    numFrames++;

    return TCL_OK;
} 



void register_key_frame_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_key_frame", key_frame_cb,  0,0);
}

