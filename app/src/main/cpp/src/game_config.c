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

/* This file is complex.  However, the ultimate purpose is to make
   adding new configuration parameters easy.  Here's what you need to
   do to add a new parameter:

   1. Choose a name and type for the parameter.  By convention,
   parameters have lower case names and words_are_separated_like_this.
   Possible types are bool, int, char, and string.  (Nothing is ruling
   out floating point types; I just haven't needed them.)  As an
   example in the subsequent steps, suppose we wish to add a parameter
   foo_bar of type string.

   2. Add a field for the parameter to the 'params' struct defined
   below.  In our example, we would add the line
       struct param foo_bar;
   to the definition of struct params.

   Note that the order of the entries in this struct determines the
   order that the parameters will appear in the configuration file.

   3. Initialize and assign a default value to the parameter in the
   init_game_configuration() function.  The INIT_PARAM_<TYPE> macros
   will do this for you.  In our example, we would add the line
       INIT_PARAM_STRING( foo_bar, "baz" )
   to assign a default value of "baz" to the parameter foo_bar.

   4. Create the getparam/setparam functions for the parameter.  This
   is done using the FN_PARAM_<TYPE> macros.  In our example, we would
   add the line 
       FN_PARAM_STRING( foo_bar )
   somewhere in the top-level scope of this file (to keep things neat
   group it with the other definitions).  The will create
   getparam_foo_bar() and setparam_foo_bar() functions that can be
   used to query the value of the parameter.

   5. Create the prototypes for the getparam/setparam functions.  This
   is done in game_config.h using the PROTO_PARAM_<TYPE> macros.  In
   our example, we would add the line
       PROTO_PARAM_STRING( foo_bar );
   to game_config.h.

   6. You're done!  */

#include "tuxracer.h"

#if defined( WIN32 )
#  define OLD_CONFIG_FILE "tuxracer.cfg"
#else
#  define OLD_CONFIG_FILE ".tuxracer"
#endif /* defined( WIN32 ) */

#if defined( WIN32 )
#  define CONFIG_DIR "config"
#  define CONFIG_FILE "options.txt"
#else
#  define CONFIG_DIR ".tuxracer"
#  define CONFIG_FILE "tuxracerConfig"
#endif /* defined( WIN32 ) */

#ifndef DATA_DIR
#  if defined( WIN32 )
#    define DATA_DIR "."
#  endif /* defined( WIN32 ) */
#endif

#ifdef __APPLE__
#include "sharedGeneralFunctions.h"
#endif

/* Identifies the parameter type */
typedef enum {
    PARAM_STRING,
    PARAM_CHAR,
    PARAM_INT,
    PARAM_BOOL
} param_type;

/* Stores the value for all types */
typedef union {
    char* string_val;
    char  char_val;
    int   int_val;
    bool_t bool_val;
} param_val;

/* Stores state for each parameter */
struct param {
    int loaded;
    char *name;
    param_type type;
    param_val val;
    param_val deflt;
    char *comment;
};

/*
 * These macros are used to initialize parameter values
 */

#define INIT_PARAM( nam, val, typename, commnt ) \
   Params.nam.loaded = False; \
   Params.nam.name = #nam; \
   Params.nam.deflt.typename ## _val  = val; \
   Params.nam.comment = commnt;

#define INIT_PARAM_STRING( nam, val, commnt ) \
   INIT_PARAM( nam, val, string, commnt ); \
   Params.nam.type = PARAM_STRING;

#define INIT_PARAM_CHAR( nam, val, commnt ) \
   INIT_PARAM( nam, val, char, commnt ); \
   Params.nam.type = PARAM_CHAR;

#define INIT_PARAM_INT( nam, val, commnt ) \
   INIT_PARAM( nam, val, int, commnt ); \
   Params.nam.type = PARAM_INT;

#define INIT_PARAM_BOOL( nam, val, commnt ) \
   INIT_PARAM( nam, val, bool, commnt ); \
   Params.nam.type = PARAM_BOOL;


/*
 * These functions are used to get and set parameter values
 */

void fetch_param_string( struct param *p )
{
    char *val;

    check_assertion( p->type == PARAM_STRING, 
		     "configuration parameter type mismatch" );

    val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    if ( val == NULL ) {
	p->val.string_val = string_copy( p->deflt.string_val );
    } else {
	p->val.string_val = string_copy( val );
    }
    p->loaded = True;

}

void set_param_string( struct param *p, char *new_val )
{
    char *ret;

    check_assertion( p->type == PARAM_STRING, 
		     "configuration parameter type mismatch" );

    if ( p->loaded ) {
	free( p->val.string_val );
    }
    ret = Tcl_SetVar( g_game.tcl_interp, p->name, new_val, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.string_val = string_copy( p->deflt.string_val );
    } else {
	p->val.string_val = string_copy( new_val );
    }
    p->loaded = True;

}

void fetch_param_char( struct param *p )
{
    char *str_val;

    check_assertion( p->type == PARAM_CHAR, 
		     "configuration parameter type mismatch" );

    str_val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    
    if ( str_val == NULL || str_val[0] == '\0' ) {
	p->val.char_val = p->deflt.char_val;
    } else {
	p->val.char_val = str_val[0];
    }
    p->loaded = True;
}

void set_param_char( struct param *p, char new_val )
{
    char buff[2];
    char *ret;

    check_assertion( p->type == PARAM_CHAR, 
		     "configuration parameter type mismatch" );

    buff[0] = new_val;
    buff[1] = '\0';

    ret = Tcl_SetVar( g_game.tcl_interp, p->name, buff, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.char_val = p->deflt.char_val;
    } else {
	p->val.char_val = new_val;
    }
    p->loaded = True;

}

void fetch_param_int( struct param *p )
{
    char *str_val;
    int val;

    check_assertion( p->type == PARAM_INT, 
		     "configuration parameter type mismatch" );
    
    str_val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    
    if ( str_val == NULL 
	 || Tcl_GetInt( g_game.tcl_interp, str_val, &val) == TCL_ERROR  ) 
    {
	p->val.int_val = p->deflt.int_val;
    } else {
	p->val.int_val = val;
    }
    p->loaded = True;
}

void set_param_int( struct param *p, int new_val )
{
    char buff[30];
    char *ret;

    check_assertion( p->type == PARAM_INT, 
		     "configuration parameter type mismatch" );

    sprintf( buff, "%d", new_val );

    ret = Tcl_SetVar( g_game.tcl_interp, p->name, buff, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.int_val = p->deflt.int_val;
    } else {
	p->val.int_val = new_val;
    }
    p->loaded = True;

}

void fetch_param_bool( struct param *p )
{
    char *str_val;
    int val;
    bool_t no_val = False;

    check_assertion( p->type == PARAM_BOOL, 
		     "configuration parameter type mismatch" );

    str_val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    
    if ( str_val == NULL ) {
	no_val = True;
    } else if ( string_cmp_no_case( str_val, "false" ) == 0 ) {
	p->val.bool_val = False;
    } else if ( string_cmp_no_case( str_val, "true" ) == 0 ) {
	p->val.bool_val = True;
    } else if ( Tcl_GetInt( g_game.tcl_interp, str_val, &val) == TCL_ERROR ) {
	no_val = True;
    } else {
	p->val.bool_val = (val == 0) ? False : True ;
    }

    if ( no_val ) {
	p->val.bool_val = p->deflt.bool_val;
    }

    p->loaded = True;
}

void set_param_bool( struct param *p, bool_t new_val )
{
    char buff[2];
    char *ret;

    check_assertion( p->type == PARAM_BOOL, 
		     "configuration parameter type mismatch" );

    sprintf( buff, "%d", new_val ? 1 : 0 );

    ret = Tcl_SetVar( g_game.tcl_interp, p->name, buff, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.bool_val = p->deflt.bool_val;
    } else {
	p->val.bool_val = new_val;
    }
    p->loaded = True;
}


/*
 * Creates set/get functions for each parameter
 */
#define FN_PARAM( name, typename, type ) \
    type getparam_ ## name() { \
        if ( !Params.name.loaded ) { \
            fetch_param_ ## typename( &( Params.name ) ); \
        } \
        return Params.name.val.typename ## _val; \
    } \
    void setparam_ ## name( type val) { \
        set_param_ ## typename( &( Params.name ), val ); } 

#define FN_PARAM_STRING( name ) \
    FN_PARAM( name, string, char* )

#define FN_PARAM_CHAR( name ) \
    FN_PARAM( name, char, char )

#define FN_PARAM_INT( name ) \
    FN_PARAM( name, int, int )

#define FN_PARAM_BOOL( name ) \
    FN_PARAM( name, bool, bool_t )


/*
 * Main parameter struct
 */
struct params {
    struct param data_dir;
    struct param fullscreen;
    struct param x_resolution;
    struct param y_resolution;
    struct param bpp_mode;
    struct param capture_mouse; 
    struct param force_window_position;
    struct param quit_key;
    struct param turn_left_key;
    struct param turn_right_key;
    struct param trick_modifier_key;
    struct param brake_key;
    struct param paddle_key;
    struct param jump_key;
    struct param reset_key;
    struct param switch_view_key;
    struct param follow_view_key;
    struct param behind_view_key;
    struct param above_view_key;
    struct param view_mode; /* coresponds to view_mode_t */
    struct param screenshot_key;
    struct param pause_key;

    struct param joystick_paddle_button;
    struct param joystick_brake_button;
    struct param joystick_jump_button;
    struct param joystick_trick_button;
    struct param joystick_continue_button;
    struct param joystick_x_axis;
    struct param joystick_y_axis;

    struct param no_audio;
    struct param sound_enabled;
    struct param music_enabled;
    struct param sound_volume; /* 0-128 */
    struct param music_volume; /* 0-128 */
    struct param audio_freq_mode; /* 0 = 11025, 
				     1 = 22050, 
				     2 = 44100 */
    struct param audio_format_mode; /* 0 = 8 bits, 
				       1 = 16 bits */
    struct param audio_stereo; 
    struct param audio_buffer_size; 

    struct param display_fps;
    struct param course_detail_level;
    struct param forward_clip_distance;
    struct param backward_clip_distance;
    struct param tree_detail_distance;
    struct param terrain_blending;
    struct param perfect_terrain_blending;
    struct param terrain_envmap;
    struct param disable_fog;
    struct param draw_tux_shadow;
    struct param tux_sphere_divisions;
    struct param tux_shadow_sphere_divisions;
    struct param draw_particles;
    struct param track_marks;
    struct param ui_snow;
    struct param nice_fog;
    struct param use_cva;
    struct param cva_hack;
    struct param use_sphere_display_list;
    struct param do_intro_animation;
    struct param mipmap_type; /* 0 = GL_NEAREST,
				 1 = GL_LINEAR,
				 2 = GL_NEAREST_MIPMAP_NEAREST,
				 3 = GL_LINEAR_MIPMAP_NEAREST,
				 4 = GL_NEAREST_MIPMAP_LINEAR,
				 5 = GL_LINEAR_MIPMAP_LINEAR
			      */
    struct param ode_solver; /* 0 = Euler,
				1 = ODE23,
				2 = ODE45
			     */
    struct param fov; 
    struct param debug; 
    struct param warning_level; 
    struct param write_diagnostic_log;
    struct param video_quality;
};

static struct params Params;


/*
 * Initialize parameter data
 */

void init_game_configuration()
{
#ifdef __APPLE__
    INIT_PARAM_STRING( 
	data_dir,getRessourcePath(), 
	"# The location of the Tux Racer data files" );
# else
	INIT_PARAM_STRING( 
					  data_dir,DATA_DIR, 
					  "# The location of the Tux Racer data files" );

#endif
    INIT_PARAM_BOOL( 
	draw_tux_shadow, False, 
	"# Set this to true to display Tux's shadow.  Note that this is a \n"
	"# hack and is quite expensive in terms of framerate.\n"
	"# [EXPERT] This looks better if your card has a stencil buffer; \n"
	"# if compiling use the --enable-stencil-buffer configure option \n"
	"# to enable the use of the stencil buffer" );

    INIT_PARAM_BOOL( 
	draw_particles, False,
	"# Controls the drawing of snow particles that are kicked up as Tux\n"
	"# turns and brakes.  Setting this to false should help improve \n"
	"# performance." );

    INIT_PARAM_INT( 
	tux_sphere_divisions, 2,
	"# [EXPERT] Higher values result in a more finely subdivided mesh \n"
	"# for Tux, and vice versa.  If you're experiencing low framerates,\n"
	"# try lowering this value." );

    INIT_PARAM_INT( 
	tux_shadow_sphere_divisions, 3,
	"# [EXPERT] The level of subdivision of Tux's shadow." );

    INIT_PARAM_BOOL( 
	nice_fog, True,
	"# [EXPERT] If true, then the GL_NICEST hint will be used when\n"
	"# rendering fog.  On some cards, setting this to false may improve\n"
	"# performance.");

    INIT_PARAM_BOOL( 
	use_sphere_display_list, False,
	"# [EXPERT]  Mesa 3.1 sometimes renders Tux strangely when display \n"
	"# lists are used.  Setting this to false should solve the problem \n"
	"# at the cost of a few Hz." );

    INIT_PARAM_BOOL( 
	display_fps, True,
	"# Set this to true to display the current framerate in Hz." );

    INIT_PARAM_INT( 
	x_resolution, 320,
	"# The horizontal size of the Tux Racer window" );

    INIT_PARAM_INT( 
	y_resolution, 480,
	"# The vertical size of the Tux Racer window" );

    INIT_PARAM_BOOL( 
	capture_mouse, False,
	"# If true, then the mouse will not be able to leave the \n"
	"# Tux Racer window.\n"
	"# If you lose keyboard focus while running Tux Racer, try setting\n"
	"# this to true." );

    INIT_PARAM_BOOL( 
	do_intro_animation, True,
	"# If false, then the introductory animation sequence will be skipped." 
	);

    INIT_PARAM_INT( 
	mipmap_type, 1,
	"# [EXPERT] Allows you to control which type of texture\n"
	"# interpolation/mipmapping is used when rendering textures.  The\n"
	"# values correspond to the following OpenGL settings:\n"
	"#\n"
        "#  0: GL_NEAREST\n"
        "#  1: GL_LINEAR\n"
        "#  2: GL_NEAREST_MIPMAP_NEAREST\n"
	"#  3: GL_LINEAR_MIPMAP_NEAREST\n"
        "#  4: GL_NEAREST_MIPMAP_LINEAR\n"
        "#  5: GL_LINEAR_MIPMAP_LINEAR\n"
	"#\n"
	"# On some cards, you may be able to improve performance by\n"
        "# decreasing this number, at the cost of lower image quality." );

    INIT_PARAM_BOOL( 
	fullscreen, True,
	"# If true then the game will run in full-screen mode." );

    INIT_PARAM_INT( 
	bpp_mode, 0,
	"# Controls how many bits per pixel are used in the game.\n"
	"# Valid values are:\n"
	"#\n"
	"#  0: Use current bpp setting of operating system\n"
	"#  1: 16 bpp\n"
	"#  2: 32 bpp\n"
	"# Note that some cards (e.g., Voodoo1, Voodoo2, Voodoo3) only support\n"
	"# 16 bits per pixel." );

    INIT_PARAM_BOOL( 
	force_window_position, False ,
	"# If true, then the Tux Racer window will automatically be\n"
	"# placed at (0,0)" );

    INIT_PARAM_INT( 
	ode_solver, 1 ,
	"# Selects the ODE (ordinary differential equation) solver.  \n"
	"# Possible values are:\n"
	"#\n"
	"#   0: Modified Euler     (fastest but least accurate)\n"
    "#   1: Runge-Kutta (2,3)\n"
	"#   2: Runge-Kutta (4,5)  (slowest but most accurate)" );

    INIT_PARAM_STRING( 
	quit_key, "q escape" ,
	"# Key binding for quitting a race" );
    INIT_PARAM_STRING( 
	turn_left_key, "j left" ,
	"# Key binding for turning left" );
    INIT_PARAM_STRING( 
	turn_right_key, "l right" ,
	"# Key binding for turning right" );
    INIT_PARAM_STRING( 
	trick_modifier_key, "d" ,
	"# Key binding for doing tricks" );
    INIT_PARAM_STRING( 
	brake_key, "k space down" ,
	"# Key binding for braking" );
    INIT_PARAM_STRING( 
	paddle_key, "i up" ,
	"# Key binding for paddling (on the ground) and flapping (in the air)" 
	);
    INIT_PARAM_STRING( 
	switch_view_key, "0" ,
	"# Key binding for the \"Switch\" camera mode" );
    INIT_PARAM_STRING( 
	follow_view_key, "1" ,
	"# Key binding for the \"Follow\" camera mode" );
    INIT_PARAM_STRING( 
	behind_view_key, "2" ,
	"# Key binding for the \"Behind\" camera mode" );
    INIT_PARAM_STRING( 
	above_view_key, "3" ,
	"# Key binding for the \"Above\" camera mode" );
    INIT_PARAM_INT( 
	view_mode, 3,
	"# Default view mode. Possible values are\n" 
	"#\n"
	"#   0: Behind\n"
	"#   1: Follow\n"
	"#   2: Above\n"
    "#   3: Tux eyes" );
    INIT_PARAM_STRING( 
	screenshot_key, "=" ,
	"# Key binding for taking a screenshot" );
    INIT_PARAM_STRING( 
	pause_key, "p" ,
	"# Key binding for pausing the game" );
    INIT_PARAM_STRING( 
	reset_key, "backspace" ,
	"# Key binding for resetting the player position" );
    INIT_PARAM_STRING( 
	jump_key, "e" ,
	"# Key binding for jumping" );

    INIT_PARAM_INT( 
	joystick_paddle_button, 0 ,
	"# Joystick button for paddling (numbering starts at 0).\n" 
	"# Set to -1 to disable." );

    INIT_PARAM_INT( 
	joystick_brake_button, 2 ,
	"# Joystick button for braking (numbering starts at 0).\n" 
	"# Set to -1 to disable." );

    INIT_PARAM_INT( 
	joystick_jump_button, 3 ,
	"# Joystick button for jumping (numbering starts at 0)" );

    INIT_PARAM_INT( 
	joystick_trick_button, 1 ,
	"# Joystick button for doing tricks (numbering starts at 0)" );

    INIT_PARAM_INT( 
	joystick_continue_button, 0 ,
	"# Joystick button for moving past intro, paused, and \n"
	"# game over screens (numbering starts at 0)" );
    
    INIT_PARAM_INT(
	joystick_x_axis, 0 ,
	"# Joystick axis to use for turning (numbering starts at 0)" );

    INIT_PARAM_INT(
	joystick_y_axis, 1 ,
	"# Joystick axis to use for paddling/braking (numbering starts at 0)" );

    INIT_PARAM_INT( 
	fov, 70 ,
	"# [EXPERT] Sets the camera field-of-view" );
    INIT_PARAM_STRING( 
	debug, "" ,
	"# [EXPERT] Controls the Tux Racer debugging modes" );
    INIT_PARAM_INT( 
	warning_level, 100 ,
	"# [EXPERT] Controls the Tux Racer warning messages" );
    INIT_PARAM_INT( 
	forward_clip_distance, 50 ,
	"# Controls how far ahead of the camera the course\n"
	"# is rendered.  Larger values mean that more of the course is\n"
	"# rendered, resulting in slower performance. Decreasing this \n"
	"# value is an effective way to improve framerates." );
    INIT_PARAM_INT( 
	backward_clip_distance, 5 ,
	"# [EXPERT] Some objects aren't yet clipped to the view frustum, \n"
	"# so this value is used to control how far up the course these \n"
	"# objects are drawn." );
    INIT_PARAM_INT( 
	tree_detail_distance, 5 ,
	"# [EXPERT] Controls the distance at which trees are drawn with \n"
	"# two rectangles instead of one." );
    INIT_PARAM_BOOL( 
	terrain_blending, False ,
	"# Controls the blending of the terrain textures.  Setting this\n"
	"# to false will help improve performance." );
    INIT_PARAM_BOOL( 
	perfect_terrain_blending, False ,
	"# [EXPERT] If true, then terrain triangles with three different\n"
	"# terrain types at the vertices will be blended correctly\n"
	"# (instead of using a faster but imperfect approximation)." );
    INIT_PARAM_BOOL( 
	terrain_envmap, false ,
	"# If true, then the ice will be drawn with an \"environment map\",\n"
	"# which gives the ice a shiny appearance.  Setting this to false\n"
	"# will help improve performance." );
    INIT_PARAM_BOOL( 
	disable_fog, False ,
	"# If true, then fog will be turned off.  Some Linux drivers for the\n"
	"# ATI Rage128 seem to have a bug in their fog implementation which\n"
	"# makes the screen nearly pure white when racing; if you experience\n"
	"# this problem then set this variable to true." );
    INIT_PARAM_BOOL( 
	use_cva, True ,
	"# [EXPERT] If true, then compiled vertex arrays will be used when\n"
	"# drawing the terrain.  Whether or not this helps performance\n"
	"# is driver- and card-dependent." );
    INIT_PARAM_BOOL( 
	cva_hack, True ,
	"# Some card/driver combinations render the terrrain incorrectly\n"
	"# when using compiled vertex arrays.  This activates a hack \n"
	"# to work around that problem." );
    INIT_PARAM_INT( 
	course_detail_level, 10 ,
	"# [EXPERT] This controls how accurately the course terrain is \n"
	"# rendered. A high value results in greater accuracy at the cost of \n"
	"# performance, and vice versa.  This value can be decreased and \n"
	"# increased in 10% increments at runtime using the F9 and F10 keys.\n"
	"# To better see the effect, activate wireframe mode using the F11 \n"
	"# key (this is a toggle)." );
    INIT_PARAM_BOOL( 
	no_audio, False ,
	"# If True, then audio in the game is completely disabled." );
    INIT_PARAM_BOOL( 
	sound_enabled, True ,
	"# Use this to turn sound effects on and off." );
    INIT_PARAM_BOOL( 
	music_enabled, True ,
	"# Use this to turn music on and off." );
    INIT_PARAM_INT( 
	sound_volume, 127 ,
	"# This controls the sound volume (valid range is 0-127)." );
    INIT_PARAM_INT( 
	music_volume, 64 ,
	"# This controls the music volume (valid range is 0-127)." );
    INIT_PARAM_INT( 
	audio_freq_mode, 1 ,
	"# The controls the frequency of the audio.  Valid values are:\n"
	"# \n"
	"#   0: 11025 Hz\n"
	"#   1: 22050 Hz\n"
	"#   2: 44100 Hz" );
    INIT_PARAM_INT( 
	audio_format_mode, 1 ,
	"# This controls the number of bits per sample for the audio.\n"
	"# Valid values are:\n"
	"#\n"
	"#   0: 8 bits\n"
	"#   1: 16 bits" );
    INIT_PARAM_BOOL( 
	audio_stereo, True ,
	"# Audio will be played in stereo of true, and mono if false" );
    INIT_PARAM_INT( 
	audio_buffer_size, 2048 ,
	"# [EXPERT] Controls the size of the audio buffer.  \n"
	"# Increase the buffer size if you experience choppy audio\n" 
	"# (at the cost of greater audio latency)" );
    INIT_PARAM_BOOL( 
	track_marks, False ,
	"# If true, then the players will leave track marks in the snow." );
    INIT_PARAM_BOOL( 
	ui_snow, True ,
	"# If true, then the ui screens will have falling snow." );

    INIT_PARAM_BOOL( 
#ifdef TR_DEBUG_MODE
	write_diagnostic_log, True,
#else
	write_diagnostic_log, False,
#endif
	"# If true, then a file called diagnostic_log.txt will be generated\n" 
	"# which you should attach to any bug reports you make.\n"
	"# To generate the file, set this variable to \"true\", and\n"
	"# then run the game so that you reproduce the bug, if possible."
	);
    INIT_PARAM_INT( 
	video_quality, 1 ,
	"# [EXPERT] Sets video quality."
	);
}


/* 
 * Create the set/get functions for parameters
 */

FN_PARAM_STRING( data_dir )
FN_PARAM_BOOL( draw_tux_shadow )
FN_PARAM_BOOL( draw_particles )
FN_PARAM_INT( tux_sphere_divisions )
FN_PARAM_INT( tux_shadow_sphere_divisions )
FN_PARAM_BOOL( nice_fog )
FN_PARAM_BOOL( use_sphere_display_list )
FN_PARAM_BOOL( display_fps )
FN_PARAM_INT( x_resolution )
FN_PARAM_INT( y_resolution )
FN_PARAM_BOOL( capture_mouse )
FN_PARAM_BOOL( do_intro_animation )
FN_PARAM_INT( mipmap_type )
FN_PARAM_BOOL( fullscreen )
FN_PARAM_INT( bpp_mode )
FN_PARAM_BOOL( force_window_position )
FN_PARAM_INT( ode_solver )
FN_PARAM_STRING( quit_key )
FN_PARAM_STRING( turn_left_key )
FN_PARAM_STRING( turn_right_key )
FN_PARAM_STRING( trick_modifier_key )
FN_PARAM_STRING( brake_key )
FN_PARAM_STRING( paddle_key )
FN_PARAM_STRING( above_view_key )
FN_PARAM_STRING( behind_view_key )
FN_PARAM_STRING( follow_view_key )
FN_PARAM_STRING( switch_view_key )
FN_PARAM_INT( view_mode )
FN_PARAM_STRING( screenshot_key )
FN_PARAM_STRING( pause_key )
FN_PARAM_STRING( reset_key )
FN_PARAM_STRING( jump_key )
FN_PARAM_INT( joystick_jump_button )
FN_PARAM_INT( joystick_brake_button )
FN_PARAM_INT( joystick_paddle_button )
FN_PARAM_INT( joystick_trick_button )
FN_PARAM_INT( joystick_continue_button )
FN_PARAM_INT( joystick_x_axis )
FN_PARAM_INT( joystick_y_axis )
FN_PARAM_INT( fov )
FN_PARAM_STRING( debug )
FN_PARAM_INT( warning_level )
FN_PARAM_INT( forward_clip_distance )
FN_PARAM_INT( backward_clip_distance )
FN_PARAM_INT( tree_detail_distance )
FN_PARAM_INT( course_detail_level )
FN_PARAM_BOOL( terrain_blending )
FN_PARAM_BOOL( perfect_terrain_blending )
FN_PARAM_BOOL( terrain_envmap )
FN_PARAM_BOOL( disable_fog )
FN_PARAM_BOOL( use_cva )
FN_PARAM_BOOL( cva_hack )
FN_PARAM_BOOL( track_marks )
FN_PARAM_BOOL( ui_snow )

FN_PARAM_BOOL( no_audio )
FN_PARAM_BOOL( sound_enabled )
FN_PARAM_BOOL( music_enabled )
FN_PARAM_INT( sound_volume )
FN_PARAM_INT( music_volume )
FN_PARAM_INT( audio_freq_mode )
FN_PARAM_INT( audio_format_mode )
FN_PARAM_BOOL( audio_stereo )
FN_PARAM_INT( audio_buffer_size )
FN_PARAM_BOOL( write_diagnostic_log )
FN_PARAM_INT( video_quality )
    


/*
 * Functions to read and write the configuration file
 */

int get_old_config_file_name( char *buff, int len )
{
#if defined( WIN32 ) 
    if ( strlen( OLD_CONFIG_FILE ) +1 > len ) {
	return 1;
    }
    strcpy( buff, OLD_CONFIG_FILE );
    return 0;
#else
    struct passwd *pwent;

    pwent = getpwuid( getuid() );
    if ( pwent == NULL ) {
	perror( "getpwuid" );
	return 1;
    }

    if ( strlen( pwent->pw_dir ) + strlen( OLD_CONFIG_FILE ) + 2 > len ) {
	return 1;
    }

    sprintf( buff, "%s/%s", pwent->pw_dir, OLD_CONFIG_FILE );
    return 0;
#endif /* defined( WIN32 ) */
}

int get_config_dir_name( char *buff, int len )
{
#if defined( WIN32 ) 
    if ( strlen( CONFIG_DIR ) +1 > len ) {
	return 1;
    }
    strcpy( buff, CONFIG_DIR );
    return 0;
#elif defined(__APPLE__)
    const char * configDir = getConfigPath();
    assert(configDir);

    if ( strlen( configDir ) + 1 > len ) {
    assert(0);
	return 1;
    }
    strcpy( buff, configDir );
    return 0;
#else
    struct passwd *pwent;

    pwent = getpwuid( getuid() );
    if ( pwent == NULL ) {
	perror( "getpwuid" );
	return 1;
    }

    if ( strlen( pwent->pw_dir ) + strlen( CONFIG_DIR) + 2 > len ) {
	return 1;
    }

    sprintf( buff, "%s/%s", pwent->pw_dir, CONFIG_DIR );
    return 0;
#endif /* defined( WIN32 ) */
}

int get_config_file_name( char *buff, int len )
{
    if (get_config_dir_name( buff, len ) != 0) {
	return 1;
    }
    if ( strlen( buff ) + strlen( CONFIG_FILE ) +2 > len ) {
	return 1;
    }

#if defined( WIN32 ) 
    strcat( buff, "\\" );
#else
    strcat( buff, "/" );
#endif /* defined( WIN32 ) */

    strcat( buff, CONFIG_FILE);
    return 0;
}

void clear_config_cache()
{
    struct param *parm;
    int i;

    for (i=0; i<sizeof(Params)/sizeof(struct param); i++) {
	parm = (struct param*)&Params + i;
	parm->loaded = False;
    }
}

void read_config_file()
{
    char config_file[BUFF_LEN];
    char config_dir[BUFF_LEN];

    clear_config_cache();

    if ( get_config_file_name( config_file, sizeof( config_file ) ) != 0 ) {
	return;
    }
    if ( get_config_dir_name( config_dir, sizeof( config_dir ) ) != 0 ) {
	return;
    }


    if ( dir_exists( config_dir ) ) {
	if ( file_exists( config_file ) ) {
	    /* File exists -- let's try to evaluate it. */
	    if ( Tcl_EvalFile( g_game.tcl_interp, config_file ) != TCL_OK ) {
		handle_error( 1, "error evalating %s: %s", config_file,
			      Tcl_GetStringResult( g_game.tcl_interp ) );
	    }
	}
	return;
    }

    /* File does not exist -- look for old version */
    if ( get_old_config_file_name( config_file, sizeof( config_file ) ) != 0 ) {
	return;
    }
    if ( !file_exists( config_file ) ) {
	return;
    }
    /* Old file exists -- let's try to evaluate it. */
    if ( Tcl_EvalFile( g_game.tcl_interp, config_file ) != TCL_OK ) {
	handle_error( 1, "error evalating deprecated %s: %s", config_file,
		      Tcl_GetStringResult( g_game.tcl_interp ) );
    } else {
	/* Remove old file and save info in new file location */
	remove(config_file);
	write_config_file();
    }
}

void write_config_file()
{
    FILE *config_stream;
    char config_file[BUFF_LEN];
    char config_dir[BUFF_LEN];
    struct param *parm;
    int i;

#ifdef  __APPLE__
    // Don't save the config file on iphone, no use for it.
    return;
#endif

    if ( get_config_file_name( config_file, sizeof( config_file ) ) != 0 ) {
	return;
    }
    if ( get_config_dir_name( config_dir, sizeof( config_dir ) ) != 0 ) {
	return;
    }

    if ( !dir_exists( config_dir ) ) {

#if defined(WIN32) && !defined(__CYGWIN__)
	if (mkdir( config_dir ) != 0) {
	    return;
	}
#else
	if (mkdir( config_dir, 0775) != 0) {
	    return;
	}
#endif

    }

    config_stream = fopen( config_file, "w" );

    if ( config_stream == NULL ) {
	print_warning( CRITICAL_WARNING, 
		       "couldn't open %s for writing: %s", 
		       config_file, strerror(errno) );
	return;
    }

    fprintf( config_stream, 
	     "# Tux Racer " VERSION " configuration file\n"
	     "#\n"
	);

    for (i=0; i<sizeof(Params)/sizeof(struct param); i++) {
	parm = (struct param*)&Params + i;
	
	//FIXME : dans la version simulateur, on ne veut as qu'il enregistre le data_dir car il change à chaque fois et sinon c'est la merde
	if (!strcmp(parm->name,"data_dir")) continue;

    // Don't save the resolution
	if (!strcmp(parm->name,"x_resolution")) continue;
	if (!strcmp(parm->name,"y_resolution")) continue;
	
	if ( parm->comment != NULL ) {
	    fprintf( config_stream, "\n# %s\n#\n%s\n#\n", 
		     parm->name, parm->comment );
	}
	switch ( parm->type ) {
	case PARAM_STRING:
	    fetch_param_string( parm );
	    fprintf( config_stream, "set %s \"%s\"\n",
		     parm->name, parm->val.string_val );
	    break;
	case PARAM_CHAR:
	    fetch_param_char( parm );
	    fprintf( config_stream, "set %s %c\n",
		     parm->name, parm->val.char_val );
	    break;
	case PARAM_INT:
	    fetch_param_int( parm );
	    fprintf( config_stream, "set %s %d\n",
		     parm->name, parm->val.int_val );
	    break;
	case PARAM_BOOL:
	    fetch_param_bool( parm );
	    fprintf( config_stream, "set %s %s\n",
		     parm->name, parm->val.bool_val ? "true" : "false" );
	    break;
	default:
	    code_not_reached();
	}
    }

    if ( fclose( config_stream ) != 0 ) {
	perror( "fclose" );
    }
}

/*
 * Tcl callback to allow reading of game configuration variables from Tcl.
 */
static int get_param_cb ( ClientData cd, Tcl_Interp *ip, 
			  int argc, char *argv[]) 
{
    int i;
    int num_params;
    struct param *parm;

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <parameter name>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    /* Search for parameter */
    parm = NULL;
    num_params = sizeof(Params)/sizeof(struct param);
    for (i=0; i<num_params; i++) {
	parm = (struct param*)&Params + i;

	if ( strcmp( parm->name, argv[1] ) == 0 ) {
	    break;
	}
    }

    /* If can't find parameter, report error */
    if ( parm == NULL || i == num_params ) {
	Tcl_AppendResult(ip, argv[0], ": invalid parameter `",
			 argv[1], "'", (char *)0 );
	return TCL_ERROR;
    }

    /* Get value of parameter */
    switch ( parm->type ) {
    case PARAM_STRING:
	fetch_param_string( parm );
	Tcl_SetObjResult( ip, Tcl_NewStringObj( parm->val.string_val, -1 ) );
	break;

    case PARAM_CHAR:
	fetch_param_char( parm );
	Tcl_SetObjResult( ip, Tcl_NewStringObj( &parm->val.char_val, 1 ) );
	break;

    case PARAM_INT:
	fetch_param_int( parm );
	Tcl_SetObjResult( ip, Tcl_NewIntObj( parm->val.int_val ) );
	break;

    case PARAM_BOOL:
	fetch_param_bool( parm );
	Tcl_SetObjResult( ip, Tcl_NewBooleanObj( parm->val.bool_val ) );
	break;

    default:
	code_not_reached();
    }

    return TCL_OK;
} 

/* 
 * Tcl callback to allow setting of game configuration variables from Tcl.
 */
static int set_param_cb ( ClientData cd, Tcl_Interp *ip, 
			  int argc, char *argv[]) 
{
    int i;
    int tmp_int;
    int num_params;
    struct param *parm;

    if ( argc != 3 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <parameter name> <value>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    /* Search for parameter */
    parm = NULL;
    num_params = sizeof(Params)/sizeof(struct param);
    for (i=0; i<num_params; i++) {
	parm = (struct param*)&Params + i;

	if ( strcmp( parm->name, argv[1] ) == 0 ) {
	    break;
	}
    }

    /* If can't find parameter, report error */
    if ( parm == NULL || i == num_params ) {
	Tcl_AppendResult(ip, argv[0], ": invalid parameter `",
			 argv[1], "'", (char *)0 );
	return TCL_ERROR;
    }

    /* Set value of parameter */
    switch ( parm->type ) {
    case PARAM_STRING:
	set_param_string( parm, argv[2] ); 
	break;

    case PARAM_CHAR:
	if ( strlen( argv[2] ) > 1 ) {
	    Tcl_AppendResult(ip, "\n", argv[0], ": value for `",
			     argv[1], "' must be a single character", 
			     (char *)0 );
	    return TCL_ERROR;
	}
	set_param_char( parm, argv[2][0] );
	break;

    case PARAM_INT:
	if ( Tcl_GetInt( ip, argv[2], &tmp_int ) != TCL_OK ) {
	    Tcl_AppendResult(ip, "\n", argv[0], ": value for `",
			     argv[1], "' must be an integer", 
			     (char *)0 );
	    return TCL_ERROR;
	}
	set_param_int( parm, tmp_int );
	break;

    case PARAM_BOOL:
	if ( Tcl_GetBoolean( ip, argv[2], &tmp_int ) != TCL_OK ) {
	    Tcl_AppendResult(ip, "\n", argv[0], ": value for `",
			     argv[1], "' must be a boolean", 
			     (char *)0 );
	    return TCL_ERROR;
	}
	check_assertion( tmp_int == 0 || tmp_int == 1, 
			 "invalid boolean value" );
	set_param_bool( parm, (bool_t) tmp_int );
	break;

    default:
	code_not_reached();
    }

    return TCL_OK;
} 

void register_game_config_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_get_param", get_param_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_set_param", set_param_cb,   0,0);
}

