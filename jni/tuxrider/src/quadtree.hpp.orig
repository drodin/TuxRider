// quadtree.hpp	-thatcher 9/15/1999 Copyright 1999-2000 Thatcher Ulrich

// Data structures for quadtree terrain storage.

// This code may be freely modified and redistributed.  I make no
// warrantees about it; use at your own risk.  If you do incorporate
// this code into a project, I'd appreciate a mention in the credits.
//
// Thatcher Ulrich <tu@tulrich.com>

// Modified for use in Tux Racer by Jasmin Patry <jfpatry@cgl.uwaterloo.ca>


#ifndef QUADTREE_HPP
#define QUADTREE_HPP


#include "tuxracer.h"
#include "viewfrustum.h"

typedef enum {
    East,
    South,
    Center
} vertex_loc_t;

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef short int16;
typedef int int32;


struct HeightMapInfo {
    scalar_t *Data;
    int	XOrigin, ZOrigin;
    int	XSize, ZSize;
    int	RowWidth;
    int	Scale;

    float	Sample(int x, int z) const;
};


struct	VertInfo {
    float	Y;
};


struct quadsquare;


// A structure used during recursive traversal of the tree to hold
// relevant but transitory data.
class quadcornerdata {
public:
    const quadcornerdata*	Parent;
    quadsquare*	Square;
    int	ChildIndex;
    int	Level;
    int	xorg, zorg;
    VertInfo	Verts[4];	// ne, nw, sw, se
};


struct quadsquare {
    quadsquare*	Child[4];

    VertInfo	Vertex[5];	// center, e, n, w, s
    float	Error[6];	// e, s, children: ne, nw, sw, se
    float	MinY, MaxY;	// Bounds for frustum culling and error testing.
    unsigned char	EnabledFlags;	// bits 0-7: e, n, w, s, ne, nw, sw, se
    unsigned char	SubEnabledCount[2];	// e, s enabled reference counts.
    bool	Static;
    bool	Dirty;	// Set when vertex data has changed, but error/enabled data has not been recalculated.

    bool ForceEastVert;
    bool ForceSouthVert;

    static double ScaleX, ScaleZ;
    static int RowSize, NumRows;
    static terrain_t *Terrain;
    static GLuint TexId[NumTerrains];
    static GLuint EnvmapTexId;

    static GLuint *VertexArrayIndices;
    static GLuint VertexArrayCounter;
    static GLuint VertexArrayMinIdx;
    static GLuint VertexArrayMaxIdx;

    static void MakeTri( int a, int b, int c, int terrain );
    static void MakeSpecialTri( int a, int b, int c, int terrain );
    static void MakeNoBlendTri( int a, int b, int c, int terrain );

    static void DrawTris();
    static void DrawEnvmapTris();
    static void InitArrayCounters();


// public:
    quadsquare(quadcornerdata* pcd);
    ~quadsquare();

    void	AddHeightMap(const quadcornerdata& cd, const HeightMapInfo& hm);
    void	StaticCullData(const quadcornerdata& cd, float ThresholdDetail);	
    float	RecomputeError(const quadcornerdata& cd);
    int	CountNodes();
	
    void	Update(const quadcornerdata& cd, const float ViewerLocation[3], float Detail);
    void	Render(const quadcornerdata& cd, GLubyte *vnc_array);

    float	GetHeight(const quadcornerdata& cd, float x, float z);

    void	SetScale(double x, double z);
    void	SetTerrain(terrain_t *terrain);
	
private:
    void	EnableEdgeVertex(int index, bool IncrementCount, const quadcornerdata& cd);
    quadsquare*	EnableDescendant(int count, int stack[], const quadcornerdata& cd);
    void	EnableChild(int index, const quadcornerdata& cd);
    void	NotifyChildDisable(const quadcornerdata& cd, int index);

    void	ResetTree();
    void	StaticCullAux(const quadcornerdata& cd, float ThresholdDetail, int TargetLevel);

    quadsquare*	GetNeighbor(int dir, const quadcornerdata& cd);
    void	CreateChild(int index, const quadcornerdata& cd);
    void	SetupCornerData(quadcornerdata* q, const quadcornerdata& pd, int ChildIndex);

    void	UpdateAux(const quadcornerdata& cd, const float ViewerLocation[3], float CenterError, clip_result_t vis);
    void	RenderAux(const quadcornerdata& cd, clip_result_t vis,
			  int terrain);
    void	SetStatic(const quadcornerdata& cd);
    void	InitVert(int i, int x, int z);

    bool	VertexTest(int x, float y, int z, float error, const float Viewer[3], int level, vertex_loc_t vertex_loc );
    bool	BoxTest(int x, int z, float size, float miny, float maxy, float error, const float Viewer[3]);
    clip_result_t ClipSquare( const quadcornerdata& cd );
};


#endif // QUADTREE_HPP
