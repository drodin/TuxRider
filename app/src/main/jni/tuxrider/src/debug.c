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

#include "debug.h"
#include "tuxracer.h"
#include "string_util.h"
#include "os_util.h"

#define BUGREPORT_FILE "diagnostic_log.txt"

static bool_t debug_setting[ NUM_DEBUG_MODES ];
static char* debug_desc[ NUM_DEBUG_MODES ] = {
    "ode",
    "quadtree",
    "control",
    "sound",
    "texture",
    "view",
    "gl_ext",
    "font",
    "ui",
    "game_logic",
    "save",
    "joystick",
    "gl_info"
};

/* Parse the debug parameter, fill in the debug_setting array */
void init_debug()
{
    char *debug_str, *tmp_str;
    char *p;
    int i;
    bool_t new_setting;

    for ( i=0; i<NUM_DEBUG_MODES; i++ ) {
	debug_setting[i] = False;
    }

    debug_str = getparam_debug();
    tmp_str = debug_str;

    while ( (p = strtok( tmp_str, " " )) != NULL ) {
	tmp_str = NULL;

	new_setting = True;

	if ( *p == '-' ) {
	    p++;
	    new_setting = False;

	    if ( *p == '\0' ) {
		print_warning( CONFIGURATION_WARNING, 
			       "solitary `-' in debug parameter -- ignored." );
		continue;
	    }
	}

	if ( *p == '\0' ) {
	    continue;
	}


	if ( string_cmp_no_case( p, "all" ) == 0 ) {
	    for (i=0; i<NUM_DEBUG_MODES; i++) {
		debug_setting[i] = new_setting;
	    }
	} else {
	    for ( i=0; i<NUM_DEBUG_MODES; i++ ) {
		if ( string_cmp_no_case( p, debug_desc[i] ) == 0 ) {
		    debug_setting[i] = new_setting;
		    break;
		}
	    }

	    if ( i == NUM_DEBUG_MODES ) {
		print_warning( CONFIGURATION_WARNING,
			       "unrecognized debug mode `%s'", p );
	    }
	}
    }
}

bool_t debug_mode_is_active( debug_mode_t mode )
{
    return debug_setting[ mode ];
}

void debug_mode_set_active( debug_mode_t mode, bool_t active )
{
    check_assertion( mode >= 0 &&
		     mode < NUM_DEBUG_MODES,
		     "Invalid debug mode" );

    debug_setting[ mode ] = active;
}

void print_debug( debug_mode_t mode, char *fmt, ... )
{
    va_list args;

    check_assertion( 0 <= mode && mode < NUM_DEBUG_MODES,
		     "invalid debugging mode" );

    if ( ! debug_mode_is_active( mode ) ) {
	return;
    }

    va_start( args, fmt );

    fprintf( stderr, PROG_NAME " debug (%s): ", debug_desc[ mode ] );
    vfprintf( stderr, fmt, args );
    fprintf( stderr, "\n" );
	
	printf( PROG_NAME " debug (%s): ", debug_desc[ mode ] );
    vprintf( fmt, args );
    printf( "\n" );

    va_end( args );
}



/*---------------------------------------------------------------------------*/
/*! 
  Opens the diagnostic log, writes a header, and activates all 
  debugging modes.  All subsequent output to stderr will be redirected to
  the diagnostic log.

  \author  jfpatry
*/
void setup_diagnostic_log()
{
    FILE *newfp;
    time_t t;
    char os_buff[BUFF_LEN];
    char time_buff[BUFF_LEN];

    /* Activate a bunch of debugging modes */
    debug_mode_set_active( DEBUG_QUADTREE, True );
    debug_mode_set_active( DEBUG_CONTROL, True );
    debug_mode_set_active( DEBUG_SOUND, True );
    debug_mode_set_active( DEBUG_TEXTURE, True );
    debug_mode_set_active( DEBUG_VIEW, True );
    debug_mode_set_active( DEBUG_GL_EXT, True );
    debug_mode_set_active( DEBUG_FONT, True );
    debug_mode_set_active( DEBUG_UI, True );
    debug_mode_set_active( DEBUG_GAME_LOGIC, True );
    debug_mode_set_active( DEBUG_SAVE, True );
    debug_mode_set_active( DEBUG_JOYSTICK, True );
    debug_mode_set_active( DEBUG_GL_INFO, True );

    /* Redirect stderr to file; taken from SDL_main.c, which is in the 
       public domain */
    newfp = freopen(BUGREPORT_FILE, "w", stderr);
    if ( newfp == NULL ) {	/* This happens on NT */
#if !defined(stderr)
	stderr = fopen(BUGREPORT_FILE, "w");
#else
	newfp = fopen(BUGREPORT_FILE, "w");
	if ( newfp ) {
	    *stderr = *newfp;
	}
#endif
    }

    /* Write bug report header */
    fprintf( stderr, "Tux Racer Diagnostic Log\n\n" );

    /* Generate time string */
    t = time( NULL );
    sprintf( time_buff, "%s", asctime( gmtime( &t ) ) );
    time_buff[ strlen(time_buff)-1 ] = (char)0; /* remove trailing newline */

    fprintf( stderr, "Generated:       %s GMT\n", time_buff );
    fprintf( stderr, "TR Version:      %s\n", VERSION );
    fprintf( stderr, "OS:              " );

    if ( get_os_version( os_buff, sizeof( os_buff ) ) == 0 ) {
	fprintf( stderr, "%s\n", os_buff );
    } else {
	fprintf( stderr, "Could not determine!\n" );
    }

    fprintf( stderr, "\n" );
}
