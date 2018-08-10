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

#if defined( HAVE_SDL )
#   include "SDL.h"
#endif

#if defined( HAVE_GL_GLX_H )
#   include <GL/glx.h>
#endif /* defined( HAVE_GL_GLX_H ) */

#include "gl_util.h"

void set_gl_options( RenderMode mode ) 
{
    /* Must set the following options:
         Enable/Disable:
	   GL_TEXTURE_2D
	   GL_DEPTH_TEST
	   GL_CULL_FACE
	   GL_LIGHTING
	   GL_NORMALIZE
	   GL_ALPHA_TEST
	   GL_BLEND
	   GL_STENCIL_TEST
	   GL_TEXTURE_GEN_S
	   GL_TEXTURE_GEN_T
	   GL_COLOR_MATERIAL
           
	 Other Functions:
	   glDepthMask
	   glShadeModel
	   glDepthFunc
    */

    /*
     * Modify defaults based on rendering mode
     * 
     * This could could be improved if it stored state and avoided
     * redundant state changes, which are costly (or so I've heard)...  
     */
    switch( mode ) {
    case GUI:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
	glDisable( GL_FOG );
        break;
    case GAUGE_BARS:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_TEXTURE_GEN_S );
	glEnable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );

	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        break;

    case TEXFONT:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
        break;

    case TEXT:
        glDisable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
        break;

    case SPLASH_SCREEN:
        glDisable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
        break;

    case COURSE:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_TEXTURE_GEN_S );
	glEnable( GL_TEXTURE_GEN_T );
	glEnable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LEQUAL );

	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	break;

    case TREES:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glEnable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
        glEnable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );

        glAlphaFunc( GL_GEQUAL, 0.5 );
        break;
        
    case PARTICLES:
        glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glEnable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );

        glAlphaFunc( GL_GEQUAL, 0.5 );
        break;
        
    case PARTICLE_SHADOWS:
        glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );

        break;

    case SKY:
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE ); 
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_FALSE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
	break;
 	
    case FOG_PLANE:
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE ); 
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
	break;

    case TUX:
        glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
        glEnable( GL_LIGHTING );
	glEnable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );

        break;

    case TUX_SHADOW:
#ifdef USE_STENCIL_BUFFER
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glEnable( GL_STENCIL_TEST );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_FALSE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );

	glStencilFunc( GL_EQUAL, 0, ~0 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
#else
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
#endif
	break;

    case TRACK_MARKS:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glEnable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_COLOR_MATERIAL );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_FALSE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LEQUAL );
	break;

    case OVERLAYS:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glEnable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_COLOR_MATERIAL );
	glDepthMask( GL_TRUE );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );

        glAlphaFunc( GL_GEQUAL, 0.5 );
        break;

    default:
	code_not_reached();
    } 
} 

/* Checking for GL errors is really just another type of assertion, so we
   turn off the check if TUXRACER_NO_ASSERT is defined */
#if defined( TUXRACER_NO_ASSERT )
void check_gl_error()
{
}
#else 
void check_gl_error()
{
    GLenum error;
    error = glGetError();
    if ( error != GL_NO_ERROR ) {
	print_warning( CRITICAL_WARNING, 
# ifdef __APPLE__
		       "OpenGL Error: %d", error
# else
		       "OpenGL Error: %s", gluErrorString( error )
# endif
               );
	fflush( stderr );
    }
}
#endif /* defined( TUXRACER_NO_ASSERT ) */

void copy_to_glfloat_array( GLfloat dest[], scalar_t src[], int n )
{
    int i;
    for (i=0; i<n; i++) {
	dest[i] = src[i];
    }
}

void init_glfloat_array( int num, GLfloat arr[], ... )
{
    int i;
    va_list args;

    va_start( args, arr );

    for (i=0; i<num; i++) {
	arr[i] = va_arg(args, double);
    }

    va_end( args );
}

/* Extension func ptrs *must* be initialized to NULL */
#ifndef __APPLE__
PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_p = NULL;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_p = NULL;
#endif

typedef void (*(*get_gl_proc_fptr_t)(const GLubyte *))(); 

void init_opengl_extensions()
{
    get_gl_proc_fptr_t get_gl_proc;

#if defined( HAVE_SDL )
    get_gl_proc = (get_gl_proc_fptr_t) SDL_GL_GetProcAddress;
#elif defined( WIN32 )
    get_gl_proc = (get_gl_proc_fptr_t) wglGetProcAddress;
#elif defined( HAVE_GLXGETPROCADDRESSARB )
    get_gl_proc = (get_gl_proc_fptr_t) glXGetProcAddressARB;
#else
    get_gl_proc = NULL;
#endif

#ifndef __APPLE__
    if ( get_gl_proc ) {
	glLockArraysEXT_p = (PFNGLLOCKARRAYSEXTPROC) 
	    (*get_gl_proc)( (GLubyte*) "glLockArraysEXT" );
	glUnlockArraysEXT_p = (PFNGLUNLOCKARRAYSEXTPROC) 
	    (*get_gl_proc)( (GLubyte*) "glUnlockArraysEXT" );
	
	if ( glLockArraysEXT_p != NULL && glUnlockArraysEXT_p != NULL ) {
	    print_debug( DEBUG_GL_EXT, 
			 "GL_EXT_compiled_vertex_array extension "
			 "supported" );
	} else {
	    print_debug( DEBUG_GL_EXT, 
			 "GL_EXT_compiled_vertex_array extension "
			 "NOT supported" );
	    glLockArraysEXT_p = NULL;
	    glUnlockArraysEXT_p = NULL;
	}
    } else
#endif
    {
	print_debug( DEBUG_GL_EXT, 
		     "No function available for obtaining GL proc addresses" );
    }
}



/*---------------------------------------------------------------------------*/
/*! 
  Prints information about the current OpenGL implemenation.
  \author  jfpatry
  \date    Created:  2000-10-20
  \date    Modified: 2000-10-20
*/
typedef struct {
    char *name;
    GLenum value;
    GLenum type;
} gl_value_t;

#ifdef __APPLE__
// FIXME
# define GL_INT GL_FIXED
#endif

/* Add more things here as needed */
gl_value_t gl_values[] = {
    { "maximum lights", GL_MAX_LIGHTS, GL_INT },
    { "modelview stack depth", GL_MAX_MODELVIEW_STACK_DEPTH, GL_INT },
    { "projection stack depth", GL_MAX_PROJECTION_STACK_DEPTH, GL_INT },
    { "max texture size", GL_MAX_TEXTURE_SIZE, GL_INT },
#ifndef __APPLE__
    { "double buffering", GL_DOUBLEBUFFER, GL_UNSIGNED_BYTE },
#endif
    { "red bits", GL_RED_BITS, GL_INT },
    { "green bits", GL_GREEN_BITS, GL_INT },
    { "blue bits", GL_BLUE_BITS, GL_INT },
    { "alpha bits", GL_ALPHA_BITS, GL_INT },
    { "depth bits", GL_DEPTH_BITS, GL_INT },
    { "stencil bits", GL_STENCIL_BITS, GL_INT } };

#ifdef __APPLE__
// FIXME
# undef GL_INT
#endif

void print_gl_info()
{
    char *extensions;
    char *p, *oldp;
    int i;
    GLint int_val;
    GLfloat float_val;
    GLboolean boolean_val;

    fprintf( stderr,
	     "  vendor: %s\n", 
	     glGetString( GL_VENDOR ) );

    fprintf( stderr,
	     "  renderer: %s\n", 
	     glGetString( GL_RENDERER ) );

    fprintf( stderr,
	     "  version: %s\n", 
	     glGetString( GL_VERSION ) );

    extensions = string_copy( (char*) glGetString( GL_EXTENSIONS ) );

    fprintf( stderr, "  extensions:\n" );

    oldp = extensions;
    while ( (p=strchr(oldp,' ')) ) {
	*p='\0';
	fprintf( stderr, "    %s\n", oldp );
	oldp = p+1;
    }
    if ( *oldp ) {
	fprintf( stderr, "    %s\n", oldp );
    }

    free( extensions );

    for ( i=0; i<sizeof(gl_values)/sizeof(gl_values[0]); i++) {
	fprintf( stderr, "  %s: ", gl_values[i].name );

	switch( gl_values[i].type ) {
#ifndef __APPLE__
	case GL_INT:
	    glGetIntegerv( gl_values[i].value, &int_val );
	    fprintf( stderr, "%d", int_val );
	    break;
#else
	case GL_FIXED:
	    glGetIntegerv( gl_values[i].value, &int_val );
	    fprintf( stderr, "%d", int_val );
	    break;
#endif
	case GL_FLOAT:
	    glGetFloatv( gl_values[i].value, &float_val );
	    fprintf( stderr, "%f", float_val );
	    break;

	case GL_UNSIGNED_BYTE:
	    glGetBooleanv( gl_values[i].value, &boolean_val );
	    fprintf( stderr, "%d", boolean_val );
	    break;

	default:
	    code_not_reached();
	}

	fprintf( stderr, "\n" );
    }


    fprintf( stderr, "\n" );
}
