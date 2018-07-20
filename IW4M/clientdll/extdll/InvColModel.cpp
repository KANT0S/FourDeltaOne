// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Project 'Inverse' - alternate world loading:
//          hooks and patches.
//
// Initial author: NTAuthority
// Started: 2013-02-19
// ==========================================================

#include "StdInc.h"
#include "cm_local.h"

#ifdef ENABLE_INVERSE
//#include "Inverse.h"
#include "InvOgre.h"
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>
#include <LinearMath/btGeometryUtil.h>

static btCollisionWorld* invColWorld;

struct InvDebugLine
{
	float from[3];
	float to[3];

	DWORD color;
};

std::vector<InvDebugLine> invDebugLines;

class DebugDrawer : public btIDebugDraw
{
public:
	virtual void     drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color);
	virtual void     drawTriangle (const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar);
	virtual void     drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
	virtual void     reportErrorWarning (const char *warningString);
	virtual void     draw3dText (const btVector3 &location, const char *textString);
	virtual void     setDebugMode (int debugMode);
	virtual int     getDebugMode () const;

private:
	DebugDrawModes modes;
};

struct ColHeader
{
	char magic[4];
	uint32_t fileSize;
	char modelName[22];
	short modelID;

	float radius;
	float center[3];
	float min[3];
	float max[3];
};

struct ColSurface
{
	uint8_t material;
	uint8_t flag;
	uint8_t brightness;
	uint8_t light;
};

struct ColSphere
{
	float radius;
	float center[3];
	ColSurface surface;
};

struct ColVertex
{
	float x;
	float y;
	float z;
};

struct ColBox
{
	float min[3];
	float max[3];
	ColSurface surface;
};

struct ColFace
{
	int v1, v2, v3;
	ColSurface surface;
};

void InvCM_LoadCOL(const char* filename)
{
	int fh;
	int length = FS_FOpenFileRead(filename, &fh, 0);

	if (length < 0)
	{
		Com_Error(0, "Couldn't load InvCol %s", filename);
	}

	ColHeader header;
	FS_Read(&header, sizeof(header), fh);

	// spheres
	uint32_t count;
	FS_Read(&count, sizeof(count), fh);

	const btQuaternion ZQ(0,0,0,1);

	for (uint32_t i = 0; i < count; i++)
	{
		ColSphere cs;
		FS_Read(&cs, sizeof(cs), fh);

		btSphereShape* sphere = new btSphereShape(cs.radius);
		
		btCollisionObject* object = new btCollisionObject();
		object->setCollisionShape(sphere);
		object->setWorldTransform(btTransform(ZQ, btVector3(10.0f, 10.0f, 10.0f) + btVector3(cs.center[0] * 39.3701f, cs.center[1] * 39.3701f, cs.center[2] * 39.3701f)));

		invColWorld->addCollisionObject(object);
	}

	// boxes
	FS_Read(&count, sizeof(count), fh);
	FS_Read(&count, sizeof(count), fh);

	for (uint32_t i = 0; i < count; i++)
	{
		ColBox cs;
		FS_Read(&cs, sizeof(cs), fh);

		btBoxShape* box = new btBoxShape(btVector3(((cs.max[0] - cs.min[0]) / 2) * 39.3701f, ((cs.max[1] - cs.min[1]) / 2) * 39.3701f, ((cs.max[2] - cs.min[2]) / 2)) * 39.3701f);

		btCollisionObject* object = new btCollisionObject();
		object->setCollisionShape(box);
		object->setWorldTransform(btTransform(ZQ, btVector3(10.0f, 10.0f, 10.0f) + btVector3(((cs.min[0] + cs.max[0]) / 2) * 39.3701f, ((cs.min[1] + cs.max[1]) / 2) * 39.3701f, ((cs.min[2] + cs.max[2]) / 2) * 39.3701f)));

		//invColWorld->addCollisionObject(object);
	}

	// trimesh
	FS_Read(&count, sizeof(count), fh);

	if (count > 0)
	{
		ColVertex* vertices = new ColVertex[count];
		FS_Read(vertices, sizeof(*vertices) * count, fh);

		uint32_t vertCount = count;
		FS_Read(&count, sizeof(count), fh);

		ColFace* faces = new ColFace[count];
		FS_Read(faces, sizeof(*faces) * count, fh);

		for (int i = 0; i < vertCount; i++)
		{
			vertices[i].x *= 39.3701f;
			vertices[i].y *= 39.3701f;
			vertices[i].z *= 39.3701f;
		}

		btTriangleIndexVertexArray *v = new btTriangleIndexVertexArray(
			count, reinterpret_cast<int*>(&(faces[0].v1)), sizeof(ColFace),
			vertCount, &(vertices[0].x), sizeof(ColVertex));

		btCollisionShape *s;

		btBvhTriangleMeshShape *tm = new btBvhTriangleMeshShape(v,true,true);
		s = tm;
		s->setMargin(0.04f);
		btTriangleInfoMap* tri_info_map = new btTriangleInfoMap();
		tri_info_map->m_edgeDistanceThreshold = 0.001f;

		btGenerateInternalEdgeInfo(tm, tri_info_map);

		btCollisionObject* object = new btCollisionObject();
		object->setCollisionShape(tm);
		object->setWorldTransform(btTransform(ZQ, btVector3(10.0f, 10.0f, 10.0f)));

		invColWorld->addCollisionObject(object);
	}

	FS_FCloseFile(fh);
}

DebugDrawer* drawer;

static char* cmod_base;
static clipMap_t cm;

#if 1
/*
=================
CMod_LoadLeafs
=================
*/
void CMod_LoadLeafs(lump_t * l)
{
	int             i;
	cLeaf_t        *out;
	dleaf_t        *in;
	int             count;

	in = (dleaf_t *)(cmod_base + l->fileofs);
	if(l->filelen % sizeof(*in))
		Com_Error(0, "MOD_LoadBmodel: funny lump size");
	count = l->filelen / sizeof(*in);

	if(count < 1)
		Com_Error(0, "Map with no leafs");

	cm.leafs = (cLeaf_t*)malloc((count) * sizeof(*cm.leafs));
	cm.numLeafs = count;

	out = cm.leafs;
	for(i = 0; i < count; i++, in++, out++)
	{
		out->cluster = LittleLong(in->cluster);
		out->area = LittleLong(in->area);
		out->firstLeafBrush = LittleLong(in->firstLeafBrush);
		out->numLeafBrushes = LittleLong(in->numLeafBrushes);
		out->firstLeafSurface = LittleLong(in->firstLeafSurface);
		out->numLeafSurfaces = LittleLong(in->numLeafSurfaces);

		if(out->cluster >= cm.numClusters)
			cm.numClusters = out->cluster + 1;
		if(out->area >= cm.numAreas)
			cm.numAreas = out->area + 1;
	}
}

/*
=================
CMod_LoadPlanes
=================
*/
void CMod_LoadPlanes(lump_t * l)
{
	int             i, j;
	cplane_t       *out;
	dplane_t       *in;
	int             count;
	int             bits;

	in = (dplane_t *)(cmod_base + l->fileofs);
	if(l->filelen % sizeof(*in))
		Com_Error(0, "MOD_LoadBmodel: funny lump size");
	count = l->filelen / sizeof(*in);

	if(count < 1)
		Com_Error(0, "Map with no planes");
	cm.planes = (cplane_t*)malloc((count) * sizeof(*cm.planes));
	cm.numPlanes = count;

	out = cm.planes;

	for(i = 0; i < count; i++, in++, out++)
	{
		bits = 0;
		for(j = 0; j < 3; j++)
		{
			out->normal[j] = LittleFloat(in->normal[j]);
			if(out->normal[j] < 0)
				bits |= 1 << j;
		}

		out->dist = LittleFloat(in->dist);
		//out->type = PlaneTypeForNormal(out->normal);
		out->signbits = bits;
	}
}

/*
=================
CMod_LoadLeafBrushes
=================
*/
void CMod_LoadLeafBrushes(lump_t * l)
{
	int             i;
	int            *out;
	int            *in;
	int             count;

	in = (int *)(cmod_base + l->fileofs);
	if(l->filelen % sizeof(*in))
		Com_Error(0, "MOD_LoadBmodel: funny lump size");
	count = l->filelen / sizeof(*in);

	cm.leafbrushes = (int*)malloc((count) * sizeof(*cm.leafbrushes));
	cm.numLeafBrushes = count;

	out = cm.leafbrushes;

	for(i = 0; i < count; i++, in++, out++)
	{
		*out = LittleLong(*in);
	}
}

/*
=================
CMod_LoadBrushSides
=================
*/
void CMod_LoadBrushSides(lump_t * l)
{
	int             i;
	cbrushside_t   *out;
	dbrushside_t   *in;
	int             count;
	int             num;

	in = (dbrushside_t *)(cmod_base + l->fileofs);
	if(l->filelen % sizeof(*in))
	{
		Com_Error(0, "MOD_LoadBmodel: funny lump size");
	}
	count = l->filelen / sizeof(*in);

	cm.brushsides = (cbrushside_t*)malloc((0 + count) * sizeof(*cm.brushsides));
	cm.numBrushSides = count;

	out = cm.brushsides;

	for(i = 0; i < count; i++, in++, out++)
	{
		num = LittleLong(in->planeNum);
		//out->planeNum = num;
		out->plane = &cm.planes[num];
		out->shaderNum = LittleLong(in->shaderNum);
		if(out->shaderNum < 0 || out->shaderNum >= rWorld->numMaterials)
		{
			Com_Error(0, "CMod_LoadBrushSides: bad shaderNum: %i", out->shaderNum);
		}
		out->surfaceFlags = rWorld->materials[out->shaderNum].surfaceFlags;
	}
}

void CM_BoundBrush(cbrush_t * b)
{
	b->bounds[0][0] = -b->sides[0].plane->dist;
	b->bounds[1][0] = b->sides[1].plane->dist;

	b->bounds[0][1] = -b->sides[2].plane->dist;
	b->bounds[1][1] = b->sides[3].plane->dist;

	b->bounds[0][2] = -b->sides[4].plane->dist;
	b->bounds[1][2] = b->sides[5].plane->dist;
}


/*
=================
CMod_LoadBrushes

=================
*/
void CMod_LoadBrushes(lump_t * l)
{
	dbrush_t       *in;
	cbrush_t       *out;
	int             i, count;

	in = (dbrush_t *)(cmod_base + l->fileofs);
	if(l->filelen % sizeof(*in))
	{
		Com_Error(0, "MOD_LoadBmodel: funny lump size");
	}
	count = l->filelen / sizeof(*in);

	cm.brushes = (cbrush_t*)malloc((0 + count) * sizeof(*cm.brushes));
	cm.numBrushes = count;

	out = cm.brushes;

	for(i = 0; i < count; i++, out++, in++)
	{
		out->sides = cm.brushsides + LittleLong(in->firstSide);
		out->numsides = LittleLong(in->numSides);

		out->shaderNum = LittleLong(in->shaderNum);
		if(out->shaderNum < 0 || out->shaderNum >= rWorld->numMaterials)
		{
			Com_Error(0, "CMod_LoadBrushes: bad shaderNum: %i", out->shaderNum);
		}
		out->contents = rWorld->materials[out->shaderNum].surfaceContents;

		CM_BoundBrush(out);
	}

}

void InvCM_LoadBSP(char* buffer)
{
	dheader_t* header = (dheader_t*)buffer;

	if (header->version != BSP_VERSION && header->version != BSP_VERSION_B)
	{
		int hVersion = header->version;

		FS_FreeFile(buffer);
		Com_Error(0, "BSP %s has version %i, should be %i.", hVersion, BSP_VERSION);
	}

	cmod_base = buffer;

	CMod_LoadLeafs(&header->lumps[LUMP_LEAFS]);
	CMod_LoadLeafBrushes(&header->lumps[LUMP_LEAFBRUSHES]);
	CMod_LoadPlanes(&header->lumps[LUMP_PLANES]);
	CMod_LoadBrushSides(&header->lumps[LUMP_BRUSHSIDES]);
	CMod_LoadBrushes(&header->lumps[LUMP_BRUSHES]);
}
#endif

/*void InvCM_LoadBSP(char* buffer)
{
	int nope;
	CM_LoadMap("hi", buffer, qtrue, &nope);
}*/

// temp stuff until ui_viewer doesn't inv anymore
static bool invCM_loaded = false;

void InvCM_Load()
{
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btVector3 worldAabbMin(-65000, -65000, -65000);
	btVector3 worldAabbMax(65000, 65000, 65000);

	btAxisSweep3* broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax);

	invColWorld = new btCollisionWorld(dispatcher, broadphase, collisionConfiguration);
	
	drawer = new DebugDrawer();
	drawer->setDebugMode( btIDebugDraw::DBG_DrawWireframe );
	invColWorld->setDebugDrawer(drawer);

	// load a collision into the world
	//InvCM_LoadCOL("inverse.col");

#if 1
	//btDynamicsWorld* dynamicsWorld = reinterpret_cast< btDynamicsWorld* >(dynamicsWorldHandle);
	btCollisionWorld* dynamicsWorld = invColWorld;

	for(int i = 0; i < cm.numLeafs; i++)
	{
		const cLeaf_t* leaf = &cm.leafs[i];

		for(int j = 0; j < leaf->numLeafBrushes; j++)
		{
			int brushnum = cm.leafbrushes[leaf->firstLeafBrush + j];

			cbrush_t* brush = &cm.brushes[brushnum];
			if(brush->checkcount == cm.checkcount)
			{
				// already checked this brush in another leaf
				continue;
			}
			brush->checkcount = cm.checkcount;

			if(brush->numsides == 0)
			{
				// don't care about invalid brushes
				continue;
			}

			if(!(brush->contents & 1))
			{
				// don't care about non-solid brushes
				continue;
			}

			btAlignedObjectArray<btVector3> planeEquations;

			for(int k = 0; k < brush->numsides; k++)
			{
				const cbrushside_t* side = brush->sides + k;
				const cplane_t* plane = side->plane;

				btVector3 planeEq(plane->normal[0], plane->normal[1], plane->normal[2]);
				planeEq[3] = -plane->dist;

				planeEquations.push_back(planeEq);
			}

			btAlignedObjectArray<btVector3>	vertices;
			btGeometryUtil::getVerticesFromPlaneEquations(planeEquations, vertices);

			if(vertices.size() > 0)
			{
				btCollisionShape* shape = new btConvexHullShape(&(vertices[0].getX()),vertices.size());
				//collisionShapes->push_back(shape);

				float mass = 0.f;
				btTransform startTransform;

				startTransform.setIdentity();
				//startTransform.setOrigin(btVector3(0,0,-10.f));

				btCollisionObject* object = new btCollisionObject();
				object->setCollisionShape(shape);
				object->setWorldTransform(startTransform);

				//dynamicsWorld->addRigidBody(body);
				dynamicsWorld->addCollisionObject(object);
			}

		}
	}
#endif

	invCM_loaded = true;
}

void InvCM_Draw()
{
	if (invColWorld)
	{
		invDebugLines.clear();
		invColWorld->debugDrawWorld();
	}
}

SIMD_FORCE_INLINE void BT_TraceSingleTest(const btTransform& rayFrom, const btTransform& rayTo,
   btCollisionObject* collisionObject,
   const btCollisionShape* collisionShape,
   btConvexShape* traceBox,
   const btTransform& colObjWorldTransform,
   btCollisionWorld::RayResultCallback& resultCallback) {

	if (collisionShape->isConvex()) {
		btConvexCast::CastResult castResult;
		castResult.m_fraction = btScalar(1);

		btConvexShape* convexShape = (btConvexShape*) collisionShape;
		btVoronoiSimplexSolver   simplexSolver;
		btSubsimplexConvexCast convexCaster(traceBox, convexShape, &simplexSolver);

		btVector3 mins, maxs;
		traceBox->getAabb(rayFrom, mins, maxs);

		if (convexCaster.calcTimeOfImpact(rayFrom, rayTo, colObjWorldTransform, colObjWorldTransform, castResult)) {
			//add hit
			castResult.m_normal = rayFrom.getBasis()*castResult.m_normal; 
			castResult.m_normal.normalize(); 

			if (castResult.m_normal.length2() > btScalar(0.0001)) {
				
				if (castResult.m_fraction < resultCallback.m_closestHitFraction) {
					btCollisionWorld::LocalRayResult localRayResult
						(
						collisionObject,
						0,
						castResult.m_normal,
						castResult.m_fraction
						);
					resultCallback.addSingleResult(localRayResult, true);
				}
			}
		}
	}
	else
	{
      if (collisionShape->isConcave()) {
         btTriangleMeshShape* triangleMesh = (btTriangleMeshShape*)collisionShape;

         btTransform worldTocollisionObject = colObjWorldTransform.inverse();

         btTransform rayFromLocal = worldTocollisionObject * rayFrom;
         btTransform rayToLocal = worldTocollisionObject * rayTo;

         struct LocalTriangleSphereCastCallback   : public btTriangleCallback {
            btTransform m_ccdSphereFromTrans;
            btTransform m_ccdSphereToTrans;
            btTransform   m_meshTransform;

            btScalar   m_ccdSphereRadius;
            btScalar   m_hitFraction;

            btVector3   m_normal;

            LocalTriangleSphereCastCallback(const btTransform& from,
               const btTransform& to, btScalar ccdSphereRadius,
               btScalar hitFraction)
               :m_ccdSphereFromTrans(from),
               m_ccdSphereToTrans(to),
               m_ccdSphereRadius(ccdSphereRadius),
               m_hitFraction(hitFraction)
            {
            }

            virtual void processTriangle(btVector3* triangle, int partId, int triangleIndex) {
               //do a swept sphere for now
               btTransform ident;
               ident.setIdentity();
               btConvexCast::CastResult castResult;
               castResult.m_fraction = m_hitFraction;
               btSphereShape   pointShape(m_ccdSphereRadius);
               btTriangleShape   triShape(triangle[0],triangle[1],triangle[2]);
               btVoronoiSimplexSolver   simplexSolver;
               btSubsimplexConvexCast convexCaster(&pointShape,&triShape,&simplexSolver);
               if (convexCaster.calcTimeOfImpact(m_ccdSphereFromTrans, m_ccdSphereToTrans,
                  ident, ident, castResult)) {
                  if (m_hitFraction > castResult.m_fraction) {
                     m_hitFraction = castResult.m_fraction;
                     m_normal = castResult.m_normal;
                  }
               }
            }
         };

         btVector3 rayAabbMin = rayFromLocal.getOrigin();
         rayAabbMin.setMin(rayToLocal.getOrigin());
         btVector3 rayAabbMax = rayFromLocal.getOrigin();
         rayAabbMax.setMax(rayToLocal.getOrigin());
         /*btScalar ccdRadius0 = traceBox->getCcdSweptSphereRadius();
         rayAabbMin -= btVector3(ccdRadius0,ccdRadius0,ccdRadius0);
         rayAabbMax += btVector3(ccdRadius0,ccdRadius0,ccdRadius0);*/

         //btScalar curHitFraction = btScalar(1.); //is this available?
         LocalTriangleSphereCastCallback raycastCallback(rayFromLocal, rayToLocal,
            /*traceBox->getCcdSweptSphereRadius()*/0.f, (resultCallback.m_closestHitFraction));

         //raycastCallback.m_hitFraction = convexbody->getHitFraction();

         triangleMesh->processAllTriangles(&raycastCallback, rayAabbMin, rayAabbMax);

         if (raycastCallback.m_normal.length2() > btScalar(0.0001)) {
            //raycastCallback.m_normal.normalize();
			raycastCallback.m_normal = rayFromLocal.getBasis()*raycastCallback.m_normal; 
			raycastCallback.m_normal.normalize(); 

            if (raycastCallback.m_hitFraction < resultCallback.m_closestHitFraction) {
               btCollisionWorld::LocalRayResult localRayResult
                  (
                     collisionObject,
                     0,
                     raycastCallback.m_normal,
                     raycastCallback.m_hitFraction
                  );
               resultCallback.addSingleResult(localRayResult, true);
            }
         }
      } else {
         //todo: use AABB tree or other BVH acceleration structure!
         if (collisionShape->isCompound()) {
            const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(collisionShape);
            for (int j = 0; j < compoundShape->getNumChildShapes(); j++) {
               btTransform childTrans = compoundShape->getChildTransform(j);
               const btCollisionShape* childCollisionShape = compoundShape->getChildShape(j);
               btTransform childWorldTrans = colObjWorldTransform * childTrans;
               BT_TraceSingleTest(rayFrom, rayTo,
                  collisionObject,
                  childCollisionShape,
                  traceBox,
                  childWorldTrans,
                  resultCallback);
            }
         }
      }
   }
}

//typedef float* vec3_t;

struct ctrace_t
{
	float fraction;
	float normal[3];
	int surfaceFlags;
	int contents;
	int material;
	int hitType;
	uint16_t hitId;
	uint16_t modelIndex;
	uint16_t partName;
	uint16_t partGroup;
	bool allsolid;
	bool startsolid;
	bool walkable;

};

#define VectorCopy(b, a) (a)[0] = (b)[0]; (a)[1] = (b)[1]; (a)[2] = (b)[2]

typedef void (__cdecl * G_DebugLine_t)(float* from, float* to, float* color, int depthTest);
G_DebugLine_t G_DebugLine = (G_DebugLine_t)0x495060;

typedef void (__cdecl * CL_AddDebugLine_t)(float* start, float* end, float* color, int depthTest, int duration, int fromServer);
CL_AddDebugLine_t CL_AddDebugLine = (CL_AddDebugLine_t)0x4C6940;

bool _inTraceDebug;

// this function replaces the functionality of CM_BoxTrace and CM_TransformedBoxTrace
// can pass NULL for the optional parametres (i.e. mins, maxs, origin or angles)
void BT_BoxTrace(ctrace_t *results, const vec3_t start_, const vec3_t mins, const vec3_t maxs, const vec3_t end_, int passEntityNum, int contentmask, const vec3_t origin, const vec3_t angles, int capsule) {
   btVector3   extents(0, 0, 0);
   btVector3   offset(0, 0, 0);
   btTransform   rayFrom;
   btTransform   rayTo;

   if (!invCM_loaded)
   {
	   InvCM_Load();
   }

   float start[3];
   float end[3];
   
   start[0] = start_[0];
   start[1] = start_[1];
   start[2] = start_[2];

   end[0] = end_[0];
   end[1] = end_[1];
   end[2] = end_[2];

   // ignore game's own collision for now
   results->fraction = 1.0f;

   // handle rotation -> produce the trace box transform
   if (angles == NULL) {
	   rayFrom.setIdentity();
	   rayTo.setIdentity();
   } else {
	   btMatrix3x3   tm;
	   tm.setEulerYPR(angles[1], angles[0], angles[2]);
	   rayFrom.setBasis(tm);
	   rayTo.setBasis(tm);
   }

   // handle dimensions
   if (mins && maxs) {
      // ensure symmetric mins and maxs and derive extents for a box shape for them
      offset[0] = (mins[0] + maxs[0]) * 0.5;
      offset[1] = (mins[1] + maxs[1]) * 0.5;
      offset[2] = (mins[2] + maxs[2]) * 0.5;
      extents = btVector3(maxs[0], maxs[1], maxs[2]) - offset;
      extents[0] = btFabs(extents[0]);
      extents[1] = btFabs(extents[1]);
      extents[2] = btFabs(extents[2]);
      rayFrom.setOrigin(btVector3(start[0], start[1], start[2])/* + offset*/);
      rayTo.setOrigin(btVector3(end[0], end[1], end[2])/* + offset*/);
   } else {
      rayFrom.setOrigin(btVector3(start[0], start[1], start[2]));
      rayTo.setOrigin(btVector3(end[0], end[1], end[2]));
   }

   // mimicking CM_TransformedBoxTrace behaviour - apply origin offset (wtf is this for?)
   if (origin != NULL) {
      rayFrom.setOrigin(rayFrom.getOrigin() - btVector3(origin[0], origin[1], origin[2]));
      rayTo.setOrigin(rayTo.getOrigin() - btVector3(origin[0], origin[1], origin[2]));
   }

   // initialize the trace struct
   //memset(&results, 0, sizeof(*results));
   //results->fraction = 1.f;

   // these are filled each time a trace ends with a shorter fraction
   // they are used after the loop to fill the trace_t
   bool		  foundCloser = false;
   float      frac = results->fraction;
   float      endpos[3], normal[3];
   btRigidBody   *hitbody;

   // do the actual tracing
   btBoxShape traceBox(extents);
   // based on btCollisionWorld::rayTest and btCollisionWorld::rayTestSingle
   // go over all objects, and if the ray intersects their aabb, do a ray-shape query using convexCaster (CCD)
   /*for (int i = 0; i < invColWorld->getNumCollisionObjects(); i++) {
      btCollisionObject *collisionObject = invColWorld->getCollisionObjectArray()[i];

      btVector3 collisionObjectAabbMin, collisionObjectAabbMax;
      collisionObject->getCollisionShape()->getAabb(collisionObject->getWorldTransform(), collisionObjectAabbMin, collisionObjectAabbMax);

      btScalar hitLambda = btScalar(1); //could use resultCallback.m_closestHitFraction, but needs testing
      btVector3 hitNormal;

      btCollisionWorld::ClosestRayResultCallback resultCallback(rayFrom.getOrigin(), rayTo.getOrigin());
	  //btCollisionWorld::ClosestConvexResultCallback resultCallback(rayFrom.getOrigin(), rayTo.getOrigin());

      //gi = (gameInfo_t *)collisionObject->getUserPointer();
      // compare object AABBs and content flags
      if (btRayAabb(rayFrom.getOrigin(), rayTo.getOrigin(), collisionObjectAabbMin, collisionObjectAabbMax, hitLambda, hitNormal)) {
         /*BT_TraceSingleTest(rayFrom, rayTo, collisionObject, collisionObject->getCollisionShape(), &traceBox,
            collisionObject->getWorldTransform(), resultCallback);* /
		  //invColWorld->convexSweepTest(&traceBox, rayFrom, rayTo, resultCallback);
		 invColWorld->rayTestSingle(rayFrom, rayTo, collisionObject, collisionObject->getCollisionShape(), collisionObject->getWorldTransform(), resultCallback);
         if (resultCallback.m_closestHitFraction < results->fraction) {
            // copy new stuff over
            frac = resultCallback.m_closestHitFraction;
            //VectorCopy(resultCallback.m_hitPointWorld, endpos);
            VectorCopy(resultCallback.m_hitNormalWorld, normal);
            hitbody = (btRigidBody *)collisionObject;

			foundCloser = true;
         }
      }
   }*/

   btCollisionWorld::ClosestConvexResultCallback resultCallback(rayFrom.getOrigin(), rayTo.getOrigin());
   invColWorld->convexSweepTest(&traceBox, rayFrom, rayTo, resultCallback);

   if (resultCallback.m_closestHitFraction < results->fraction) {
	   // copy new stuff over
	   frac = resultCallback.m_closestHitFraction;
	   //VectorCopy(resultCallback.m_hitPointWorld, endpos);
	   VectorCopy(resultCallback.m_hitNormalWorld, normal);
	   hitbody = (btRigidBody *)resultCallback.m_hitCollisionObject;

	   foundCloser = true;
   }

   float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

   //CL_AddDebugLine(start, end, red, true, 100, true);
   InvDebugLine line;
   memcpy(line.from, start, sizeof(start));
   memcpy(line.to, end, sizeof(end));
   line.color = 0xFF00FFFF;

   invDebugLines.push_back(line);

   //gi = (gameInfo_t *)hitbody->getUserPointer();;
   if (foundCloser)
   {
	   if (_inTraceDebug)
	   {
			OutputDebugString(va("tr: %g n %g %g %g\n", frac, normal[0], normal[1], normal[2]));
	   }

	   results->fraction = frac;
	   //VectorCopy(endpos, results->endpos);
	   //results->entityNum = gi->entnum;
	   VectorCopy(normal, results->normal);
	   //results->plane.type = PlaneTypeForNormal(results->normal);
	   //results->contents = gi->contentflags;
	   results->contents = 1;
	   results->material = ((Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "mc/mtl_plastic_case_enemy"))->surfaceTypeBits;
	   results->walkable = true;
	   results->hitType = 0;
	   results->modelIndex = 0;
   }
}

StompHook cmTraceHook;
DWORD cmTraceHookLoc = 0x6062E0;

StompHook cmTraceRetHook;
DWORD cmTraceRetHookLoc = 0x606362;

DWORD cmTraceRet = 0;
float* cmTraceMask;

void InvCM_DoTrace(ctrace_t* results, float start[3], float end[3], float bounds[6], float origin[3])
{
	BT_BoxTrace(results, start, &bounds[0], &bounds[3], end, 0, 0, NULL, NULL, 0);

	/*vec3_t mins;
	vec3_t maxs;

	mins[0] = (bounds[0] - bounds[3]) / 2;
	mins[1] = (bounds[1] - bounds[4]) / 2;
	mins[2] = (bounds[3] - bounds[5]) / 2;

	maxs[0] = (bounds[3] - bounds[0]) / 2;
	maxs[1] = (bounds[4] - bounds[1]) / 2;
	maxs[2] = (bounds[5] - bounds[2]) / 2;

	vec3_t start;
	vec3_t end;
	
	VectorCopy(start_, start);
	VectorCopy(end_, end);

	/*vec3_t offset;
	offset[0] = (mins[0] + maxs[0]) * 0.5f;
	offset[1] = (mins[1] + maxs[1]) * 0.5f;
	offset[2] = (mins[2] + maxs[2]) * 0.5f;

	VectorSubtract(start_, offset, start);
	VectorSubtract(end_, offset, start);* /

	trace_t cresults;
	//CM_BoxTrace(&cresults, start, end, &bounds[0], &bounds[3], 0, CONTENTS_SOLID, qfalse);
	CM_BoxTrace(&cresults, start, end, mins, maxs, 0, CONTENTS_SOLID, qfalse);

	if (_inTraceDebug)
	{
		OutputDebugString(va("trace mins %g %g %g maxs %g %g %g\n", bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]));
	}

	results->fraction = cresults.fraction;
	VectorCopy(cresults.plane.normal, results->normal);
	results->contents = cresults.contents;
	results->hitType = 0;
	results->modelIndex = 0;
	results->walkable = true;
	results->surfaceFlags = cresults.surfaceFlags;
	results->allsolid = cresults.allsolid;
	results->startsolid = cresults.startsolid;*/
}

void __declspec(naked) CM_TraceRetHookStub()
{
	__asm
	{
		//mov eax, cmTraceRet
		//mov [esp], eax
		jmp InvCM_DoTrace
	}
}

void __declspec(naked) CM_TraceHookStub()
{
	__asm
	{
		//push eax
		//mov eax, [esp + 4h]
		mov cmTraceRet, eax
		mov cmTraceMask, ecx
		//mov eax, offset CM_TraceRetHookStub
		//mov [esp + 4h], eax
		//pop eax

		sub esp, 0BCh
		push 6062E6h
		retn
	}
}

static CallHook svFrameHook;
static DWORD svFrameHookLoc = 0x47DFA5;

void __declspec(naked) SV_FrameHookStub()
{
	__asm
	{
		call InvCM_Draw
		jmp svFrameHook.pOriginal
	}
}

CallHook groundTraceDbgHook;
DWORD groundTraceDbgHookLoc = 0x573675;

void GroundTraceDbgHookFunc(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
	void (__cdecl * gt)(int, int, int, int, int, int, int) = (void (__cdecl *)(int, int, int, int, int, int, int))groundTraceDbgHook.pOriginal;

	_inTraceDebug = true;
	gt(a1, a2, a3, a4, a5, a6, a7);
	_inTraceDebug = false;
}

void InvCM_Patch()
{
	cmTraceHook.initialize(cmTraceHookLoc, CM_TraceHookStub);
	cmTraceHook.installHook();

	cmTraceRetHook.initialize(cmTraceRetHookLoc, CM_TraceRetHookStub);
	cmTraceRetHook.installHook();

	svFrameHook.initialize(svFrameHookLoc, SV_FrameHookStub);
	svFrameHook.installHook();

	groundTraceDbgHook.initialize(groundTraceDbgHookLoc, GroundTraceDbgHookFunc);
	groundTraceDbgHook.installHook();
}

#define BT_TO_V(bt, v) float v[3]; v[0] = bt.x(); v[1] = bt.y(); v[2] = bt.z();

void DebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{

}

void DebugDrawer::reportErrorWarning(const char *warningString)
{
	Com_Printf(0, warningString);
}

void DebugDrawer::draw3dText(const btVector3 &location, const char *textString)
{

}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	BT_TO_V(from, f);
	BT_TO_V(to, t);
	float red[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
	//CL_AddDebugLine(f, t, red, true, 0, true);

	InvDebugLine line;
	memcpy(line.from, f, sizeof(f));
	memcpy(line.to, t, sizeof(t));

	line.color = 0xFFFF00FF;

	invDebugLines.push_back(line);
}

void DebugDrawer::drawTriangle(const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar)
{
	BT_TO_V(v0, t0);
	BT_TO_V(v1, t1);
	BT_TO_V(v2, t2);

	float red[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	/*CL_AddDebugLine(t0, t1, red, true, 0, true);
	CL_AddDebugLine(t1, t2, red, true, 0, true);
	CL_AddDebugLine(t2, t0, red, true, 0, true);*/

	InvDebugLine line;
	memcpy(line.from, t0, sizeof(t0));
	memcpy(line.to, t1, sizeof(t1));
	line.color = 0xFF0000FF;

	invDebugLines.push_back(line);
}

void DebugDrawer::setDebugMode( int debugMode )
{
	modes = (DebugDrawModes)debugMode;
}

int DebugDrawer::getDebugMode() const
{
	return modes;
}

#endif


extern "C" void * Hunk_Alloc(int size, ha_pref preference)
{
	void* buf = malloc(size);
	memset(buf, 0, size);
	return buf;
}