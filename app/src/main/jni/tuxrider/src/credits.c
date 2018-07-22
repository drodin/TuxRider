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

#define CREDITS_MAX_Y -140
#define CREDITS_MIN_Y 64

typedef struct {
    char *binding;
    char *text;
} credit_line_t;

static credit_line_t credit_lines[] = 
{
{ "credits_h1", "Tux Rider" },
{ "credits_h1", "Android Edition" },
{ "credits_text", "http://tuxrider.drodin.com" },
{ "credits_text", "" },
{ "credits_text", "Coded by Dmitry Rodin." },
{ "credits_text", "Is a port of iPhone:" },
{ "credits_text", "" },
{ "credits_h1", "Tux Rider" },
{ "credits_h1", "World Challenge" },
{ "credits_text", "http://barlow.server.free.fr" },
{ "credits_text", "" },
{ "credits_text", "Adapted from" },
{ "credits_text", "the open-source project" },
{ "credits_text", "Tux Racer" },
{"credits_text", "tuxracer.sourceforge.net" },
{ "credits_text", "" },
{ "credits_h2", "Tux Racer" },
{ "credits_h2", "Core Development Team" },
{ "credits_text_small", "(Alphabetical Order)" },
{ "credits_text", "Patrick \"Pog\" Gilhuly" },
{ "credits_text", "Eric \"Monster\" Hall" },
{ "credits_text", "Rick Knowles" },
{ "credits_text", "Vincent Ma" },
{ "credits_text", "Jasmin Patry" },
{ "credits_text", "Mark Riddell" },
{ "credits_text", "" },
{ "credits_h2", "Tux Rider World Challenge" },
{ "credits_h2", "Core Development Team" },
{ "credits_text", "Emmanuel de Roux" },
{ "credits_text", "Pedro de Barlow" },
{ "credits_text", "" },
{ "credits_h2", "Music" },
{ "credits_text", "Christian Picon" },
{ "credits_text", " <cpicon92@gmail.com>" },
{ "credits_text", "" },
{ "credits_h2", "Other Data" },
{ "credits_text", "From Extreme Tux Racer" },
{ "credits_text", "www.extremetuxracer.com" },
{ "credits_text", "" },
{ "credits_h2", "Tux Rider World Challenge" },
{ "credits_h2", "Team Wish To Thank:" },
{ "credits_text_small", "(In No Particular Order)" },
{ "credits_text", "All Myosotis 3 staff" },
{ "credits_text", "Two little unkown babys" },
{ "credits_text", "Larry Ewing" },
{ "credits_text", "Thatcher Ulrich" },
{ "credits_text", "Steve Baker" },
{ "credits_text", "Ingo Ruhnke" },
{ "credits_text", "James Barnard" },
{ "credits_text", "Alan Levy" },
{ "credits_text", "University of Waterloo" },
{ "credits_text", "Computer Graphics Lab" },
{ "credits_text", "" },
{ "credits_text_small", "Tux Racer is a trademark" },
{ "credits_text_small", "of Jasmin F. Patry" },
{ "credits_text_small", "Tux Racer is copyright" },
{ "credits_text_small", "1999-2008 Jasmin F. Patry" },
};

static scalar_t y_offset = 0;


/*---------------------------------------------------------------------------*/
/*! 
 Returns to the game type select screen
 \author  jfpatry
 \date    Created:  2000-09-27
 \date    Modified: 2000-09-27
 */
static void go_back() 
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
void mouse_cb( int button, int state, int x, int y )
{
    if ( state == WS_MOUSE_DOWN ) {
        go_back();
    }
}


/*---------------------------------------------------------------------------*/
/*! 
 Scrolls the credits text up the screen.
 \author  jfpatry
 \date    Created:  2000-09-27
 \date    Modified: 2000-09-27
 */
static void draw_credits_text( scalar_t time_step )
{
    int w = getparam_x_resolution();
    int h = getparam_y_resolution();
    font_t *font;
    int i;
    scalar_t y;
    int string_w, asc, desc;
    
    y_offset += time_step * 30;
    y = CREDITS_MIN_Y+y_offset;
    
    glPushMatrix();
    {
        glTranslatef( w/2, y, 0 );
        
        for (i=0; i<sizeof( credit_lines ) / sizeof( credit_lines[0] ); i++) {
            credit_line_t line = credit_lines[i];
            
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
    
    if ( y > h+CREDITS_MAX_Y ) {
        y_offset = 0;
    }
    
}

static void credits_init(void) 
{
    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( mouse_cb );
    winsys_set_motion_func( ui_event_motion_func );
    winsys_set_passive_motion_func( ui_event_motion_func );
    
    y_offset = 0;
    
    play_music( "start_screen" );
}

static void credits_loop( scalar_t time_step )
{
    int width, height;
    width = getparam_x_resolution();
    height = getparam_y_resolution();
    
    check_gl_error();
    
    update_audio();
    
    clear_rendering_context();
    
    set_gl_options( GUI );
    
    ui_setup_display();
    
    draw_credits_text( time_step );
    
    if (getparam_ui_snow()) {
        update_ui_snow( time_step, False );
        draw_ui_snow();
    }
    
    ui_draw_menu_decorations();
    
    ui_draw();
    
    reshape( width, height );
    
    winsys_swap_buffers();
} 

START_KEYBOARD_CB( credits_key_cb )
{
    if ( !release ) {
        go_back();
    }
}
END_KEYBOARD_CB

void credits_register()
{
    int status = 0;
    
    status |= add_keymap_entry( CREDITS, 
                               DEFAULT_CALLBACK, 
                               NULL, NULL, credits_key_cb );
    
    check_assertion( status == 0, "out of keymap entries" );
    
    register_loop_funcs( CREDITS, credits_init, credits_loop, NULL );
}


