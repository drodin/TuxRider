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
#include "event_select.h"
#include "ui_mgr.h"
#include "ui_theme.h"
#include "button.h"
#include "listbox.h"
#include "loop.h"
#include "render_util.h"
#include "audio.h"
#include "gl_util.h"
#include "keyboard.h"
#include "multiplayer.h"
#include "course_load.h"
#include "fonts.h"
#include "textures.h"
#include "course_mgr.h"
#include "save.h"
#include "ui_snow.h"
#include "joystick.h"

static listbox_t *event_listbox = NULL;
static listbox_t *cup_listbox = NULL;
static button_t *back_btn = NULL;
static button_t *continue_btn = NULL;
/*
#ifdef __APPLE__
static button_t *howToPlay_btn = NULL;
#endif
*/
static list_elem_t cur_event = NULL;
static event_data_t *event_data = NULL;
static list_elem_t cur_cup = NULL;

static char* event_list_elem_to_string_func( list_elem_data_t elem )
{
    return get_event_name( (event_data_t*) elem );
}

static char* cup_list_elem_to_string_func( list_elem_data_t elem )
{
    return get_cup_name( (cup_data_t*) elem );
}


/*---------------------------------------------------------------------------*/
/*! 
 Updates the enabled states of the buttons
 \author  jfpatry
 \date    Created:  2000-09-24
 \date    Modified: 2000-09-24
 */
static void update_button_enabled_states( )
{
    if ( continue_btn == NULL ) {
        return;
    }
    
    if ( is_cup_complete( event_data, cur_cup ) ||
        is_cup_first_incomplete_cup( event_data, cur_cup ) )
    {
        button_set_enabled( continue_btn, True );
    } else {
        button_set_enabled( continue_btn, False );
    }
}


static void set_cur_cup_to_first_incomplete( event_data_t *event_data,
                                            list_t cup_list ) 
{
    cur_cup = get_last_complete_cup_for_event( event_data );
    
    if ( cur_cup == NULL ) {
        cur_cup = get_list_head( cup_list );
    } else if ( cur_cup != get_list_tail( cup_list ) ) {
        cur_cup = get_next_list_elem( cup_list, cur_cup ); 
    }
}

static void event_listbox_item_change_cb( listbox_t *listbox, void *userdata )
{
    list_elem_t cur_event;
    list_t cup_list;
    
    check_assertion( userdata == NULL, "userdata is not null" );
    
    cur_event = listbox_get_current_item( listbox );
    event_data = (event_data_t*) get_list_elem_data( cur_event );
    
    cup_list =  get_event_cup_list( event_data );
    listbox_set_item_list( cup_listbox, cup_list,
                          cup_list_elem_to_string_func );
    
    set_cur_cup_to_first_incomplete( event_data, cup_list );
    
    listbox_set_current_item( cup_listbox, cur_cup );
    
    update_button_enabled_states();
    
    ui_set_dirty();
}

static void cup_listbox_item_change_cb( listbox_t *listbox, void *userdata )
{
    check_assertion( userdata == NULL, "userdata is not null" );
    
    cur_cup = listbox_get_current_item( listbox );
    
    update_button_enabled_states();
}

static void back_click_cb( button_t *button, void *userdata )
{
    check_assertion( userdata == NULL, "userdata is not null" );
    
    set_game_mode( GAME_TYPE_SELECT );
    
    ui_set_dirty();
}

static void continue_click_cb( button_t *button, void *userdata )
{
    cup_data_t *cup_data;
    player_data_t *plyr = get_player_data( local_player() );
    
    check_assertion( userdata == NULL, "userdata is not null" );
    
    cur_event = listbox_get_current_item( event_listbox );
    event_data = (event_data_t*) get_list_elem_data( cur_event );
    
    cur_cup = listbox_get_current_item( cup_listbox );
    cup_data = (cup_data_t*) get_list_elem_data( cur_cup );
    
    g_game.current_event = get_event_name( event_data );
    g_game.current_cup = get_cup_name( cup_data );
    
    plyr->lives = INIT_NUM_LIVES;
    
    set_game_mode( RACE_SELECT );
    
    ui_set_dirty();
}

/*
#ifdef __APPLE__
#include "sharedGeneralFunctions.h"
static void howToPlay_click_cb( button_t *button, void *userdata )
{
    check_assertion( userdata == NULL, "userdata is not null" );
    
    showHowToPlay();
}
#endif
*/

static void set_widget_positions_and_draw_decorations()
{
    int h = getparam_y_resolution();
    int box_width, box_height, box_max_y;
    int x_org, y_org;
    char *string;
    font_t *font;
    int text_width, asc, desc;
    list_t cup_list;
    GLuint texobj;
    
#ifdef __APPLE__
    /* set the dimensions of the box in which all widgets should fit */
    box_height = 200 * mHeight / 320;
    box_width = getparam_x_resolution();
    box_max_y = h - 150  * mHeight / 320;
    
    button_set_position( 
                        back_btn,
                        make_point2d( 0,
                                     0 ) );
    
    button_set_position(
                        continue_btn,
                        make_point2d( box_width - button_get_width( continue_btn ),
                                     0 ) );
                                     
    /*
    button_set_position(
                        howToPlay_btn,
                        make_point2d( x_org + 160 - button_get_width( continue_btn )/2.0,
                                     100 ) );
    */
    //Out of the field of view
    //in Tux Rider World Challenge, there is no more events and cup, just training
    //I'm adapting from The Tux Racer system by selecting the first and only event
    //I hide its list box out of the field of view
    //and each cup for this event is a training
    
    listbox_set_position( 
                         event_listbox,
                         make_point2d( 1000,
                                       1000 ) );
    
    listbox_set_position( 
                         cup_listbox,
                         make_point2d( box_width/2.0 - 100 * mHeight / 320 ,
                                       box_height/2.0 + 10 * mHeight / 320) );
    
#else
    int w = getparam_x_resolution();

    /* set the dimensions of the box in which all widgets should fit */
    box_width = 460;
    box_height = 310;
    box_max_y = h - 128;
    
    x_org = w/2 - box_width/2;
    y_org = h/2 - box_height/2;
    if ( y_org + box_height > box_max_y ) {
        y_org = box_max_y - box_height;
    }
    
    button_set_position( 
                        back_btn,
                        make_point2d( x_org + 131 - button_get_width( back_btn )/2.0,
                                     42 ) );
    
    button_set_position(
                        continue_btn,
                        make_point2d( x_org + 329 - button_get_width( continue_btn )/2.0,
                                     42 ) );
    
    listbox_set_position( 
                         cup_listbox,
                         make_point2d( x_org + 52,
                                      y_org + 103 ) );
    
    listbox_set_position( 
                         event_listbox,
                         make_point2d( x_org + 52,
                                      y_org + 193 ) );
    
#endif
    
    /* 
     * Draw other stuff 
     */
    
    /* Event & cup icons */
    if ( !get_texture_binding( get_event_icon_texture_binding( event_data ),
                              &texobj ) ) {
        texobj = 0;
    }
    
    glBindTexture( GL_TEXTURE_2D, texobj );
    
    glBegin( GL_QUADS );
    {
        point2d_t tll, tur;
        point2d_t ll, ur;
        
#ifdef __APPLE__
        //out of the field of view
        ll = make_point2d( 0, 1000 );
        ur = make_point2d( 44, 1000 + 44 );
        tll = make_point2d( 0, 0 );
        tur = make_point2d( 44.0/64.0, 44.0/64.0 );
#else
        ll = make_point2d( x_org, y_org + 193 );
        ur = make_point2d( x_org + 44, y_org + 193 + 44 );
        tll = make_point2d( 0, 0 );
        tur = make_point2d( 44.0/64.0, 44.0/64.0 );
#endif
        
        glTexCoord2f( tll.x, tll.y );
        glVertex2f( ll.x, ll.y );
        
        glTexCoord2f( tur.x, tll.y );
        glVertex2f( ur.x, ll.y );
        
        glTexCoord2f( tur.x, tur.y );
        glVertex2f( ur.x, ur.y );
        
        glTexCoord2f( tll.x, tur.y );
        glVertex2f( ll.x, ur.y );
    }
    glEnd();
    
    
    if ( !get_texture_binding( 
                              get_cup_icon_texture_binding( 
                                                           (cup_data_t*) get_list_elem_data( cur_cup ) ), &texobj ) ) 
    {
        texobj = 0;
    }
    
    glBindTexture( GL_TEXTURE_2D, texobj );
    
#ifdef __APPLE__DISABLED__
    {
        point2d_t tll, tur;
        point2d_t ll, ur;
        
        ll = make_point2d( x_org, y_org + 103 );
        ur = make_point2d( x_org + 44, y_org + 103 + 44 );
        tll = make_point2d( 0, 0 );
        tur = make_point2d( 44.0/64.0, 44.0/64.0 );
        
        const GLfloat vertices []=
        {
            ll.x, ll.y,
            ur.x, ll.y,
            ur.x, ur.y,
            ll.x, ur.y
        };
        
        const GLfloat texCoords []=
        {
            tll.x, tll.y,
            tur.x, tll.y,
            tur.x, tur.y,
            tll.x, tur.y,
        };
        
        glEnableClientState (GL_VERTEX_ARRAY);
        glVertexPointer (2, GL_FLOAT , 0, vertices);	
        glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
#else
    glBegin( GL_QUADS );
    {
        point2d_t tll, tur;
        point2d_t ll, ur;
        
#ifdef __APPLE__
        ll = make_point2d( 70 * mHeight / 320, box_height/2.0 + 10 * mHeight / 320 );
        ur = make_point2d( 70 * mHeight / 320 + 44 * mHeight / 320, box_height/2.0 + 10 * mHeight / 320 + 44 * mHeight / 320 );
        tll = make_point2d( 0, 0 );
        tur = make_point2d( 44.0/64.0, 44.0/64.0 );
#else
        ll = make_point2d( x_org, y_org + 103 );
        ur = make_point2d( x_org + 44, y_org + 103 + 44 );
        tll = make_point2d( 0, 0 );
        tur = make_point2d( 44.0/64.0, 44.0/64.0 );
#endif
        
        glTexCoord2f( tll.x, tll.y );
        glVertex2f( ll.x, ll.y );
        
        glTexCoord2f( tur.x, tll.y );
        glVertex2f( ur.x, ll.y );
        
        glTexCoord2f( tur.x, tur.y );
        glVertex2f( ur.x, ur.y );
        
        glTexCoord2f( tll.x, tur.y );
        glVertex2f( ll.x, ur.y );
    }
    glEnd();
#endif
    
    if ( !get_font_binding( "menu_label", &font ) ) {
        print_warning( IMPORTANT_WARNING,
                      "Couldn't get font for binding menu_label" );
    } else {
        bind_font_texture( font );
#ifdef __APPLE__
        string = "Select a training";
#else
        string = "Select event and cup";
#endif
        get_font_metrics( font, string, &text_width,  &asc, &desc );
        
        glPushMatrix();
        {
            glTranslatef( box_width/2.0 - text_width/2.0,
                         190 * mHeight / 320, 
                         0 );
            
            draw_string( font, string );
        }
        glPopMatrix();
    }
    
    if ( !get_font_binding( "event_and_cup_label", &font ) ) {
        print_warning( IMPORTANT_WARNING,
                      "Couldn't get font for binding menu_label" );
    } 
#ifdef __APPLE__
    
    cur_event = listbox_get_current_item( event_listbox );
    event_data = (event_data_t*) get_list_elem_data( cur_event );
    cup_list = get_event_cup_list( event_data );
    cur_cup = listbox_get_current_item( cup_listbox );
    
    if ( is_cup_complete( event_data, cur_cup ) ) {
        string = "You've won this Training !";
    } else if ( is_cup_first_incomplete_cup( event_data, cur_cup ) ) {
        string = "You must complete this training first";
    } else {
        string = "You cannot enter this training yet"; 
    }
    
    if ( !get_font_binding( "cup_status", &font ) ) {
        print_warning( IMPORTANT_WARNING,
                      "Couldn't get font for binding cup_status" );
    } else {
        bind_font_texture( font );
        
        get_font_metrics( font, string, &text_width, &asc, &desc );
        
        glPushMatrix();
        {
            glTranslatef( box_width/2.0 - text_width/2.0,
                         70 * mHeight / 320, 
                         0 );
            
            draw_string( font, string );
        }
        glPopMatrix();
    }
    
#else
    else {
        bind_font_texture( font );
        string = "Event:";
        get_font_metrics( font, string, &text_width,  &asc, &desc );
        
        glPushMatrix();
        {
            glTranslatef( x_org,
                         y_org + 193 + 44 + 5 + desc, 
                         0 );
            
            draw_string( font, string );
        }
        glPopMatrix();
        
        string = "Cup:";
        
        glPushMatrix();
        {
            glTranslatef( x_org,
                         y_org + 103 + 44 + 5 + desc, 
                         0 );
            
            draw_string( font, string );
        }
        glPopMatrix();
    }
    
    cur_event = listbox_get_current_item( event_listbox );
    event_data = (event_data_t*) get_list_elem_data( cur_event );
    cup_list = get_event_cup_list( event_data );
    cur_cup = listbox_get_current_item( cup_listbox );
    
    if ( is_cup_complete( event_data, cur_cup ) ) {
        string = "You've won this cup!";
    } else if ( is_cup_first_incomplete_cup( event_data, cur_cup ) ) {
        string = "You must complete this cup next";
    } else {
        string = "You cannot enter this cup yet"; 
    }
    
    if ( !get_font_binding( "cup_status", &font ) ) {
        print_warning( IMPORTANT_WARNING,
                      "Couldn't get font for binding cup_status" );
    } else {
        bind_font_texture( font );
        
        get_font_metrics( font, string, &text_width, &asc, &desc );
        
        glPushMatrix();
        {
            glTranslatef( x_org + box_width/2.0 - text_width/2.0,
                         y_org + 70, 
                         0 );
            
            draw_string( font, string );
        }
        glPopMatrix();
    }
#endif
}

static void event_select_init(void)
{
    list_t event_list = NULL;
    list_t cup_list = NULL;
    point2d_t dummy_pos = {0, 0};
    
    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( ui_event_mouse_func );
    winsys_set_motion_func( ui_event_motion_func );
    winsys_set_passive_motion_func( ui_event_motion_func );
    
    event_list = get_events_list();
    
    if ( g_game.prev_mode != RACE_SELECT ) {
        cur_event = get_list_head( event_list );
        event_data = (event_data_t*)get_list_elem_data( cur_event );
        cup_list = get_event_cup_list( event_data );
        
        set_cur_cup_to_first_incomplete( event_data, cup_list );
    } else {
        event_data = (event_data_t*)get_list_elem_data( cur_event );
        cup_list = get_event_cup_list( event_data );
    }
    
    
    
    /* 
     * Create widgets 
     */
    
    /* back button */
    back_btn = button_create( dummy_pos,
                             80 * mWidth / 480, 48 * mHeight / 320, 
                             "button_label", 
                             (mWidth>320)?"Back":"<< " );
    button_set_hilit_font_binding( back_btn, "button_label_hilit" );
    button_set_visible( back_btn, True );
    button_set_click_event_cb( back_btn, back_click_cb, NULL );
    
    /* continue button */
    continue_btn = button_create( dummy_pos,
                                 80 * mWidth / 480, 48 * mHeight / 320,
                                 "button_label",
                                 (mWidth>320)?"Go":" >>" );
    button_set_hilit_font_binding( continue_btn, "button_label_hilit" );
    button_set_disabled_font_binding( continue_btn, "button_label_disabled" );
    button_set_visible( continue_btn, True );
    button_set_click_event_cb( continue_btn, continue_click_cb, NULL );

/*
#ifdef __APPLE__
    howToPlay_btn = button_create( dummy_pos,
                                  150, 40, 
                                  "button_label", 
                                  "How to play ?" );
    button_set_hilit_font_binding( howToPlay_btn, "button_label_hilit" );
    button_set_visible( howToPlay_btn, True );
    button_set_click_event_cb( howToPlay_btn, howToPlay_click_cb, NULL );
#endif
*/
    
    /* event listbox */
#ifdef __APPLE__
    event_listbox = listbox_create( dummy_pos,
                                   (300 - 52)  * mHeight / 320, 44 * mHeight / 320,
                                   "listbox_item",
                                   event_list,
                                   event_list_elem_to_string_func );
    
#else
    event_listbox = listbox_create( dummy_pos,
                                   (300 - 52) * mHeight / 320, 44 * mHeight /320,
                                   "listbox_item",
                                   event_list,
                                   event_list_elem_to_string_func );
    
#endif
    
    listbox_set_current_item( event_listbox, cur_event );
    
    listbox_set_item_change_event_cb( event_listbox, 
                                     event_listbox_item_change_cb, 
                                     NULL );
    
    listbox_set_visible( event_listbox, True );
    
    /* cup listbox */
    
#ifdef __APPLE__
    cup_listbox = listbox_create( dummy_pos,
                                 (300 - 22) * mHeight / 320, 44 * mHeight / 320,
                                 "listbox_item",
                                 cup_list,
                                 cup_list_elem_to_string_func );
    
    
#else
    cup_listbox = listbox_create( dummy_pos,
                                 (460 - 52) * mHeight / 320, 44 * mHeight / 320,
                                 "listbox_item",
                                 cup_list,
                                 cup_list_elem_to_string_func );
    
#endif
    
    
    listbox_set_current_item( cup_listbox, cur_cup );
    
    listbox_set_item_change_event_cb( cup_listbox, 
                                     cup_listbox_item_change_cb, 
                                     NULL );
    
    listbox_set_visible( cup_listbox, True );
    
    update_button_enabled_states();
    
    play_music( "start_screen" );
}

static void event_select_loop( scalar_t time_step )
{
    check_gl_error();
    
    update_audio();
    
    set_gl_options( GUI );
    
    clear_rendering_context();
    
    ui_setup_display();
    
    if (getparam_ui_snow()) {
        update_ui_snow( time_step, False );
        draw_ui_snow();
    }
    
    ui_draw_menu_decorations();
    
    set_widget_positions_and_draw_decorations();
    
    ui_draw();
    
    reshape( getparam_x_resolution(), getparam_y_resolution() );
    
    winsys_swap_buffers();
}

static void event_select_term(void)
{
    button_delete( back_btn );
    back_btn = NULL;
    
    button_delete( continue_btn );
    continue_btn = NULL;
/*
#ifdef __APPLE__
    button_delete( howToPlay_btn );
    continue_btn = NULL;
#endif
*/

    listbox_delete( event_listbox );
    event_listbox = NULL;
    
    listbox_delete( cup_listbox );
    cup_listbox = NULL;
}

START_KEYBOARD_CB( event_select_key_cb )
{
    if ( release ) {
        return;
    }
    
    if ( special ) {
        switch( key ) {
            /*
            case WSK_LEFT:
                listbox_goto_prev_item( event_listbox );
                break;
            case WSK_RIGHT:
                listbox_goto_next_item( event_listbox );
                break;
            */
            case WSK_RIGHT:
                listbox_goto_next_item( cup_listbox );
                break;
            case WSK_LEFT:
                listbox_goto_prev_item( cup_listbox );
                break;
        }
    } else {
        switch (key) {
            case '0': /* Enter */
            case 'e':
                if ( continue_btn ) {
                    button_simulate_mouse_click( continue_btn );
                    ui_set_dirty();
                }
                break;
            case 27: /* Esc */
            case 'q':
                if ( back_btn ) {
                    button_simulate_mouse_click( back_btn );
                    ui_set_dirty();
                }
                break;
        }
    }
    
    ui_check_dirty();
}
END_KEYBOARD_CB

void event_select_register()
{
    int status = 0;
    
    status |= 
	add_keymap_entry( EVENT_SELECT,
                     DEFAULT_CALLBACK, NULL, NULL, event_select_key_cb );
    
    check_assertion( status == 0,
                    "out of keymap entries" );
    
    register_loop_funcs( EVENT_SELECT,
                        event_select_init,
                        event_select_loop,
                        event_select_term );
}



/* EOF */
