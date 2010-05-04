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
#include "fog.h"
#include "viewfrustum.h"
#include "keyboard.h"
#include "hud.h"
#include "phys_sim.h"
#include "part_sys.h"
#include "course_load.h"
#include "game_config.h"
#include "joystick.h"

static void abort_intro( player_data_t *plyr ) {
    point2d_t start_pt = get_start_pt();
    
    set_game_mode( RACING );

    plyr->orientation_initialized = False;
    plyr->view.initialized = False;

    plyr->pos.x = start_pt.x;
    plyr->pos.z = start_pt.y;

    winsys_post_redisplay();
}

void intro_init(void) 
{
    int i, num_items;
    item_t *item_locs;
    
    player_data_t *plyr = get_player_data( local_player() );
    point2d_t start_pt = get_start_pt();

    init_key_frame();

    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( NULL );
    winsys_set_motion_func( NULL );
    winsys_set_passive_motion_func( NULL );

    plyr->orientation_initialized = False;

    plyr->view.initialized = False;

    g_game.time = 0.0;
    plyr->herring = 0;
    plyr->score = 0;
    
#ifdef __APPLE__
    plyr->tricks = 0;
    plyr->control.is_flying=False;
    plyr->control.fly_total_time=0;
#endif

    plyr->pos.x = start_pt.x;
    plyr->pos.z = start_pt.y;

    init_physical_simulation();

    plyr->vel = make_vector( 0, 0, 0 );

    clear_particles();

    set_view_mode( plyr, ABOVE );
    update_view( plyr, EPS ); 

    /* reset all items as collectable */
    num_items = get_num_items();
    item_locs = get_item_locs();
    for (i = 0; i < num_items; i++ ) {
	if ( item_locs[i].collectable != -1 ) {
	    item_locs[i].collectable = 1;
	}
    }

    play_music( "intro" );
}

void intro_loop( scalar_t time_step )
{
    int width, height;
    player_data_t *plyr = get_player_data( local_player() );

    if ( getparam_do_intro_animation() == False ) {
	set_game_mode( RACING );
	return;
    }

    width = getparam_x_resolution();
    height = getparam_y_resolution();

    check_gl_error();

    /* Check joystick */
    if ( is_joystick_active() ) {
	update_joystick();

	if ( is_joystick_continue_button_down() ) {
	    abort_intro( plyr );
	    return;
	}
    }

#ifdef __APPLE__
    int saved_tux_sphere_divisions = getparam_tux_sphere_divisions();
    setparam_tux_sphere_divisions(10);
#endif

    new_frame_for_fps_calc();

    update_audio();

    update_key_frame( plyr, time_step );

    clear_rendering_context();

    setup_fog();

    update_view( plyr, time_step );

    setup_view_frustum( plyr, NEAR_CLIP_DIST, 
			getparam_forward_clip_distance() );

    draw_sky( plyr->view.pos );

    draw_fog_plane();

    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    setup_course_lighting();
    render_course();
    draw_trees();

    draw_tux();
    draw_tux_shadow();

    draw_hud( plyr );

    reshape( width, height );
    winsys_swap_buffers();
#ifdef __APPLE__
    setparam_tux_sphere_divisions(saved_tux_sphere_divisions);
#endif
} 

START_KEYBOARD_CB( intro_cb )
{
    if ( release ) return;

    abort_intro( plyr );
}
END_KEYBOARD_CB

void intro_register()
{
    int status = 0;

    register_loop_funcs( INTRO, intro_init, intro_loop, NULL );

    status |= add_keymap_entry(
	INTRO, DEFAULT_CALLBACK, NULL, NULL, intro_cb );

    check_assertion( status == 0, "out of keymap entries" );

    return;
}

