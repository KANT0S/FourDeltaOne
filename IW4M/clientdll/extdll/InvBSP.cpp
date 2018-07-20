// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: extdll
// Purpose: BSP loading code.
//
// Initial author: NTAuthority
// Started: 2013-05-21
// ==========================================================

#include "StdInc.h"

#ifdef ENABLE_INVERSE
#include "InvOgre.h"

InvWorld* rWorld;
static InvWorld invWorld;
static char* fileBase;

static void LoadShaders(lump_t* shaders)
{
	dshader_t* start = (dshader_t*)(fileBase + shaders->fileofs);
	int count = shaders->filelen / sizeof(dshader_t);
	
	invWorld.materials = new cshader_t[count];
	invWorld.numMaterials = count;

	for (int i = 0; i < count; i++)
	{
		//Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName( OGRE_RESGROUP_NAME);//Ogre::MaterialPtr(NULL);//Ogre::MaterialManager::getSingleton().getByName(start[i].shader, OGRE_RESGROUP_NAME);
		Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(start[i].shader, OGRE_RESGROUP_NAME);

		if (matPtr.isNull())
		{
			matPtr = Ogre::MaterialManager::getSingleton().create(start[i].shader, OGRE_RESGROUP_NAME);
			
			std::string texture_path = "Q:\\DLs\\aa2map\\example\\";
			texture_path += start[i].shader;
			texture_path += ".jpg";

			std::ifstream ifs(texture_path.c_str(), std::ios::binary | std::ios::in);

			if (!ifs.is_open())
			{
				texture_path = "Q:\\DLs\\aa2map\\example\\";
				texture_path += start[i].shader;
				texture_path += ".tga";

				ifs = std::ifstream(texture_path.c_str(), std::ios::binary | std::ios::in);
			}

			if (!ifs.is_open())
			{
				texture_path = "Q:\\DLs\\aa2map\\example\\";
				texture_path += start[i].shader;
				texture_path += ".bmp";

				ifs = std::ifstream(texture_path.c_str(), std::ios::binary | std::ios::in);
			}

			if (!ifs.is_open())
			{
				texture_path = "Q:\\DLs\\aa2map\\example\\";
				texture_path += start[i].shader;
				texture_path += ".png";

				ifs = std::ifstream(texture_path.c_str(), std::ios::binary | std::ios::in);
			}

			if (ifs.is_open())
			{
				Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(texture_path, &ifs, false));
				Ogre::Image img;
				img.load(data_stream);

				try
				{
					Ogre::TexturePtr texPtr = Ogre::TextureManager::getSingleton().loadImage(texture_path, OGRE_RESGROUP_NAME, img);

					matPtr->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName(texture_path);
				}
				catch (...)
				{

				}
			}
			//Com_Error(0, "Unknown material %s referenced from BSP", start[i].shader);
		}

		invWorld.materials[i].material = matPtr;
		invWorld.materials[i].surfaceContents = start[i].contentFlags;
		invWorld.materials[i].surfaceFlags = start[i].surfaceFlags;
	}
}

static void LoadPlanes(lump_t* planes)
{
	// no-op for now
}

static int FatLightmap(int lm)
{
	return lm;
}

static int FatPackU(float i, int lm)
{
	return i;
}

static int FatPackV(float i, int lm)
{
	return i;
}

void SetPlaneSignbits (cplane_t *out);

/*void SetPlaneSignbits (cplane_t *out) {
	int	bits, j;

	// for fast box on planeside test
	bits = 0;
	for (j=0 ; j<3 ; j++) {
		if (out->normal[j] < 0) {
			bits |= 1<<j;
		}
	}
	out->signbits = bits;
}*/

/*
===============
ParseFace
===============
*/
static void ParseFace( dsurface_t *ds, drawVert_t *verts, float *hdrVertColors, msurface_t *surf, int *indexes  ) {
	int			i, j;
	srfSurfaceFace_t	*cv;
	srfTriangle_t  *tri;
	int			numVerts, numTriangles, badTriangles;
	int realLightmapNum;

	realLightmapNum = LittleLong( ds->lightmapNum );

	// get fog volume
	surf->fogIndex = LittleLong( ds->fogNum ) + 1;

	// get shader value
	surf->shader = invWorld.materials[ds->shaderNum].material;
	/*if ( r_singleShader->integer && !surf->shader->isSky ) {
		surf->shader = tr.defaultShader;
	}*/

	numVerts = LittleLong(ds->numVerts);
	if (numVerts > MAX_FACE_POINTS) {
		Com_Printf( 0, "WARNING: MAX_FACE_POINTS exceeded: %i\n", numVerts);
		numVerts = MAX_FACE_POINTS;
		//surf->shader = tr.defaultShader;
	}

	numTriangles = LittleLong(ds->numIndexes) / 3;

	//cv = ri.Hunk_Alloc(sizeof(*cv), h_low);
	cv = (srfSurfaceFace_t *)surf->data;
	cv->surfaceType = SF_FACE;

	cv->numTriangles = numTriangles;
	cv->triangles = (srfTriangle_t *)malloc(numTriangles * sizeof(cv->triangles[0]));

	cv->numVerts = numVerts;
	cv->verts = (srfVert_t *)malloc(numVerts * sizeof(cv->verts[0]));

	// copy vertexes
	surf->cullinfo.type = CULLINFO_PLANE | CULLINFO_BOX;
	ClearBounds(surf->cullinfo.bounds[0], surf->cullinfo.bounds[1]);
	verts += LittleLong(ds->firstVert);
	for(i = 0; i < numVerts; i++)
	{
		vec4_t color;

		for(j = 0; j < 3; j++)
		{
			cv->verts[i].xyz[j] = LittleFloat(verts[i].xyz[j]);
			cv->verts[i].normal[j] = LittleFloat(verts[i].normal[j]);
		}
		AddPointToBounds(cv->verts[i].xyz, surf->cullinfo.bounds[0], surf->cullinfo.bounds[1]);
		for(j = 0; j < 2; j++)
		{
			cv->verts[i].st[j] = LittleFloat(verts[i].st[j]);
			//cv->verts[i].lightmap[j] = LittleFloat(verts[i].lightmap[j]);
		}
		cv->verts[i].lightmap[0] = FatPackU(LittleFloat(verts[i].lightmap[0]), realLightmapNum);
		cv->verts[i].lightmap[1] = FatPackV(LittleFloat(verts[i].lightmap[1]), realLightmapNum);

		/*if (hdrVertColors)
		{
			color[0] = hdrVertColors[(ds->firstVert + i) * 3    ];
			color[1] = hdrVertColors[(ds->firstVert + i) * 3 + 1];
			color[2] = hdrVertColors[(ds->firstVert + i) * 3 + 2];
		}
		else
		{
			//hack: convert LDR vertex colors to HDR
			if (r_hdr->integer)
			{
				color[0] = verts[i].color[0] + 1.0f;
				color[1] = verts[i].color[1] + 1.0f;
				color[2] = verts[i].color[2] + 1.0f;
			}
			else
			{
				color[0] = verts[i].color[0];
				color[1] = verts[i].color[1];
				color[2] = verts[i].color[2];
			}
		}*/
		color[0] = verts[i].color[0];
		color[1] = verts[i].color[1];
		color[2] = verts[i].color[2];
		color[3] = verts[i].color[3] / 255.0f;

		cv->verts[i].vertexColors[0] = color[0];
		cv->verts[i].vertexColors[1] = color[1];
		cv->verts[i].vertexColors[2] = color[2];
		cv->verts[i].vertexColors[3] = color[3];

		//R_ColorShiftLightingFloats( color, cv->verts[i].vertexColors, 1.0f / 255.0f );
	}

	// copy triangles
	badTriangles = 0;
	indexes += LittleLong(ds->firstIndex);
	for(i = 0, tri = cv->triangles; i < numTriangles; i++, tri++)
	{
		for(j = 0; j < 3; j++)
		{
			tri->indexes[j] = LittleLong(indexes[i * 3 + j]);

			if(tri->indexes[j] < 0 || tri->indexes[j] >= numVerts)
			{
				Com_Error(0, "Bad index in face surface");
			}
		}

		if ((tri->indexes[0] == tri->indexes[1]) || (tri->indexes[1] == tri->indexes[2]) || (tri->indexes[0] == tri->indexes[2]))
		{
			tri--;
			badTriangles++;
		}
	}

	if (badTriangles)
	{
		Com_Printf(0, "Face has bad triangles, originally shader %s %d tris %d verts, now %d tris\n", "apple", numTriangles, numVerts, numTriangles - badTriangles);
		cv->numTriangles -= badTriangles;
	}

	// take the plane information from the lightmap vector
	for ( i = 0 ; i < 3 ; i++ ) {
		cv->plane.normal[i] = LittleFloat( ds->lightmapVecs[2][i] );
	}
	cv->plane.dist = DotProduct( cv->verts[0].xyz, cv->plane.normal );
	SetPlaneSignbits( &cv->plane );
	//cv->plane.type = PlaneTypeForNormal( cv->plane.normal );
	surf->cullinfo.plane = cv->plane;

	surf->data = (surfaceType_t *)cv;

#ifdef USE_VERT_TANGENT_SPACE
	// Calculate tangent spaces
	{
		srfVert_t      *dv[3];

		for(i = 0, tri = cv->triangles; i < numTriangles; i++, tri++)
		{
			dv[0] = &cv->verts[tri->indexes[0]];
			dv[1] = &cv->verts[tri->indexes[1]];
			dv[2] = &cv->verts[tri->indexes[2]];

			R_CalcTangentVectors(dv);
		}
	}
#endif
}


/*
===============
ParseMesh
===============
*/
static void ParseMesh ( dsurface_t *ds, drawVert_t *verts, float *hdrVertColors, msurface_t *surf ) {
	srfGridMesh_t	*grid;
	int				i, j;
	int				width, height, numPoints;
	static srfVert_t points[MAX_PATCH_SIZE*MAX_PATCH_SIZE];
	vec3_t			bounds[2];
	vec3_t			tmpVec;
	static surfaceType_t	skipData = SF_SKIP;
	int realLightmapNum;

	realLightmapNum = LittleLong( ds->lightmapNum );

	// get fog volume
	surf->fogIndex = LittleLong( ds->fogNum ) + 1;

	// get shader value
	surf->shader = invWorld.materials[ds->shaderNum].material;
	/*if ( r_singleShader->integer && !surf->shader->isSky ) {
		surf->shader = tr.defaultShader;
	}*/

	// we may have a nodraw surface, because they might still need to
	// be around for movement clipping
	if ( invWorld.materials[ LittleLong( ds->shaderNum ) ].surfaceFlags & SURF_NODRAW ) {
		surf->data = &skipData;
		return;
	}

	width = LittleLong( ds->patchWidth );
	height = LittleLong( ds->patchHeight );

	if(width < 0 || width > MAX_PATCH_SIZE || height < 0 || height > MAX_PATCH_SIZE)
		Com_Error(0, "ParseMesh: bad size");

	verts += LittleLong( ds->firstVert );
	numPoints = width * height;
	for(i = 0; i < numPoints; i++)
	{
		vec4_t color;

		for(j = 0; j < 3; j++)
		{
			points[i].xyz[j] = LittleFloat(verts[i].xyz[j]);
			points[i].normal[j] = LittleFloat(verts[i].normal[j]);
		}

		for(j = 0; j < 2; j++)
		{
			points[i].st[j] = LittleFloat(verts[i].st[j]);
			//points[i].lightmap[j] = LittleFloat(verts[i].lightmap[j]);
		}
		points[i].lightmap[0] = FatPackU(LittleFloat(verts[i].lightmap[0]), realLightmapNum);
		points[i].lightmap[1] = FatPackV(LittleFloat(verts[i].lightmap[1]), realLightmapNum);

		/*if (hdrVertColors)
		{
			color[0] = hdrVertColors[(ds->firstVert + i) * 3    ];
			color[1] = hdrVertColors[(ds->firstVert + i) * 3 + 1];
			color[2] = hdrVertColors[(ds->firstVert + i) * 3 + 2];
		}
		else
		{
			//hack: convert LDR vertex colors to HDR
			if (r_hdr->integer)
			{
				color[0] = verts[i].color[0] + 1.0f;
				color[1] = verts[i].color[1] + 1.0f;
				color[2] = verts[i].color[2] + 1.0f;
			}
			else
			{*/
				color[0] = verts[i].color[0];
				color[1] = verts[i].color[1];
				color[2] = verts[i].color[2];
			//}
		//}
		color[3] = verts[i].color[3] / 255.0f;

		points[i].vertexColors[0] = color[0];
		points[i].vertexColors[1] = color[1];
		points[i].vertexColors[2] = color[2];
		points[i].vertexColors[3] = color[3];
	}

	// pre-tesseleate
	grid = R_SubdividePatchToGrid( width, height, points );
	surf->data = (surfaceType_t *)grid;

	// copy the level of detail origin, which is the center
	// of the group of all curves that must subdivide the same
	// to avoid cracking
	for ( i = 0 ; i < 3 ; i++ ) {
		bounds[0][i] = LittleFloat( ds->lightmapVecs[0][i] );
		bounds[1][i] = LittleFloat( ds->lightmapVecs[1][i] );
	}
	VectorAdd( bounds[0], bounds[1], bounds[1] );
	VectorScale( bounds[1], 0.5f, grid->lodOrigin );
	VectorSubtract( bounds[0], grid->lodOrigin, tmpVec );
	grid->lodRadius = VectorLength( tmpVec );
}

/*
===============
ParseTriSurf
===============
*/
static void ParseTriSurf( dsurface_t *ds, drawVert_t *verts, float *hdrVertColors, msurface_t *surf, int *indexes ) {
	srfTriangles_t *cv;
	srfTriangle_t  *tri;
	int             i, j;
	int             numVerts, numTriangles, badTriangles;

	// get fog volume
	surf->fogIndex = LittleLong( ds->fogNum ) + 1;

	// get shader
	surf->shader = invWorld.materials[ds->shaderNum].material;
	/*if ( r_singleShader->integer && !surf->shader->isSky ) {
		surf->shader = tr.defaultShader;
	}*/

	numVerts = LittleLong(ds->numVerts);
	numTriangles = LittleLong(ds->numIndexes) / 3;

	//cv = ri.Hunk_Alloc(sizeof(*cv), h_low);
	cv = (srfTriangles_t *)surf->data;
	cv->surfaceType = SF_TRIANGLES;

	cv->numTriangles = numTriangles;
	cv->triangles = (srfTriangle_t*)malloc(numTriangles * sizeof(cv->triangles[0]));

	cv->numVerts = numVerts;
	cv->verts = (srfVert_t*)malloc(numVerts * sizeof(cv->verts[0]));

	surf->data = (surfaceType_t *) cv;

	// copy vertexes
	surf->cullinfo.type = CULLINFO_BOX;
	ClearBounds(surf->cullinfo.bounds[0], surf->cullinfo.bounds[1]);
	verts += LittleLong(ds->firstVert);
	for(i = 0; i < numVerts; i++)
	{
		vec4_t color;

		for(j = 0; j < 3; j++)
		{
			cv->verts[i].xyz[j] = LittleFloat(verts[i].xyz[j]);
			cv->verts[i].normal[j] = LittleFloat(verts[i].normal[j]);
		}

		AddPointToBounds( cv->verts[i].xyz, surf->cullinfo.bounds[0], surf->cullinfo.bounds[1] );

		for(j = 0; j < 2; j++)
		{
			cv->verts[i].st[j] = LittleFloat(verts[i].st[j]);
			cv->verts[i].lightmap[j] = LittleFloat(verts[i].lightmap[j]);
		}

		/*if (hdrVertColors)
		{
			color[0] = hdrVertColors[(ds->firstVert + i) * 3    ];
			color[1] = hdrVertColors[(ds->firstVert + i) * 3 + 1];
			color[2] = hdrVertColors[(ds->firstVert + i) * 3 + 2];
		}
		else
		{
			//hack: convert LDR vertex colors to HDR
			if (r_hdr->integer)
			{
				color[0] = verts[i].color[0] + 1.0f;
				color[1] = verts[i].color[1] + 1.0f;
				color[2] = verts[i].color[2] + 1.0f;
			}
			else
			{
				color[0] = verts[i].color[0];
				color[1] = verts[i].color[1];
				color[2] = verts[i].color[2];
			}
		}*/
		color[0] = verts[i].color[0];
		color[1] = verts[i].color[1];
		color[2] = verts[i].color[2];
		color[3] = verts[i].color[3] / 255.0f;

		cv->verts[i].vertexColors[0] = color[0];
		cv->verts[i].vertexColors[1] = color[1];
		cv->verts[i].vertexColors[2] = color[2];
		cv->verts[i].vertexColors[3] = color[3];

		//R_ColorShiftLightingFloats( color, cv->verts[i].vertexColors, 1.0f / 255.0f );
	}

	// copy triangles
	badTriangles = 0;
	indexes += LittleLong(ds->firstIndex);
	for(i = 0, tri = cv->triangles; i < numTriangles; i++, tri++)
	{
		for(j = 0; j < 3; j++)
		{
			tri->indexes[j] = LittleLong(indexes[i * 3 + j]);

			if(tri->indexes[j] < 0 || tri->indexes[j] >= numVerts)
			{
				Com_Error(0, "Bad index in face surface");
			}
		}

		if ((tri->indexes[0] == tri->indexes[1]) || (tri->indexes[1] == tri->indexes[2]) || (tri->indexes[0] == tri->indexes[2]))
		{
			tri--;
			badTriangles++;
		}
	}

	if (badTriangles)
	{
		Com_Printf(0, "Trisurf has bad triangles, originally shader %s %d tris %d verts, now %d tris\n", "hi", numTriangles, numVerts, numTriangles - badTriangles);
		cv->numTriangles -= badTriangles;
	}
}

static void LoadSurfaces(lump_t* srfLump, lump_t* drawVertLump, lump_t* drawIdxLump)
{
	dsurface_t* surfaces;
	drawVert_t* drawVerts;
	int* indices;

	int count = srfLump->filelen / sizeof(dsurface_t);

	surfaces = (dsurface_t*)(srfLump->fileofs + fileBase);
	drawVerts = (drawVert_t*)(drawVertLump->fileofs + fileBase);
	indices = (int*)(drawIdxLump->fileofs + fileBase);

	msurface_t* out = new msurface_t[count];

	invWorld.surfaces = out;
	invWorld.numSurfaces = count;

	// pre-allocate surfaces
	for (int i = 0; i < count; i++)
	{
		dsurface_t* inSurf = &surfaces[i];
		msurface_t* outSurf = &out[i];

		switch (inSurf->surfaceType)
		{
			case MST_TRIANGLE_SOUP:
				outSurf->data = (surfaceType_t*)new srfTriangles_t;
				break;
			case MST_PLANAR:
				outSurf->data = (surfaceType_t*)new srfSurfaceFace_t;
				break;
			case MST_FLARE:
				//outSurf->data = (surfaceType_t*)new srfFlare_t;
				break;
		}
	}

	// load surfaces
	int numFaces = 0, numMeshes = 0, numTriSurfs = 0, numFlares = 0;

	for (int i = 0; i < count; i++)
	{
		dsurface_t* inSurf = &surfaces[i];
		msurface_t* outSurf = &out[i];

		switch (inSurf->surfaceType)
		{
			case MST_PATCH:
				ParseMesh(inSurf, drawVerts, nullptr, outSurf);

				{
					srfGridMesh_t *surface = (srfGridMesh_t *)out->data;

					out->cullinfo.type = CULLINFO_BOX | CULLINFO_SPHERE;
					VectorCopy(surface->meshBounds[0], out->cullinfo.bounds[0]);
					VectorCopy(surface->meshBounds[1], out->cullinfo.bounds[1]);
					VectorCopy(surface->localOrigin, out->cullinfo.localOrigin);
					out->cullinfo.radius = surface->meshRadius;
				}

				numMeshes++;

				break;
			case MST_TRIANGLE_SOUP:
				ParseTriSurf(inSurf, drawVerts, nullptr, outSurf, indices);
				numTriSurfs++;
				break;
			case MST_PLANAR:
				ParseFace(inSurf, drawVerts, nullptr, outSurf, indices);
				numFaces++;
				break;
			case MST_FLARE:
				break;
			default:
				Com_Error(0, "Bad surfaceType");
		}
	}
	
	Com_Printf( 0, "...loaded %d faces, %i meshes, %i trisurfs, %i flares\n", 
		numFaces, numMeshes, numTriSurfs, numFlares );
}

/*
=================
BSPSurfaceCompare
compare function for qsort()
=================
*/
static int BSPSurfaceCompare(const void *a, const void *b)
{
	msurface_t   *aa, *bb;

	aa = *(msurface_t **) a;
	bb = *(msurface_t **) b;

	// shader first
	/*if(aa->shader->sortedIndex < bb->shader->sortedIndex)
		return -1;

	else if(aa->shader->sortedIndex > bb->shader->sortedIndex)
		return 1;*/
	int stringCompare = _stricmp(aa->shader->getName().c_str(), bb->shader->getName().c_str());

	if (stringCompare != 0)
	{
		return stringCompare;
	}

	// by fogIndex
	if(aa->fogIndex < bb->fogIndex)
		return -1;

	else if(aa->fogIndex > bb->fogIndex)
		return 1;

	return 0;
}


static void CopyVert(const srfVert_t * in, srfVert_t * out)
{
	int             j;

	for(j = 0; j < 3; j++)
	{
		out->xyz[j]       = in->xyz[j];
#ifdef USE_VERT_TANGENT_SPACE
		out->tangent[j]   = in->tangent[j];
		out->bitangent[j] = in->bitangent[j];
#endif
		out->normal[j]    = in->normal[j];
		out->lightdir[j]  = in->lightdir[j];
	}

	for(j = 0; j < 2; j++)
	{
		out->st[j] = in->st[j];
		out->lightmap[j] = in->lightmap[j];
	}

	for(j = 0; j < 4; j++)
	{
		out->vertexColors[j] = in->vertexColors[j];
	}
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

static void R_CreateWorldVBO(void)
{
	int             i, j, k;

	int             numVerts;
	srfVert_t      *verts;

	int             numTriangles;
	srfTriangle_t  *triangles;

	int             numSurfaces;
	msurface_t   *surface;
	msurface_t  **surfacesSorted;

	int             startTime, endTime;

	startTime = Com_Milliseconds();

	numVerts = 0;
	numTriangles = 0;
	numSurfaces = 0;
	for(k = 0, surface = &invWorld.surfaces[0]; k < invWorld.numSurfaces /* s_worldData.numWorldSurfaces */; k++, surface++)
	{
		if(*surface->data == SF_FACE)
		{
			srfSurfaceFace_t *face = (srfSurfaceFace_t *) surface->data;

			if(face->numVerts)
				numVerts += face->numVerts;

			if(face->numTriangles)
				numTriangles += face->numTriangles;

			numSurfaces++;
		}
		else if(*surface->data == SF_GRID)
		{
			srfGridMesh_t  *grid = (srfGridMesh_t *) surface->data;

			if(grid->numVerts)
				numVerts += grid->numVerts;

			if(grid->numTriangles)
				numTriangles += grid->numTriangles;

			numSurfaces++;
		}
		else if(*surface->data == SF_TRIANGLES)
		{
			srfTriangles_t *tri = (srfTriangles_t *) surface->data;

			if(tri->numVerts)
				numVerts += tri->numVerts;

			if(tri->numTriangles)
				numTriangles += tri->numTriangles;

			numSurfaces++;
		}
	}

	if(!numVerts || !numTriangles)
		return;

	Com_Printf(0, "...calculating world VBO ( %i verts %i tris )\n", numVerts, numTriangles);

	// create arrays

	verts = (srfVert_t *)malloc(numVerts * sizeof(srfVert_t));

	triangles = (srfTriangle_t *)malloc(numTriangles * sizeof(srfTriangle_t));

	// presort surfaces
	surfacesSorted = (msurface_t **)malloc(numSurfaces * sizeof(*surfacesSorted));

	j = 0;
	for(k = 0, surface = &invWorld.surfaces[0]; k < invWorld.numSurfaces; k++, surface++)
	{
		if(*surface->data == SF_FACE || *surface->data == SF_GRID || *surface->data == SF_TRIANGLES)
		{
			surfacesSorted[j++] = surface;
		}
	}

	qsort(surfacesSorted, numSurfaces, sizeof(*surfacesSorted), BSPSurfaceCompare);

	// set up triangle indices
	numVerts = 0;
	numTriangles = 0;
	for(k = 0, surface = surfacesSorted[k]; k < numSurfaces; k++, surface = surfacesSorted[k])
	{
		if(*surface->data == SF_FACE)
		{
			srfSurfaceFace_t *srf = (srfSurfaceFace_t *) surface->data;

			srf->firstIndex = numTriangles * 3;

			if(srf->numTriangles)
			{
				srfTriangle_t  *tri;

				srf->minIndex = numVerts + srf->triangles->indexes[0];
				srf->maxIndex = numVerts + srf->triangles->indexes[0];

				for(i = 0, tri = srf->triangles; i < srf->numTriangles; i++, tri++)
				{
					for(j = 0; j < 3; j++)
					{
						triangles[numTriangles + i].indexes[j] = numVerts + tri->indexes[j];
						srf->minIndex = MIN(srf->minIndex, numVerts + tri->indexes[j]);
						srf->maxIndex = MAX(srf->maxIndex, numVerts + tri->indexes[j]);
					}
				}

				numTriangles += srf->numTriangles;
			}

			if(srf->numVerts)
				numVerts += srf->numVerts;
		}
		else if(*surface->data == SF_GRID)
		{
			srfGridMesh_t  *srf = (srfGridMesh_t *) surface->data;

			srf->firstIndex = numTriangles * 3;

			if(srf->numTriangles)
			{
				srfTriangle_t  *tri;

				srf->minIndex = numVerts + srf->triangles->indexes[0];
				srf->maxIndex = numVerts + srf->triangles->indexes[0];

				for(i = 0, tri = srf->triangles; i < srf->numTriangles; i++, tri++)
				{
					for(j = 0; j < 3; j++)
					{
						triangles[numTriangles + i].indexes[j] = numVerts + tri->indexes[j];
						srf->minIndex = MIN(srf->minIndex, numVerts + tri->indexes[j]);
						srf->maxIndex = MAX(srf->maxIndex, numVerts + tri->indexes[j]);
					}
				}

				numTriangles += srf->numTriangles;
			}

			if(srf->numVerts)
				numVerts += srf->numVerts;
		}
		else if(*surface->data == SF_TRIANGLES)
		{
			srfTriangles_t *srf = (srfTriangles_t *) surface->data;

			srf->firstIndex = numTriangles * 3;

			if(srf->numTriangles)
			{
				srfTriangle_t  *tri;

				srf->minIndex = numVerts + srf->triangles->indexes[0];
				srf->maxIndex = numVerts + srf->triangles->indexes[0];

				for(i = 0, tri = srf->triangles; i < srf->numTriangles; i++, tri++)
				{
					for(j = 0; j < 3; j++)
					{
						triangles[numTriangles + i].indexes[j] = numVerts + tri->indexes[j];
						srf->minIndex = MIN(srf->minIndex, numVerts + tri->indexes[j]);
						srf->maxIndex = MAX(srf->maxIndex, numVerts + tri->indexes[j]);
					}
				}

				numTriangles += srf->numTriangles;
			}

			if(srf->numVerts)
				numVerts += srf->numVerts;
		}
	}

	// build vertices
	numVerts = 0;
	for(k = 0, surface = surfacesSorted[k]; k < numSurfaces; k++, surface = surfacesSorted[k])
	{
		if(*surface->data == SF_FACE)
		{
			srfSurfaceFace_t *srf = (srfSurfaceFace_t *) surface->data;

			srf->firstVert = numVerts;

			if(srf->numVerts)
			{
				for(i = 0; i < srf->numVerts; i++)
				{
					CopyVert(&srf->verts[i], &verts[numVerts + i]);
				}

				numVerts += srf->numVerts;
			}
		}
		else if(*surface->data == SF_GRID)
		{
			srfGridMesh_t  *srf = (srfGridMesh_t *) surface->data;

			srf->firstVert = numVerts;

			if(srf->numVerts)
			{
				for(i = 0; i < srf->numVerts; i++)
				{
					CopyVert(&srf->verts[i], &verts[numVerts + i]);
				}

				numVerts += srf->numVerts;
			}
		}
		else if(*surface->data == SF_TRIANGLES)
		{
			srfTriangles_t *srf = (srfTriangles_t *) surface->data;

			srf->firstVert = numVerts;

			if(srf->numVerts)
			{
				for(i = 0; i < srf->numVerts; i++)
				{
					CopyVert(&srf->verts[i], &verts[numVerts + i]);
				}

				numVerts += srf->numVerts;
			}
		}
	}

/*#ifdef USE_VERT_TANGENT_SPACE
	s_worldData.vbo = R_CreateVBO2(va("staticBspModel0_VBO %i", 0), numVerts, verts,
		ATTR_POSITION | ATTR_TEXCOORD | ATTR_LIGHTCOORD | ATTR_TANGENT | ATTR_BITANGENT |
		ATTR_NORMAL | ATTR_COLOR | ATTR_LIGHTDIRECTION, VBO_USAGE_STATIC);
#else
	s_worldData.vbo = R_CreateVBO2(va("staticBspModel0_VBO %i", 0), numVerts, verts,
		ATTR_POSITION | ATTR_TEXCOORD | ATTR_LIGHTCOORD |
		ATTR_NORMAL | ATTR_COLOR | ATTR_LIGHTDIRECTION, VBO_USAGE_STATIC);
#endif

	s_worldData.ibo = R_CreateIBO2(va("staticBspModel0_IBO %i", 0), numTriangles, triangles, VBO_USAGE_STATIC);*/

	invWorld.vDecl = Ogre::HardwareBufferManager::getSingleton().createVertexDeclaration();

	size_t offset = 0;
	invWorld.vDecl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	invWorld.vDecl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	invWorld.vDecl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

	/*invWorld.vDecl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);*/

	// --------------------
	// create vertex buffer
	struct invVert
	{
		float position[3];
		float normal[3];
		float coords[2];
	};

	invVert* vertexData = new invVert[numVerts];

	for (int i = 0; i < numVerts; i++)
	{
		VectorCopy(verts[i].xyz, vertexData[i].position);
		VectorCopy(verts[i].normal, vertexData[i].normal);
		vertexData[i].coords[0] = verts[i].st[0];
		vertexData[i].coords[1] = verts[i].st[1];
	}

	invWorld.worldVBO = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, numVerts, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	invWorld.worldVBO->writeData(0, invWorld.worldVBO->getSizeInBytes(), vertexData);

	delete[] vertexData;

	// --------------------------------
	// create index buffer
	// --------------------------------

	int* indexData = new int[numTriangles * 3];
	for (int i = 0; i < numTriangles; i++)
	{
		indexData[3 * i] = triangles[i].indexes[0];
		indexData[1 + (3 * i)] = triangles[i].indexes[1];
		indexData[2 + (3 * i)] = triangles[i].indexes[2];
	}

	invWorld.worldIBO = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_32BIT, numTriangles * 3, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	invWorld.worldIBO->writeData(0, invWorld.worldIBO->getSizeInBytes(), indexData);

	delete[] indexData;

	endTime = Com_Milliseconds();
	Com_Printf(0, "world VBO calculation time = %5.2f seconds\n", (endTime - startTime) / 1000.0);

	free(surfacesSorted);

	free(triangles);
	free(verts);
}

#define MAX_MULTIDRAW_PRIMITIVES  16384
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static void CreateWorldMeshes()
{
	msurface_t** surfacesSorted = (msurface_t **)malloc(invWorld.numSurfaces * sizeof(*surfacesSorted));

	int j = 0;
	int k;
	msurface_t* surface;

	for (k = 0, surface = &invWorld.surfaces[0]; k < invWorld.numSurfaces; k++, surface++)
	{
		if(*surface->data == SF_FACE || *surface->data == SF_GRID || *surface->data == SF_TRIANGLES)
		{
			surfacesSorted[j++] = surface;
		}
	}

	qsort(surfacesSorted, invWorld.numSurfaces, sizeof(*surfacesSorted), BSPSurfaceCompare);

	static int multiDrawMinIndex[MAX_MULTIDRAW_PRIMITIVES];
	static int multiDrawMaxIndex[MAX_MULTIDRAW_PRIMITIVES];
	static int* multiDrawFirstIndex[MAX_MULTIDRAW_PRIMITIVES];
	static int* multiDrawLastIndex[MAX_MULTIDRAW_PRIMITIVES];
	static int multiDrawNumIndexes[MAX_MULTIDRAW_PRIMITIVES];
	static int multiDrawPrimitives = 0;
	int multidrawsMerged = 0;
	int numMultidraws = 0;
	int numMultidrawPrims = 0;

	Ogre::MaterialPtr lastShader;

	for (int i = 0; i < invWorld.numSurfaces; i++)
	{
		surface = surfacesSorted[i];

		if (surface->shader != lastShader)
		{
			multiDrawPrimitives = 0;
			lastShader = surface->shader;
		}

		int firstIndex;
		int numIndexes;
		int minIndex, maxIndex;

		switch (*surface->data)
		{
			case SF_TRIANGLES:
			{
				srfTriangles_t* srf = (srfTriangles_t*)surface->data;

				firstIndex = srf->firstIndex;
				numIndexes = srf->numTriangles * 3;
				minIndex = srf->minIndex;
				maxIndex = srf->maxIndex;

				break;
			}
			case SF_FACE:
			{
				srfSurfaceFace_t* srf = (srfSurfaceFace_t*)surface->data;

				firstIndex = srf->firstIndex;
				numIndexes = srf->numTriangles * 3;
				minIndex = srf->minIndex;
				maxIndex = srf->maxIndex;

				break;
			}

			case SF_GRID:
			{
				srfGridMesh_t* srf = (srfGridMesh_t*)surface->data;

				firstIndex = srf->firstIndex;
				numIndexes = srf->numTriangles * 3;
				minIndex = srf->minIndex;
				maxIndex = srf->maxIndex;

				break;
			}

			default:
				continue;
		}

		/*int mergeForward = -1;
		int mergeBack = -1;
		int* firstIndexOffset = (int*)BUFFER_OFFSET(firstIndex * sizeof(int));
		int* lastIndexOffset  = (int*)BUFFER_OFFSET((firstIndex + numIndexes) * sizeof(int));

		if (/*r_mergeMultidraws->integer* /true)
		{
			i = 0;

			if (false)
			{
				// lazy merge, only check the last primitive
				if (multiDrawPrimitives)
				{
					i = multiDrawPrimitives - 1;
				}
			}

			for (; i < multiDrawPrimitives; i++)
			{
				if (multiDrawLastIndex[i] == firstIndexOffset)
				{
					mergeBack = i;
				}

				if (lastIndexOffset == multiDrawFirstIndex[i])
				{
					mergeForward = i;
				}
			}
		}

		if (mergeBack != -1 && mergeForward == -1)
		{
			multiDrawNumIndexes[mergeBack] += numIndexes;
			multiDrawLastIndex[mergeBack]   = multiDrawFirstIndex[mergeBack] + multiDrawNumIndexes[mergeBack];
			multiDrawMinIndex[mergeBack] = MIN(multiDrawMinIndex[mergeBack], minIndex);
			multiDrawMaxIndex[mergeBack] = MAX(multiDrawMaxIndex[mergeBack], maxIndex);
			multidrawsMerged++;
		}
		else if (mergeBack == -1 && mergeForward != -1)
		{
			multiDrawNumIndexes[mergeForward] += numIndexes;
			multiDrawFirstIndex[mergeForward]  = firstIndexOffset;
			multiDrawLastIndex[mergeForward]   = multiDrawFirstIndex[mergeForward] + multiDrawNumIndexes[mergeForward];
			multiDrawMinIndex[mergeForward] = MIN(multiDrawMinIndex[mergeForward], minIndex);
			multiDrawMaxIndex[mergeForward] = MAX(multiDrawMaxIndex[mergeForward], maxIndex);
			multidrawsMerged++;
		}
		else if (mergeBack != -1 && mergeForward != -1)
		{
			multiDrawNumIndexes[mergeBack] += numIndexes + multiDrawNumIndexes[mergeForward];
			multiDrawLastIndex[mergeBack]   = multiDrawFirstIndex[mergeBack] + multiDrawNumIndexes[mergeBack];
			multiDrawMinIndex[mergeBack] = MIN(multiDrawMinIndex[mergeBack], MIN(multiDrawMinIndex[mergeForward], minIndex));
			multiDrawMaxIndex[mergeBack] = MAX(multiDrawMaxIndex[mergeBack], MAX(multiDrawMaxIndex[mergeForward], maxIndex));
			multiDrawPrimitives--;

			if (mergeForward != multiDrawPrimitives)
			{
				multiDrawNumIndexes[mergeForward] = multiDrawNumIndexes[multiDrawPrimitives];
				multiDrawFirstIndex[mergeForward] = multiDrawFirstIndex[multiDrawPrimitives];
			}
			multidrawsMerged += 2;
		}
		else if (mergeBack == -1 && mergeForward == -1)
		{
			multiDrawNumIndexes[multiDrawPrimitives] = numIndexes;
			multiDrawFirstIndex[multiDrawPrimitives] = firstIndexOffset;
			multiDrawLastIndex[multiDrawPrimitives] = lastIndexOffset;
			multiDrawMinIndex[multiDrawPrimitives] = minIndex;
			multiDrawMaxIndex[multiDrawPrimitives] = maxIndex;
			multiDrawPrimitives++;
		}*/

		// -----------------------------------------------------
		// create the mesh using the multiDraw above
		// -----------------------------------------------------

		// if the next surface has a different shader or we're the last
		/*if (i == (invWorld.numSurfaces - 1) || surfacesSorted[i + 1]->shader != lastShader)
		{
			for (j = 0; j < multiDrawPrimitives; j++)
			{*/
				Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual(va("staticWorldObject %i", numMultidrawPrims), OGRE_RESGROUP_NAME);
				Ogre::SubMesh* sub = mesh->createSubMesh();

				mesh->sharedVertexData = new Ogre::VertexData(invWorld.vDecl, invWorld.vBind);
				mesh->sharedVertexData->vertexCount = numIndexes;//multiDrawNumIndexes[j];//invWorld.worldVBO->getNumVertices();
				
				sub->indexData->indexBuffer = invWorld.worldIBO;
				sub->indexData->indexCount = numIndexes;
				sub->indexData->indexStart = firstIndex;//((int)multiDrawFirstIndex[j] / sizeof(int));

				//sub->setMaterialName("hello", OGRE_RESGROUP_NAME);
				sub->setMaterialName(lastShader->getName(), OGRE_RESGROUP_NAME);

				Ogre::AxisAlignedBox nullBounds;
				nullBounds.setMaximum(surface->cullinfo.bounds[0][0], surface->cullinfo.bounds[0][1], surface->cullinfo.bounds[0][2]);
				nullBounds.setMaximum(surface->cullinfo.bounds[1][0], surface->cullinfo.bounds[1][1], surface->cullinfo.bounds[1][2]);

				nullBounds.setInfinite();

				mesh->_setBounds(nullBounds);
				mesh->_setBoundingSphereRadius(99999);

				mesh->load();

				Ogre::Entity* ent = ogre_sm->createEntity(mesh);

				Ogre::SceneNode* node = ogre_sm->getRootSceneNode()->createChildSceneNode();
				node->attachObject(ent);

				numMultidrawPrims++;
			//}
		//}

		numMultidraws++;
	}

	free(surfacesSorted);
}

void InvCM_LoadBSP(char* buffer);

void Inverse_LoadBSP(const char* filename)
{
	char* buffer;
	int length = FS_ReadFile(filename, &buffer);

	if (length < 0)
	{
		Com_Error(1, "Could not load BSP (%s).", filename);
	}

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("hello", OGRE_RESGROUP_NAME, true);

	rWorld = &invWorld;

	dheader_t* header = (dheader_t*)buffer;

	if (header->version != BSP_VERSION && header->version != BSP_VERSION_B)
	{
		int hVersion = header->version;

		FS_FreeFile(buffer);
		Com_Error(1, "BSP %s has version %i, should be %i.", filename, hVersion, BSP_VERSION);
	}

	fileBase = buffer;

	LoadShaders(&header->lumps[LUMP_SHADERS]);
	LoadPlanes(&header->lumps[LUMP_PLANES]);
	LoadSurfaces(&header->lumps[LUMP_SURFACES], &header->lumps[LUMP_DRAWVERTS], &header->lumps[LUMP_DRAWINDEXES]);

	R_CreateWorldVBO();

	invWorld.vBind = new Ogre::VertexBufferBinding();
	invWorld.vBind->setBinding(0, invWorld.worldVBO);
	
	CreateWorldMeshes();

	InvCM_LoadBSP(buffer);

	FS_FreeFile(buffer);
}
#endif