#pragma once

#include "Inverse.h"
#include <Ogre.h>

#include "cm_local.h"

#define LittleLong(x) (x)
#define LittleFloat(x) (x)

#define OGRE_RESGROUP_NAME "Inverse"

struct cshader_t
{
	Ogre::MaterialPtr material;
	int surfaceFlags;
	int surfaceContents;
};

typedef enum {
	SF_BAD,
	SF_SKIP,				// ignore
	SF_FACE,
	SF_GRID,
	SF_TRIANGLES,
	SF_POLY,
	SF_MDV,
	SF_MD4,
	SF_MDR,
	SF_IQM,
	SF_FLARE,
	SF_ENTITY,				// beams, rails, lightning, etc that can be determined by entity
	SF_DISPLAY_LIST,
	SF_VBO_MESH,
	SF_VBO_MDVMESH,

	SF_NUM_SURFACE_TYPES,
	SF_MAX = 0x7fffffff			// ensures that sizeof( surfaceType_t ) == sizeof( int )
} surfaceType_t;

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2

#define CULLINFO_NONE   0
#define CULLINFO_BOX    1
#define CULLINFO_SPHERE 2
#define CULLINFO_PLANE  4

/*typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte	pad[2];
} cplane_t;*/

typedef struct cullinfo_s {
	int             type;
	vec3_t          bounds[2];
	vec3_t			localOrigin;
	float			radius;
	cplane_t        plane;
} cullinfo_t;


typedef Ogre::Material shader_t;
typedef int glIndex_t;
//typedef bool qboolean;

typedef shader_t* qhandle_t;

//#define qtrue true
//#define qfalse false

/*typedef enum
{ qfalse, qtrue } qboolean;*/

typedef struct {
	vec3_t		xyz;
	float		st[2];
	byte		modulate[4];
} polyVert_t;

typedef struct srfPoly_s {
	surfaceType_t	surfaceType;
	qhandle_t		hShader;
	int				fogIndex;
	int				numVerts;
	polyVert_t		*verts;
} srfPoly_t;

typedef struct srfDisplayList_s {
	surfaceType_t	surfaceType;
	int				listNum;
} srfDisplayList_t;


typedef struct srfFlare_s {
	surfaceType_t	surfaceType;
	vec3_t			origin;
	vec3_t			normal;
	vec3_t			color;
} srfFlare_t;

typedef struct
{
	vec3_t          xyz;
	vec2_t          st;
	vec2_t          lightmap;
	vec3_t          normal;
#ifdef USE_VERT_TANGENT_SPACE
	vec3_t          tangent;
	vec3_t          bitangent;
#endif
	vec3_t          lightdir;
	vec4_t			vertexColors;

#if DEBUG_OPTIMIZEVERTICES
	unsigned int    id;
#endif
} srfVert_t;

#ifdef USE_VERT_TANGENT_SPACE
#define srfVert_t_cleared(x) srfVert_t (x) = {{0, 0, 0}, {0, 0}, {0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0, 0}}
#else
#define srfVert_t_cleared(x) srfVert_t (x) = {{0, 0, 0}, {0, 0}, {0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0, 0}}
#endif


typedef struct
{
	int             indexes[3];
	int             neighbors[3];
	vec4_t          plane;
	qboolean        facingLight;
	qboolean        degenerated;
} srfTriangle_t;


typedef struct srfGridMesh_s
{
	surfaceType_t   surfaceType;

	// dynamic lighting information
	int				dlightBits;
	int             pshadowBits;

	// culling information
	vec3_t			meshBounds[2];
	vec3_t			localOrigin;
	float			meshRadius;

	// lod information, which may be different
	// than the culling information to allow for
	// groups of curves that LOD as a unit
	vec3_t			lodOrigin;
	float			lodRadius;
	int				lodFixed;
	int				lodStitched;

	// vertexes
	int				width, height;
	float			*widthLodError;
	float			*heightLodError;

	int             numTriangles;
	srfTriangle_t  *triangles;

	int             numVerts;
	srfVert_t      *verts;

	// BSP VBO offsets
	int             firstVert;
	int             firstIndex;
	glIndex_t       minIndex;
	glIndex_t       maxIndex;
} srfGridMesh_t;


typedef struct
{
	surfaceType_t   surfaceType;

	// dynamic lighting information
	int			dlightBits;
	int         pshadowBits;

	// culling information
	cplane_t        plane;
	//	vec3_t          bounds[2];

	// triangle definitions
	int             numTriangles;
	srfTriangle_t  *triangles;

	int             numVerts;
	srfVert_t      *verts;

	// BSP VBO offsets
	int             firstVert;
	int             firstIndex;
	glIndex_t       minIndex;
	glIndex_t       maxIndex;
} srfSurfaceFace_t;


// misc_models in maps are turned into direct geometry by xmap
typedef struct
{
	surfaceType_t   surfaceType;

	// dynamic lighting information
	int			dlightBits;
	int         pshadowBits;

	// culling information
	//	vec3_t          bounds[2];

	// triangle definitions
	int             numTriangles;
	srfTriangle_t  *triangles;

	int             numVerts;
	srfVert_t      *verts;

	// BSP VBO offsets
	int             firstVert;
	int             firstIndex;
	glIndex_t       minIndex;
	glIndex_t       maxIndex;
} srfTriangles_t;
typedef struct msurface_s {
	//int					viewCount;		// if == tr.viewCount, already added
	Ogre::MaterialPtr   shader;
	int					fogIndex;
	cullinfo_t          cullinfo;

	surfaceType_t		*data;			// any of srf*_t
} msurface_t;


#define	CONTENTS_NODE		-1
typedef struct mnode_s {
	// common with leaf and node
	int			contents;		// -1 for nodes, to differentiate from leafs
	int             visCounts[1];	// node needs to be traversed if current
	vec3_t		mins, maxs;		// for bounding box culling
	struct mnode_s	*parent;

	// node specific
	cplane_t	*plane;
	struct mnode_s	*children[2];	

	// leaf specific
	int			cluster;
	int			area;

	int         firstmarksurface;
	int			nummarksurfaces;
} mnode_t;

typedef struct {
	vec3_t		bounds[2];		// for culling
	int	        firstSurface;
	int			numSurfaces;
} bmodel_t;

typedef struct {
	char		name[MAX_QPATH];		// ie: maps/tim_dm2.bsp
	char		baseName[MAX_QPATH];	// ie: tim_dm2

	int			dataSize;

	int			numShaders;
	dshader_t	*shaders;

	int			numBModels;
	bmodel_t	*bmodels;

	int			numplanes;
	cplane_t	*planes;

	int			numnodes;		// includes leafs
	int			numDecisionNodes;
	mnode_t		*nodes;

	int         numWorldSurfaces;

	int			numsurfaces;
	msurface_t	*surfaces;
	int         *surfacesViewCount;
	int         *surfacesDlightBits;
	int			*surfacesPshadowBits;

	int			numMergedSurfaces;
	msurface_t	*mergedSurfaces;
	int         *mergedSurfacesViewCount;
	int         *mergedSurfacesDlightBits;
	int			*mergedSurfacesPshadowBits;

	int			nummarksurfaces;
	int         *marksurfaces;
	int         *viewSurfaces;

	int			numfogs;

	vec3_t		lightGridOrigin;
	vec3_t		lightGridSize;
	vec3_t		lightGridInverseSize;
	int			lightGridBounds[3];
	byte		*lightGridData;
	float		*hdrLightGrid;


	int			numClusters;
	int			clusterBytes;
	const byte	*vis;			// may be passed in by CM_LoadMap to save space

	byte		*novis;			// clusterBytes of 0xff

	char		*entityString;
	char		*entityParsePoint;
} world_t;

#define DotProduct( x,y )         ( ( x )[0] * ( y )[0] + ( x )[1] * ( y )[1] + ( x )[2] * ( y )[2] )
#define VectorSubtract( a,b,c )   ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1],( c )[2] = ( a )[2] - ( b )[2] )
#define VectorAdd( a,b,c )        ( ( c )[0] = ( a )[0] + ( b )[0],( c )[1] = ( a )[1] + ( b )[1],( c )[2] = ( a )[2] + ( b )[2] )
#define VectorCopy( a,b )         ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2] )
#define VectorScale( v, s, o )    ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ),( o )[2] = ( v )[2] * ( s ) )
#define VectorMA( v, s, b, o )    ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ) )

static inline vec_t VectorLength( const vec3_t v ) {
	return (vec_t)sqrt (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static inline vec_t VectorLengthSquared( const vec3_t v ) {
	return (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static inline void ClearBounds( vec3_t mins, vec3_t maxs ) {
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}

static inline void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs ) {
	if ( v[0] < mins[0] ) {
		mins[0] = v[0];
	}
	if ( v[0] > maxs[0]) {
		maxs[0] = v[0];
	}

	if ( v[1] < mins[1] ) {
		mins[1] = v[1];
	}
	if ( v[1] > maxs[1]) {
		maxs[1] = v[1];
	}

	if ( v[2] < mins[2] ) {
		mins[2] = v[2];
	}
	if ( v[2] > maxs[2]) {
		maxs[2] = v[2];
	}
}

#define MAX_GRID_SIZE 129

#define VectorClear(a)			((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b)		((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])

static inline void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross ) {
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

#define Com_Memset memset
#define Com_Memcpy memcpy


static inline vec_t VectorNormalize2( const vec3_t v, vec3_t out) {
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

	if (length)
	{
		/* writing it this way allows gcc to recognize that rsqrt can be used */
		ilength = 1/(float)sqrt (length);
		/* sqrt(length) = length * (1 / sqrt(length)) */
		length *= ilength;
		out[0] = v[0]*ilength;
		out[1] = v[1]*ilength;
		out[2] = v[2]*ilength;
	} else {
		VectorClear( out );
	}

	return length;

}

static inline vec_t VectorNormalize( vec3_t v ) {
	// NOTE: TTimo - Apple G4 altivec source uses double?
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

	if ( length ) {
		/* writing it this way allows gcc to recognize that rsqrt can be used */
		ilength = 1/(float)sqrt (length);
		/* sqrt(length) = length * (1 / sqrt(length)) */
		length *= ilength;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

#define MAX_PATCH_SIZE 32
#define MAX_FACE_POINTS 64

srfGridMesh_t *R_SubdividePatchToGrid( int width, int height,
									  srfVert_t points[MAX_PATCH_SIZE*MAX_PATCH_SIZE] );

struct InvWorld
{
	cshader_t* materials;
	int numMaterials;

	msurface_t* surfaces;
	int numSurfaces;

	Ogre::HardwareVertexBufferSharedPtr worldVBO;
	Ogre::HardwareIndexBufferSharedPtr worldIBO;

	Ogre::VertexDeclaration* vDecl;
	Ogre::VertexBufferBinding* vBind;
};

extern Ogre::Root* ogre_root;
extern Ogre::SceneManager* ogre_sm;
extern Ogre::Camera* ogre_camera;

extern InvWorld* rWorld;

#if 0
typedef struct
{
	cplane_t       *plane;
	int             children[2];	// negative numbers are leafs
} cNode_t;

typedef struct
{
	int             cluster;
	int             area;

	int             firstLeafBrush;
	int             numLeafBrushes;

	int             firstLeafSurface;
	int             numLeafSurfaces;
} cLeaf_t;

typedef struct cmodel_s
{
	vec3_t          mins, maxs;
	cLeaf_t         leaf;		// submodels don't reference the main tree
} cmodel_t;

typedef struct cbrushedge_s
{
	vec3_t          p0;
	vec3_t          p1;
} cbrushedge_t;

typedef struct
{
	int     numpoints;
	int     maxpoints;
	float   points[8][5];           // variable sized
} winding_t;


typedef struct
{
	cplane_t       *plane;
	int             planeNum;
	int             surfaceFlags;
	int             shaderNum;
	winding_t      *winding;
} cbrushside_t;

typedef struct
{
	int             shaderNum;	// the shader that determined the contents
	int             contents;
	vec3_t          bounds[2];
	int             numsides;
	cbrushside_t   *sides;
	int             checkcount;	// to avoid repeated testings
	qboolean        collided;	// marker for optimisation
	cbrushedge_t   *edges;
	int             numEdges;
} cbrush_t;


typedef struct cPlane_s
{
	float           plane[4];
	int             signbits;	// signx + (signy<<1) + (signz<<2), used as lookup during collision
	struct cPlane_s *hashChain;
} cPlane_t;

// 3 or four + 6 axial bevels + 4 or 3 * 4 edge bevels
#define MAX_FACET_BEVELS (4 + 6 + 16)

// a facet is a subdivided element of a patch aproximation or model
typedef struct
{
	int             surfacePlane;

	int             numBorders;
	int             borderPlanes[MAX_FACET_BEVELS];
	int             borderInward[MAX_FACET_BEVELS];
	qboolean        borderNoAdjust[MAX_FACET_BEVELS];
} cFacet_t;

typedef struct cSurfaceCollide_s
{
	vec3_t          bounds[2];

	int             numPlanes;	// surface planes plus edge planes
	cPlane_t       *planes;

	int             numFacets;
	cFacet_t       *facets;
} cSurfaceCollide_t;

typedef struct
{
	int             type;

	int             checkcount;	// to avoid repeated testings
	int             surfaceFlags;
	int             contents;

	cSurfaceCollide_t *sc;
} cSurface_t;

typedef struct
{
	int             floodnum;
	int             floodvalid;
} cArea_t;

typedef struct
{
	char            name[MAX_QPATH];

	int             numShaders;
	dshader_t      *shaders;

	int             numBrushSides;
	cbrushside_t   *brushsides;

	int             numPlanes;
	cplane_t       *planes;

	int             numNodes;
	cNode_t        *nodes;

	int             numLeafs;
	cLeaf_t        *leafs;

	int             numLeafBrushes;
	int            *leafbrushes;

	int             numLeafSurfaces;
	int            *leafsurfaces;

	int             numSubModels;
	cmodel_t       *cmodels;

	int             numBrushes;
	cbrush_t       *brushes;

	int             numClusters;
	int             clusterBytes;
	byte           *visibility;
	qboolean        vised;		// if false, visibility is just a single cluster of ffs

	int             numEntityChars;
	char           *entityString;

	int             numAreas;
	cArea_t        *areas;
	int            *areaPortals;	// [ numAreas*numAreas ] reference counts

	int             numSurfaces;
	cSurface_t    **surfaces;	// non-patches will be NULL

	int             floodvalid;
	int             checkcount;	// incremented on each trace

	qboolean        perPolyCollision;
} clipMap_t;
#endif