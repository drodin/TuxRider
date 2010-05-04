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
#include "ui_theme.h"
#include "textures.h"

colour_t ui_background_colour = { 0.48, 0.63, 0.90, 0. };
colour_t ui_foreground_colour = { 1.0, 1.0, 1.0, 1.0 }; 
colour_t ui_highlight_colour = { 1.0, 0.89, 0.01, 1.0 };
colour_t ui_disabled_colour = { 1.0, 1.0, 1.0, 0.6 };
#ifdef __APPLE__
colour_t ui_enabled_but_disabled_colour = { 1.0, 1.0, 1.0, 1.0 };
#endif

static void draw_quad(int x, int y, int w, int h)
{
    glPushMatrix();
    {
	glTranslatef( x, y, 0 );
	
		#ifdef __APPLE__DISABLED__
    	
    	   const GLfloat vertices []=
    	   {
    	       0, 0,
    	       w, 0,
    	       w, h,
    	       0, h
    	   };

    		const GLfloat texCoords []=
    	   {
    	       0,0,
    	       1,0,
    	       1,1,
    	       0,1
    	   };

    	   glEnableClientState (GL_VERTEX_ARRAY);
    	   glVertexPointer (2, GL_FLOAT , 0, vertices);	
    	   glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    	   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    	#else
	
	glBegin( GL_QUADS );
	{
	    glTexCoord2f( 0, 0 );
	    glVertex2f( 0, 0 );
	    
	    glTexCoord2f( 1, 0 );
	    glVertex2f( w, 0 );
	    
	    glTexCoord2f( 1, 1 );
	    glVertex2f( w, h );
	    
	    glTexCoord2f( 0, 1 );
	    glVertex2f( 0, h );
	}
	glEnd();
	
	#endif
    }
    glPopMatrix();
}

void ui_draw_menu_decorations()
{
    GLuint texobj;
    char *bl = "menu_bottom_left";
    char *br = "menu_bottom_right";
    char *tl = "menu_top_left";
    char *tr = "menu_top_right";
    char *title = "menu_title";
    int w = getparam_x_resolution();
    int h = getparam_y_resolution();

    glEnable( GL_TEXTURE_2D );
    glColor4f( 1., 1., 1., 1. );

    /* bottom left */
    if ( get_texture_binding( bl, &texobj ) ) {
        glBindTexture( GL_TEXTURE_2D, texobj );
        draw_quad( 0, 0, 256, 256 );
    }


    /* bottom right */
    if ( get_texture_binding( br, &texobj ) ) {
        glBindTexture( GL_TEXTURE_2D, texobj );
        draw_quad( w-256, 0, 256, 256 );
    }


    /* top left */
    if ( get_texture_binding( tl, &texobj ) ) {
        glBindTexture( GL_TEXTURE_2D, texobj );
        draw_quad( 0, h-256, 256, 256 );
    }


    /* top right */
    if ( get_texture_binding( tr, &texobj ) ) {
        glBindTexture( GL_TEXTURE_2D, texobj );
        draw_quad( w-256, h-256, 256, 256 );
    }


    /* title */
    if ( get_texture_binding( title, &texobj ) ) {
        glBindTexture( GL_TEXTURE_2D, texobj );
        draw_quad( w/2-128, h-128, 256, 128 );
    }

}
/* EOF */
