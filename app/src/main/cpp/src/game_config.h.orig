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

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "tux_types.h"

void init_game_configuration();
void read_config_file();
void write_config_file();
void clear_config_cache();
void register_game_config_callbacks( Tcl_Interp *ip );
int get_config_dir_name( char *buff, int len );

#define PROTO_PARAM( name, type ) \
    type getparam_ ## name(); \
    void setparam_ ## name( type val )

#define PROTO_PARAM_STRING( name ) \
    PROTO_PARAM( name, char* )

#define PROTO_PARAM_CHAR( name ) \
    PROTO_PARAM( name, char )

#define PROTO_PARAM_INT( name ) \
    PROTO_PARAM( name, int )

#define PROTO_PARAM_BOOL( name ) \
    PROTO_PARAM( name, bool_t )


PROTO_PARAM_STRING( data_dir );

PROTO_PARAM_BOOL( draw_tux_shadow );

PROTO_PARAM_BOOL( draw_particles );

PROTO_PARAM_INT( tux_sphere_divisions );

PROTO_PARAM_INT( tux_shadow_sphere_divisions );

PROTO_PARAM_BOOL( nice_fog );

PROTO_PARAM_BOOL( use_sphere_display_list );

PROTO_PARAM_BOOL( display_fps );

PROTO_PARAM_INT( x_resolution );

PROTO_PARAM_INT( y_resolution );

PROTO_PARAM_BOOL( capture_mouse );

PROTO_PARAM_BOOL( do_intro_animation );

PROTO_PARAM_INT( mipmap_type );

PROTO_PARAM_BOOL( fullscreen );

PROTO_PARAM_INT( bpp_mode );

PROTO_PARAM_BOOL( force_window_position );

PROTO_PARAM_INT( ode_solver );

PROTO_PARAM_STRING( quit_key );

PROTO_PARAM_STRING( turn_left_key );

PROTO_PARAM_STRING( turn_right_key );

PROTO_PARAM_STRING( trick_modifier_key );

PROTO_PARAM_STRING( brake_key );

PROTO_PARAM_STRING( paddle_key );

PROTO_PARAM_STRING( above_view_key );

PROTO_PARAM_STRING( behind_view_key );

PROTO_PARAM_STRING( eye_view_key );

PROTO_PARAM_STRING( follow_view_key );

PROTO_PARAM_INT( view_mode );

PROTO_PARAM_STRING( screenshot_key );

PROTO_PARAM_STRING( pause_key );
    
PROTO_PARAM_STRING( reset_key );

PROTO_PARAM_STRING( jump_key );

PROTO_PARAM_INT( joystick_jump_button );

PROTO_PARAM_INT( joystick_brake_button );

PROTO_PARAM_INT( joystick_paddle_button );

PROTO_PARAM_INT( joystick_trick_button );

PROTO_PARAM_INT( joystick_continue_button );

PROTO_PARAM_INT( joystick_x_axis );

PROTO_PARAM_INT( joystick_y_axis );

PROTO_PARAM_INT( fov );

PROTO_PARAM_STRING( debug );

PROTO_PARAM_INT( warning_level );

PROTO_PARAM_INT( forward_clip_distance );

PROTO_PARAM_INT( backward_clip_distance );

PROTO_PARAM_INT( tree_detail_distance );

PROTO_PARAM_INT( course_detail_level );

PROTO_PARAM_BOOL( terrain_blending );

PROTO_PARAM_BOOL( perfect_terrain_blending );

PROTO_PARAM_BOOL( terrain_envmap );

PROTO_PARAM_BOOL( disable_fog );

PROTO_PARAM_BOOL( use_cva );

PROTO_PARAM_BOOL( cva_hack );

PROTO_PARAM_BOOL( no_audio );

PROTO_PARAM_BOOL( sound_enabled );

PROTO_PARAM_BOOL( music_enabled );

PROTO_PARAM_INT( sound_volume );

PROTO_PARAM_INT( music_volume );

PROTO_PARAM_INT( audio_freq_mode );

PROTO_PARAM_INT( audio_format_mode );

PROTO_PARAM_BOOL( audio_stereo );

PROTO_PARAM_INT( audio_buffer_size );

PROTO_PARAM_BOOL( track_marks );

PROTO_PARAM_BOOL( ui_snow );

PROTO_PARAM_BOOL( write_diagnostic_log );

#endif 

#ifdef __cplusplus
} /* extern "C" */
#endif
