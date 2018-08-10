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

#include "lights.h"
#include "tuxracer.h"
#include "hier_cb.h"
#include "hier.h"
#include "gl_util.h"

#define MAX_ARM_ANGLE 30.0
#define MAX_PADDLING_ANGLE 35.0
#define MAX_EXT_PADDLING_ANGLE 30.0
#define MAX_KICK_PADDLING_ANGLE 20.0

static void register_tux_callbacks( Tcl_Interp *ip );

static bool_t    tuxLoaded = False;
static char*     tuxRootNode;
static char*     tuxLeftShoulderJoint;
static char*     tuxRightShoulderJoint;
static char*     tuxLeftHipJoint;
static char*     tuxRightHipJoint;
static char*     tuxLeftKneeJoint;
static char*     tuxRightKneeJoint;
static char*     tuxLeftAnkleJoint;
static char*     tuxRightAnkleJoint;
static char*     tuxTailJoint;
static char*     tuxNeck;
static char*     tuxHead;

void adjust_tux_joints( scalar_t turnFact, bool_t isBraking, 
			scalar_t paddling_factor, scalar_t speed,
			vector_t net_force, scalar_t flap_factor )
{
    scalar_t turning_angle[2] = {0., 0.};
    scalar_t paddling_angle = 0.;
    scalar_t ext_paddling_angle = 0.;  /* arm extension during paddling */
    scalar_t kick_paddling_angle = 0.;  /* leg kicking during paddling */
    scalar_t braking_angle = 0.;
    scalar_t force_angle = 0.; /* amount that legs move because of force */
    scalar_t turn_leg_angle = 0.; /* amount legs move when turning */
    scalar_t flap_angle = 0.;

    /* move arms */
    reset_scene_node( tuxLeftShoulderJoint );
    reset_scene_node( tuxRightShoulderJoint );

    if ( isBraking ) {
	braking_angle = MAX_ARM_ANGLE;
    }

    paddling_angle = MAX_PADDLING_ANGLE * sin(paddling_factor * M_PI);
    ext_paddling_angle = MAX_EXT_PADDLING_ANGLE * sin(paddling_factor * M_PI);
    kick_paddling_angle = MAX_KICK_PADDLING_ANGLE * 
	sin(paddling_factor * M_PI * 2.0);

    turning_angle[0] = max(-turnFact,0.0) * MAX_ARM_ANGLE;
    turning_angle[1] = max(turnFact,0.0) * MAX_ARM_ANGLE;

    flap_angle = MAX_ARM_ANGLE * (0.5 + 0.5*sin(M_PI*flap_factor*6-M_PI/2));

    /* Adjust arms for turning */
    rotate_scene_node( tuxLeftShoulderJoint, 'z', 
		       min( braking_angle + paddling_angle + turning_angle[0],
			    MAX_ARM_ANGLE ) + flap_angle );
    rotate_scene_node( tuxRightShoulderJoint, 'z',
		       min( braking_angle + paddling_angle + turning_angle[1], 
			    MAX_ARM_ANGLE ) + flap_angle );


    /* Adjust arms for paddling */
    rotate_scene_node( tuxLeftShoulderJoint, 'y', -ext_paddling_angle );
    rotate_scene_node( tuxRightShoulderJoint, 'y', ext_paddling_angle );

    force_angle = max( -20.0, min( 20.0, -net_force.z / 300.0 ) );
    turn_leg_angle = turnFact * 10;
    
	/* Adjust hip joints */
    reset_scene_node( tuxLeftHipJoint );
    rotate_scene_node( tuxLeftHipJoint, 'z', -20 + turn_leg_angle
		       + force_angle );
    reset_scene_node( tuxRightHipJoint );
    rotate_scene_node( tuxRightHipJoint, 'z', -20 - turn_leg_angle
		       + force_angle );
	
    /* Adjust knees */
    reset_scene_node( tuxLeftKneeJoint );
    rotate_scene_node( tuxLeftKneeJoint, 'z', -10 + turn_leg_angle
		       - min( 35, speed ) + kick_paddling_angle
		       + force_angle );
    reset_scene_node( tuxRightKneeJoint );
    rotate_scene_node( tuxRightKneeJoint, 'z', -10 - turn_leg_angle
		       - min( 35, speed ) - kick_paddling_angle 
		       + force_angle );

    /* Adjust ankles */
    reset_scene_node( tuxLeftAnkleJoint );
    rotate_scene_node( tuxLeftAnkleJoint, 'z', -20 + min(50, speed ) );
    reset_scene_node( tuxRightAnkleJoint );
    rotate_scene_node( tuxRightAnkleJoint, 'z', -20 + min(50, speed ) );

	/* Turn tail */
    reset_scene_node( tuxTailJoint );
    rotate_scene_node( tuxTailJoint, 'z', turnFact * 20 );

	/* Adjust head and neck */
    reset_scene_node( tuxNeck );
    rotate_scene_node( tuxNeck, 'z', -50 );
    reset_scene_node( tuxHead );
    rotate_scene_node( tuxHead, 'z', -30 );

	/* Turn head when turning */
    rotate_scene_node( tuxHead, 'y', -turnFact * 70 );

}

void draw_tux()
{
    if(get_player_data(0)->view.mode == TUXEYE) return;

    GLfloat dummy_colour[]  = { 0.0, 0.0, 0.0, 1.0 };

    /* XXX: For some reason, inserting this call here makes Tux render
     * with correct lighting under Mesa 3.1. I'm guessing it's a Mesa bug.
     */
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dummy_colour );

    set_gl_options( TUX );

    /* Turn on lights
     */
    setup_course_lighting();

    draw_scene_graph( tuxRootNode );
} 

void load_tux()
{
    char cwd[BUFF_LEN];

    if ( tuxLoaded == True ) 
        return;

    tuxLoaded = True;

    registerHierCallbacks( g_game.tcl_interp );
    register_tux_callbacks( g_game.tcl_interp );

    initialize_scene_graph();

    if ( getcwd( cwd, BUFF_LEN ) == NULL ) {
	handle_system_error( 1, "getcwd failed" );
    }

    if ( chdir( getparam_data_dir() ) != 0 ) {
	/* Print a more informative warning since this is a common error */
	handle_system_error( 
	    1, "Can't find the tuxracer data "
	    "directory.  Please check the\nvalue of `data_dir' in "
	    "~/.tuxracer/options and set it to the location where you\n"
	    "installed the TRWC-data files.\n\n"
	    "Couldn't chdir to %s", getparam_data_dir() );
	/*
        handle_system_error( 1, "couldn't chdir to %s", getparam_data_dir() );
	*/
    } 

    if ( Tcl_EvalFile( g_game.tcl_interp, "tux.tcl") == TCL_ERROR ) {
        handle_error( 1, "error evalating %s/tux.tcl: %s\n"
		      "Please check the value of `data_dir' in ~/.tuxracer/options "
		      "and make sure it\npoints to the location of the "
		      "latest version of the TRWC-data files.", 
		      getparam_data_dir(), 
		      Tcl_GetStringResult( g_game.tcl_interp ) );
    } 

    check_assertion( !Tcl_InterpDeleted( g_game.tcl_interp ),
		     "Tcl interpreter deleted" );

    if ( chdir( cwd ) != 0 ) {
	handle_system_error( 1, "couldn't chdir to %s", cwd );
    } 
} 

char* get_tux_root_node() { return tuxRootNode; } 
char* get_tux_left_shoulder_joint() { return tuxLeftShoulderJoint; } 
char* get_tux_right_shoulder_joint() { return tuxRightShoulderJoint; } 
char* get_tux_left_hip_joint() { return tuxLeftHipJoint; } 
char* get_tux_right_hip_joint() { return tuxRightHipJoint; } 
char* get_tux_left_knee_joint() { return tuxLeftKneeJoint; } 
char* get_tux_right_knee_joint() { return tuxRightKneeJoint; } 
char* get_tux_left_ankle_joint() { return tuxLeftAnkleJoint; } 
char* get_tux_right_ankle_joint() { return tuxRightAnkleJoint; } 
char* get_tux_tail_joint() { return tuxTailJoint; } 
char* get_tux_neck() { return tuxNeck; } 
char* get_tux_head() { return tuxHead; } 

static int head_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
                         "Usage: ", argv[0], " <head joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxHead = string_copy( argv[1] );

    return TCL_OK;
} 

static int neck_cb ( ClientData cd, Tcl_Interp *ip, 
		     int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <neck joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxNeck = string_copy( argv[1] );

    return TCL_OK;
} 

static int root_node_cb ( ClientData cd, Tcl_Interp *ip, 
			  int argc, char *argv[]) 
{
    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <root node>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxRootNode = string_copy( argv[1] );

    return TCL_OK;
} 

static int left_shoulder_cb ( ClientData cd, Tcl_Interp *ip, 
			      int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <left shoulder joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxLeftShoulderJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int right_shoulder_cb ( ClientData cd, Tcl_Interp *ip, 
			       int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <right shoulder joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxRightShoulderJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int left_hip_cb ( ClientData cd, Tcl_Interp *ip, 
			 int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <left hip joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxLeftHipJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int right_hip_cb ( ClientData cd, Tcl_Interp *ip, 
			  int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <right hip joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxRightHipJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int left_knee_cb ( ClientData cd, Tcl_Interp *ip, 
			  int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <left knee joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxLeftKneeJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int right_knee_cb ( ClientData cd, Tcl_Interp *ip, 
			   int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <right knee joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxRightKneeJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int left_ankle_cb ( ClientData cd, Tcl_Interp *ip, 
			   int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <left ankle joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxLeftAnkleJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int right_ankle_cb ( ClientData cd, Tcl_Interp *ip, 
			    int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <right ankle joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxRightAnkleJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int tail_cb ( ClientData cd, Tcl_Interp *ip, 
		     int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <tail joint>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    tuxTailJoint = string_copy( argv[1] );

    return TCL_OK;
} 


static void register_tux_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_root_node", root_node_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_left_shoulder",  left_shoulder_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_right_shoulder",  right_shoulder_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_left_hip",  left_hip_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_right_hip",  right_hip_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_left_knee",  left_knee_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_right_knee",  right_knee_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_left_ankle",  left_ankle_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_right_ankle",  right_ankle_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_neck",  neck_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_head", head_cb,  0,0);
    Tcl_CreateCommand (ip, "tux_tail", tail_cb,  0,0);
}

