
#include "tuxracer.h"
#include "course_load.h"
#include "quadtree.hpp"

#define CULL_DETAIL_FACTOR 25

static quadsquare *root = (quadsquare*) NULL;
static quadcornerdata root_corner_data = { (quadcornerdata*)NULL };


extern "C" void reset_course_quadtree()
{
    delete root;
    root = (quadsquare*)NULL;
}

static int get_root_level( int nx, int nz )
{
    int xlev, zlev;

    check_assertion( nx > 0, "heightmap has x dimension of 0 size" );
    check_assertion( nz > 0, "heightmap has z dimension of 0 size" );

    xlev = (int) ( log( nx ) / log ( 2.0 ) );
    zlev = (int) ( log( nz ) / log ( 2.0 ) );


    /* Check to see if nx, nz are powers of 2 
     */

    if ( ( nx >> xlev ) << xlev == nx ) {
	/* do nothing */
    } else {
	nx += 1;
    }

    if ( ( nz >> zlev ) << zlev == nz ) {
	/* do nothing */
    } else {
	nz += 1;
    }

    return max( xlev, zlev );
}

static void point_to_float_array( float dest[3], point_t src )
{
    dest[0] = src.x;
    dest[1] = src.y;
    dest[2] = src.z;
}


extern "C" void init_course_quadtree( scalar_t *elevation, int nx, int nz, 
			   scalar_t scalex, scalar_t scalez,
			   point_t view_pos, scalar_t detail )
{
    HeightMapInfo hm;
    int i;

    hm.Data = elevation;
    hm.XOrigin = 0;
    hm.ZOrigin = 0;
    hm.XSize = nx;
    hm.ZSize = nz;
    hm.RowWidth = hm.XSize;
    hm.Scale = 0;

    root_corner_data.Square = (quadsquare*)NULL;
    root_corner_data.ChildIndex = 0;
    root_corner_data.Level = get_root_level( nx, nz );
    root_corner_data.xorg = 0;
    root_corner_data.zorg = 0;

    for (i=0; i<4; i++) {
	root_corner_data.Verts[i].Y = 0;
	root_corner_data.Verts[i].Y = 0;
    }

    root = new quadsquare( &root_corner_data );

    root->AddHeightMap( root_corner_data, hm );
    root->SetScale( scalex, scalez );
    root->SetTerrain( get_course_terrain_data() );

    // Debug info.
    print_debug( DEBUG_QUADTREE, "nodes = %d\n", root->CountNodes());
    print_debug( DEBUG_QUADTREE, "max error = %g\n", 
		 root->RecomputeError(root_corner_data));

    // Get rid of unnecessary nodes in flat-ish areas.
    print_debug( DEBUG_QUADTREE, 
		 "Culling unnecessary nodes (detail factor = %d)...\n",
		 CULL_DETAIL_FACTOR);
    root->StaticCullData(root_corner_data, CULL_DETAIL_FACTOR);

    // Post-cull debug info.
    print_debug( DEBUG_QUADTREE, "nodes = %d\n", root->CountNodes());
    print_debug( DEBUG_QUADTREE, "max error = %g\n", 
		 root->RecomputeError(root_corner_data));


    // Run the update function a few times before we start rendering
    // to disable unnecessary quadsquares, so the first frame won't
    // be overloaded with tons of triangles.

    float ViewerLoc[3];
    point_to_float_array( ViewerLoc, view_pos );

    for (i = 0; i < 10; i++) {
	root->Update(root_corner_data, (const float*) ViewerLoc, 
		     detail);
    }
}


extern "C" void update_course_quadtree( point_t view_pos, float detail )
{
    float ViewerLoc[3];

    point_to_float_array( ViewerLoc, view_pos );

    root->Update( root_corner_data, ViewerLoc, detail );
}

extern "C" void render_course_quadtree()
{
    GLubyte *vnc_array;

    get_gl_arrays( &vnc_array );

    root->Render( root_corner_data, vnc_array );
}
