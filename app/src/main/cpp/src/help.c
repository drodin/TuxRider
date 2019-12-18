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
#include "game_logic_util.h"
#include "fonts.h"
#include "ui_mgr.h"
#include "ui_theme.h"
#include "ui_snow.h"
#include "joystick.h"

#define HELP_MAX_Y -140 * mHeight / 320
#define HELP_MIN_Y 64 * mHeight / 320

typedef struct {
    char *binding;
    char *text;
} help_line_t;

static help_line_t help_lines[] = 
{
{ "credits_h2", "Privacy Policy:" },
{ "credits_text", "We don't collect any personal data from our apps." },
{ "credits_text", "If you have any questions please don't hesitate to" },
{ "credits_text", "contact us: tuxrider@drodin.com" },
};

static scalar_t help_y_offset;


/*---------------------------------------------------------------------------*/
/*! 
 Returns to the game type select screen
 \author  jfpatry
 \date    Created:  2000-09-27
 \date    Modified: 2000-09-27
 */
static void help_go_back() 
{
    set_game_mode( GAME_TYPE_SELECT );
    winsys_post_redisplay();
}


/*---------------------------------------------------------------------------*/
/*! 
 mouse callback
 \author  jfpatry
 \date    Created:  2000-09-27
 \date    Modified: 2000-09-27
 */
void help_mouse_cb( int button, int state, int x, int y )
{
    if ( state == WS_MOUSE_DOWN ) {
        help_go_back();
    }
}


/*---------------------------------------------------------------------------*/
/*! 
 Scrolls the help text up the screen.
 \author  jfpatry
 \date    Created:  2000-09-27
 \date    Modified: 2000-09-27
 */
static void draw_help_text( scalar_t time_step )
{
    int w = getparam_x_resolution();
    int h = getparam_y_resolution();
    font_t *font;
    int i;
    scalar_t y;
    int string_w, asc, desc;
    
    y = HELP_MIN_Y+help_y_offset;
    
    glPushMatrix();
    {
        glTranslatef( w/2, y, 0 );
        
        for (i=0; i<sizeof( help_lines ) / sizeof( help_lines[0] ); i++) {
            help_line_t line = help_lines[i];
            
            if ( !get_font_binding( line.binding, &font ) ) {
                print_warning( IMPORTANT_WARNING,
                              "Couldn't get font for binding %s", 
                              line.binding );
            } else {
                get_font_metrics( font, line.text, &string_w, &asc, &desc );
                
                glTranslatef( 0, -asc, 0 );
                y += -asc;
                
                glPushMatrix();
                {
                    bind_font_texture( font );
                    glTranslatef( -string_w/2, 0, 0 );
                    draw_string( font, line.text );
                }
                glPopMatrix();
                
                glTranslatef( 0, -desc, 0 );
                y += -desc;
            }
        }
        
    }
    glPopMatrix();
    
}

static void help_init(void) 
{
    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( help_mouse_cb );
    winsys_set_motion_func( ui_event_motion_func );
    winsys_set_passive_motion_func( ui_event_motion_func );
    
    help_y_offset = getparam_y_resolution()-160 * mHeight / 320;
    
    play_music( "start_screen" );
}

static void help_loop( scalar_t time_step )
{
    int width, height;
    width = getparam_x_resolution();
    height = getparam_y_resolution();
    
    check_gl_error();
    
    update_audio();
    
    clear_rendering_context();
    
    set_gl_options( GUI );
    
    ui_setup_display();
    
    draw_help_text( time_step );
    
    if (getparam_ui_snow()) {
        update_ui_snow( time_step, False );
        draw_ui_snow();
    }
    
    ui_draw_menu_decorations();
    
    ui_draw();
    
    reshape( width, height );
    
    winsys_swap_buffers();
} 

START_KEYBOARD_CB( help_key_cb )
{
    if ( !release ) {
        help_go_back();
    }
}
END_KEYBOARD_CB

void help_register()
{
    int status = 0;
    
    status |= add_keymap_entry( HELP, 
                               DEFAULT_CALLBACK, 
                               NULL, NULL, help_key_cb );
    
    check_assertion( status == 0, "out of keymap entries" );
    
    register_loop_funcs( HELP, help_init, help_loop, NULL );
}


