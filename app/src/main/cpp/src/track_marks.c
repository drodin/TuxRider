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

#include "track_marks.h"
#include "gl_util.h"
#include "tux.h"
#include "hier.h"
#include "phys_sim.h"
#include "textures.h"
#include "alglib.h"
#include "course_render.h"
#include "render_util.h"

#undef TRACK_TRIANGLES

#define TRACK_WIDTH  0.7
#define MAX_TRACK_MARKS 1000
#define MAX_CONTINUE_TRACK_DIST TRACK_WIDTH*4
#define MAX_CONTINUE_TRACK_TIME .1
#define SPEED_TO_START_TRENCH 0.0
#define SPEED_OF_DEEPEST_TRENCH 10

#ifdef TRACK_TRIANGLES
  #define TRACK_HEIGHT 0.1
  #define MAX_TRACK_DEPTH 10
  #define MAX_TRIS MAX_TRACK_MARKS
#else
  #define TRACK_HEIGHT 0.08
  #define MAX_TRACK_DEPTH 0.7
#endif

typedef enum track_types_t {
    TRACK_HEAD,
    TRACK_MARK,
    TRACK_TAIL,
    NUM_TRACK_TYPES
} track_types_t;

typedef struct track_quad_t {
    point_t v1, v2, v3, v4;
    point2d_t t1, t2, t3, t4;
    vector_t n1, n2, n3, n4;
    track_types_t track_type;
    scalar_t alpha;
} track_quad_t;

typedef struct track_marks_t {
    track_quad_t quads[MAX_TRACK_MARKS];
    int current_mark;
    int next_mark;
    scalar_t last_mark_time;
    point_t last_mark_pos;
} track_marks_t;

static track_marks_t track_marks;
static bool_t continuing_track;



#ifdef TRACK_TRIANGLES
typedef struct track_tris_t {
    triangle_t tri[MAX_TRIS];
    track_types_t *track_type[MAX_TRIS];
    scalar_t *alpha[MAX_TRIS];
    int first_mark;
    int next_mark;
    int current_start;
    int current_end;
    int num_tris;
} track_tris_t;

typedef struct track_tri_t {
    point_t v1, v2, v3;
} track_tri_t;

static track_tris_t track_tris;

static void draw_tri( triangle_t *tri, scalar_t alpha )
{
    vector_t nml;
    GLfloat c[4] = {1.0, 0.0, 0.0, 1.0}; 

/*    set_material_alpha( white, black, 1.0, alpha ); */
    set_material_alpha( white, black, 1.0, 1.0 );  

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, c);

#ifdef __APPLE__DISABLED__
    {
        const GLfloat vertices2 []=
        {
            tri->p[0].x, tri->p[0].y, tri->p[0].z,
            tri->p[1].x, tri->p[1].y, tri->p[1].z,
            tri->p[2].x, tri->p[2].y, tri->p[2].z
        };

        const GLshort texCoords2 []=
        {
            tri->t[0].x, tri->t[0].y,
            tri->t[1].x, tri->t[1].y,
            tri->t[2].x, tri->t[2].y,
        };

        glEnableClientState (GL_VERTEX_ARRAY);
        glVertexPointer (3, GL_FLOAT , 0, vertices2);	
        glTexCoordPointer(2, GL_SHORT, 0, texCoords2);

        nml = find_course_normal( tri->p[0].x, tri->p[0].z );
        glNormal3f( nml.x, nml.y, nml.z );
        glDrawArrays(GL_TRIANGLES, 0, 1);

        nml = find_course_normal( tri->p[1].x, tri->p[1].z );
        glNormal3f( nml.x, nml.y, nml.z );
        glDrawArrays(GL_TRIANGLES, 1, 1);

        nml = find_course_normal( tri->p[2].x, tri->p[2].z );
        glNormal3f( nml.x, nml.y, nml.z );
        glDrawArrays(GL_TRIANGLES, 2, 1);
    }
#else
    glBegin(GL_TRIANGLES);

    nml = find_course_normal( tri->p[0].x, tri->p[0].z );
    glNormal3f( nml.x, nml.y, nml.z );
    glTexCoord2f( tri->t[0].x, tri->t[0].y );
    glVertex3f( tri->p[0].x, tri->p[0].y, tri->p[0].z );
    
    nml = find_course_normal( tri->p[1].x, tri->p[1].z );
    glNormal3f( nml.x, nml.y, nml.z );
    glTexCoord2f( tri->t[1].x, tri->t[1].y );
    glVertex3f( tri->p[1].x, tri->p[1].y, tri->p[1].z );
    
    nml = find_course_normal( tri->p[2].x, tri->p[2].z );
    glNormal3f( nml.x, nml.y, nml.z );
    glTexCoord2f( tri->t[2].x, tri->t[2].y );
    glVertex3f( tri->p[2].x, tri->p[2].y, tri->p[2].z );

    glEnd();
#endif
}

static void draw_tri_tracks( void )
{
    GLuint texid[NUM_TRACK_TYPES];
    int i;

    set_gl_options( TRACK_MARKS ); 

    glColor4f( 0, 0, 0, 1);

    get_texture_binding( "track_head", &texid[TRACK_HEAD] );
    get_texture_binding( "track_mark", &texid[TRACK_MARK] );
    get_texture_binding( "track_tail", &texid[TRACK_TAIL] );

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    set_material( white, black, 1.0 );
    setup_course_lighting();

    for( i = 0; i < track_tris.num_tris; i++ ) {
	glBindTexture( GL_TEXTURE_2D, 
		       texid[*track_tris.track_type[(track_tris.first_mark+i)%MAX_TRIS]] );
	draw_tri( &track_tris.tri[(track_tris.first_mark+i)%MAX_TRIS],
		  *track_tris.alpha[(track_tris.first_mark+i)%MAX_TRIS] );
    }
}

static void add_tri_tracks_from_tri( point_t p1, point_t p2, point_t p3,
				     point2d_t t1, point2d_t t2, point2d_t t3,
				     track_types_t *track_type, scalar_t *alpha )
{
    scalar_t minx, maxx;
    scalar_t minz, maxz;
    int nx, nz;
    scalar_t width, length;
    int i, j, k;
    scalar_t xstep, zstep;
    line_t cut;
    int num_tris;
    int this_set_end;
    int num_new;

    /* Make 'em planar. Calculate y later anyway */
    p1.y = p2.y = p3.y = 0;

    get_course_divisions( &nx, &nz );
    get_course_dimensions( &width, &length );
    xstep = width/(nx-1);
    zstep = length/(nz-1);

    minx = min(min(p1.x, p2.x), p3.x);
    minz = min(min(p1.z, p2.z), p3.z);
    maxx = max(max(p1.x, p2.x), p3.x);
    maxz = max(max(p1.z, p2.z), p3.z);

    track_tris.current_start = track_tris.next_mark;
    track_tris.current_end = track_tris.next_mark;

    track_tris.tri[track_tris.next_mark].p[0] = p1;
    track_tris.tri[track_tris.next_mark].p[1] = p2;
    track_tris.tri[track_tris.next_mark].p[2] = p3;
    track_tris.tri[track_tris.next_mark].t[0] = t1;
    track_tris.tri[track_tris.next_mark].t[1] = t2;
    track_tris.tri[track_tris.next_mark].t[2] = t3; 

    /*
     * Make lengthwise cuts
     */
    for( i = (int)((minx/xstep)+0.9999); i < (int)(maxx/xstep); i++ ) {
	cut.pt = make_point( i*xstep, 0, 0 );
	cut.nml = make_vector( 1, 0, 0 );
	this_set_end = track_tris.current_end;
	for ( j = track_tris.current_start; j <= this_set_end; j++ ) {
	    num_tris = cut_triangle( &track_tris.tri[j%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+1)%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+2)%MAX_TRIS],
				     cut );
	    track_tris.current_end = (track_tris.current_end + num_tris - 1)%MAX_TRIS;
	}
    }
    
    /*
     * Make cross cuts
     */
    for( i = (int)((minz/zstep)+0.9999); i < (int)(maxz/zstep); i++ ) {
	cut.pt = make_point( 0, 0, i*zstep );
	cut.nml = make_vector( 0, 0, 1 );
	this_set_end = track_tris.current_end;
	for ( j = track_tris.current_start; j <= this_set_end; j++ ) {
	    num_tris = cut_triangle( &track_tris.tri[j%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+1)%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+2)%MAX_TRIS],
				     cut );
	    track_tris.current_end = (track_tris.current_end + num_tris - 1)%MAX_TRIS;
	}
    }

    /*
     * Make diagonal cuts
     */
    for( i = (int)((minx/xstep)+0.9999), j = (int)((minz/zstep)+0.9999);
	 (i < (int)(maxx/xstep)) && (j < (int)(maxz/zstep));
	 i++, j++) {
	if ( (i+j)%2 != 0 ) {
	    i--;
	}
	cut.pt = make_point( i*xstep, 0, j*zstep );
	cut.nml = make_vector( 1, 0, 1 );
	normalize_vector(&cut.nml);
	for ( k = track_tris.current_start; k <= this_set_end; k++ ) {
	    num_tris = cut_triangle( &track_tris.tri[k%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+1)%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+2)%MAX_TRIS],
				     cut );
	    track_tris.current_end = (track_tris.current_end + num_tris - 1)%MAX_TRIS;
	}
    }

    /*
     * Make other diagonal cuts
     */
    for( i = (int)((minx/xstep)+0.9999), j = (int)(maxz/zstep);
	 (i < (int)(maxx/xstep)) && (j > (int)((minz/zstep) + 0.9999));
	 i++, j--) {
	if ( (i+j)%2 != 0 ) {
	    i--;
	}
	cut.pt = make_point( i*xstep, 0, j*zstep );
	cut.nml = make_vector( 1, 0, 1 );
	normalize_vector(&cut.nml);
	for ( k = track_tris.current_start; k <= this_set_end; k++ ) {
	    num_tris = cut_triangle( &track_tris.tri[k%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+1)%MAX_TRIS],
				     &track_tris.tri[(track_tris.current_end+2)%MAX_TRIS],
				     cut );
	    track_tris.current_end = (track_tris.current_end + num_tris - 1)%MAX_TRIS;
	}
    }


    /* Reset first, next and num_tris */
    if (track_tris.current_start <= track_tris.current_end) {
	num_new = track_tris.current_end - track_tris.current_start + 1;
	track_tris.num_tris = track_tris.num_tris + num_new;
	track_tris.next_mark = (track_tris.current_end+1)%MAX_TRIS;
	if ( ((track_tris.num_tris - num_new) > 0) &&
	     (track_tris.first_mark >= track_tris.current_start) && 
	     (track_tris.first_mark <= track_tris.current_end) ) {
	    track_tris.num_tris = track_tris.num_tris - (track_tris.current_end - track_tris.first_mark + 1);
	    track_tris.first_mark = track_tris.next_mark;
	}

    } else {
	num_new = (track_tris.current_end + 1) + (MAX_TRIS - track_tris.current_start);
	track_tris.num_tris = track_tris.num_tris + num_new;
	track_tris.next_mark = (track_tris.current_end+1)%MAX_TRIS;
	if (track_tris.first_mark >= track_tris.current_start) {
	    track_tris.num_tris = track_tris.num_tris - (track_tris.current_end + 1) - 
		(MAX_TRIS - track_tris.first_mark);
	    track_tris.first_mark = track_tris.next_mark;
	} else if (track_tris.first_mark <= track_tris.current_end) {
	    track_tris.num_tris = track_tris.num_tris - (track_tris.current_end - track_tris.first_mark + 1);
	    track_tris.first_mark = track_tris.next_mark;
	}

    }

    for ( i = 0; i < num_new; i++ ) {
	track_tris.alpha[(track_tris.current_start+i)%MAX_TRIS] = alpha;
	track_tris.track_type[(track_tris.current_start+i)%MAX_TRIS] = track_type;
	track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[0].y = 
	    find_y_coord( track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[0].x, 
			  track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[0].z ) +
	    TRACK_HEIGHT; 
	track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[1].y =
	    find_y_coord( track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[1].x, 
			  track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[1].z ) +
	    TRACK_HEIGHT; 
	track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[2].y = 
	    find_y_coord( track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[2].x, 
			  track_tris.tri[(track_tris.current_start+i)%MAX_TRIS].p[2].z ) +
	    TRACK_HEIGHT; 
    }

}

static void add_tri_tracks_from_quad( track_quad_t *q )
{
    add_tri_tracks_from_tri( q->v1, q->v2, q->v3, q->t1, q->t2, q->t3,
			     &q->track_type, &q->alpha );
    add_tri_tracks_from_tri( q->v2, q->v4, q->v3, q->t2, q->t4, q->t3,
			     &q->track_type, &q->alpha );
}

#endif



void init_track_marks(void)
{
    track_marks.current_mark = 0;
    track_marks.next_mark = 0;
    track_marks.last_mark_time = -99999;
    track_marks.last_mark_pos = make_point(-9999, -9999, -9999);
    continuing_track = False;
#ifdef TRACK_TRIANGLES
    track_tris.first_mark = 0;
    track_tris.next_mark = 0;
    track_tris.num_tris = 0;
#endif
}



void draw_track_marks(void)
{
#ifdef TRACK_TRIANGLES
    draw_tri_tracks();
#else
    GLuint texid[NUM_TRACK_TYPES];
    int current_quad, num_quads;
    int first_quad;
    track_quad_t *q, *qnext;
    colour_t track_colour;

    track_colour = white;

    if (getparam_track_marks() == False) {
	return;
    }

    set_gl_options( TRACK_MARKS ); 

    glColor4f( 0, 0, 0, 1);

    get_texture_binding( "track_head", &texid[TRACK_HEAD] );
    get_texture_binding( "track_mark", &texid[TRACK_MARK] );
    get_texture_binding( "track_tail", &texid[TRACK_TAIL] );

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    set_material( white, black, 1.0 );
    setup_course_lighting();

    num_quads = min( track_marks.current_mark, MAX_TRACK_MARKS -
		     track_marks.next_mark + track_marks.current_mark );
    first_quad = track_marks.current_mark - num_quads;

    for ( current_quad = 0;
	  current_quad < num_quads;
	  current_quad++ ) 
    {
	q = &track_marks.quads[(first_quad + current_quad)%MAX_TRACK_MARKS];

	track_colour.a = q->alpha;
	set_material( track_colour, black, 1.0 );

	glBindTexture( GL_TEXTURE_2D, texid[q->track_type] );

	if ((q->track_type == TRACK_HEAD) || (q->track_type == TRACK_TAIL)) {
#ifdef __APPLE__DISABLED__
        {
            const GLfloat vertices2 []=
            {
                q->v1.x, q->v1.y, q->v1.z,
                q->v2.x, q->v2.y, q->v2.z,
                q->v4.x, q->v4.y, q->v4.z,
                q->v3.x, q->v3.y, q->v3.z,
            };

            const GLshort texCoords2 []=
            {
                q->t1.x, q->t1.y,
                q->t2.x, q->t2.y,
                q->t4.x, q->t4.y,
                q->t3.x, q->t3.y
            };

            glEnableClientState (GL_VERTEX_ARRAY);
            glVertexPointer (3, GL_FLOAT , 0, vertices2);	
            glTexCoordPointer(2, GL_SHORT, 0, texCoords2);

            glNormal3f( q->n1.x, q->n1.y, q->n1.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 1);

            glNormal3f( q->n2.x, q->n2.y, q->n2.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 1, 1);

            glNormal3f( q->n4.x, q->n4.y, q->n4.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 2, 1);

            glNormal3f( q->n3.x, q->n3.y, q->n3.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 3, 1);
        }

#else
	    glBegin(GL_QUADS);
	    
	    glNormal3f( q->n1.x, q->n1.y, q->n1.z );
	    glTexCoord2f( q->t1.x, q->t1.y );
	    glVertex3f( q->v1.x, q->v1.y, q->v1.z );
	
	    glNormal3f( q->n2.x, q->n2.y, q->n2.z );
	    glTexCoord2f( q->t2.x, q->t2.y );
	    glVertex3f( q->v2.x, q->v2.y, q->v2.z );

	    glNormal3f( q->n4.x, q->n4.y, q->n4.z );
	    glTexCoord2f( q->t4.x, q->t4.y );
	    glVertex3f( q->v4.x, q->v4.y, q->v4.z );
	
	    glNormal3f( q->n3.x, q->n3.y, q->n3.z );
	    glTexCoord2f( q->t3.x, q->t3.y );
	    glVertex3f( q->v3.x, q->v3.y, q->v3.z );
	
	    glEnd();
#endif
	} else {

#ifdef __APPLE__DISABLED__
        {
            const GLfloat vertices2 []=
            {
                q->v1.x, q->v1.y, q->v1.z,
                q->v2.x, q->v2.y, q->v2.z,
                q->v4.x, q->v4.y, q->v4.z,
                q->v3.x, q->v3.y, q->v3.z,
            };

            const GLshort texCoords2 []=
            {
                q->t1.x, q->t1.y,
                q->t2.x, q->t2.y,
                q->t4.x, q->t4.y,
                q->t3.x, q->t3.y
            };

            glEnableClientState (GL_VERTEX_ARRAY);
            glVertexPointer (3, GL_FLOAT , 0, vertices2);	
            glTexCoordPointer(2, GL_SHORT, 0, texCoords2);

            glNormal3f( q->n1.x, q->n1.y, q->n1.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 1);

            glNormal3f( q->n2.x, q->n2.y, q->n2.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 1, 1);

            glNormal3f( q->n4.x, q->n4.y, q->n4.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 2, 1);

            glNormal3f( q->n3.x, q->n3.y, q->n3.z );
            glDrawArrays(GL_TRIANGLE_STRIP, 3, 1);
            
            // FIXME
        }

#else
	    glBegin(GL_QUAD_STRIP);

	    glNormal3f( q->n2.x, q->n2.y, q->n2.z );
	    glTexCoord2f( q->t2.x, q->t2.y );
	    glVertex3f( q->v2.x, q->v2.y, q->v2.z );

	    glNormal3f( q->n1.x, q->n1.y, q->n1.z );
	    glTexCoord2f( q->t1.x, q->t1.y );
	    glVertex3f( q->v1.x, q->v1.y, q->v1.z );

	    glNormal3f( q->n4.x, q->n4.y, q->n4.z );
	    glTexCoord2f( q->t4.x, q->t4.y );
	    glVertex3f( q->v4.x, q->v4.y, q->v4.z );

	    glNormal3f( q->n3.x, q->n3.y, q->n3.z );
	    glTexCoord2f( q->t3.x, q->t3.y );
	    glVertex3f( q->v3.x, q->v3.y, q->v3.z );


	    qnext = &track_marks.quads[(first_quad+current_quad+1)%MAX_TRACK_MARKS];
	    while (( qnext->track_type == TRACK_MARK ) && (current_quad+1 < num_quads)) {
		current_quad++;
		q = &track_marks.quads[(first_quad+current_quad)%MAX_TRACK_MARKS];
		track_colour.a = qnext->alpha;
		set_material( track_colour, black, 1.0 );

		glNormal3f( q->n4.x, q->n4.y, q->n4.z );
		glTexCoord2f( q->t4.x, q->t4.y );
		glVertex3f( q->v4.x, q->v4.y, q->v4.z );

		glNormal3f( q->n3.x, q->n3.y, q->n3.z );
		glTexCoord2f( q->t3.x, q->t3.y );
		glVertex3f( q->v3.x, q->v3.y, q->v3.z );
		
		qnext = &track_marks.quads[(first_quad+current_quad+1)%MAX_TRACK_MARKS];
	    }
	    glEnd();
#endif
	}

    }
#endif

}

void break_track_marks( void )
{
    track_quad_t *qprev, *qprevprev;
    qprev = &track_marks.quads[(track_marks.current_mark-1)%MAX_TRACK_MARKS];
    qprevprev = &track_marks.quads[(track_marks.current_mark-2)%MAX_TRACK_MARKS];

    if (track_marks.current_mark > 0) {
	qprev->track_type = TRACK_TAIL;
	qprev->t1 = make_point2d(0.0, 0.0);
	qprev->t2 = make_point2d(1.0, 0.0);
	qprev->t3 = make_point2d(0.0, 1.0);
	qprev->t4 = make_point2d(1.0, 1.0);
	qprevprev->t3.y = max((int)(qprevprev->t3.y+0.5), (int)(qprevprev->t1.y+1));
	qprevprev->t4.y = max((int)(qprevprev->t3.y+0.5), (int)(qprevprev->t1.y+1));
    }
    track_marks.last_mark_time = -99999;
    track_marks.last_mark_pos = make_point(-9999, -9999, -9999);
    continuing_track = False;
}

void add_track_mark( player_data_t *plyr )
{
    vector_t width_vector;
    vector_t left_vector;
    vector_t right_vector;
    scalar_t magnitude;
    track_quad_t *q, *qprev, *qprevprev;
    vector_t vel;
    scalar_t speed;
    point_t left_wing, right_wing;
    scalar_t left_y, right_y;
    scalar_t dist_from_surface;
    plane_t surf_plane;
    scalar_t comp_depth;
    scalar_t tex_end;
    scalar_t terrain_weights[NumTerrains];
    scalar_t dist_from_last_mark;
    vector_t vector_from_last_mark;

    if (getparam_track_marks() == False) {
	return;
    }

    q = &track_marks.quads[track_marks.current_mark%MAX_TRACK_MARKS];
    qprev = &track_marks.quads[(track_marks.current_mark-1)%MAX_TRACK_MARKS];
    qprevprev = &track_marks.quads[(track_marks.current_mark-2)%MAX_TRACK_MARKS];

    vector_from_last_mark = subtract_points( plyr->pos, track_marks.last_mark_pos );
    dist_from_last_mark = normalize_vector( &vector_from_last_mark );
	
	
    get_surface_type(plyr->pos.x, plyr->pos.z, terrain_weights);
    if (terrain_weights[Snow] < 0.5) {
	break_track_marks();
	return;
    }

    vel = plyr->vel;
    speed = normalize_vector( &vel );
    if (speed < SPEED_TO_START_TRENCH) {
	break_track_marks();
	return;
    }

    width_vector = cross_product( plyr->direction, make_vector( 0, 1, 0 ) );
    magnitude = normalize_vector( &width_vector );
    if ( magnitude == 0 ) {
	break_track_marks();
	return;
    }

    left_vector = scale_vector( TRACK_WIDTH/2.0, width_vector );
    right_vector = scale_vector( -TRACK_WIDTH/2.0, width_vector );
    left_wing =  point_minus_vector( plyr->pos, left_vector );
    right_wing = point_minus_vector( plyr->pos, right_vector );
    left_y = find_y_coord( left_wing.x, left_wing.z );
    right_y = find_y_coord( right_wing.x, right_wing.z );
    if (fabs(left_y-right_y) > MAX_TRACK_DEPTH) {
	break_track_marks();
	return;
    }

    surf_plane = get_local_course_plane( plyr->pos );
    dist_from_surface = distance_to_plane( surf_plane, plyr->pos );
    comp_depth = get_compression_depth(Snow);
    if ( dist_from_surface >= (2*comp_depth) ) {
	break_track_marks();
	return;
    }


    if (!continuing_track) {
	break_track_marks();
	q->track_type = TRACK_HEAD;
	q->v1 = make_point( left_wing.x, left_y + TRACK_HEIGHT, left_wing.z );
	q->v2 = make_point( right_wing.x, right_y + TRACK_HEIGHT, right_wing.z );
	q->n1 = find_course_normal( q->v1.x, q->v1.z);
	q->n2 = find_course_normal( q->v2.x, q->v2.z);
	q->t1 = make_point2d(0.0, 0.0);
	q->t2 = make_point2d(1.0, 0.0);
	track_marks.next_mark = track_marks.current_mark + 1;
    } else {
	if ( track_marks.next_mark == track_marks.current_mark ) {
	    q->v1 = qprev->v3;
	    q->v2 = qprev->v4;
	    q->n1 = qprev->n3;
	    q->n2 = qprev->n4;
	    q->t1 = qprev->t3; 
	    q->t2 = qprev->t4;
	    if ( qprev->track_type != TRACK_HEAD ) {
		qprev->track_type = TRACK_MARK;
	    }
	    q->track_type = TRACK_MARK;
	}
	q->v3 = make_point( left_wing.x, left_y + TRACK_HEIGHT, left_wing.z );
	q->v4 = make_point( right_wing.x, right_y + TRACK_HEIGHT, right_wing.z );
	q->n3 = find_course_normal( q->v3.x, q->v3.z);
	q->n4 = find_course_normal( q->v4.x, q->v4.z);
	tex_end = speed*g_game.time_step/TRACK_WIDTH;
	if (q->track_type == TRACK_HEAD) {
	    q->t3= make_point2d(0.0, 1.0);
	    q->t4= make_point2d(1.0, 1.0);
	} else {
	    q->t3 = make_point2d(0.0, q->t1.y + tex_end);
	    q->t4 = make_point2d(1.0, q->t2.y + tex_end);
	}

#ifdef TRACK_TRIANGLES
	add_tri_tracks_from_quad(q);
#endif
	track_marks.current_mark++;
	track_marks.next_mark = track_marks.current_mark;
    }

    q->alpha = min( (2*comp_depth-dist_from_surface)/(4*comp_depth), 1.0 );

    track_marks.last_mark_time = g_game.time;
    continuing_track = True;

}
