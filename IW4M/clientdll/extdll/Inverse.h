// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Inverse main header file
//
// Initial author: NTAuthority
// Started: 2013-02-19
// ==========================================================

#pragma once

#include <d3d9.h>

// model class
struct GfxPackedVertex
{
	float x;
	float y;
	float z;
	DWORD color;
	WORD texCoords[2];
	float normal[3];
};

typedef unsigned short r_index_t;

class InvModel;

class InvMesh
{
friend class InvModel;

private:
	IDirect3DVertexBuffer9* vertexBuffer;
	IDirect3DIndexBuffer9* indexBuffer;

	GfxPackedVertex* vertices;
	r_index_t* indices;

public:
	int numVertices;
	int numTriangles;

	Material* material;

public:
	~InvMesh()
	{
		delete[] vertices;
		delete[] indices;

		if (vertexBuffer)
		{
			vertexBuffer->Release();
		}

		if (indexBuffer)
		{
			indexBuffer->Release();
		}
	}

	IDirect3DIndexBuffer9* GetIndexBuffer() { return indexBuffer; }
	IDirect3DVertexBuffer9* GetVertexBuffer() { return vertexBuffer; }

	Material* GetMaterial() { return material; }
};

class InvModel
{
private:
	InvMesh* meshes;
	int numMeshes;

	void Load(const char* filename);
public:
	InvModel(const char* filename) { Load(filename); }
	~InvModel();

	InvMesh* GetMeshes(int* numMeshes);
};

// collision system
void InvCM_Load();
void InvCM_Patch();

void InvCM_Draw();

// streamer backend


struct GfxShaderData
{
	char pad[12];
	char num1;
	char num2;
	char num3;
	char pad2;
	DWORD num4;
};

struct GfxCmdBufContext2
{
	char pad[144];
	IDirect3DDevice9* device;
	int pad2;
	int vertexType;
	char pad3[28];
	Material* material;
	int materialTechniqueType;
	MaterialTechnique* technique;
	GfxShaderData* shaderData;
};

struct GfxCmdBufContext
{
	char* a1;
	GfxCmdBufContext2* a2;
};

struct GfxViewInfo
{
	// this part is actually GfxViewParms
	float viewMatrix[16];
	float projectionMatrix[16];
	float viewProjectionMatrix[16];
	float inverseViewProjectionMatrix[16];
	float origin[4];
	float axis[9];
	float depthHackNearClip;

};

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

#define	SURF_NODRAW				0x80	// don't generate a drawsurface at all