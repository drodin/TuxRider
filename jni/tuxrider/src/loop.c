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
#include "loop.h"
#include "ui_mgr.h"

#if defined( HAVE_SDL )
#   include "SDL.h"
#endif /* defined( HAVE_SDL ) */


/*
 * Types
 */

typedef struct {
    mode_init_func_ptr_t init_func;
    mode_loop_func_ptr_t loop_func;
    mode_term_func_ptr_t term_func;
} mode_funcs_t;


/*
 * Static Variables
 */

static scalar_t clock_time;
static mode_funcs_t mode_funcs[ NUM_GAME_MODES ] = 
{ 
    { NULL, NULL, NULL },   /* SPLASH */
    { NULL, NULL, NULL },   /* START */
    { NULL, NULL, NULL },   /* INTRO */
    { NULL, NULL, NULL },   /* RACING */
    { NULL, NULL, NULL },   /* GAME_OVER */
    { NULL, NULL, NULL },   /* PAUSED */
    { NULL, NULL, NULL }    /* RESET */
};

static game_mode_t new_mode = NO_MODE;


/*
 * Function definitions
 */

void register_loop_funcs( game_mode_t mode, 
			  mode_init_func_ptr_t init_func,
			  mode_loop_func_ptr_t loop_func,
			  mode_term_func_ptr_t term_func )
{
    check_assertion( mode >= 0 && mode < NUM_GAME_MODES,
		     "invalid game mode" );
    mode_funcs[ mode ].init_func = init_func;
    mode_funcs[ mode ].loop_func = loop_func;
    mode_funcs[ mode ].term_func = term_func;
}

scalar_t get_clock_time()
{
#if defined( HAVE_GETTIMEOFDAY )

    struct timeval tv;
    gettimeofday( &tv, NULL );

    return (scalar_t) tv.tv_sec + (scalar_t) tv.tv_usec * 1.e-6;

#elif defined( HAVE_SDL ) 

    return SDL_GetTicks() * 1.e-3;

#else

#   error "We have no way to determine the time on this system."

#endif /* defined( HAVE_GETTIMEOFDAY ) */
} 

void reset_time_step_clock()
{
    clock_time = get_clock_time( );
} 

static scalar_t calc_time_step()
{
    scalar_t cur_time, time_step;

    cur_time = get_clock_time( );
    time_step = cur_time - clock_time;
    if (time_step < EPS ) {
        time_step = EPS;
    }  
    clock_time = cur_time;
    return time_step;
} 

void set_game_mode( game_mode_t mode ) 
{
    new_mode = mode;
}

/* This is the main loop of the game.  It dispatches into the loop
   functions of the various game modes.  */
void main_loop()
{
    if ( getparam_capture_mouse() ) {
	int w = getparam_x_resolution();
	int h = getparam_y_resolution();
	point2d_t pos;

	pos = ui_get_mouse_position();

	/* Flip y coordinates */
	pos.y = h - pos.y;

	if ( pos.x < 0 ) {
	    pos.x = 0;
	}
	if ( pos.x > w-1 ) {
	    pos.x = w-1;
	}
	if ( pos.y < 0 ) {
	    pos.y = 0;
	}
	if ( pos.y > h-1 ) {
	    pos.y = h-1;
	}

	winsys_warp_pointer( pos.x, pos.y );
    }

    if ( g_game.mode != new_mode ) {

	if ( g_game.mode >= 0 && 
	     mode_funcs[ g_game.mode ].term_func != NULL ) 
	{
	    ( mode_funcs[ g_game.mode ].term_func )( );
	}

	g_game.prev_mode = g_game.mode;

	g_game.mode = new_mode;
	
	if ( mode_funcs[ g_game.mode ].init_func != NULL ) {
	    ( mode_funcs[ g_game.mode ].init_func )( );

	    /* Reset time step clock so that there isn't a sudden
	       jump when we start the new mode */
            reset_time_step_clock();
	}
    }

    g_game.time_step = calc_time_step();
    g_game.secs_since_start += g_game.time_step;

    if ( mode_funcs[ g_game.mode ].loop_func != NULL ) {
	( mode_funcs[ g_game.mode ].loop_func )( g_game.time_step );
    }
}


/*---------------------------------------------------------------------------*/
/*! 
  Returns true iff a mode change will occur the next time main_loop() runs.

  \author  jfpatry
*/
bool_t is_mode_change_pending()
{
    return g_game.mode != new_mode;
}
