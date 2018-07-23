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
#ifndef WINSYS_H
#define WINSYS_H 1

#include "tuxracer.h"
#include "tux_types.h"

#if defined( HAVE_SDL )
#   include "SDL.h"
#elif defined( HAVE_GLUT )
#   include "GL/glut.h"
#elif defined( __APPLE__ )

#else
#   error "Neither SDL nor GLUT are present."
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/* Keysyms */

#if defined( HAVE_SDL ) 
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* SDL version */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

typedef enum {
    WSK_NOT_AVAIL = SDLK_UNKNOWN,

    /* Numeric keypad */
    WSK_KP0 = SDLK_KP0,
    WSK_KP1 = SDLK_KP1,
    WSK_KP2 = SDLK_KP2,
    WSK_KP3 = SDLK_KP3,
    WSK_KP4 = SDLK_KP4,
    WSK_KP5 = SDLK_KP5,
    WSK_KP6 = SDLK_KP6,
    WSK_KP7 = SDLK_KP7,
    WSK_KP8 = SDLK_KP8,
    WSK_KP9 = SDLK_KP9,
    WSK_KP_PERIOD = SDLK_KP_PERIOD,
    WSK_KP_DIVIDE = SDLK_KP_DIVIDE,
    WSK_KP_MULTIPLY = SDLK_KP_MULTIPLY,
    WSK_KP_MINUS = SDLK_KP_MINUS,
    WSK_KP_PLUS = SDLK_KP_PLUS,
    WSK_KP_ENTER = SDLK_KP_ENTER,
    WSK_KP_EQUALS = SDLK_KP_EQUALS,

    /* Arrows + Home/End pad */
    WSK_UP = SDLK_UP,
    WSK_DOWN = SDLK_DOWN,
    WSK_RIGHT = SDLK_RIGHT,
    WSK_LEFT = SDLK_LEFT,
    WSK_INSERT = SDLK_INSERT,
    WSK_HOME = SDLK_HOME,
    WSK_END = SDLK_END,
    WSK_PAGEUP = SDLK_PAGEUP,
    WSK_PAGEDOWN = SDLK_PAGEDOWN,

    /* Function keys */
    WSK_F1 = SDLK_F1,
    WSK_F2 = SDLK_F2,
    WSK_F3 = SDLK_F3,
    WSK_F4 = SDLK_F4,
    WSK_F5 = SDLK_F5,
    WSK_F6 = SDLK_F6,
    WSK_F7 = SDLK_F7,
    WSK_F8 = SDLK_F8,
    WSK_F9 = SDLK_F9,
    WSK_F10 = SDLK_F10,
    WSK_F11 = SDLK_F11,
    WSK_F12 = SDLK_F12,
    WSK_F13 = SDLK_F13,
    WSK_F14 = SDLK_F14,
    WSK_F15 = SDLK_F15,

    /* Key state modifier keys */
    WSK_NUMLOCK = SDLK_NUMLOCK,
    WSK_CAPSLOCK = SDLK_CAPSLOCK,
    WSK_SCROLLOCK = SDLK_SCROLLOCK,
    WSK_RSHIFT = SDLK_RSHIFT,
    WSK_LSHIFT = SDLK_LSHIFT,
    WSK_RCTRL = SDLK_RCTRL,
    WSK_LCTRL = SDLK_LCTRL,
    WSK_RALT = SDLK_RALT,
    WSK_LALT = SDLK_LALT,
    WSK_RMETA = SDLK_RMETA,
    WSK_LMETA = SDLK_LMETA,

    WSK_LAST

} winsys_keysym_t;

typedef enum {
    WS_LEFT_BUTTON = SDL_BUTTON_LEFT,
    WS_MIDDLE_BUTTON = SDL_BUTTON_MIDDLE,
    WS_RIGHT_BUTTON = SDL_BUTTON_RIGHT
} winsys_mouse_button_t;

typedef enum {
    WS_MOUSE_DOWN = SDL_PRESSED,
    WS_MOUSE_UP = SDL_RELEASED
} winsys_button_state_t;

#elif defined(HAVE_GLUT)
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* GLUT version */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* GLUT doesn't define as many keysyms as SDL; we map those to 
   WSK_NOT_AVAIL (0) */

typedef enum {
    WSK_NOT_AVAIL = 0,

    /* Numeric keypad */
    WSK_KP0 = 0,
    WSK_KP1 = 0,
    WSK_KP2 = 0,
    WSK_KP3 = 0,
    WSK_KP4 = 0,
    WSK_KP5 = 0,
    WSK_KP6 = 0,
    WSK_KP7 = 0,
    WSK_KP8 = 0,
    WSK_KP9 = 0,
    WSK_KP_PERIOD = 0,
    WSK_KP_DIVIDE = 0,
    WSK_KP_MULTIPLY = 0,
    WSK_KP_MINUS = 0,
    WSK_KP_PLUS = 0,
    WSK_KP_ENTER = 0,
    WSK_KP_EQUALS = 0,

    /* Arrows + Home/End pad */
    WSK_UP = GLUT_KEY_UP,
    WSK_DOWN = GLUT_KEY_DOWN,
    WSK_RIGHT = GLUT_KEY_RIGHT,
    WSK_LEFT = GLUT_KEY_LEFT,
    WSK_INSERT = GLUT_KEY_INSERT,
    WSK_HOME = GLUT_KEY_HOME,
    WSK_END = GLUT_KEY_END,
    WSK_PAGEUP = GLUT_KEY_PAGE_UP,
    WSK_PAGEDOWN = GLUT_KEY_PAGE_DOWN,

    /* Function keys */
    WSK_F1 = GLUT_KEY_F1,
    WSK_F2 = GLUT_KEY_F2,
    WSK_F3 = GLUT_KEY_F3,
    WSK_F4 = GLUT_KEY_F4,
    WSK_F5 = GLUT_KEY_F5,
    WSK_F6 = GLUT_KEY_F6,
    WSK_F7 = GLUT_KEY_F7,
    WSK_F8 = GLUT_KEY_F8,
    WSK_F9 = GLUT_KEY_F9,
    WSK_F10 = GLUT_KEY_F10,
    WSK_F11 = GLUT_KEY_F11,
    WSK_F12 = GLUT_KEY_F12,
    WSK_F13 = 0,
    WSK_F14 = 0,
    WSK_F15 = 0,

    /* Key state modifier keys */
    WSK_NUMLOCK = 0,
    WSK_CAPSLOCK = 0,
    WSK_SCROLLOCK = 0,
    WSK_RSHIFT = 0,
    WSK_LSHIFT = 0,
    WSK_RCTRL = 0,
    WSK_LCTRL = 0,
    WSK_RALT = 0,
    WSK_LALT = 0,
    WSK_RMETA = 0,
    WSK_LMETA = 0,

    WSK_LAST = UCHAR_MAX /* GLUT doesn't define a max key, but this is more
			    than enough as of version 3.7 */
} winsys_keysym_t;

typedef enum {
    WS_LEFT_BUTTON = GLUT_LEFT_BUTTON,
    WS_MIDDLE_BUTTON = GLUT_MIDDLE_BUTTON,
    WS_RIGHT_BUTTON = GLUT_RIGHT_BUTTON 
} winsys_mouse_button_t;

typedef enum {
    WS_MOUSE_DOWN = GLUT_DOWN,
    WS_MOUSE_UP = GLUT_UP
} winsys_button_state_t;

#elif defined(__APPLE__)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* GLUT version */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* GLUT doesn't define as many keysyms as SDL; we map those to 
   WSK_NOT_AVAIL (0) */

typedef enum {
    WSK_NOT_AVAIL = 0,

    /* Numeric keypad */
    WSK_KP0 = 0,
    WSK_KP1 = 0,
    WSK_KP2 = 0,
    WSK_KP3 = 0,
    WSK_KP4 = 0,
    WSK_KP5 = 0,
    WSK_KP6 = 0,
    WSK_KP7 = 0,
    WSK_KP8 = 0,
    WSK_KP9 = 0,
    WSK_KP_PERIOD = 0,
    WSK_KP_DIVIDE = 0,
    WSK_KP_MULTIPLY = 0,
    WSK_KP_MINUS = 0,
    WSK_KP_PLUS = 0,
    WSK_KP_ENTER = 0,
    WSK_KP_EQUALS = 0,

    /* Arrows + Home/End pad */
    WSK_UP,
    WSK_DOWN,
    WSK_RIGHT,
    WSK_LEFT,
    WSK_INSERT,
    WSK_HOME,
    WSK_END,
    WSK_PAGEUP,
    WSK_PAGEDOWN,

    /* Function keys */
    WSK_F1,
    WSK_F2,
    WSK_F3,
    WSK_F4,
    WSK_F5,
    WSK_F6,
    WSK_F7,
    WSK_F8,
    WSK_F9,
    WSK_F10,
    WSK_F11,
    WSK_F12,
    WSK_F13 = 0,
    WSK_F14 = 0,
    WSK_F15 = 0,

    /* Key state modifier keys */
    WSK_NUMLOCK = 0,
    WSK_CAPSLOCK = 0,
    WSK_SCROLLOCK = 0,
    WSK_RSHIFT = 0,
    WSK_LSHIFT = 0,
    WSK_RCTRL = 0,
    WSK_LCTRL = 0,
    WSK_RALT = 0,
    WSK_LALT = 0,
    WSK_RMETA = 0,
    WSK_LMETA = 0,

    WSK_LAST = UCHAR_MAX /* GLUT doesn't define a max key, but this is more
			    than enough as of version 3.7 */
} winsys_keysym_t;

typedef enum {
    WS_LEFT_BUTTON,
    WS_MIDDLE_BUTTON,
    WS_RIGHT_BUTTON
} winsys_mouse_button_t;

typedef enum {
    WS_MOUSE_DOWN,
    WS_MOUSE_UP
} winsys_button_state_t;

#endif /* defined( HAVE_SDL ) */


typedef void (*winsys_display_func_t)();
typedef void (*winsys_idle_func_t)();
typedef void (*winsys_reshape_func_t)( int w, int h );
typedef void (*winsys_keyboard_func_t)( unsigned int key, bool_t special,
					bool_t release, int x, int y );
typedef void (*winsys_mouse_func_t)( int button, int state, int x, int y );
typedef void (*winsys_motion_func_t)( int x, int y );

typedef void (*winsys_atexit_func_t)( void );

void winsys_post_redisplay();
void winsys_set_display_func( winsys_display_func_t func );
void winsys_set_idle_func( winsys_idle_func_t func );
void winsys_set_reshape_func( winsys_reshape_func_t func );
void winsys_set_keyboard_func( winsys_keyboard_func_t func );
void winsys_set_mouse_func( winsys_mouse_func_t func );
void winsys_set_motion_func( winsys_motion_func_t func );
void winsys_set_passive_motion_func( winsys_motion_func_t func );

void winsys_swap_buffers();
void winsys_enable_key_repeat( bool_t enabled );
void winsys_warp_pointer( int x, int y );
void winsys_show_cursor( bool_t visible );

void winsys_init( int *argc, char **argv, char *window_title,
		  char *icon_title );
void winsys_shutdown();

void winsys_process_events(); /* Never returns */

void winsys_atexit( winsys_atexit_func_t func );

void winsys_exit( int code );
void winsys_show_preferences( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WINSYS_H */

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
