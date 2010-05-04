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

#ifndef _GL_UTIL_H_
#define _GL_UTIL_H_

#include "tuxracer.h"

#ifndef __APPLE__

/* Hack to fix compiling problem with old gl.h's, reported by Steve
   Baker <sjbaker1@airmail.net>.  Some old gl.h's don't include glext.h, but
   do this:

   #define GL_EXT_compiled_vertex_array    1

   since they do define the glLockArraysEXT/glUnlockArraysEXT
   functions.  However, this prevents PFNGLLOCKARRAYSEXTPROC /
   PFNGLUNLOCKARRAYSEXTPROC from being defined in glext.h.  So, we do
   the following, which at worst results in a warning (and is awfully
   ugly):

   #undef GL_EXT_compiled_vertex_array

   The *correct* thing to do would be for gl.h to #include glext.h, as
   recent gl.h's do.  However, versions of Mesa as recent as 3.2.1
   don't do this, so we have to work around it.
*/
#undef GL_EXT_compiled_vertex_array

/* Shouldn't need to include glext.h if gl.h is recent, but alas we can't
 * count on that...  */
#include <GL/glext.h>

#if !defined(GL_GLEXT_VERSION) || GL_GLEXT_VERSION < 6
#   error "*** You need a more recent copy of glext.h.  You can get one at http://oss.sgi.com/projects/ogl-sample/ABI/glext.h ; it goes in /usr/include/GL. ***"
#endif

extern PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_p;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_p;

#endif

typedef enum {
    GUI,
    GAUGE_BARS,
    TEXFONT,
    TEXT,
    COURSE,
    TREES,
    PARTICLES,
    PARTICLE_SHADOWS,
    BACKGROUND,
    TUX,
    TUX_SHADOW,
    SKY,
    FOG_PLANE,
    TRACK_MARKS,
    OVERLAYS, 
    SPLASH_SCREEN
} RenderMode;

void set_gl_options( RenderMode mode );

void check_gl_error();

void copy_to_glfloat_array( GLfloat dest[], scalar_t src[], int n );

void init_glfloat_array( int num, GLfloat arr[], ... );

void init_opengl_extensions();

void print_gl_info();

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
