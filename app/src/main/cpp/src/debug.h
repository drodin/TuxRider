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

#include "tuxracer.h"

#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "TRDebug.h"

typedef enum {
    DEBUG_ODE,
    DEBUG_QUADTREE,
    DEBUG_CONTROL,
    DEBUG_SOUND,
    DEBUG_TEXTURE,
    DEBUG_VIEW,
    DEBUG_GL_EXT,
    DEBUG_FONT,
    DEBUG_UI,
    DEBUG_GAME_LOGIC,
    DEBUG_SAVE,
    DEBUG_JOYSTICK,
    DEBUG_GL_INFO,
    NUM_DEBUG_MODES
} debug_mode_t;

void init_debug();
bool_t debug_mode_is_active( debug_mode_t mode );
void debug_mode_set_active( debug_mode_t mode, bool_t active );
void print_debug( debug_mode_t mode, char *fmt, ... );
void setup_diagnostic_log();

/* Define assertion macros (these do nothing if TUXRACER_NO_ASSERT
   is defined) */
#ifdef TUXRACER_NO_ASSERT

#define check_assertion( condition, desc )  /* noop */

#else

#define check_assertion( condition, desc ) \
    if ( condition ) {} else { \
        fprintf( stderr, "!!! " PROG_NAME " unexpected error [%s:%d]: %s\n", \
		 __FILE__, __LINE__, desc ); \
        abort(); \
    }


#endif /* TUXRACER_NO_ASSERT */

#define code_not_reached() \
    check_assertion( 0, "supposedly unreachable code reached!" )

#endif /* _DEBUG_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif
