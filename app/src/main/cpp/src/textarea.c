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
#include "textarea.h"
#include "ui_mgr.h"
#include "ui_theme.h"
#include "button.h"
#include "list.h"
#include "fonts.h"
#include "render_util.h"

#ifdef __APPLE__
#define DEFAULT_ARROW_BUTTON_HEIGHT 15 * 2
#define DEFAULT_ARROW_BUTTON_WIDTH  27
#define DEFAULT_ARROW_REGION_WIDTH  36
#define DEFAULT_ARROW_VERT_SEPARATION  4
#define DEFAULT_BORDER_WIDTH 2
#define DEFAULT_TEXT_PAD 4
#else
#define DEFAULT_ARROW_BUTTON_HEIGHT 15
#define DEFAULT_ARROW_BUTTON_WIDTH  27
#define DEFAULT_ARROW_REGION_WIDTH  36
#define DEFAULT_ARROW_VERT_SEPARATION  4
#define DEFAULT_BORDER_WIDTH 4
#define DEFAULT_TEXT_PAD 4
#endif

struct textarea_ {
    point2d_t pos;
    scalar_t w;
    scalar_t h;
    scalar_t arrow_width;
    scalar_t border_width;
    scalar_t text_pad;
    scalar_t text_region_height; /* calculated */
    scalar_t text_region_width; /* calculated */
    char *font_binding;
    colour_t border_colour;
    colour_t background_colour;
    button_t *up_button;
    button_t *down_button;
    char *text_orig;
    char *text_lines;
    list_t line_list;
    list_elem_t top_line;
    bool_t visible;
    bool_t active;
};


/*---------------------------------------------------------------------------*/
/*! 
  Calculates the dimensions of the area in which text is drawn
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static void calc_text_region_dims( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    ta->text_region_width = ta->w - 2*ta->border_width -
	2*ta->text_pad - ta->arrow_width;
    ta->text_region_height = ta->h - 2*ta->border_width -
	2*ta->text_pad;
}


/*---------------------------------------------------------------------------*/
/*! 
  Calculates the width of a string in pixels
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static int string_width( char *string, font_t *font )
{
    int w, asc, desc;
    get_font_metrics( font, string, &w, &asc, &desc );
    return w;
}


/*---------------------------------------------------------------------------*/
/*! 
  Breaks the text up into lines
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static void create_lines( textarea_t *ta )
{
    char *cur_ptr;
    char *old_space_ptr;
    char *space_ptr;
    list_elem_t cur_line;
    font_t *font;

    check_assertion( ta != NULL, "ta is NULL" );

    if ( !get_font_binding( ta->font_binding, &font ) ) {
	print_warning( IMPORTANT_WARNING,
		       "Couldn't get font for binding %s", ta->font_binding );
	return;
    }

    if ( ta->text_lines != NULL ) {
	free( ta->text_lines );
	ta->text_lines = NULL;
    }

    if ( ta->line_list != NULL ) {
	del_list( ta->line_list );
	ta->line_list = NULL;
    }

    ta->text_lines = string_copy( ta->text_orig );
    ta->line_list = create_list();

    cur_ptr = ta->text_lines;
    cur_line = NULL;

    while ( *cur_ptr != '\0' ) {
	old_space_ptr = NULL;
	space_ptr = strchr( cur_ptr, ' ' );
	    
	while (True) {

	    if ( space_ptr != NULL ) {
		*space_ptr = '\0';
	    }

	    if ( string_width( cur_ptr, font ) > ta->text_region_width ) {
		break;
	    }

	    old_space_ptr = space_ptr;

	    if ( space_ptr == NULL ) {
		/* Entire string fits in area */
		break;
	    }

	    *space_ptr = ' ';

	    space_ptr = strchr( space_ptr+1, ' ' );
	}

	if ( old_space_ptr == NULL ) {
	    /* Either string is too wide for area, or the entire 
	       remaining portion of string fits in area (space_ptr == NULL).
	    */
	    cur_line = insert_list_elem( ta->line_list, cur_line,
					 (list_elem_data_t) cur_ptr );
	    cur_ptr += strlen( cur_ptr );

	    if ( space_ptr != NULL ) {
		/* Advance past the NULL since there's more string left */
		cur_ptr += 1;
	    }
	} else {
	    if ( space_ptr != NULL ) {
		*space_ptr = ' ';
	    }
	    *old_space_ptr = '\0';
	    cur_line = insert_list_elem( ta->line_list, cur_line,
					 (list_elem_data_t) cur_ptr );
	    cur_ptr = old_space_ptr + 1; 
	}
    }

    ta->top_line = get_list_head( ta->line_list );
}



/*---------------------------------------------------------------------------*/
/*! 
  Draws the lines in the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static void draw_text_lines( textarea_t *ta, font_t *font )
{
    list_elem_t cur_line;
    int cur_y;
    int w, asc, desc;
    char *line;
    int height_used;

    check_assertion( ta != NULL, "ta is NULL" );

    cur_line = ta->top_line;

    if ( cur_line == NULL ) {
	return;
    }

    bind_font_texture( font );

    line = (char*) get_list_elem_data( cur_line );

    check_assertion( line != NULL, "line is NULL in line_list" );

    get_font_metrics( font, line, &w, &asc, &desc );

    glPushMatrix();
    {
	cur_y = ta->pos.y + ta->h - ta->border_width - ta->text_pad - asc;
	glTranslatef( ta->pos.x + ta->border_width + ta->text_pad ,
		      cur_y,
		      0 );
	height_used = asc + desc;
	    
	while (True) {
	    if ( height_used > ta->text_region_height ) {
		if ( ta->down_button ) {
		    button_set_enabled( ta->down_button, True );
		}
		break;
	    }

	    get_font_metrics( font, line, &w, &asc, &desc );
	    draw_string( font, line );

	    cur_line = get_next_list_elem( ta->line_list, cur_line );

	    if ( cur_line == NULL ) {
		if ( ta->down_button ) {
		    button_set_enabled( ta->down_button, False );
		}
		break;
	    }

	    line = (char*) get_list_elem_data( cur_line );

	    glTranslatef( -w, -(asc+desc), 0 );

	    height_used += asc + desc;
	}
    }
    glPopMatrix();
}


/*---------------------------------------------------------------------------*/
/*! 
  Draws the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
void textarea_draw( textarea_t *ta )
{
    font_t *font;

    check_assertion( ta != NULL, "ta is NULL" );

    glDisable( GL_TEXTURE_2D );

    if(ta->background_colour.a != 0.) {
        glColor3dv( (scalar_t*)&ta->border_colour );

        glRectf( ta->pos.x, 
             ta->pos.y,
             ta->pos.x + ta->w,
             ta->pos.y + ta->h );

        glColor3dv( (scalar_t*)&ta->background_colour );

        glRectf( ta->pos.x + ta->border_width, 
             ta->pos.y + ta->border_width,
             ta->pos.x + ta->w - ta->border_width,
             ta->pos.y + ta->h - ta->border_width );
    }

    glEnable( GL_TEXTURE_2D );

    if ( !get_font_binding( ta->font_binding, &font ) ) {
	print_warning( IMPORTANT_WARNING,
		       "Couldn't get font object for binding %s",
		       ta->font_binding );
    } else {
	draw_text_lines( ta, font );
    }

    if ( ta->up_button != NULL && button_is_enabled(ta->up_button) ) {
	button_draw( ta->up_button );
    }

    if ( ta->down_button != NULL && button_is_enabled(ta->down_button) ) {
	button_draw( ta->down_button ); 
    }
}


/*---------------------------------------------------------------------------*/
/*! 
  Draw callback for textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static void textarea_draw_cb( void *widget )
{
    check_assertion( widget != NULL, "widget is NULL" );

    textarea_draw( (textarea_t*) widget );
}


/*---------------------------------------------------------------------------*/
/*! 
  Updates the enabled states of the arrow buttons
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static void update_button_enabled_states( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    if ( ta->top_line == NULL ) {
	/* No lines */
	button_set_enabled( ta->up_button, False );
	button_set_enabled( ta->down_button, False );
    } else {
	if ( ta->top_line == get_list_head( ta->line_list ) ) {
	    button_set_enabled( ta->up_button, False );
	} else {
	    button_set_enabled( ta->up_button, True );
	}

	if ( ta->top_line == get_list_tail( ta->line_list ) ) {
	    button_set_enabled( ta->down_button, False );
	} else {
	    button_set_enabled( ta->down_button, True );
	}
    }
}


/*---------------------------------------------------------------------------*/
/*! 
  Updates the positions of the arrow buttons
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static void update_button_positions( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    button_set_position( 
	ta->up_button,
	make_point2d(
	    ta->pos.x + ta->w - ta->border_width -
	    button_get_width( ta->up_button ),
	    ta->pos.y + ta->h - ta->border_width -
	    button_get_height( ta->up_button ) ) );
    button_set_position(
	ta->down_button,
	make_point2d(
	    ta->pos.x + ta->w - ta->border_width -
	    button_get_width( ta->down_button ),
	    ta->pos.y + ta->border_width ) );
}


/*---------------------------------------------------------------------------*/
/*! 
  Callback called when one of arrow buttons is pressed.
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
static void textarea_arrow_click_cb( button_t *button, void *userdata )
{
    textarea_t *ta = (textarea_t*) userdata;

    check_assertion( ta != NULL, "ta is NULL" );

    if ( button == ta->up_button ) {
	ta->top_line = get_prev_list_elem( ta->line_list,
					   ta->top_line );
    } else if ( button == ta->down_button ) {
	ta->top_line = get_next_list_elem( ta->line_list,
					   ta->top_line );
    } else {
	check_assertion( 0, "textarea arrow click callback called with "
			 "unknown button" );
    }
    
    update_button_enabled_states( ta );

    ui_set_dirty();
}

/*---------------------------------------------------------------------------*/
/*! 
  Creates a textarea widget
  \return  New textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
textarea_t* textarea_create( point2d_t pos, scalar_t w, scalar_t h,
			     char *font_binding, char *text )
{
    textarea_t *ta;
    char *binding;
    point2d_t ll, ur;

    ta = (textarea_t*)malloc( sizeof(textarea_t) );

    check_assertion( ta != NULL, "out of memory" );

    ta->pos = pos;
    ta->w = w;
    ta->h = h;
    ta->arrow_width = DEFAULT_ARROW_REGION_WIDTH;
    ta->border_width = DEFAULT_BORDER_WIDTH;
    ta->text_pad = DEFAULT_TEXT_PAD;

    calc_text_region_dims( ta );

    ta->font_binding = font_binding;
    ta->border_colour = ui_foreground_colour;
    ta->background_colour = ui_background_colour;

    /* 
     * Create buttons 
     */
    ta->up_button = button_create(
	make_point2d( 0, 0 ), /* position will be set later */
	DEFAULT_ARROW_BUTTON_WIDTH,
	DEFAULT_ARROW_BUTTON_HEIGHT,
	NULL,
	NULL );

    binding = "textarea_arrows";

    ll = make_point2d( 2.0/64.0, 17.0/64.0 );
    ur = make_point2d( 29.0/64.0, 32.0/64.0 );
    button_set_image( ta->up_button, binding, ll, ur, white );

    ll = make_point2d( 34.0/64.0, 17.0/64.0 );
    ur = make_point2d( 61.0/64.0, 32.0/64.0 );
    button_set_disabled_image( ta->up_button, binding, ll, ur, 
			       white );

    ll = make_point2d( 34.0/64.0, 49.0/64.0 );
    ur = make_point2d( 61.0/64.0, 64.0/64.0 );
    button_set_hilit_image( ta->up_button, binding, ll, ur, 
			    white );

    ll = make_point2d( 2.0/64.0, 49.0/64.0 );
    ur = make_point2d( 29.0/64.0, 64.0/64.0 );
    button_set_clicked_image( ta->up_button, binding, ll, ur, 
			      white );

    button_set_click_event_cb( ta->up_button,
			       textarea_arrow_click_cb,
			       ta );

    ta->down_button = button_create(
	make_point2d( 0, 0 ), /* position will be set later */
	DEFAULT_ARROW_BUTTON_WIDTH,
	DEFAULT_ARROW_BUTTON_HEIGHT,
	NULL,
	NULL );

    binding = "textarea_arrows";

    ll = make_point2d( 2.0/64.0, 1.0/64.0 );
    ur = make_point2d( 29.0/64.0, 16.0/64.0 );
    button_set_image( ta->down_button, binding, ll, ur, white );

    ll = make_point2d( 34.0/64.0, 1.0/64.0 );
    ur = make_point2d( 61.0/64.0, 16.0/64.0 );
    button_set_disabled_image( ta->down_button, binding, ll, ur, 
			       white );

    ll = make_point2d( 34.0/64.0, 33.0/64.0 );
    ur = make_point2d( 61.0/64.0, 48.0/64.0 );
    button_set_hilit_image( ta->down_button, binding, ll, ur, 
			    white );

    ll = make_point2d( 2.0/64.0, 33.0/64.0 );
    ur = make_point2d( 29.0/64.0, 48.0/64.0 );
    button_set_clicked_image( ta->down_button, binding, ll, ur, 
			      white );

    button_set_click_event_cb( ta->down_button, 
			       textarea_arrow_click_cb,
			       ta );

    update_button_positions( ta );
    

    if ( text == NULL ) {
	text = "";
    }
    ta->text_orig = string_copy( text );
    ta->line_list = NULL;
    ta->text_lines = NULL;

    create_lines( ta );

    update_button_enabled_states( ta );

    ta->visible = False;
    ta->active = False;

    return ta;
}



/*---------------------------------------------------------------------------*/
/*! 
  Deletes the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
void textarea_delete( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    textarea_set_visible( ta, False );
    textarea_set_active( ta, False );

    if ( ta->up_button != NULL ) {
	button_delete( ta->up_button );
    }
    if ( ta->down_button != NULL ) {
	button_delete( ta->down_button );
    }

    if ( ta->line_list != NULL ) {
	del_list( ta->line_list );
    }

    if ( ta->text_lines != NULL ) {
	free( ta->text_lines );
    }

    if ( ta->text_orig != NULL ) {
	free( ta->text_orig );
    }

    free( ta );
}


/*---------------------------------------------------------------------------*/
/*! 
  Sets the text to be displayed in the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
void textarea_set_text( textarea_t *ta, char *text )
{
    check_assertion( ta != NULL, "ta is NULL" );

    if ( ta->text_orig != NULL ) {
	free( ta->text_orig );
	ta->text_orig = NULL;
    }

    if ( text == NULL ) {
	text = "";
    }

    ta->text_orig = string_copy( text );

    create_lines( ta );

    update_button_enabled_states( ta );
}


/*---------------------------------------------------------------------------*/
/*! 
  Sets the position of the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
void textarea_set_position( textarea_t *ta, point2d_t pos )
{
    check_assertion( ta != NULL, "ta is NULL" );

    ta->pos = pos;
    update_button_positions( ta );
}


/*---------------------------------------------------------------------------*/
/*! 
  Returns the height of the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
int textarea_get_height( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    return ta->h;
}


/*---------------------------------------------------------------------------*/
/*! 
  Returns the width of the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
int textarea_get_width( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    return ta->w;
}


/*---------------------------------------------------------------------------*/
/*! 
  Sets the visibility of the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
void textarea_set_visible( textarea_t *ta, bool_t visible )
{
    check_assertion( ta != NULL, "ta is NULL" );

    /* This check is necessary to prevent infinite mutual recursion */
    if ( ta->active != visible ) {
	textarea_set_active( ta, visible );
    }

    if ( !ta->visible && visible ) {
	ui_add_widget_draw_callback( ta, textarea_draw_cb );

	ui_set_dirty();
    } else if ( ta->visible && !visible ) {
	ui_delete_widget_draw_callback( ta );

	ui_set_dirty();
    }

    ta->visible = visible;
}


/*---------------------------------------------------------------------------*/
/*! 
  Returns the visible status of the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
bool_t textarea_get_visible( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    return ta->visible;
}


/*---------------------------------------------------------------------------*/
/*! 
  Sets the active state of the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
void textarea_set_active( textarea_t *ta, bool_t active )
{
    check_assertion( ta != NULL, "ta is NULL" );

    if ( ta->up_button ) {
	button_set_active( ta->up_button, active );
    }
    if ( ta->down_button ) {
	button_set_active( ta->down_button, active );
    }

    ta->active = active;

    if ( !active ) {
	textarea_set_visible( ta, False );
    }
}


/*---------------------------------------------------------------------------*/
/*! 
  Returns the active state of the textarea widget
  \author  jfpatry
  \date    Created:  2000-09-23
  \date    Modified: 2000-09-23
*/
bool_t textarea_get_active( textarea_t *ta )
{
    check_assertion( ta != NULL, "ta is NULL" );

    return ta->active;
}

/* EOF */
