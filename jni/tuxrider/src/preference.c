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
#include "preference.h"
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

static listbox_t *sound_listbox = NULL;
static listbox_t *video_listbox = NULL;
static button_t *chancel_btn = NULL;
static button_t *save_btn = NULL;

static list_elem_t cur_sound = NULL;
static list_elem_t cur_video = NULL;

static char* sound_list_elem_to_string_func( list_elem_data_t elem )
{
    switch (*((int*)elem)) {
        case 0:
            return "No";
        case 1:
            return "Yes";
        default:
            return "";
    }
}

static char* video_list_elem_to_string_func( list_elem_data_t elem )
{
    switch (*((int*)elem)) {
        case 0:
            return "Low";
        case 1:
            return "Med";
        case 2:
            return "High";
        default:
            return "";
    }
}

static void sound_listbox_item_change_cb( listbox_t *listbox, void *userdata )
{
    check_assertion( userdata == NULL, "userdata is not null" );
   
    cur_sound = listbox_get_current_item( listbox );     
    //ui_set_dirty();
}

static void video_listbox_item_change_cb( listbox_t *listbox, void *userdata )
{
    check_assertion( userdata == NULL, "userdata is not null" );
    
    cur_video = listbox_get_current_item( listbox );
    //ui_set_dirty();
}

static void chancel_click_cb( button_t *button, void *userdata )
{
    check_assertion( userdata == NULL, "userdata is not null" );
    
    set_game_mode( GAME_TYPE_SELECT );
    
    ui_set_dirty();
}

static void save_click_cb( button_t *button, void *userdata )
{
    check_assertion( userdata == NULL, "userdata is not null" );

    cur_sound = listbox_get_current_item( sound_listbox );
    int snd = *((int*) get_list_elem_data( cur_sound ));

    cur_video = listbox_get_current_item( video_listbox );
    int vid = *((int*) get_list_elem_data( cur_video ));

    setparam_sound_enabled(snd);

    if (getparam_music_enabled() == 1 && snd == 0)
        stopMusic();
    setparam_music_enabled (snd);
    
    saveparamSoundEnabled(snd);

    setparam_video_quality (vid);
    set_video_quality(vid);

    saveparamVideoQuality(vid);
    
    set_game_mode( GAME_TYPE_SELECT );
    
    ui_set_dirty();
}

static void draw_preference()
{
    int h = getparam_y_resolution();
    int box_width, box_height, box_max_y;
    int x_org, y_org;
    char *string;
    font_t *font;
    int text_width, asc, desc;
    list_t cup_list;
    GLuint texobj;
    
    box_width = getparam_x_resolution();
    box_height = 310 * mHeight / 320;
    box_max_y = h - 128 * mHeight / 320;
    x_org = 0;
    y_org = h/2 - box_height/2 + 30 * mHeight / 320;
    
    if ( y_org + box_height > box_max_y ) {
        y_org = box_max_y - box_height;
    }
    
    button_set_position( 
                        chancel_btn,
                        make_point2d( 0,
                                     0 ) );
    
    button_set_position(
                        save_btn,
                        make_point2d( box_width - button_get_width( save_btn ),
                                     0 ) );
    listbox_set_position( 
                         sound_listbox,
                         make_point2d( box_width - 140 * mHeight / 320,
                                      y_org + 290 * mHeight / 320 ) );

    listbox_set_position( 
                         video_listbox,
                         make_point2d( box_width - 140 * mHeight / 320,
                                      y_org + 190 * mHeight / 320 ) );
    
    if ( !get_font_binding( "menu_label", &font ) ) {
        print_warning( IMPORTANT_WARNING,
                      "Couldn't get font for binding menu_label" );
    } else {
        bind_font_texture( font );
        
        string = "Audio enabled";
        
        glPushMatrix();
        {
            glTranslatef( x_org + 70 * mHeight / 320,
                         y_org + 300 * mHeight / 320, 
                         0 );
            
            draw_string( font, string );
        }
        glPopMatrix();

        string = "Video quality";
        
        glPushMatrix();
        {
            glTranslatef( x_org + 70 * mHeight / 320,
                         y_org + 200 * mHeight / 320, 
                         0 );
            
            draw_string( font, string );
        }
        glPopMatrix();
    }
    
}

static void preference_init(void)
{
    list_t sound_list = NULL;
    list_t video_list = NULL;
    point2d_t dummy_pos = {0, 0};

    int* opt1 = malloc(sizeof(int));
    *opt1 = 0;
    int* opt2 = malloc(sizeof(int));
    *opt2 = 1;
    int* opt3 = malloc(sizeof(int));
    *opt3 = 2;

    winsys_set_display_func( main_loop );
    winsys_set_idle_func( main_loop );
    winsys_set_reshape_func( reshape );
    winsys_set_mouse_func( ui_event_mouse_func );
    winsys_set_motion_func( ui_event_motion_func );
    winsys_set_passive_motion_func( ui_event_motion_func );
    
    sound_list = create_list();
    list_elem_t last_sound = get_list_tail( sound_list );
    last_sound = insert_list_elem( sound_list, last_sound, (list_elem_data_t) opt1);
    if (getparam_sound_enabled() == *opt1)
        cur_sound = last_sound;
    last_sound = insert_list_elem( sound_list, last_sound, (list_elem_data_t) opt2);
    if (getparam_sound_enabled() == *opt2)
        cur_sound = last_sound;

    video_list = create_list();
    list_elem_t last_video = get_list_tail( video_list );
    last_video = insert_list_elem( video_list, last_video, (list_elem_data_t) opt1);
    if (getparam_video_quality() == *opt1)
        cur_video = last_video;
    last_video = insert_list_elem( video_list, last_video, (list_elem_data_t) opt2);
    if (getparam_video_quality() == *opt2)
        cur_video = last_video;
    last_video = insert_list_elem( video_list, last_video, (list_elem_data_t) opt3);    
    if (getparam_video_quality() == *opt3)
        cur_video = last_video;

    chancel_btn = button_create( dummy_pos,
                             80 * mHeight / 320, 48 * mHeight / 320, 
                             "button_label", 
                             "Back" );
    button_set_hilit_font_binding( chancel_btn, "button_label_hilit" );
    button_set_visible( chancel_btn, True );
    button_set_click_event_cb( chancel_btn, chancel_click_cb, NULL );
    
    save_btn = button_create( dummy_pos,
                                 80 * mHeight / 320, 48 * mHeight / 320,
                                 "button_label",
                                 "Save" );
    button_set_hilit_font_binding( save_btn, "button_label_hilit" );
    button_set_disabled_font_binding( save_btn, "button_label_disabled" );
    button_set_visible( save_btn, True );
    button_set_click_event_cb( save_btn, save_click_cb, NULL );

    sound_listbox = listbox_create( dummy_pos,
                                   120 * mHeight / 320, 44 * mHeight / 320,
                                   "listbox_item",
                                   sound_list,
                                   sound_list_elem_to_string_func );
    
    listbox_set_current_item( sound_listbox, cur_sound );
    
    listbox_set_item_change_event_cb( sound_listbox, 
                                     sound_listbox_item_change_cb, 
                                     NULL );
    
    listbox_set_visible( sound_listbox, True );


    
    video_listbox = listbox_create( dummy_pos,
                                 120 * mHeight / 320, 44 * mHeight / 320,
                                 "listbox_item",
                                 video_list,
                                 video_list_elem_to_string_func );
    
    listbox_set_current_item( video_listbox, cur_video );
    
    listbox_set_item_change_event_cb( video_listbox, 
                                     video_listbox_item_change_cb, 
                                     NULL );
    
    listbox_set_visible( video_listbox, True );
    
    play_music( "start_screen" );
}

static void preference_loop( scalar_t time_step )
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
    
    draw_preference();
    
    ui_draw();
    
    reshape( getparam_x_resolution(), getparam_y_resolution() );
    
    winsys_swap_buffers();
}

static void preference_term(void)
{
    button_delete( chancel_btn );
    chancel_btn = NULL;
    
    button_delete( save_btn );
    save_btn = NULL;

    listbox_delete( sound_listbox );
    sound_listbox = NULL;

    listbox_delete( video_listbox );
    video_listbox = NULL;
}

START_KEYBOARD_CB( preference_key_cb )
{
    if ( release ) {
        return;
    }
    
    if ( special ) {
        switch( key ) {
            case WSK_UP:
                listbox_goto_prev_item( sound_listbox );
                break;
            case WSK_DOWN:
                listbox_goto_next_item( sound_listbox );
                break;
            case WSK_RIGHT:
                listbox_goto_next_item( video_listbox );
                break;
            case WSK_LEFT:
                listbox_goto_prev_item( video_listbox );
                break;
        }
    } else {
        switch (key) {
            case 13: /* Enter */
            case 'e':
                if ( save_btn ) {
                    button_simulate_mouse_click( save_btn );
                    ui_set_dirty();
                }
                break;
            case 27: /* Esc */
            case 'q':
                if ( chancel_btn ) {
                    button_simulate_mouse_click( chancel_btn );
                    ui_set_dirty();
                }
                break;
        }
    }
    
    ui_check_dirty();
}
END_KEYBOARD_CB

void preference_register()
{
    int status = 0;
    
    status |= 
	add_keymap_entry( PREFERENCE,
                     DEFAULT_CALLBACK, NULL, NULL, preference_key_cb );
    
    check_assertion( status == 0,
                    "out of keymap entries" );
    
    register_loop_funcs( PREFERENCE,
                        preference_init,
                        preference_loop,
                        preference_term );
}



/* EOF */
