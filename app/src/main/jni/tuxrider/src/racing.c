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
#include "audio.h"
#include "keyboard.h"
#include "keyframe.h"
#include "course_render.h"
#include "multiplayer.h"
#include "gl_util.h"
#include "fps.h"
#include "loop.h"
#include "render_util.h"
#include "view.h"
#include "tux.h"
#include "tux_shadow.h"
#include "phys_sim.h"
#include "part_sys.h"
#include "screenshot.h"
#include "fog.h"
#include "viewfrustum.h"
#include "track_marks.h"
#include "hud.h"
#include "joystick.h"
#include "sharedGeneralFunctions.h"

/* Time constant for automatic steering centering (s) */
#define TURN_DECAY_TIME_CONSTANT 0.5

/* Maximum time over which jump can be charged */
#define MAX_JUMP_AMT 1.0

/* Time constant for automatic rolling centering (s) */
#define ROLL_DECAY_TIME_CONSTANT 0.2

#define JUMP_CHARGE_DECAY_TIME_CONSTANT 0.1

/* If too high off the ground, tux flaps instead of jumping */
#define JUMP_MAX_START_HEIGHT 0.30

static bool_t right_turn;
static bool_t left_turn;
static bool_t trick_modifier;
static bool_t paddling;
static bool_t charging;
static bool_t braking;
static scalar_t charge_start_time;
static int last_terrain;

void racing_init(void) 
{
    player_data_t *plyr = get_player_data( local_player() );
    
    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( NULL );
    winsys_set_motion_func( NULL );
    winsys_set_passive_motion_func( NULL );
    winsys_set_mouse_func( NULL );
    
    /* Initialize view */
    if ( getparam_view_mode() < 0 || 
        getparam_view_mode() >= NUM_VIEW_MODES ) 
    {
        setparam_view_mode( ABOVE );
    }
    set_view_mode( plyr, (view_mode_t)getparam_view_mode() );
    
    /* We need to reset controls here since callbacks won't have been
     called in paused mode. This results in duplication between this
     code and init_physical_simulation.  Oh well. */
    left_turn = right_turn = paddling = False;
    trick_modifier = False;
    charging = False;
    plyr->control.turn_fact = 0.0;
    plyr->control.turn_animation = 0.0;
    plyr->control.is_braking = False;
    plyr->control.is_paddling = False;
    plyr->control.jumping = False;
    plyr->control.jump_charging = False;
    
#ifdef __APPLE__
    plyr->control.is_flying=False;
    plyr->control.fly_total_time=0;
#endif
    
    /* Set last_terrain to a value not used below */
    last_terrain = 0;
    
    if ( g_game.prev_mode != PAUSED ) {
        init_physical_simulation();
    }
    
    last_terrain = 0;
    
    g_game.race_aborted = False;
#ifdef __APPLE__
    g_game.race_time_over = False;
#endif
    
    play_music( "racing" );
    /* play_sound( "start_race", 0 ); */
}

void calc_jump_amt( scalar_t time_step )
{
    player_data_t *plyr = get_player_data( local_player() );
    
    if ( plyr->control.jump_charging ) {
        plyr->control.jump_amt = min( 
                                     MAX_JUMP_AMT, g_game.time - charge_start_time );
    } else if ( plyr->control.jumping ) {
        plyr->control.jump_amt *= 
	    ( 1.0 - ( g_game.time - plyr->control.jump_start_time ) / 
         JUMP_FORCE_DURATION );
    } else {
        plyr->control.jump_amt = 0;
    }
}

void racing_loop( scalar_t time_step )
{
    int width, height;
    player_data_t *plyr = get_player_data( local_player() );
    bool_t joy_left_turn = False;
    bool_t joy_right_turn = False;
    scalar_t joy_turn_fact = 0.0;
    bool_t joy_paddling = False;
    bool_t joy_braking = False;
    bool_t joy_tricks = False;
    bool_t joy_charging = False;
    bool_t airborne;
    vector_t dir;
    scalar_t speed;
    scalar_t terrain_weights[NumTerrains];
    int new_terrain = 0;
    int slide_volume;
    
    
    dir = plyr->vel;
    speed = normalize_vector(&dir);
    
    airborne = (bool_t) ( plyr->pos.y > ( find_y_coord(plyr->pos.x, 
                                                       plyr->pos.z) + 
                                         JUMP_MAX_START_HEIGHT ) );
    
    width = getparam_x_resolution();
    height = getparam_y_resolution();
    
    check_gl_error();
    
    new_frame_for_fps_calc();
    
    update_audio();
    
    clear_rendering_context();
    
    setup_fog();
    
    
    /*
     * Joystick
     */
    if ( is_joystick_active() ) {
        scalar_t joy_x;
        scalar_t joy_y;
        
        update_joystick();
        
        joy_x = get_joystick_x_axis();
        joy_y = get_joystick_y_axis();
        
        if ( joy_x > 0.1 ) {
            joy_right_turn = True;
            joy_turn_fact = joy_x;
        } else if ( joy_x < -0.1 ) {
            joy_left_turn = True;
            joy_turn_fact = joy_x;
        }
        
        if ( getparam_joystick_brake_button() >= 0 ) {
            joy_braking = 
            is_joystick_button_down( getparam_joystick_brake_button() );
        } 
        if ( !joy_braking ) {
            joy_braking = (bool_t) ( joy_y > 0.5 );
        }
        
        if ( getparam_joystick_paddle_button() >= 0 ) {
            joy_paddling = 
            is_joystick_button_down( getparam_joystick_paddle_button() );
        }
        if ( !joy_paddling ) {
            joy_paddling = (bool_t) ( joy_y < -0.5 );
        }
        
        if ( getparam_joystick_jump_button() >= 0 ) {
            joy_charging = 
            is_joystick_button_down( getparam_joystick_jump_button() );
        }
        
        if ( getparam_joystick_trick_button() >= 0 ) {
            joy_tricks = 
            is_joystick_button_down( getparam_joystick_trick_button() );
        }
    }
    
    /* Update braking */
    plyr->control.is_braking = (bool_t) ( braking || joy_braking );
    
    if ( airborne ) {
        new_terrain = (1<<NumTerrains);
        
        /*
         * Tricks
         */
        if ( trick_modifier || joy_tricks ) {
            if ( left_turn || joy_left_turn ) {
                plyr->control.barrel_roll_left = True;
            }
            if ( right_turn || joy_right_turn ) {
                plyr->control.barrel_roll_right = True;
            }
            if ( paddling || joy_paddling ) {
                plyr->control.front_flip = True;
            }
            if ( plyr->control.is_braking ) {
                plyr->control.back_flip = True;
            }
#ifdef __APPLE__
            plyr->tricks+=1;
            TRDebugLog("tricks : %d",plyr->tricks);
#endif
        }
        
        
    } else {
        
        get_surface_type(plyr->pos.x, plyr->pos.z, terrain_weights);
        if (terrain_weights[Snow] > 0) {
            new_terrain |= (1<<Snow);
        }
        if (terrain_weights[Rock] > 0) {
            new_terrain |= (1<<Rock);
        } 
        if (terrain_weights[Ice] > 0) {
            new_terrain |= (1<<Ice);
        }
        
    }
    
    /*
     * Jumping
     */
    calc_jump_amt( time_step );
    
    if ( ( charging || joy_charging ) && 
        !plyr->control.jump_charging && !plyr->control.jumping ) 
    {
        plyr->control.jump_charging = True;
        charge_start_time = g_game.time;
    }
    
    if ( ( !charging && !joy_charging ) && plyr->control.jump_charging ) {
        plyr->control.jump_charging = False;
        plyr->control.begin_jump = True;
    }
    
    
    /* 
     * Turning 
     */
    if ( ( left_turn || joy_left_turn )  ^ (right_turn || joy_right_turn ) ) {
        bool_t turning_left = (bool_t) ( left_turn || joy_left_turn );
        
        if ( joy_left_turn || joy_right_turn ) {
            plyr->control.turn_fact = joy_turn_fact;
        } else {
#ifdef __APPLE__
            scalar_t iPhone_turn_fact;
            iPhone_turn_fact=accelerometerTurnFact();
            plyr->control.turn_fact = iPhone_turn_fact;
#else
            plyr->control.turn_fact = (turning_left?-1:1);
#endif
        }
        
        plyr->control.turn_animation += (turning_left?-1:1) *
	    0.15 * time_step / 0.05;
        plyr->control.turn_animation = 
	    min(1.0, max(-1.0, plyr->control.turn_animation));
    } else {
        plyr->control.turn_fact = 0;
        
        /* Decay turn animation */
        if ( time_step < ROLL_DECAY_TIME_CONSTANT ) {
            plyr->control.turn_animation *= 
            1.0 - time_step/ROLL_DECAY_TIME_CONSTANT;
        } else {
            plyr->control.turn_animation = 0.0;
        }
    }
    
    
    /*
     * Paddling
     */
    if ( ( paddling || joy_paddling ) && plyr->control.is_paddling == False ) {
        plyr->control.is_paddling = True;
        plyr->control.paddle_time = g_game.time;
    }
    
    /*
     * Play flying sound (__APPLE__ : and add Flying time to plyr->control.fly_total_time)
     */
    if (new_terrain & (1<<NumTerrains)) {
        set_sound_volume("flying_sound", min(128, speed*2));
        if (!(last_terrain & (1<<NumTerrains))) {
            play_sound( "flying_sound", -1 );
#ifdef __APPLE__
            plyr->control.is_flying=true;
            plyr->control.fly_start_time = g_game.time;
#endif
        }
    } else {
        if (last_terrain & (1<<NumTerrains)) {
#ifdef __APPLE__
            plyr->control.is_flying=false;
            plyr->control.fly_end_time = g_game.time;
            plyr->control.fly_total_time += plyr->control.fly_end_time-plyr->control.fly_start_time;
#endif
            halt_sound( "flying_sound" );
        }
    }
    
    /*
     * Play sliding sound
     */
    slide_volume = min( (((pow(plyr->control.turn_fact, 2)*128)) +
                         (plyr->control.is_braking?128:0) +
                         (plyr->control.jumping?128:0) +
                         20) *
                       (speed/10), 128 );
    if (new_terrain & (1<<Snow)) {
        set_sound_volume("snow_sound", slide_volume * terrain_weights[Snow]);
        if (!(last_terrain & (1<<Snow))) {
            play_sound( "snow_sound", -1 );
        }
    } else {
        if (last_terrain & (1<<Snow)) {
            halt_sound( "snow_sound" );
        }
    }
    if (new_terrain & (1<<Rock)) {
        set_sound_volume("rock_sound", 128*pow((speed/2), 2) * 
                         terrain_weights[Rock]);
        if (!(last_terrain & (1<<Rock))) {
            play_sound( "rock_sound", -1 );
        }
    } else {
        if (last_terrain & (1<<Rock)) {
            halt_sound( "rock_sound" );
        }
    }
    if (new_terrain & (1<<Ice)) {
        set_sound_volume("ice_sound", slide_volume * terrain_weights[Ice]);
        if (!(last_terrain & (1<<Ice))) {
            play_sound( "ice_sound", -1 );
        }
    } else {
        if (last_terrain & (1<<Ice)) {
            halt_sound( "ice_sound" );
        }
    }
    last_terrain = new_terrain; 
    
    
    /*
     * Tricks
     */
    if ( plyr->control.barrel_roll_left || plyr->control.barrel_roll_right ) {
        plyr->control.barrel_roll_factor += 
		( plyr->control.barrel_roll_left ? -1 : 1 ) * 0.15 * time_step / 0.05;
        if ( (plyr->control.barrel_roll_factor  > 1) ||
            (plyr->control.barrel_roll_factor  < -1) ) {
            plyr->control.barrel_roll_factor = 0;
            plyr->control.barrel_roll_left = plyr->control.barrel_roll_right = False;
        }
    }
    if ( plyr->control.front_flip || plyr->control.back_flip ) {
        plyr->control.flip_factor += 
		( plyr->control.back_flip ? -1 : 1 ) * 0.15 * time_step / 0.05;
        if ( (plyr->control.flip_factor  > 1) ||
            (plyr->control.flip_factor  < -1) ) {
            plyr->control.flip_factor = 0;
            plyr->control.front_flip = plyr->control.back_flip = False;
        }
    }
    
    update_player_pos( plyr, time_step );
	
    /* 
     * Track Marks
     */
    add_track_mark( plyr );
    
    
    update_view( plyr, time_step );
    
    setup_view_frustum( plyr, NEAR_CLIP_DIST, 
                       getparam_forward_clip_distance() );
    
    draw_sky(plyr->view.pos);
    
    draw_fog_plane();
    
    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    setup_course_lighting();
    render_course();
    draw_trees();
    
    if ( getparam_draw_particles() ) {
        update_particles( time_step );
        draw_particles( plyr );
    }
    
    draw_tux();
    draw_tux_shadow();
    
    draw_hud( plyr );
    
    reshape( width, height );
    
    winsys_swap_buffers();
    
    g_game.time += time_step;
} 

static void racing_term(void)
{
    halt_sound( "flying_sound" );
    halt_sound( "rock_sound" );
    halt_sound( "ice_sound" );
    halt_sound( "snow_sound" );
    break_track_marks();
}


START_KEYBOARD_CB( quit_racing_cb )
{
    if ( release ) return;
    g_game.race_aborted = True;
    set_game_mode( GAME_OVER );
}
END_KEYBOARD_CB


START_KEYBOARD_CB( turn_left_cb )
{
    left_turn = (bool_t) !release;
}
END_KEYBOARD_CB


START_KEYBOARD_CB( turn_right_cb )
{
    right_turn = (bool_t) !release;
}
END_KEYBOARD_CB


START_KEYBOARD_CB( trick_modifier_cb )
{
    trick_modifier = (bool_t) !release;
}
END_KEYBOARD_CB


START_KEYBOARD_CB( brake_cb )
{
    braking = (bool_t) !release;
}
END_KEYBOARD_CB

START_KEYBOARD_CB( paddle_cb )
{
    paddling = (bool_t) !release;
}
END_KEYBOARD_CB


START_KEYBOARD_CB( above_view_cb )
{
    if ( release ) return;
    set_view_mode( plyr, ABOVE );
    setparam_view_mode( ABOVE );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( follow_view_cb )
{
    if ( release ) return;
    set_view_mode( plyr, FOLLOW );
    setparam_view_mode( FOLLOW );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( behind_view_cb )
{
    if ( release ) return;
    set_view_mode( plyr, BEHIND );
    setparam_view_mode( BEHIND );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( switch_view_cb )
{
    if ( release ) return;
    int current_view = getparam_view_mode();
    if (current_view == TUXEYE)
        current_view = ABOVE;
    else
        current_view = TUXEYE;
    set_view_mode( plyr, current_view );
    setparam_view_mode( current_view );
    saveparamViewMode( current_view );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( screenshot_cb )
{
    if ( release ) return;
    screenshot();
}
END_KEYBOARD_CB

START_KEYBOARD_CB( pause_cb )
{
    if ( release ) return;
    set_game_mode( PAUSED );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( reset_cb )
{
    if ( release ) return;
    set_game_mode( RESET );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( jump_cb )
{
    charging = (bool_t) !release;
}
END_KEYBOARD_CB

void racing_register()
{
    int status = 0;
    
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "escape", getparam_quit_key, quit_racing_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "j", getparam_turn_left_key, turn_left_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "l", getparam_turn_right_key, turn_right_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "d", getparam_trick_modifier_key, 
                               trick_modifier_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "space", getparam_brake_key, brake_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "k", getparam_paddle_key, paddle_cb );

    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "0", getparam_switch_view_key, 
                               switch_view_cb );    
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "1", getparam_behind_view_key, 
                               behind_view_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "2", getparam_follow_view_key, 
                               follow_view_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "3", getparam_above_view_key, above_view_cb );
    
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "=", getparam_screenshot_key, screenshot_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "p", getparam_pause_key, pause_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "r", getparam_reset_key, reset_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
                               "i", getparam_jump_key, jump_cb );
    
    check_assertion( status == 0, "out of keymap entries" );
    
    register_loop_funcs( RACING, racing_init, racing_loop, racing_term );
}
