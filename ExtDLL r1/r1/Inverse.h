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
	float unk; // either 1.0f or -1.0f
	DWORD color;
	/*PackedTexCoords*/ DWORD texCoords;
	DWORD normal;
	DWORD unk2;
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

// streamer backend
