/*
===========================================================================

Wolfenstein: Enemy Territory GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Wolfenstein: Enemy Territory GPL Source Code (Wolf ET Source Code).  

Wolf ET Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolf ET Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolf ET Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Wolf: ET Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Wolf ET Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/


//#include "qfiles.h"
// BSP structs from Q3
#define BSP_IDENT	(('P'<<24)+('S'<<16)+('B'<<8)+'I')
// little-endian "IBSP"

#define BSP_VERSION			46
#define BSP_VERSION_B		47


// there shouldn't be any problem with increasing these values at the
// expense of more memory allocation in the utilities
#define	MAX_MAP_MODELS		0x400
#define	MAX_MAP_BRUSHES		0x8000
#define	MAX_MAP_ENTITIES	0x800
#define	MAX_MAP_ENTSTRING	0x40000
#define	MAX_MAP_SHADERS		0x400

#define	MAX_MAP_AREAS		0x100	// MAX_MAP_AREA_BYTES in q_shared must match!
#define	MAX_MAP_FOGS		0x100
#define	MAX_MAP_PLANES		0x20000
#define	MAX_MAP_NODES		0x20000
#define	MAX_MAP_BRUSHSIDES	0x20000
#define	MAX_MAP_LEAFS		0x20000
#define	MAX_MAP_LEAFFACES	0x20000
#define	MAX_MAP_LEAFBRUSHES 0x40000
#define	MAX_MAP_PORTALS		0x20000
#define	MAX_MAP_LIGHTING	0x800000
#define	MAX_MAP_LIGHTGRID	0x800000
#define	MAX_MAP_VISIBILITY	0x200000

#define	MAX_MAP_DRAW_SURFS	0x20000
#define	MAX_MAP_DRAW_VERTS	0x80000
#define	MAX_MAP_DRAW_INDEXES	0x80000


// key / value pair sizes in the entities lump
#define	MAX_KEY				32
#define	MAX_VALUE			1024

// the editor uses these predefined yaw angles to orient entities up or down
#define	ANGLE_UP			-1
#define	ANGLE_DOWN			-2

#define	LIGHTMAP_WIDTH		128
#define	LIGHTMAP_HEIGHT		128

#define MAX_WORLD_COORD		( 128*1024 )
#define MIN_WORLD_COORD		( -128*1024 )
#define WORLD_SIZE			( MAX_WORLD_COORD - MIN_WORLD_COORD )

//=============================================================================
#define	MAX_QPATH		64

typedef struct {
	int		fileofs, filelen;
} lump_t;

#define	LUMP_ENTITIES		0
#define	LUMP_SHADERS		1
#define	LUMP_PLANES			2
#define	LUMP_NODES			3
#define	LUMP_LEAFS			4
#define	LUMP_LEAFSURFACES	5
#define	LUMP_LEAFBRUSHES	6
#define	LUMP_MODELS			7
#define	LUMP_BRUSHES		8
#define	LUMP_BRUSHSIDES		9
#define	LUMP_DRAWVERTS		10
#define	LUMP_DRAWINDEXES	11
#define	LUMP_FOGS			12
#define	LUMP_SURFACES		13
#define	LUMP_LIGHTMAPS		14
#define	LUMP_LIGHTGRID		15
#define	LUMP_VISIBILITY		16
#define	HEADER_LUMPS		17

typedef struct {
	int			ident;
	int			version;

	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

typedef struct {
	float		mins[3], maxs[3];
	int			firstSurface, numSurfaces;
	int			firstBrush, numBrushes;
} dmodel_t;

typedef struct {
	char		shader[MAX_QPATH];
	int			surfaceFlags;
	int			contentFlags;
} dshader_t;

// planes x^1 is allways the opposite of plane x

typedef struct {
	float		normal[3];
	float		dist;
} dplane_t;

typedef struct {
	int			planeNum;
	int			children[2];	// negative numbers are -(leafs+1), not nodes
	int			mins[3];		// for frustom culling
	int			maxs[3];
} dnode_t;

typedef struct {
	int			cluster;			// -1 = opaque cluster (do I still store these?)
	int			area;

	int			mins[3];			// for frustum culling
	int			maxs[3];

	int			firstLeafSurface;
	int			numLeafSurfaces;

	int			firstLeafBrush;
	int			numLeafBrushes;
} dleaf_t;

typedef struct {
	int			planeNum;			// positive plane side faces out of the leaf
	int			shaderNum;
} dbrushside_t;

typedef struct {
	int			firstSide;
	int			numSides;
	int			shaderNum;		// the shader that determines the contents flags
} dbrush_t;

typedef struct {
	char		shader[MAX_QPATH];
	int			brushNum;
	int			visibleSide;	// the brush side that ray tests need to clip against (-1 == none)
} dfog_t;

typedef struct {
	vec3_t		xyz;
	float		st[2];
	float		lightmap[2];
	vec3_t		normal;
	byte		color[4];
} drawVert_t;

#define drawVert_t_cleared(x) drawVert_t (x) = {{0, 0, 0}, {0, 0}, {0, 0}, {0, 0, 0}, {0, 0, 0, 0}}

typedef enum {
	MST_BAD,
	MST_PLANAR,
	MST_PATCH,
	MST_TRIANGLE_SOUP,
	MST_FLARE
} mapSurfaceType_t;

typedef struct {
	int			shaderNum;
	int			fogNum;
	int			surfaceType;

	int			firstVert;
	int			numVerts;

	int			firstIndex;
	int			numIndexes;

	int			lightmapNum;
	int			lightmapX, lightmapY;
	int			lightmapWidth, lightmapHeight;

	vec3_t		lightmapOrigin;
	vec3_t		lightmapVecs[3];	// for patches, [0] and [1] are lodbounds

	int			patchWidth;
	int			patchHeight;
} dsurface_t;

#include "q_shared.h"
//#include "../../shared/tr_types.h"
#ifdef __cplusplus
extern "C" {
#endif
//void            CM_LoadMap(const char *name, qboolean clientload, int *checksum);
void CM_LoadMap(const char *name, char* buf, qboolean clientload, int *checksum);
void            CM_ClearMap(void);

clipHandle_t    CM_InlineModel(int index);	// 0 = world, 1 + are bmodels
clipHandle_t    CM_TempBoxModel(const vec3_t mins, const vec3_t maxs, int capsule);
void            CM_SetTempBoxModelContents(int contents);	// DHM - Nerve

void            CM_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs);

int             CM_NumClusters(void);
int             CM_NumInlineModels(void);
char           *CM_EntityString(void);

// returns an ORed contents mask
int             CM_PointContents(const vec3_t p, clipHandle_t model);
int             CM_TransformedPointContents(const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles);

void            CM_BoxTrace(trace_t * results, const vec3_t start, const vec3_t end,
							const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, int capsule);
void            CM_TransformedBoxTrace(trace_t * results, const vec3_t start, const vec3_t end,
									   const vec3_t mins, const vec3_t maxs,
									   clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles, int capsule);

byte           *CM_ClusterPVS(int cluster);

int             CM_PointLeafnum(const vec3_t p);

// only returns non-solid leafs
// overflow if return listsize and if *lastLeaf != list[listsize-1]
int             CM_BoxLeafnums(const vec3_t mins, const vec3_t maxs, int *list, int listsize, int *lastLeaf);

int             CM_LeafCluster(int leafnum);
int             CM_LeafArea(int leafnum);

void            CM_AdjustAreaPortalState(int area1, int area2, qboolean open);
qboolean        CM_AreasConnected(int area1, int area2);

int             CM_WriteAreaBits(byte * buffer, int area);

// cm_tag.c
//int             CM_LerpTag(orientation_t * tag, const refEntity_t * refent, const char *tagName, int startIndex);


// cm_marks.c
int             CM_MarkFragments(int numPoints, const vec3_t * points, const vec3_t projection,
								 int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t * fragmentBuffer);

// cm_patch.c
void            CM_DrawDebugSurface(void (*drawPoly) (int color, int numPoints, float *points));

#ifdef __cplusplus
};
#endif