// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Project 'Inverse' - model loader
//
// Initial author: NTAuthority
// Started: 2013-02-19
// ==========================================================

#include "StdInc.h"
#include "Inverse.h"

#ifdef ENABLE_INVERSE
#include "iqm.h"
#include "s10e5.h"

DWORD __declspec(naked) Vec2PackTexCoords(float* coords)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		push    ebx
		sub     esp, 10h
		mov     eax, [ebp+8]
		mov     ebx, [eax+4]
		mov     eax, [eax]
		mov     [ebp-8], eax
		mov     eax, [ebp-8]
		mov     edx, eax
		sar     edx, 10h
		and     edx, 0C000h
		lea     eax, [eax+eax-80000000h]
		sar     eax, 0Eh
		cmp     eax, 3FFEh
		jle     loc_1CEA97

		mov     eax, 3FFFh

loc_1CEA5E:
		mov     ecx, edx
		or      ecx, eax
		mov     [ebp-8], ebx
		mov     eax, [ebp-8]
		mov     edx, eax
		sar     edx, 10h
		and     edx, 0C000h
		lea     eax, [eax+eax-80000000h]
		sar     eax, 0Eh
		cmp     eax, 3FFEh
		jle     loc_1CEAA2

		mov     eax, 3FFFh

loc_1CEA89:
		or      edx, eax
		shl     ecx, 10h
		lea     eax, [edx+ecx]
		add     esp, 10h
		pop     ebx
		pop     ebp
		retn


loc_1CEA97:
		cmp     eax, 0FFFFC000h
		jg      loc_1CEAB9

		xor     eax, eax
		jmp     loc_1CEA5E

loc_1CEAB9:
		and     eax, 3FFFh
		jmp     loc_1CEA5E

loc_1CEAA2:
		cmp     eax, 0FFFFC000h
		jg      loc_1CEAC0

		xor     eax, eax
		or      edx, eax
		shl     ecx, 10h
		lea     eax, [edx+ecx]
		add     esp, 10h
		pop     ebx
		pop     ebp
		retn

loc_1CEAC0:
		and     eax, 3FFFh
		jmp     loc_1CEA89

	}
}

typedef void (__cdecl * Vec3NormalizeTo_t)(const float* in, float* out);
Vec3NormalizeTo_t Vec3NormalizeTo = (Vec3NormalizeTo_t)0x402570;

typedef float (__cdecl * Vec3Normalize_t)(float* vec);
Vec3Normalize_t Vec3Normalize = (Vec3Normalize_t)0x453500;

DWORD Vec3PackUnitVec(float* vec)
{
	float n[3];
	Vec3NormalizeTo(vec, n);

	DWORD v2 = 0;
	float v15 = FLT_MAX;
	float v16 = FLT_MAX;

	char v14[4];
	memset(v14, 0, sizeof(v14));

	for (int v1 = 0; ; v1++)
	{
		double v3 = v1 + 192.0;
		float v4 = 32385.0f / v3;

		v14[0] = (n[0] * v4 + 127.5f);
		v14[1] = (n[1] * v4 + 127.5f);
		v14[2] = (n[2] * v4 + 127.5f);

		float v5 = v3 / 32385.0f;

		float v18[3];
		v18[0] = (v14[0] - 127.0f) * v5;
		v18[1] = (v14[1] - 127.0f) * v5;
		v18[2] = (v14[2] - 127.0f) * v5;

		float v13 = fabs(Vec3Normalize(v18) - 1.0f);

		if (v13 < 0.001)
		{
			float v7 = (v18[0] * n[0]) + (v18[1] * n[1]) + (v18[2] * n[2]);
			float v8 = v7 - 1.0f;
			float v9, v10 = 0;

			float v17 = fabs(v8);

			if (v17 < v15)
			{
				v9 = v17;
				v10 = v13;
			}
			else
			{
				v9 = v17;

				if (v17 != v15)
				{
					goto LABEL_6;
				}

				v10 = v13;

				if (v13 >= v16)
				{
					goto LABEL_6;
				}
			}

			v15 = v9;
			v2 = *(DWORD*)v14;
			v16 = v10;

			if ((v9 + v10) == 0)
			{
				return *(DWORD*)v14;
			}
		}

LABEL_6:
		v14[3] = v1;

		if (v1 == 0xFF)
		{
			return v2;
		}
	}
}

// taken from PatchMW2CModels
void Load_VertexBuffer(void* data, void** where, int len)
{
	DWORD func = 0x5112C0;

	__asm
	{
		push edi

		mov eax, len
		mov edi, where
		push data

		call func

		add esp, 4
		pop edi
	}
}

typedef void* (__cdecl * R_AllocStaticIndexBuffer_t)(void** store, int length);
R_AllocStaticIndexBuffer_t R_AllocStaticIndexBuffer = (R_AllocStaticIndexBuffer_t)0x51E7A0;

void Load_IndexBuffer(void* data, void** storeHere, int count)
{
	static dvar_t* r_loadForRenderer = *(dvar_t**)0x69F0ED4;

	if (r_loadForRenderer->current.boolean)
	{
		void* buffer = R_AllocStaticIndexBuffer(storeHere, 2 * count);
		memcpy(buffer, data, 2 * count);

		__asm
		{
			push ecx
			mov ecx, storeHere
			mov ecx, [ecx]

			mov eax, [ecx]
			add eax, 30h
			mov eax, [eax]

			push ecx
			call eax

			pop ecx
		}
	}
}

void InvModel::Load(const char* filename)
{
	int fh;
	int length = FS_FOpenFileRead(filename, &fh, 0);

	if (length < 0)
	{
		Com_Error(0, "Couldn't load InvModel %s", filename);
	}

	iqmheader hdr;
	FS_Read(&hdr, sizeof(hdr), fh);

	int numMeshes = hdr.num_meshes;
	int numVerts = hdr.num_vertexes;
	int numTris = hdr.num_triangles;

	// load strings
	char* iqmStrings = NULL;

	if (hdr.ofs_text)
	{
		iqmStrings = new char[hdr.num_text];
		FS_Seek(fh, hdr.ofs_text, FS_SEEK_SET);
		FS_Read(iqmStrings, hdr.num_text, fh);
	}

	// load indices
	iqmtriangle* triangles = new iqmtriangle[numTris];

	FS_Seek(fh, hdr.ofs_triangles, FS_SEEK_SET);
	FS_Read(triangles, sizeof(iqmtriangle) * numTris, fh);

	// load vertex things
	float* iqmPositions = new float[3 * numVerts];
	float* iqmNormals = new float[3 * numVerts];
	float* iqmTexCoords = new float[2 * numVerts];
	DWORD* iqmColors = new DWORD[numVerts];
	memset(iqmColors, 0xFF, numVerts * sizeof(DWORD));

	for (uint32_t i = 0; i < hdr.num_vertexarrays; i++)
	{
		iqmvertexarray va;
		FS_Seek(fh, hdr.ofs_vertexarrays + (sizeof(va) * i), FS_SEEK_SET);
		FS_Read(&va, sizeof(va), fh);

		void* readBuffer = NULL;
		int elSize = 0;

		switch (va.type)
		{
			case IQM_POSITION:
				readBuffer = iqmPositions;
				elSize = 12;
				break;
			case IQM_TEXCOORD:
				readBuffer = iqmTexCoords;
				elSize = 8;
				break;
			case IQM_NORMAL:
				readBuffer = iqmNormals;
				elSize = 12;
				break;
			case IQM_COLOR:
				readBuffer = iqmColors;
				elSize = 4;
				break;
		}

		if (!readBuffer)
		{
			continue;
		}

		FS_Seek(fh, va.offset, FS_SEEK_SET);
		FS_Read(readBuffer, elSize * numVerts, fh);
	}

	// load meshes
	iqmmesh* meshes = new iqmmesh[numMeshes];
	FS_Seek(fh, hdr.ofs_meshes, FS_SEEK_SET);
	FS_Read(meshes, sizeof(iqmmesh) * numMeshes, fh);

	this->meshes = new InvMesh[numMeshes];

	for (int i = 0; i < numMeshes; i++)
	{
		iqmmesh* imesh = &meshes[i];
		InvMesh* omesh = &this->meshes[i];

		omesh->indices = new r_index_t[imesh->num_triangles * 3];
		omesh->vertices = new GfxPackedVertex[imesh->num_vertexes];

		for (uint32_t j = 0; j < imesh->num_vertexes; j++)
		{
			int v = imesh->first_vertex + j;

			GfxPackedVertex vertex;
			memset(&vertex, 0, sizeof(vertex));

			vertex.x = iqmPositions[(v * 3)] * 39.3701f;
			vertex.y = iqmPositions[(v * 3) + 1] * 39.3701f;
			vertex.z = iqmPositions[(v * 3) + 2] * 39.3701f;

			vertex.color = iqmColors[v];

			//vertex.texCoords = Vec2PackTexCoords(&iqmTexCoords[v * 2]);
			s10e5 texX(iqmTexCoords[v * 2]);
			s10e5 texY(iqmTexCoords[(v * 2) + 1]);

			vertex.texCoords[0] = texX.bits();
			vertex.texCoords[1] = texY.bits();
			vertex.normal[0] = iqmNormals[v * 3];
			vertex.normal[1] = iqmNormals[(v * 3) + 1];
			vertex.normal[2] = iqmNormals[(v * 3) + 2];
			//vertex.unk = 1.0f;

			omesh->vertices[j] = vertex;
		}

		for (uint32_t j = 0; j < imesh->num_triangles; j++)
		{
			int t = imesh->first_triangle + j;

			omesh->indices[(j * 3)] = triangles[t].vertex[0];
			omesh->indices[(j * 3) + 1] = triangles[t].vertex[1];
			omesh->indices[(j * 3) + 2] = triangles[t].vertex[2];
		}

		omesh->material = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "mc/mtl_plastic_case_enemy");//(Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, &iqmStrings[imesh->material]);

		omesh->numVertices = imesh->num_vertexes;
		omesh->numTriangles = imesh->num_triangles;

		// create buffers!
		IDirect3DVertexBuffer9* vertexBuffer;
		IDirect3DIndexBuffer9* indexBuffer;

		Load_VertexBuffer(omesh->vertices, (void**)&vertexBuffer, 32 * omesh->numVertices);
		Load_IndexBuffer(omesh->indices, (void**)&indexBuffer, 3 * omesh->numTriangles);

		omesh->vertexBuffer = vertexBuffer;
		omesh->indexBuffer = indexBuffer;
	}

	this->numMeshes = numMeshes;

	delete[] meshes;
	delete[] iqmColors;
	delete[] iqmNormals;
	delete[] iqmTexCoords;
	delete[] iqmPositions;

	delete[] triangles;
	delete[] iqmStrings;

	FS_FCloseFile(fh);
}

InvMesh* InvModel::GetMeshes(int* numMeshes)
{
	*numMeshes = this->numMeshes;

	return meshes;
}
#endif