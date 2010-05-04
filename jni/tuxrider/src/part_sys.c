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
#include "part_sys.h"
#include "phys_sim.h"
#include "gl_util.h"
#include "tcl_util.h"
#include "course_render.h"
#include "render_util.h"
#include "textures.h"

/* This constant is here as part of a debugging check to prevent an infinite 
   number of particles from being created */
#define MAX_PARTICLES 500000

#define START_RADIUS 0.04
#define OLD_PART_SIZE 0.07
#define NEW_PART_SIZE 0.02
#define MIN_AGE     -0.2
#define MAX_AGE      1.0

#define VARIANCE_FACTOR 0.8

#define PARTICLE_SHADOW_HEIGHT 0.05
#define PARTICLE_SHADOW_ALPHA 0.1

typedef struct _Particle {
    point_t pt;
    short type;
    scalar_t base_size;
    scalar_t cur_size;
    scalar_t terrain_height;
    scalar_t age;
    scalar_t death;
    scalar_t alpha;
    vector_t vel;
    struct _Particle *next;
} Particle;

static GLfloat particle_colour[4];

static Particle* head = NULL;
static int num_particles = 0;

scalar_t frand() 
{
    return (scalar_t)rand()/RAND_MAX;
} 

void create_new_particles( point_t loc, vector_t vel, int num ) 
{
    Particle *newp;
    int i;
    scalar_t speed;

    speed = normalize_vector( &vel );

    /* Debug check to track down infinite particle bug */
    if ( num_particles + num > MAX_PARTICLES ) {
	check_assertion( 0, "maximum number of particles exceeded" );
    } 

    for (i=0; i<num; i++) {

        newp = (Particle*)malloc( sizeof( Particle) );

        if ( newp == NULL ) {
            handle_system_error( 1, "out of memory" );
        } 

        num_particles += 1;

        newp->next = head;
        head = newp;

        newp->pt.x = loc.x + 2.*(frand() - 0.5) * START_RADIUS;
        newp->pt.y = loc.y;
        newp->pt.z = loc.z + 2.*(frand() - 0.5) * START_RADIUS;

	newp->type = (int) (frand() * (4.0 - EPS));

	newp->base_size = ( frand() + 0.5 ) * OLD_PART_SIZE;
	newp->cur_size = NEW_PART_SIZE;

        newp->age = frand() * MIN_AGE;
        newp->death = frand() * MAX_AGE;


        newp->vel = add_vectors( 
	    scale_vector( speed, vel ),
	    make_vector( VARIANCE_FACTOR * (frand() - 0.5) * speed, 
			 VARIANCE_FACTOR * (frand() - 0.5) * speed,
			 VARIANCE_FACTOR * (frand() - 0.5) * speed  ) );
    }
} 

void update_particles( scalar_t time_step )
{
    Particle **p, *q;
    scalar_t ycoord;

    for (p = &head; *p != NULL; ) {
        (**p).age += time_step;

        if ( (**p).age < 0 ) continue;

	(**p).pt = move_point( (**p).pt, 
			       scale_vector( time_step, (**p).vel ) );

	ycoord = find_y_coord( (**p).pt.x, (**p).pt.z );

	if ( (**p).pt.y < ycoord - 3 ) {
	    (**p).age = (**p).death + 1;
	} 

        if ( (**p).age >= (**p).death ) {
            q = *p;
            *p = q->next;
            free(q);
            num_particles -= 1;
            continue;
        } 

        (**p).alpha = ( (**p).death - (**p).age ) / (**p).death;

	(**p).cur_size = NEW_PART_SIZE + 
	    ( OLD_PART_SIZE - NEW_PART_SIZE ) * ( (**p).age / (**p).death );

        (**p).vel.y += -EARTH_GRAV * time_step;

        p = &( (**p).next );
    } 
} 

void draw_particles( player_data_t *plyr )
{
    Particle *p;
    GLuint   texture_id;
    char *binding;
    point2d_t min_tex_coord, max_tex_coord;

    set_gl_options( PARTICLES );

    binding = "snow_particle";
    if (!get_texture_binding( "snow_particle", &texture_id ) ) {
	print_warning( IMPORTANT_WARNING,
		       "Couldn't get texture for binding %s", 
		       binding );
	texture_id = 0;
    } 

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glBindTexture( GL_TEXTURE_2D, texture_id );


    for (p=head; p!=NULL; p = p->next) {
        if ( p->age < 0 ) continue;

	if ( p->type == 0 || p->type == 1 ) {
	    min_tex_coord.y = 0;
	    max_tex_coord.y = 0.5;
	} else {
	    min_tex_coord.y = 0.5;
	    max_tex_coord.y = 1.0;
	}

	if ( p->type == 0 || p->type == 3 ) {
	    min_tex_coord.x = 0;
	    max_tex_coord.x = 0.5;
	} else {
	    min_tex_coord.x = 0.5;
	    max_tex_coord.x = 1.0;
	}

	glColor4f( particle_colour[0], 
		   particle_colour[1], 
		   particle_colour[2],
		   particle_colour[3] * p->alpha );

	draw_billboard( plyr, p->pt, p->cur_size, p->cur_size,
			False, min_tex_coord, max_tex_coord );
    } 

} 

void clear_particles()
{
    Particle *p, *q;

    p = head;
    for (;;) {
        if (p == NULL ) break;
        q=p;
        p=p->next;
        free(q);
    } 
    head = NULL;
    num_particles = 0;
}

void reset_particles() {
    particle_colour[0] = 1.0;
    particle_colour[1] = 1.0;
    particle_colour[2] = 1.0;
    particle_colour[3] = 1.0;
} 

static int particle_colour_cb(ClientData cd, Tcl_Interp *ip, 
			      int argc, char *argv[]) 
{
    scalar_t tmp_arr[4];
    bool_t error = False;

    if ( argc == 2 ) {
	/* New format */
	if ( get_tcl_tuple ( ip, argv[1], tmp_arr, 4 ) == TCL_ERROR ) {
	    error = True;
	} else {
	    copy_to_glfloat_array( particle_colour, tmp_arr, 4 );
	}
    } else {
	/* Old format */
	if (argc < 3) {
	    error = True;
	} else {

	    NEXT_ARG;

	    print_warning( DEPRECATION_WARNING,
			   "This format for tux_particle_colour is deprecated."
			   "  The new format is:\n"
			   "\ttux_particle_colour {r g b a}" );

	    while ( !error && argc > 0 ) {
		
		if ( strcmp( "-ambient_and_diffuse", *argv ) == 0 ||
		     strcmp( "-diffuse",  *argv ) == 0 ) 
		{
		    NEXT_ARG;
		    if ( argc == 0 ) {
			error = True;
			break;
		    }
		    if ( get_tcl_tuple ( ip, *argv, tmp_arr, 4 ) == 
			 TCL_ERROR ) 
		    {
			error = True;
			break;
		    }
		    copy_to_glfloat_array( particle_colour, tmp_arr, 4 );
		} else if ( strcmp( "-specular", *argv ) == 0 ) {
		    /* Ignore */
		    NEXT_ARG;
		} else if ( strcmp( "-shininess", *argv ) == 0 ) {
		    /* Ignore */
		    NEXT_ARG;
		} else {
		    print_warning( TCL_WARNING, 
				   "tux_particle_colour: unrecognized "
				   "parameter `%s'", *argv );
		}
		
		NEXT_ARG;
	    }
	}
    }

    if ( error ) {
	print_warning( TCL_WARNING, "error in call to tux_particle_colour" );
	Tcl_AppendResult(
	    ip, 
	    "\nUsage: tux_particle_colour {r g b a}", (char *) 0 );
	return TCL_ERROR;
    }

    return TCL_OK;
}

void register_particle_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_particle_colour", particle_colour_cb,  0,0);
    Tcl_CreateCommand (ip, "tux_particle_color", particle_colour_cb,  0,0);
}
