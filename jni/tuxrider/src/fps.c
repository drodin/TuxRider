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
#include "fps.h"
#include "render_util.h"

/* Number of frames to use in FPS calculation
 */
#define NUM_FRAMES 10 

static const colour_t text_colour = { 0.0, 0.0, 0.0, 1.0 };

static int      g_frames = 0;
static clock_t  g_t_old = 0;
static scalar_t g_fps  = 0;
static scalar_t g_initialized = 0;

void new_frame_for_fps_calc()
{
    clock_t t_new;

    if ( ! g_initialized ) { 
	g_t_old = clock();
	g_initialized = 1;
    }

    g_frames += 1;

    if ( g_frames >= NUM_FRAMES ) {
	t_new = clock();
	g_frames = 0;
	g_fps = (scalar_t) NUM_FRAMES / 
	        (scalar_t) (t_new-g_t_old) * 
	        (scalar_t) CLOCKS_PER_SEC;

	g_t_old = t_new;
    }
}

scalar_t get_fps()
{
    return g_fps;
}
