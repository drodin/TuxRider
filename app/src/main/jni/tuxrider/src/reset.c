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
#include "game_config.h"
#include "audio.h"
#include "multiplayer.h"
#include "gl_util.h"
#include "fps.h"
#include "render_util.h"
#include "phys_sim.h"
#include "view.h"
#include "course_render.h"
#include "tux.h"
#include "tux_shadow.h"
#include "loop.h"
#include "fog.h"
#include "viewfrustum.h"
#include "hud.h"
#include "course_load.h"

#define BLINK_IN_PLACE_TIME 0.5
#define TOTAL_RESET_TIME 1.0

static const colour_t text_colour = { 0.0, 0.0, 0.0, 1.0 };
static scalar_t reset_start_time;
static bool_t position_reset;


void reset_init(void) 
{
    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( NULL );
    winsys_set_motion_func( NULL );
    winsys_set_passive_motion_func( NULL );
    winsys_set_mouse_func( NULL );
    
    reset_start_time = get_clock_time();
    position_reset = False;

}

void reset_loop( scalar_t time_step )
{
    int width, height;
    player_data_t *plyr = get_player_data( local_player() );
    scalar_t elapsed_time = get_clock_time() - reset_start_time;
    scalar_t course_width, course_length;
    static bool_t tux_visible = True; 
    static int tux_visible_count = 0;
    item_type_t  *item_types;
    item_t       *item_locs;
    int  i, first_reset, last_reset, num_item_types;
    int best_loc;

    width = getparam_x_resolution();
    height = getparam_y_resolution();

    check_gl_error();

    new_frame_for_fps_calc();

    update_audio();

    clear_rendering_context();

    setup_fog();

    update_player_pos( plyr, EPS );
	
    update_view( plyr, EPS );

    setup_view_frustum( plyr, NEAR_CLIP_DIST, 
			getparam_forward_clip_distance() );

    draw_sky(plyr->view.pos);

    draw_fog_plane();

    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    setup_course_lighting();
    render_course();
    draw_trees();

    if ((elapsed_time > BLINK_IN_PLACE_TIME) && (!position_reset)) {
	item_types = get_item_types();
	item_locs  = get_item_locs();
	num_item_types = get_num_item_types();
	first_reset = 0;
	last_reset = 0;
	for ( i = 0; i < num_item_types; i++ ) {
	    if (item_types[i].reset_point == True) {
		last_reset = first_reset + item_types[i].num_items - 1;
		break;
	    } else {
		first_reset += item_types[i].num_items;
	    }
	}

	if (last_reset == 0) {
	    /* didn't find a reset point item type */
	    get_course_dimensions( &course_width, &course_length );
	    plyr->pos.x = course_width/2.0;
	    plyr->pos.z = min(plyr->pos.z + 10, -1.0);
	} else {
	    /* BFI */
	    best_loc = -1;
	    for ( i = first_reset; i <= last_reset; i++) {
		if (item_locs[i].ray.pt.z > plyr->pos.z ) { 
		    if (best_loc == -1 || 
			item_locs[i].ray.pt.z < item_locs[best_loc].ray.pt.z)
		    {
			best_loc = i;
		    }
		}
	    }

	    if ( best_loc == -1 ) {
		get_course_dimensions( &course_width, &course_length );
		plyr->pos.x = course_width/2.0;
		plyr->pos.z = min(plyr->pos.z + 10, -1.0);
	    } else if ( item_locs[best_loc].ray.pt.z <= plyr->pos.z ) {
		get_course_dimensions( &course_width, &course_length );
		plyr->pos.x = course_width/2.0;
		plyr->pos.z = min(plyr->pos.z + 10, -1.0);
	    } else {
		plyr->pos.x = item_locs[best_loc].ray.pt.x;
		plyr->pos.z = item_locs[best_loc].ray.pt.z;
	    }
	}

	/* Re-initialize the camera */
	plyr->view.initialized = False;

	init_physical_simulation();
	position_reset = True;
    }

    if (tux_visible) { 
	draw_tux();
	draw_tux_shadow();
    } 
    if (++tux_visible_count > 3) {
	tux_visible = (bool_t) !tux_visible;
	tux_visible_count = 0;
    }

    draw_hud( plyr );

    reshape( width, height );

    winsys_swap_buffers();

    g_game.time += time_step;

    if (elapsed_time > TOTAL_RESET_TIME) {
	set_game_mode( RACING );
	winsys_post_redisplay();
    }

    
} 

void reset_register()
{
    register_loop_funcs( RESET, reset_init, reset_loop, NULL );
}


