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
#include "course_load.h"
#include "course_render.h"
#include "course_quad.h"
#include "image.h"
#include "textures.h"
#include "phys_sim.h"
#include "tcl_util.h"
#include "keyframe.h"
#include "gl_util.h"
#include "lights.h"
#include "fog.h"
#include "part_sys.h"
#include "multiplayer.h"
#include "audio_data.h"
#include "track_marks.h"

#define MAX_TREES 8192
#define MAX_TREE_TYPES 32
#define MAX_ITEMS 8192
#define MAX_ITEM_TYPES 128

#define MIN_ANGLE 5
#define MAX_ANGLE 80

#define ICE_IMG_VAL  0
#define ROCK_IMG_VAL 127
#define SNOW_IMG_VAL 255
#define TREE_IMG_THRESHOLD 128
#define NUM_TERRAIN_TYPES 3

#define DEP_TREE_RED 255
#define DEP_TREE_GREEN 0
#define DEP_TREE_BLUE 0

static bool_t        course_loaded = False;

/* This array stores the heightmap.  It is stored as follows: suppose
   your at the top of a course, looking down.  The elevation data
   starts at the very top-left of the course, and proceeds from left
   to right.  So the very top strip of the course is stored first,
   followed by the next strip in left-to-right order, and so on until
   the botton strip. */
static scalar_t     *elevation;
static scalar_t      elev_scale;
static scalar_t      course_width, course_length;
static scalar_t      play_width, play_length;
static scalar_t      course_angle;
static int           nx, ny;
static tree_t        tree_locs[MAX_TREES];
static int           num_trees;
static terrain_t    *terrain;
static point2d_t     start_pt;
static char         *course_author = NULL;
static char         *course_name = NULL;
#ifdef __APPLE__
static char			*calculation_mode = NULL;
#endif
static int           base_height_value;
static tree_type_t   tree_types[MAX_TREE_TYPES];
static int           num_tree_types = 0;
static int           tree_dep_call = -1;

static item_t        item_locs[MAX_ITEMS];
static item_type_t   item_types[MAX_ITEM_TYPES];
static int           num_item_types = 0;
static int           num_items;

/* Interleaved vertex, normal, and color data */
static GLubyte      *vnc_array = NULL;

scalar_t     *get_course_elev_data()    { return elevation; }
terrain_t    *get_course_terrain_data() { return terrain; }
scalar_t      get_course_angle()        { return course_angle; } 
tree_t       *get_tree_locs()           { return tree_locs; }
int           get_num_trees()           { return num_trees; }
polyhedron_t  get_tree_polyhedron(int type) { return tree_types[type].poly; }
char         *get_tree_name(int type)       { return tree_types[type].name; }
point2d_t     get_start_pt()            { return start_pt; }
void          set_start_pt( point2d_t p ) { start_pt = p; }
char         *get_course_author()       { return course_author; }
char         *get_course_name()         { return course_name; }
#ifdef __APPLE__
char		 *get_calculation_mode()    { if (calculation_mode != NULL) return calculation_mode; else return "default"; }
#endif

item_t       *get_item_locs()           { return item_locs; }
int           get_num_items()           { return num_items; }
char         *get_item_name(int type)   { return item_types[type].name; }
int           get_num_item_types()      { return num_item_types; }
item_type_t  *get_item_types()          { return item_types; }

void get_gl_arrays( GLubyte **vnc_arr )
{
    *vnc_arr = vnc_array;
}

void get_course_dimensions( scalar_t *width, scalar_t *length )
{
    *width = course_width;
    *length = course_length;
} 

void get_play_dimensions( scalar_t *width, scalar_t *length )
{
    *width = play_width;
    *length = play_length;
} 

static void
free_polygon_list(int num_polygons, polygon_t *polygon_list)
{
    int i;
    for(i = 0; i < num_polygons; i++) {
        free(polygon_list[i].vertices);
    }
    free(polygon_list);
}

/*! 
  Returns the base (minimum) height of the terrain at \c distance
  \pre     A course has been loaded
  \arg \c  distance the (non-negative) distance down the course

  \return  Minimum height (y-coord) of terrain
  \author  jfpatry
  \date    Created:  2000-08-30
  \date    Modified: 2000-08-30
*/
scalar_t get_terrain_base_height( scalar_t distance )
{
    scalar_t slope = tan( ANGLES_TO_RADIANS( course_angle ) );
    scalar_t base_height;
    
    check_assertion( distance > -EPS,
		     "distance should be positive" );

    /* This will need to be fixed once we add variably-sloped terrain */
    base_height = -slope * distance - 
	base_height_value / 255.0 * elev_scale;
    return base_height;
}

/*! 
  Returns the maximum height of the terrain at \c distance
  \pre     A course has been loaded
  \arg \c  distance the (non-negative) distance down the course

  \return  Maximum height (y-coord) of terrain
  \author  jfpatry
  \date    Created:  2000-08-30
  \date    Modified: 2000-08-30
*/
scalar_t get_terrain_max_height( scalar_t distance )
{
    return get_terrain_base_height( distance ) + elev_scale;
}

void get_course_divisions( int *x, int *y )
{
    *x = nx;
    *y = ny;
} 


static void reset_course()
{
    int i;

    /*
     * Defaults
     */
    num_trees     = 0;
    num_items     = 0;
    course_angle  = 20.;
    course_width  = 50.;
    course_length = 130.;
    play_width  = 50.;
    play_length = 130.;
    nx = ny = -1;
    start_pt.x = 0;
    start_pt.y = 0;
    base_height_value = 127; /* 50% grey */

    set_course_mirroring( False );

    reset_lights();
    reset_fog();
    reset_particles();

    if ( course_author != NULL ) {
	free( course_author );
    }
    course_author = NULL;

    if ( course_name != NULL ) {
	free( course_name );
    }
    course_name = NULL;
#ifdef __APPLE__
    if ( calculation_mode != NULL ) {
        free( calculation_mode );
    }
    calculation_mode = NULL;
#endif
    if ( course_loaded == False ) return;

    reset_course_quadtree();

    free( elevation ); elevation = NULL;
    free( terrain ); terrain = NULL;

    free( vnc_array ); vnc_array = NULL;

    for ( i = 0; i < num_tree_types; i++) {
	unbind_texture( tree_types[i].name );
	free( tree_types[i].name );
	tree_types[i].name = NULL;

	free_polygon_list( tree_types[i].poly.num_polygons, tree_types[i].poly.polygons );

	tree_types[i].poly.polygons = NULL;

	free( tree_types[i].poly.vertices );
	tree_types[i].poly.vertices = NULL;

	free( tree_types[i].texture );
	tree_types[i].texture = NULL;

	tree_types[i].poly.num_vertices  = 0;
	tree_types[i].poly.num_polygons  = 0;
	tree_types[i].texture = NULL;
	tree_types[i].num_trees = 0;

	tree_types[i].pos = NULL;
	tree_types[i].insert_pos = NULL;
    }
    num_tree_types = 0;
    tree_dep_call = -1;

    for ( i = 0; i < num_item_types; i++) {
	if (item_types[i].reset_point == False) {
	    unbind_texture( item_types[i].name );
	}

	free( item_types[i].name );
	item_types[i].name = NULL;

	free( item_types[i].texture );
	item_types[i].texture = NULL;

	item_types[i].pos = NULL;
	item_types[i].insert_pos = NULL;

	item_types[i].num_items = 0;
    }
    num_item_types = 0;

    course_loaded = False;

    reset_key_frame();
} 

bool_t course_exists( int num )
{
    char buff[BUFF_LEN];
    struct stat s;

    sprintf( buff, "%s/courses/%d", getparam_data_dir(), num );
    if ( stat( buff, &s ) != 0 ) {
	return False;
    }
    if ( ! S_ISDIR( s.st_mode ) ) {
	return False;
    }
    return True;
}

void fill_gl_arrays()
{
    int x,y;
    vector_t *normals = get_course_normals();
    vector_t nml;
    int idx;

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    /* Align vertices and normals on 16-byte intervals (Q3A does this) */
    vnc_array = (GLubyte*) malloc( STRIDE_GL_ARRAY * nx * ny );

    for (x=0; x<nx; x++) {
	for (y=0; y<ny; y++) {
	    idx = STRIDE_GL_ARRAY*(y*nx+x);
	   
#define floatval(i) (*(GLfloat*)(vnc_array+idx+(i)*sizeof(GLfloat)))

	    floatval(0) = (GLfloat)x / (nx-1.) * course_width;
	    floatval(1) = ELEV(x,y);
	    floatval(2) = -(GLfloat)y / (ny-1.) * course_length;

	    nml = normals[ x + y * nx ];
	    floatval(4) = nml.x;
	    floatval(5) = nml.y;
	    floatval(6) = nml.z;
	    floatval(7) = 1.0f;
	   
#undef floatval
#define byteval(i) (*(GLubyte*)(vnc_array+idx+8*sizeof(GLfloat) +\
    i*sizeof(GLubyte)))

	    byteval(0) = 255;
	    byteval(1) = 255;
	    byteval(2) = 255;
	    byteval(3) = 255;

#undef byteval

	}
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer( 3, GL_FLOAT, STRIDE_GL_ARRAY, vnc_array );

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer( GL_FLOAT, STRIDE_GL_ARRAY, 
		     vnc_array + 3*sizeof(GLfloat) );

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer( 4, GL_UNSIGNED_BYTE, STRIDE_GL_ARRAY, 
		    vnc_array + 8*sizeof(GLfloat) );
}

void load_course( char *course )
{
    char buff[BUFF_LEN];
    char cwd[BUFF_LEN];

    reset_course();

    if ( getcwd( cwd, BUFF_LEN ) == NULL ) {
	handle_system_error( 1, "getcwd failed" );
    }

    sprintf( buff, "%s/courses/%s", getparam_data_dir(), course );
    if ( chdir( buff ) != 0 ) {
	handle_system_error( 1, "Couldn't chdir to %s", buff );
    } 

    if ( Tcl_EvalFile( g_game.tcl_interp, "./course.tcl") == TCL_ERROR ) {
	handle_error( 1, "Error evaluating %s/course.tcl: %s",  
		      buff, Tcl_GetStringResult( g_game.tcl_interp ) );
    } 

    if ( chdir( cwd ) != 0 ) {
	handle_system_error( 1, "Couldn't chdir to %s", cwd );
    } 

    check_assertion( !Tcl_InterpDeleted( g_game.tcl_interp ),
		     "Tcl interpreter deleted" );

    calc_normals();

    fill_gl_arrays();

    init_course_quadtree( elevation, nx, ny, course_width/(nx-1.), 
			  -course_length/(ny-1),
			  g_game.player[local_player()].view.pos, 
			  getparam_course_detail_level() );

    init_track_marks();

    course_loaded = True;

    /* flush unused audio files */
    delete_unused_audio_data();
} 


static terrain_t intensity_to_terrain( int intensity )
{
    int dist[NUM_TERRAIN_TYPES];
    int min;
    terrain_t min_idx;
    int i;
    
    dist[Ice]  = abs( intensity - ICE_IMG_VAL );
    dist[Snow] = abs( intensity - SNOW_IMG_VAL );
    dist[Rock] = abs( intensity - ROCK_IMG_VAL );

    min = dist[0];
    min_idx = (terrain_t)0;
    for (i=1; i<NUM_TERRAIN_TYPES; i++) {
	if ( dist[i] < min ) {
	    min = dist[i];
	    min_idx = (terrain_t)i;
	}
    }
    return min_idx;
}

static int course_dim_cb ( ClientData cd, Tcl_Interp *ip, 
			   int argc, char *argv[]) 
{
    double width, length;

    if ( ( argc != 3 ) && ( argc != 5 ) ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <course width> <course length>",
			 " [<play width> <play length>]", (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &width ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid course width", 
			 (char *)0 );
        return TCL_ERROR;
    } 
    if ( Tcl_GetDouble( ip, argv[2], &length ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid course length", 
			 (char *)0 );
        return TCL_ERROR;
    } 

    course_width = width;
    course_length = length;

    if ( argc == 5 ) {
	if ( Tcl_GetDouble( ip, argv[3], &width ) != TCL_OK ) {
	    Tcl_AppendResult(ip, argv[0], ": invalid play width", 
			     (char *)0 );
	    return TCL_ERROR;
	} 
	if ( Tcl_GetDouble( ip, argv[4], &length ) != TCL_OK ) {
	    Tcl_AppendResult(ip, argv[0], ": invalid play length", 
			     (char *)0 );
	    return TCL_ERROR;
	} 
	play_width = width;
	play_length = length;
    } else {
	play_width = course_width;
	play_length = course_length;
    }

    return TCL_OK;
} 

static int angle_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    double angle;

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <angle>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &angle ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    if ( angle < MIN_ANGLE ) {
	print_warning( TCL_WARNING, "course angle is too small. Setting to %f",
		       MIN_ANGLE );
	angle = MIN_ANGLE;
    }

    if ( MAX_ANGLE < angle ) {
	print_warning( TCL_WARNING, "course angle is too large. Setting to %f",
		       MAX_ANGLE );
	angle = MAX_ANGLE;
    }

    course_angle = angle;

    return TCL_OK;
} 


static int elev_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    IMAGE *elev_img;
    scalar_t slope;
    int   x,y;
    int   pad;

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <elevation bitmap>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if (course_loaded) {
	print_warning( TCL_WARNING, "ignoring %s: course already loaded",
		       argv[0] );
	return TCL_OK;
    }

    elev_img = ImageLoad( argv[1] );
    if ( elev_img == NULL ) {
	print_warning( TCL_WARNING, "%s: couldn't load %s", argv[0], argv[1] );
	return TCL_ERROR;
    }

    nx = elev_img->sizeX;
    ny = elev_img->sizeY;

    elevation = (scalar_t *)malloc( sizeof(scalar_t)*nx*ny );

    if ( elevation == NULL ) {
	handle_system_error( 1, "malloc failed" );
    }

    slope = tan( ANGLES_TO_RADIANS( course_angle ) );

    pad = 0;    /* RGBA images rows are aligned on 4-byte boundaries */
    for (y=0; y<ny; y++) {
        for (x=0; x<nx; x++) {
	    ELEV(nx-1-x, ny-1-y) = 
		( ( elev_img->data[ (x + nx * y) * elev_img->sizeZ + pad ] 
		    - base_height_value ) / 255.0 ) * elev_scale
		- (scalar_t) (ny-1.-y)/ny * course_length * slope;
        } 
        pad += (nx*elev_img->sizeZ) % 4;
    } 

    free( elev_img->data );
    free( elev_img );

    return TCL_OK;
} 

static int terrain_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    IMAGE *terrain_img;
    int   x,y;
    int   pad;
    int   idx;

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <terrain bitmap>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    terrain_img = ImageLoad( argv[1] );

    if ( terrain_img == NULL ) {
	print_warning( TCL_WARNING, "%s: couldn't load %s", argv[0], argv[1] );
        Tcl_AppendResult(ip, argv[0], ": couldn't load ", argv[1],
			 (char *)0 );
	return TCL_ERROR;
    }

    if ( nx != terrain_img->sizeX || ny != terrain_img->sizeY ) {
        Tcl_AppendResult(ip, argv[0], ": terrain bitmap must have same " 
			 "dimensions as elevation bitmap",
			 (char *)0 );

	return TCL_ERROR;
    }

    terrain = (terrain_t *)malloc( sizeof(terrain_t)*nx*ny );

    if ( terrain == NULL ) {
	handle_system_error( 1, "malloc failed" );
    }

    pad = 0;
    for (y=0; y<ny; y++) {
        for (x=0; x<nx; x++) {
            idx = (nx-1-x) + nx*(ny-1-y);
	    terrain[idx] = intensity_to_terrain(
		terrain_img->data[(x+nx*y)*terrain_img->sizeZ+pad] );
        } 
        pad += (nx*terrain_img->sizeZ) % 4;
    } 

    free( terrain_img->data );
    free( terrain_img );

    return TCL_OK;
} 

static int bgnd_img_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <background image>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if (!load_and_bind_texture( "background", argv[1] )) {
      Tcl_AppendResult(ip, argv[0], ": could not load texture", (char *) 0);
      return TCL_ERROR;
	}

    return TCL_OK;
} 

/*
// set the defaults that the deprecated tree calls assume
*/
static void tree_defaults (tree_type_t * type) {

    check_assertion( type != NULL, "need a non-NULL tree type");
    type->name = string_copy("tree");
    type->diam = 2.0;
    type->height = 4.5;
    type->vary = 0.5;
    type->poly.num_vertices = 0;
    type->poly.vertices = NULL;
    type->poly.num_polygons = 0;
    type->poly.polygons = NULL;
    type->texture = NULL;
    type->num_trees = 0;
    type->red = DEP_TREE_RED;
    type->green = DEP_TREE_GREEN;
    type->blue = DEP_TREE_BLUE;
    type->pos = NULL;
    type->insert_pos = NULL;
}

/*
// deprecated
*/
static int tree_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <tree texture>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if (tree_dep_call == -1) {
	if ( num_tree_types + 1 >= MAX_TREE_TYPES ) {
	    Tcl_AppendResult(ip, argv[0], ": max number of tree types reached",
			     (char *)0 );
	    return TCL_ERROR;
	}
	tree_dep_call = num_tree_types++;
	tree_defaults( &tree_types[tree_dep_call] );
    }

    if (!load_and_bind_texture( tree_types[tree_dep_call].name, argv[1] )) {
	Tcl_AppendResult(ip, argv[0], ": could not load texture", (char *) 0);
	return TCL_ERROR;
    }

    tree_types[tree_dep_call].texture =
		    string_copy( tree_types[tree_dep_call].name );

    return TCL_OK;
} 

static int ice_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <ice texture>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if (!load_and_bind_texture( "ice", argv[1] )) {
      Tcl_AppendResult(ip, argv[0], ": could not load texture", (char *) 0);
      return TCL_ERROR;
	}

    return TCL_OK;
} 

static int rock_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <rock texture>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if (!load_and_bind_texture( "rock", argv[1] )) {
      Tcl_AppendResult(ip, argv[0], ": could not load texture", (char *) 0);
      return TCL_ERROR;
	}

    return TCL_OK;
} 

static int snow_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <snow texture>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( !load_and_bind_texture( "snow", argv[1] ) ) {
      Tcl_AppendResult(ip, argv[0], ": could not load texture", (char *) 0);
	  return TCL_ERROR;
    }

    return TCL_OK;
} 

static int start_pt_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    double xcd, ycd;

    if ( argc != 3 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <x coord> <y coord>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &xcd ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid x coordinate", (char *)0 );
        return TCL_ERROR;
    } 
    if ( Tcl_GetDouble( ip, argv[2], &ycd ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid y coordinate", (char *)0 );
        return TCL_ERROR;
    } 

    if ( !( xcd > 0 && xcd < course_width ) ) {
	print_warning( TCL_WARNING, "%s: x coordinate out of bounds, "
		       "using 0\n", argv[0] );
	xcd = 0;
    }

    if ( !( ycd > 0 && ycd < course_length ) ) {
	print_warning( TCL_WARNING, "%s: y coordinate out of bounds, "
		       "using 0\n", argv[0] );
	ycd = 0;
    }

    start_pt.x = xcd;
    start_pt.y = -ycd;

    return TCL_OK;
} 

static int elev_scale_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    double scale;
    
    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <scale>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &scale ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid scale", (char *)0 );
        return TCL_ERROR;
    } 

    if ( scale <= 0 ) {
	print_warning( TCL_WARNING, "%s: scale must be positive", argv[0] );
	return TCL_ERROR;
    }

    elev_scale = scale;

    return TCL_OK;
} 

static int is_tree( unsigned char pixel[], tree_type_t ** which_type )
{
    int           min_distance = pixel[0] + pixel[1] + pixel[2];
    int i;
    int distance;
 
    *which_type = NULL;
    for (i = 0; i < num_tree_types; i++) {
	/* assume red green blue pixel ordering */
	distance = abs ( tree_types[i].red - pixel[0] ) +
		    abs ( tree_types[i].green - pixel[1] ) +
		    abs ( tree_types[i].blue - pixel[2] );
	if (distance < min_distance) {
	    min_distance = distance;
	    *which_type = &tree_types[i];
	}
    }
    
    return min_distance;
}

static int is_item( unsigned char pixel[], item_type_t ** which_type )
{
    int      min_distance = pixel[0] + pixel[1] + pixel[2];
    int i;
    int distance;
 
    *which_type = NULL;
    for (i = 0; i < num_item_types; i++) {
	/* assume red green blue pixel ordering */
	distance = abs ( item_types[i].red - pixel[0] ) +
		    abs ( item_types[i].green - pixel[1] ) +
		    abs ( item_types[i].blue - pixel[2] );
	if (distance < min_distance) {
	    min_distance = distance;
	    *which_type = &item_types[i];
	}
    }
    
    return min_distance;
}

/*
// modified
// now more accurate to call this object_cb - no matter
*/
static int trees_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    IMAGE *treeImg;
    int sx, sy, sz;
    int x,y;
    int pad;
    tree_type_t *which_tree;
    item_type_t *which_item;
    point2d_t * pos;
    int i;
    list_elem_t elem;
    int best_tree_dist, best_item_dist;

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <tree location bitmap>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    treeImg = ImageLoad( argv[1] );
    if ( treeImg == NULL ) {
	print_warning( TCL_WARNING, "%s: couldn't load %s", 
		       argv[0], argv[1] );
        Tcl_AppendResult(ip, argv[0], ": couldn't load ", argv[1], 
			 (char *)0 );
	return TCL_ERROR;
    }

    if ( num_tree_types == 0 && num_item_types == 0 ) {
	print_warning( IMPORTANT_WARNING,
		       "tux_trees callback called with no tree or item "
		       "types set" );
    }

    sx = treeImg->sizeX;
    sy = treeImg->sizeY;
    sz = treeImg->sizeZ;

    for (i = 0; i < num_tree_types; i++) {
	tree_types[i].pos = create_list();
	tree_types[i].insert_pos = 0;
	tree_types[i].num_trees = 0;
    }

    for (i = 0; i < num_item_types; i++) {
	item_types[i].pos = create_list();
	item_types[i].insert_pos = 0;
	item_types[i].num_items = 0;
    }

    num_trees = 0;
    num_items = 0;
    pad = 0;
    for (y=0; y<sy; y++) {
        for (x=0; x<sx; x++) {
            best_tree_dist = is_tree ( &treeImg->data[ (x + y*sx)*sz + pad ],
					 &which_tree );
            best_item_dist = is_item ( &treeImg->data[ (x + y*sx)*sz + pad ],
					 &which_item );

	    if ( best_tree_dist < best_item_dist && which_tree != NULL ) {
                if (num_trees+1 == MAX_TREES ) {
                    fprintf( stderr, "%s: maximum number of trees reached.\n", 
			     argv[0] );
                    break;
                }
		num_trees += 1;
		which_tree->num_trees += 1;
		pos = (point2d_t *) malloc(sizeof(point2d_t));
		pos->x = (sx-x)/(scalar_t)(sx-1.)*course_width;
		pos->y = -(sy-y)/(scalar_t)(sy-1.)*course_length;
		which_tree->insert_pos = insert_list_elem(which_tree->pos,
			which_tree->insert_pos, (list_elem_data_t) pos);

            } else if ( which_item != NULL ) {
                if (num_items+1 == MAX_ITEMS ) {
                    fprintf( stderr, "%s: maximum number of items reached.\n", 
			     argv[0] );
                    break;
                }
		num_items += 1;
		which_item->num_items += 1;
		pos = (point2d_t *) malloc(sizeof(point2d_t));
		pos->x = (sx-x)/(scalar_t)(sx-1.)*course_width;
		pos->y = -(sy-y)/(scalar_t)(sy-1.)*course_length;
		which_item->insert_pos = insert_list_elem(which_item->pos,
			which_item->insert_pos, (list_elem_data_t) pos);
	    }

        }
        pad += ( sx * sz ) % 4; /* to compensate for word-aligned rows */
    }

    /*
    // double pass so that tree and object types are clumped together - reduce
    // texture switching
    */
    num_trees = 0;
    for (i = 0; i < num_tree_types; i++) {
	elem = get_list_head(tree_types[i].pos);
	while (elem != NULL) {
	    pos = (point2d_t *) get_list_elem_data(elem);

	    tree_locs[num_trees].ray.pt.x = pos->x;
	    tree_locs[num_trees].ray.pt.z = pos->y;
	    tree_locs[num_trees].ray.pt.y = find_y_coord( pos->x, pos->y );
	    free( pos );
	    elem = get_next_list_elem(tree_types[i].pos, elem);

	    tree_locs[num_trees].ray.vec = make_vector( 0, 1, 0);

	    tree_locs[num_trees].height = 
		    (scalar_t)rand()/RAND_MAX*tree_types[i].vary*2;
	    tree_locs[num_trees].height -= tree_types[i].vary;
	    tree_locs[num_trees].height = tree_types[i].height + 
			tree_locs[num_trees].height * tree_types[i].height;
	    tree_locs[num_trees].diam = (tree_locs[num_trees].height /
			tree_types[i].height) * tree_types[i].diam;
	    tree_locs[num_trees].tree_type = i;
	    num_trees++;
	}
	del_list( tree_types[i].pos );
    }

    num_items = 0;
    for (i = 0; i < num_item_types; i++) {
	elem = get_list_head(item_types[i].pos);
	while (elem != NULL) {
	    pos = (point2d_t *) get_list_elem_data(elem);

	    item_locs[num_items].ray.pt.x = pos->x;
	    item_locs[num_items].ray.pt.z = pos->y;
	    item_locs[num_items].ray.pt.y = find_y_coord( pos->x, pos->y )
					    + item_types[i].above_ground;
	    free( pos );
	    elem = get_next_list_elem(item_types[i].pos, elem);

	    item_locs[num_items].ray.vec = make_vector( 0, 1, 0);

	    item_locs[num_items].height = item_types[i].height ; 
	    item_locs[num_items].diam = item_types[i].diam;
	    item_locs[num_items].item_type = i;
	    if ( item_types[i].nocollision )  {
		item_locs[num_items].collectable = -1;
	    } else {
		item_locs[num_items].collectable = 1;
	    }
	    if ( item_types[i].reset_point )  {
		item_locs[num_items].drawable = False;
	    } else {
		item_locs[num_items].drawable = True;
	    }
	    num_items++;
	}
	del_list( item_types[i].pos );
    }


    free( treeImg->data );
    free( treeImg );

    return TCL_OK;
} 

/*
// deprecated
*/
static int tree_size_cb( ClientData cd, Tcl_Interp *ip, 
			 int argc, char *argv[]) 
{
    double diam, height;

    if ( argc != 3 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <diameter> <height>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &diam ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid diameter",
			 (char *)0 );
        return TCL_ERROR;
    } 
    if ( Tcl_GetDouble( ip, argv[2], &height ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid height",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if (tree_dep_call == -1) {
	if ( num_tree_types + 1 >= MAX_TREE_TYPES ) {
	    Tcl_AppendResult(ip, argv[0], ": max number of tree types reached",
			     (char *)0 );
	    return TCL_ERROR;
	}
	tree_dep_call = num_tree_types++;
	tree_defaults( &tree_types[tree_dep_call] );
    }

    tree_types[tree_dep_call].diam   = diam;
    tree_types[tree_dep_call].height = height;

    return TCL_OK;
} 

/*
 * The following functions are used to get the parameters for 
 * tux_tree_poly
 */

/*
 * Gets the vertex list
 */
static int
get_polyhedron_vertices(Tcl_Interp *ip, char *arg, 
			int *num_vertices, point_t **vertex_list)
{
  int i;
  char **indices=0;
  int rtn;
  scalar_t pt[3];
  
  rtn = Tcl_SplitList(ip, arg, num_vertices, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "A vertex list must be provided\n",
		     (char *) 0);
    
    Tcl_Free((char *) indices);
    return (TCL_ERROR);
  }

  *vertex_list = (point_t*)malloc(sizeof(point_t)*(*num_vertices));
  for(i = 0; i < *num_vertices; i++) {
    get_tcl_tuple(ip, indices[i], pt, 3);
    (*vertex_list)[i] = make_point_from_array(pt);
  }

  Tcl_Free((char *) indices);

  return TCL_OK;
}

/*
 * gets the vertex-index-list of a single polygon
 */
static int
get_single_polygon(Tcl_Interp *ip, char *arg, 
		   polygon_t *polygon)
{
  int i;
  char **indices=0;
  int rtn;
  int num_vertices;
  
  rtn = Tcl_SplitList(ip, arg, &num_vertices, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "a list of vertices must be provided for each polygon\n",
		     (char *) 0);
    Tcl_Free((char *) indices);
    return (TCL_ERROR);
  }
  
  polygon->num_vertices = num_vertices;
  polygon->vertices = (int*)malloc(sizeof(int)*num_vertices);
  
  for(i = 0; i < num_vertices; i++)
    Tcl_GetInt(ip, indices[i], &(polygon->vertices[i]));

  Tcl_Free((char *) indices);

  return TCL_OK;
}

/*
 * gets the polygon list.
 */
static int
get_polyhedron_polygon(Tcl_Interp *ip, char *arg, 
		       int *num_polygons, polygon_t **polygon_list)
{
  int i;
  char **indices=0;
  int rtn;

  rtn = Tcl_SplitList(ip, arg, num_polygons, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "A polygon list must be provided\n",
		     (char *) 0);
    Tcl_Free((char *) indices);
    return TCL_ERROR;
  }

  *polygon_list = (polygon_t*)malloc(sizeof(polygon_t)*(*num_polygons));
  for(i = 0; i < *num_polygons; i++)
    get_single_polygon(ip, indices[i], &((*polygon_list)[i]));

  Tcl_Free((char *) indices);

  return TCL_OK;
}

/*
 * finally the polyhedron object.
 */
/*
// deprecated
*/
static int 
tree_poly_cb(ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
  int           num_vertices;
  point_t     *vertices;
  int           num_polygons;
  polygon_t     *polygons;
    
  if (argc != 3) {
    Tcl_AppendResult(ip, 
		     argv[0], 
		     "invalid number of arguments\n"
		     "Usage: tux_tree_poly <vertex list> <face list>"
		     " <polygon list>",
		     (char *) 0 );
    return TCL_ERROR;
  }

    if (tree_dep_call == -1) {
	if ( num_tree_types + 1 >= MAX_TREE_TYPES ) {
	    Tcl_AppendResult(ip, argv[0], ": max number of tree types reached",
			     (char *)0 );
	    return TCL_ERROR;
	}
	tree_dep_call = num_tree_types++;
	tree_defaults( &tree_types[tree_dep_call] );
    }

  if( get_polyhedron_vertices(ip, argv[1], &num_vertices, &vertices) == TCL_OK )
    if( get_polyhedron_polygon(ip, argv[2], &num_polygons, &polygons) == TCL_OK ) {
        tree_types[tree_dep_call].poly.num_polygons = num_polygons;
        tree_types[tree_dep_call].poly.polygons     = polygons;
        tree_types[tree_dep_call].poly.num_vertices = num_vertices;
        tree_types[tree_dep_call].poly.vertices     = vertices;
        return TCL_OK;
    }

  Tcl_AppendResult(ip, argv[0], ": error obtaining polyhedron data", 
		   (char *)0 );
  return TCL_ERROR;
}

static int friction_cb ( ClientData cd, Tcl_Interp *ip, 
			 int argc, char *argv[]) 
{
    double fric[3];
    scalar_t fric_s[3];
    int i;

    if ( argc != 4 ) {
        fprintf( stderr, "Usage: %s <ice> <rock> <snow>", argv[0] );
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <ice coeff.> <rock coeff.> "
			 "<snow coeff.>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &fric[0] ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid ice coefficient",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[2], &fric[1] ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid rock coefficient",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[3], &fric[2] ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid snow coefficient",
			 (char *)0 );
        return TCL_ERROR;
    }

    for ( i=0; i<sizeof(fric)/sizeof(fric[0]); i++) {
	fric_s[i] = fric[i];
    }

    set_friction_coeff( fric_s );

    return TCL_OK;
} 

static int course_author_cb( ClientData cd, Tcl_Interp *ip, 
			     int argc, char *argv[]) 
{
    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <author's name>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( course_author != NULL ) {
        free(course_author);
    }
    course_author = string_copy( argv[1] );

    return TCL_OK;
} 

static int course_name_cb( ClientData cd, Tcl_Interp *ip, 
			   int argc, char *argv[]) 
{
    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <course name>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( course_name != NULL ) {
        free(course_name);
    }
    course_name = string_copy( argv[1] );

    return TCL_OK;
} 

#ifdef __APPLE__
static int calculation_mode_cb( ClientData cd, Tcl_Interp *ip, 
                          int argc, char *argv[]) 
{
    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
                         "Usage: ", argv[0], " <calculation mode>",
                         (char *)0 );
        return TCL_ERROR;
    } 
    
    if ( calculation_mode != NULL ) {
        free(calculation_mode);
    }
    calculation_mode = string_copy( argv[1] );
    
    return TCL_OK;
} 
#endif

static int base_height_value_cb( ClientData cd, Tcl_Interp *ip, 
				 int argc, char *argv[]) 
{
    int value;

    if ( argc != 2 ) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <base height>",
			 (char *)0 );
        return TCL_ERROR;
    } 

    if ( Tcl_GetInt( ip, argv[1], &value ) != TCL_OK ) {
        Tcl_AppendResult(ip, argv[0], ": invalid base height",
			 (char *)0 );
        return TCL_ERROR;
    } 

    base_height_value = value;

    return TCL_OK;
} 

static int tree_props_cb( ClientData cd, Tcl_Interp *ip, 
				 int argc, char *argv[])
{
    int         i;
    int         num_vertices;
    point_t     *vertices;
    int         num_polys;
    polygon_t   *polys;
    int         rtn, num_col;
    char **     indices = 0;
    int         error = 0;
    int         convert_temp;

    if ( num_tree_types + 1 >= MAX_TREE_TYPES ) {
	Tcl_AppendResult(ip, argv[0], ": max number of tree types reached",
			 (char *)0 );
	return TCL_ERROR;
    }

    /* fill in values not specified with defaults */
    tree_types[num_tree_types].name = NULL;
    tree_types[num_tree_types].diam = 2.0;
    tree_types[num_tree_types].height = 4.5;
    tree_types[num_tree_types].vary = 0.5;
    tree_types[num_tree_types].poly.num_vertices = 0;
    tree_types[num_tree_types].poly.vertices = NULL;
    tree_types[num_tree_types].poly.num_polygons = 0;
    tree_types[num_tree_types].poly.polygons = NULL;
    tree_types[num_tree_types].texture = NULL;
    tree_types[num_tree_types].num_trees = 0;
    tree_types[num_tree_types].red = 255;
    tree_types[num_tree_types].green = 255;
    tree_types[num_tree_types].blue = 255;
    tree_types[num_tree_types].pos = NULL;
    tree_types[num_tree_types].insert_pos = NULL;

    for ( i = 1; (i < argc - 1) && !error; i += 2 ) {
	if ( strcmp( "-name", argv[i] ) == 0 ) {
	    tree_types[num_tree_types].name = string_copy(argv[i+1]);

	} else if ( strcmp( "-diameter", argv[i] ) == 0 ) {
	    if ( Tcl_GetDouble( ip, argv[i+1],
		    &tree_types[num_tree_types].diam) != TCL_OK ) {
		Tcl_AppendResult(ip, argv[0], ": invalid diameter",
				 (char *)0 );
		error = 1;
	    }

	} else if ( strcmp( "-height", argv[i] ) == 0 ) {
	    if ( Tcl_GetDouble( ip, argv[i+1],
		    &tree_types[num_tree_types].height) != TCL_OK ) {
		Tcl_AppendResult(ip, argv[0], ": invalid height",
				 (char *)0 );
		error = 1;
	    }

	} else if ( strcmp( "-texture", argv[i] ) == 0 ) {
	    if ( !tree_types[num_tree_types].texture ) {
		tree_types[num_tree_types].texture = string_copy(argv[i+1]);
	    } else {
		Tcl_AppendResult(ip, argv[0], ": specify only one texture",
				(char *)0 );
	    }

	} else if ( strcmp( "-colour", argv[i] ) == 0 ) {
	    rtn = Tcl_SplitList(ip, argv[i+1], &num_col, &indices);
	    if( rtn != TCL_OK ) {
		Tcl_AppendResult(ip, "a list of colours must be provided\n",
			     (char *) 0);
		error = 1;
	    } else if (num_col == 3 || num_col == 4) {
		Tcl_GetInt(ip, indices[0], &convert_temp);
		tree_types[num_tree_types].red = (unsigned char) convert_temp;
		Tcl_GetInt(ip, indices[1], &convert_temp);
		tree_types[num_tree_types].green = (unsigned char) convert_temp;
		Tcl_GetInt(ip, indices[2], &convert_temp);
		tree_types[num_tree_types].blue = (unsigned char) convert_temp;
	    } else {
		Tcl_AppendResult(ip, argv[0], ": must specify three colours"
			" to link with tree type", (char *) 0);
		error = 1;
	    }
	    Tcl_Free((char *) indices);
        indices = NULL;

	} else if ( strcmp( "-polyhedron", argv[i] ) == 0 ) {
	    rtn = Tcl_SplitList(ip, argv[i+1], &num_col, &indices);
	    if( rtn != TCL_OK || num_col != 2 ) {
		Tcl_AppendResult(ip, "two sublists of vertices and polygons"
				     " must be specified\n",
			     (char *) 0);
		Tcl_Free((char *) indices);
		error = 1;
	    } else if( get_polyhedron_vertices(ip, indices[0], &num_vertices,
			&vertices) == TCL_OK ) {
		if( get_polyhedron_polygon(ip, indices[1], &num_polys, &polys)
			== TCL_OK ) {
		    tree_types[num_tree_types].poly.num_polygons = num_polys;
		    tree_types[num_tree_types].poly.polygons     = polys;
		    tree_types[num_tree_types].poly.num_vertices = num_vertices;
		    tree_types[num_tree_types].poly.vertices     = vertices;
		} else {
		    free( vertices );
		    error = 1;
		}
	    } else {
		error = 1;
	    }
		Tcl_Free((char *) indices);
	} else if ( strcmp( "-size_varies", argv[i] ) == 0 ) {
	    if ( Tcl_GetDouble( ip, argv[i+1],
		    &tree_types[num_tree_types].vary) != TCL_OK ) {
		Tcl_AppendResult(ip, argv[0], ": invalid size variance",
				 (char *)0 );
		error = 1;
	    }
	} else {
	    print_warning( TCL_WARNING, "tux_props_cb: unrecognized "
			    "parameter '%s'", argv[i]);
	    /* not sure if next arg is valid command - try it */
	    i -= 1;
	}
    }

    if ( tree_types[num_tree_types].name == 0 || 
		tree_types[num_tree_types].poly.vertices == 0 ||
		tree_types[num_tree_types].texture == 0 ) {
	Tcl_AppendResult(ip, argv[0], ": some mandatory elements not filled",
		" tree name, texture name and tree polygon must be supplied.",
		(char *)0 );
	free( tree_types[num_tree_types].name );
	free( tree_types[num_tree_types].texture );
	free( tree_types[num_tree_types].poly.vertices );
	free_polygon_list( tree_types[num_tree_types].poly.num_polygons, tree_types[num_tree_types].poly.polygons );
	return TCL_ERROR;

    } else if ( error ) {
	free( tree_types[num_tree_types].name );
	free( tree_types[num_tree_types].texture );
	free( tree_types[num_tree_types].poly.vertices );
	free_polygon_list( tree_types[num_tree_types].poly.num_polygons, tree_types[num_tree_types].poly.polygons );
	return TCL_ERROR;

    } else if (!bind_texture( tree_types[num_tree_types].name,
		tree_types[num_tree_types].texture )) {
	Tcl_AppendResult(ip, argv[0], ": could not bind texture ",
		    tree_types[num_tree_types].texture, (char *) 0);
	free( tree_types[num_tree_types].name );
	free( tree_types[num_tree_types].texture );
	free( tree_types[num_tree_types].poly.vertices );
	free_polygon_list( tree_types[num_tree_types].poly.num_polygons, tree_types[num_tree_types].poly.polygons );
	return TCL_ERROR;
    }

    num_tree_types += 1;
    return TCL_OK;
}

static int item_spec_cb( ClientData cd, Tcl_Interp *ip, 
				 int argc, char *argv[])
{
    int          rtn, num_col;
    char **      indices = NULL;
    int          convert_temp;
    char *       err_msg = "";
    char         buff[BUFF_LEN];

    if ( num_item_types + 1 >= MAX_ITEM_TYPES ) {
	Tcl_AppendResult(ip, argv[0], ": max number of item types reached",
			 (char *)0 );
	return TCL_ERROR;
    }

    item_types[num_item_types].name = NULL;
    item_types[num_item_types].texture = NULL;
    item_types[num_item_types].diam = .8;
    item_types[num_item_types].height = 0.5;
    item_types[num_item_types].above_ground = 0.0;
    item_types[num_item_types].red = 255;
    item_types[num_item_types].green = 255;
    item_types[num_item_types].blue = 255;
    item_types[num_item_types].nocollision = False;
    item_types[num_item_types].reset_point = False;
    item_types[num_item_types].pos = NULL;
    item_types[num_item_types].insert_pos = NULL;
    item_types[num_item_types].num_items = 0;
    item_types[num_item_types].use_normal = False;

    NEXT_ARG;

    while ( *argv != NULL ) {
	if ( strcmp( "-name", *argv ) == 0 ) {
	    NEXT_ARG;
	    CHECK_ARG( "-name", err_msg, item_spec_bail );

	    item_types[num_item_types].name = string_copy(*argv);

	} else if ( strcmp( "-height", *argv ) == 0 ) {
	    NEXT_ARG;
	    CHECK_ARG( "-height", err_msg, item_spec_bail );

	    if ( Tcl_GetDouble( ip, *argv,
		    &item_types[num_item_types].height) != TCL_OK ) {
		Tcl_AppendResult(ip, argv[0], ": invalid height\n",
					(char *) 0);
	    }

	} else if ( strcmp( "-diameter", *argv ) == 0 ) {
	    NEXT_ARG;
	    CHECK_ARG( "-diameter", err_msg, item_spec_bail );

	    if ( Tcl_GetDouble( ip, *argv,
		    &item_types[num_item_types].diam) != TCL_OK ) {
		Tcl_AppendResult(ip, argv[0], ": invalid diameter\n",
					(char *) 0);
	    }
	
	} else if ( strcmp( "-texture", *argv ) == 0 ) {
	    NEXT_ARG;
	    CHECK_ARG( "-texture", err_msg, item_spec_bail );

	    if ( !item_types[num_item_types].texture ) {
		item_types[num_item_types].texture = string_copy(*argv);
	    } else {
		Tcl_AppendResult(ip, argv[0], ": specify only one texture\n",
				(char *)0 );
	    }

	} else if ( strcmp( "-above_ground", *argv ) == 0 ) {
	    NEXT_ARG;
	    CHECK_ARG( "-above_ground", err_msg, item_spec_bail );

	    if ( Tcl_GetDouble( ip, *argv,
		    &item_types[num_item_types].above_ground) != TCL_OK ) {
		Tcl_AppendResult(ip, argv[0], ": invalid height above ground\n",
					(char *) 0);
	    }

	} else if ( strcmp( "-colour", *argv ) == 0 ||
		    strcmp( "-color", *argv ) == 0 ) 
	{
	    NEXT_ARG;
	    CHECK_ARG( "-colour", err_msg, item_spec_bail );

	    rtn = Tcl_SplitList(ip, *argv, &num_col, &indices);
	    if( rtn != TCL_OK ) {
		err_msg = "Must provide a list of colours for -colour";
		goto item_spec_bail; 
	    }

	    if (num_col == 3 || num_col == 4) {
		Tcl_GetInt(ip, indices[0], &convert_temp);
		item_types[num_item_types].red = (unsigned char) convert_temp;
		Tcl_GetInt(ip, indices[1], &convert_temp);
		item_types[num_item_types].green = (unsigned char) convert_temp;
		Tcl_GetInt(ip, indices[2], &convert_temp);
		item_types[num_item_types].blue = (unsigned char) convert_temp;
	    } else {
		err_msg = "Colour specification must have 3 or 4 elements";
		goto item_spec_bail;
	    }
	    Tcl_Free((char *) indices);
	    indices = NULL;

	} else if ( strcmp( "-nocollision", *argv ) == 0 ||
		    strcmp( "-nocollect", *argv ) == 0 ) {
	    item_types[num_item_types].nocollision = True;

	} else if ( strcmp( "-reset_point", *argv ) == 0 ) {
	    item_types[num_item_types].reset_point = True;
	    item_types[num_item_types].nocollision = True;
	} else if ( strcmp( "-normal", *argv ) == 0 ) {
	    NEXT_ARG;
	    CHECK_ARG( "-normal", err_msg, item_spec_bail );

	    if ( get_tcl_tuple( 
		ip, *argv, (scalar_t*)&(item_types[num_item_types].normal), 3 )
		 != TCL_OK )
	    {
		err_msg = "Must specify a list of size three for -normal";
		goto item_spec_bail;
	    }

	    normalize_vector( &(item_types[num_item_types].normal) );

	    item_types[num_item_types].use_normal = True;

	} else {
	    sprintf( buff, "Unrecognized option `%s'", *argv );
	    goto item_spec_bail;
	}

	NEXT_ARG;
    }

    if ( item_types[num_item_types].name == 0 ||
	 ( item_types[num_item_types].texture == 0 &&
	   item_types[num_item_types].reset_point == False ) ) 
    {
	err_msg = "Some mandatory elements not filled.  "
	    "Item name and texture name must be supplied.";
	goto item_spec_bail;
    }

    if ( item_types[num_item_types].reset_point == False &&
	 !bind_texture( item_types[num_item_types].name,
			item_types[num_item_types].texture )) 
    {
	err_msg = "could not bind specified texture";
	goto item_spec_bail;
    }

    num_item_types += 1;
    return TCL_OK;

item_spec_bail:
    if ( indices ) {
	Tcl_Free( (char*) indices );
	indices = NULL;
    }

    if ( item_types[num_item_types].name ) {
	free( item_types[num_item_types].name );
	item_types[num_item_types].name = NULL;
    }

    if ( item_types[num_item_types].texture ) {
	free( item_types[num_item_types].texture );
	item_types[num_item_types].texture = NULL;
    }    

    Tcl_AppendResult(
	ip,
	"Error in call to tux_item_spec: ", 
	err_msg,
	"\n",
	"Usage: tux_item_spec -name <name> -height <height> "
	"-diameter <diameter> -colour {r g b [a]} "
	"[-texture <texture>] [-above_ground <distance>] "
	"[-nocollect] [-reset_point] [-normal {x y z}]",
	(NULL) );
    return TCL_ERROR;
}

void register_course_load_tcl_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_course_dim", course_dim_cb,  0,0);
    Tcl_CreateCommand (ip, "tux_angle",      angle_cb,  0,0);
    Tcl_CreateCommand (ip, "tux_elev_scale", elev_scale_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_elev",       elev_cb,        0,0);
    Tcl_CreateCommand (ip, "tux_terrain",    terrain_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_trees",      trees_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_tree_size",  tree_size_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_tree_poly",  tree_poly_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_bgnd_img",   bgnd_img_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_tree_tex",   tree_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_ice_tex",    ice_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_rock_tex",   rock_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_snow_tex",   snow_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_start_pt",   start_pt_cb,   0,0);
#ifdef __APPLE__
    Tcl_CreateCommand (ip, "tux_calculation_mode",   calculation_mode_cb,   0,0);
#endif
    Tcl_CreateCommand (ip, "tux_friction",   friction_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_course_author", course_author_cb, 0,0);
    Tcl_CreateCommand (ip, "tux_course_name", course_name_cb, 0,0);
    Tcl_CreateCommand (ip, "tux_base_height_value", base_height_value_cb, 0,0);
    Tcl_CreateCommand (ip, "tux_tree_props",  tree_props_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_item_spec",  item_spec_cb,   0,0);
}

