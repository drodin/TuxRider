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
#include "game_config.h"
#include "multiplayer.h"
#include "gl_util.h"
#include "fps.h"
#include "render_util.h"
#include "phys_sim.h"
#include "view.h"
#include "course_render.h"
#include "tux.h"
#include "tux_shadow.h"
#include "keyboard.h"
#include "loop.h"
#include "fog.h"
#include "viewfrustum.h"
#include "hud.h"
#include "part_sys.h"
#include "game_logic_util.h"
#include "fonts.h"
#include "ui_mgr.h"
#include "joystick.h"

#define NEXT_MODE RACING

static void mouse_cb( int button, int state, int x, int y )
{
    set_game_mode( NEXT_MODE );
    winsys_post_redisplay();
}


/*---------------------------------------------------------------------------*/
/*! 
  Draws the text for the paused screen
  \author  jfpatry
  \date    Created:  2000-09-26
  \date    Modified: 2000-09-26
*/
void draw_paused_text( void )
{
    int w = getparam_x_resolution();
    int h = getparam_y_resolution();
    int x_org, y_org;
    int box_width, box_height;
    char *string;
    int string_w, asc, desc;
    font_t *font;

    box_width = 200;
    box_height = 300;

    x_org = w/2.0 - box_width/2.0;
    y_org = h/2.0 - box_height/2.0;   

    if ( !get_font_binding( "paused", &font ) ) {
	print_warning( IMPORTANT_WARNING,
		       "Couldn't get font for binding paused" );
    } else {
	string = "Paused";

	get_font_metrics( font, string, &string_w, &asc, &desc );
	
	glPushMatrix();
	{
	    glTranslatef( x_org + box_width/2.0 - string_w/2.0,
			  y_org + box_height/2.0, 
			  0 );
	    bind_font_texture( font );
	    draw_string( font, string );
	}
	glPopMatrix();
    }
}

void paused_init(void) 
{
    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( mouse_cb );
    winsys_set_motion_func( ui_event_motion_func );
    winsys_set_passive_motion_func( ui_event_motion_func );

    play_music( "paused" );
}

void paused_loop( scalar_t time_step )
{
    player_data_t *plyr = get_player_data( local_player() );
    int width, height;
    width = getparam_x_resolution();
    height = getparam_y_resolution();

    check_gl_error();

    /* Check joystick */
    if ( is_joystick_active() ) {
	update_joystick();

	if ( is_joystick_continue_button_down() )
	{
	    set_game_mode( NEXT_MODE );
	    winsys_post_redisplay();
	    return;
	}
    }

    new_frame_for_fps_calc();

    update_audio();

    clear_rendering_context();

    setup_fog();

    update_player_pos( plyr, 0 );
    update_view( plyr, 0 );

    setup_view_frustum( plyr, NEAR_CLIP_DIST, 
			getparam_forward_clip_distance() );

    draw_sky( plyr->view.pos );

    draw_fog_plane( );

    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    setup_course_lighting();
    render_course();
    draw_trees();

    if ( getparam_draw_particles() ) {
	draw_particles( plyr );
    }

    draw_tux();
    draw_tux_shadow();

    set_gl_options( GUI );

    ui_setup_display();

    draw_paused_text();

    draw_hud( plyr );

    reshape( width, height );

    winsys_swap_buffers();
} 

START_KEYBOARD_CB( paused_cb )
{
    if ( release ) return;
    set_game_mode( NEXT_MODE );
    winsys_post_redisplay();
}
END_KEYBOARD_CB

void paused_register()
{
    int status = 0;

    status |= add_keymap_entry( PAUSED, 
				DEFAULT_CALLBACK, NULL, NULL, paused_cb );

    check_assertion( status == 0, "out of keymap entries" );

    register_loop_funcs( PAUSED, paused_init, paused_loop, NULL );
}
