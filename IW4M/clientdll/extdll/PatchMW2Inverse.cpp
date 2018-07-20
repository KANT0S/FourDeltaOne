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
#include "Inverse.h"

#ifdef ENABLE_INVERSE
#include <xnamath.h>

InvModel* g_testModel;

IDirect3DVertexDeclaration9* g_invDecl;
IDirect3DVertexShader9* g_invVS;
IDirect3DPixelShader9* g_invPS;

IDirect3DDevice9** g_d3d = (IDirect3DDevice9**)0x66DEF88;

#define g_dx (*g_d3d)

void* LoadShader(const char* filename)
{
	static char shaderBuffer[65535];

	// todo: replace!
	const char* n = va("X:\\iw4m-branch\\clientdll\\Release\\%s.cso", filename);
	FILE* f = fopen(n, "rb");

	if (!f)
	{
		Com_Error(0, "lol no file %s", n);
	}

	fseek(f, 0, SEEK_END);
	int end = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(shaderBuffer, 1, end, f);
	fclose(f);

	return shaderBuffer;
}

IDirect3DVertexShader9* LoadVertexShader(const char* name)
{
	IDirect3DVertexShader9* shader = nullptr;
	g_dx->CreateVertexShader((DWORD*)LoadShader(name), &shader);

	return shader;
}

IDirect3DPixelShader9* LoadPixelShader(const char* name)
{
	IDirect3DPixelShader9* shader = nullptr;
	g_dx->CreatePixelShader((DWORD*)LoadShader(name), &shader);

	return shader;
}

void Inverse_Load()
{
	//g_testModel = new InvModel("inverse.iqm");

	D3DVERTEXELEMENT9 elements[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT16_2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		D3DDECL_END()
	};

	g_dx->CreateVertexDeclaration(elements, &g_invDecl);

	//g_invVS = LoadVertexShader("inv_vs");
	//g_invPS = LoadPixelShader("inv_ps");

	InvCM_Load();
}

void Inverse_Draw()
{

}

void Inverse_UpdateVertexDecl(GfxCmdBufContext2* src)
{
	src->device->SetVertexDeclaration(g_invDecl);
	src->device->SetVertexShader(g_invVS);
	src->device->SetPixelShader(g_invPS);
}

DWORD r_setupPassLoc = 0x54D180;

void __declspec(naked) R_SetupPass(GfxCmdBufContext)
{
	__asm
	{
		xor eax, eax
		jmp r_setupPassLoc
	}
}

void R_UpdateVertexDecl(GfxCmdBufContext2* a1)
{
	__asm
	{
		push esi
		mov esi, a1
		mov eax, 54D0E0h
		call eax
		pop esi
	}
}

CallHook opaqueDrawCallHook;
DWORD opaqueDrawCallHookLoc = 0x559FC6;

typedef void (__cdecl * R_Set3DViewport_t)(int, GfxCmdBufContext);
R_Set3DViewport_t R_Set3DViewport = (R_Set3DViewport_t)0x52F6F0;

typedef void (__cdecl * R_SetPassPixelShaderStableArguments_t)(int a1, GfxCmdBufContext);
R_SetPassPixelShaderStableArguments_t R_SetPassPixelShaderStableArguments = (R_SetPassPixelShaderStableArguments_t)0x54CB00;

typedef void (__cdecl * R_SetPassShaderObjectArguments_t)(int a1, GfxCmdBufContext);
R_SetPassShaderObjectArguments_t R_SetPassShaderObjectArguments_ = (R_SetPassShaderObjectArguments_t)0x54C990;

void __declspec(naked) R_SetPassShaderObjectArguments(int a1, GfxCmdBufContext a2, int a3)
{
	__asm
	{
		push edx
		push ebx
		mov eax, [esp + 8 + 4]
		mov ebx, [esp + 8 + 8]
		mov ecx, [esp + 8 + 0Ch]
		mov edx, [esp + 8 + 10h]

		push ecx
		push ebx
		push eax
		mov eax, edx

		call R_SetPassShaderObjectArguments_

		add esp, 0Ch

		pop ebx
		pop edx

		retn
	}
}

typedef void (__cdecl * R_SetPassShaderPrimArguments_t)(int a1, GfxCmdBufContext);
R_SetPassShaderPrimArguments_t R_SetPassShaderPrimArguments_ = (R_SetPassShaderPrimArguments_t)0x54C870;

void __declspec(naked) R_SetPassShaderPrimArguments(int a1, GfxCmdBufContext a2, int a3)
{
	__asm
	{
		push edx
		push ebx
		mov eax, [esp + 8 + 4]
		mov ebx, [esp + 8 + 8]
		mov ecx, [esp + 8 + 0Ch]
		mov edx, [esp + 8 + 10h]

		push ecx
		push ebx
		push eax
		mov eax, edx

		call R_SetPassShaderPrimArguments_

		add esp, 0Ch

		pop ebx
		pop edx

		retn
	}
}

typedef void (__cdecl * R_SetupPassVertexShaderArgs_t)(GfxCmdBufContext);
R_SetupPassVertexShaderArgs_t R_SetupPassVertexShaderArgs = (R_SetupPassVertexShaderArgs_t)0x54CCB0;

struct InvDebugLine
{
	float from[3];
	float to[3];

	DWORD color;
};

extern std::vector<InvDebugLine> invDebugLines;

void Inverse_DrawInvertified(GfxViewInfo* viewInfo, int a1, GfxCmdBufContext context);

static GfxViewInfo* viewInfo;

void R_DrawOpaqueHookFunc(int a1, GfxCmdBufContext context)
{
	R_Set3DViewport(a1, context);

	D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"INVERSE");

	/*int numMeshes = 0;
	InvMesh* meshes = g_testModel->GetMeshes(&numMeshes);

	Material* material = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "$default3d");

	context.a2->material = material;
	context.a2->materialTechniqueType = 4;
	context.a2->technique = material->techniqueSet->remappedTechniqueSet->techniques[4];
	//context.a2->materialTechniqueType = 9;
	//context.a2->technique = material->techniqueSet->remappedTechniqueSet->techniques[9];

	context.a2->vertexType = material->techniqueSet->remappedTechniqueSet->pad[0] + 2;

	D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"setup pass");
	R_SetupPass(context);
	D3DPERF_EndEvent();

	D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"vertex decl");
	R_UpdateVertexDecl(context.a2);
	Inverse_UpdateVertexDecl(context.a2);
	D3DPERF_EndEvent();

	D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"vertex stable");
	R_SetupPassVertexShaderArgs(context);		
	D3DPERF_EndEvent();

	if (context.a2->shaderData->num3)
	{
		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"pixel stable");
		R_SetPassPixelShaderStableArguments(context.a2->shaderData->num3, context);
		D3DPERF_EndEvent();
	}

	DWORD ad1 = (context.a2->shaderData->num4) + (8 * context.a2->shaderData->num1);

	if (context.a2->shaderData->num2)
	{
		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"pass object");
		//__asm mov eax, ad1
		R_SetPassShaderObjectArguments(context.a2->shaderData->num2, context, ad1);
		D3DPERF_EndEvent();
	}

	ad1 = (context.a2->shaderData->num4);

	if (context.a2->shaderData->num1)
	{
		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"pass prim");
		//__asm mov eax, ad1
		R_SetPassShaderPrimArguments(context.a2->shaderData->num1, context, ad1);
		D3DPERF_EndEvent();
	}*/

	Inverse_DrawInvertified(viewInfo, a1, context);

	D3DPERF_EndEvent();
	//for (int i = 0; i < numMeshes; i++)
	{
		//InvMesh* mesh = &meshes[i];

		/*Material* material = mesh->GetMaterial();

		context.a2->material = material;
		context.a2->materialTechniqueType = 4;
		context.a2->technique = material->techniqueSet->remappedTechniqueSet->techniques[4];
		//context.a2->materialTechniqueType = 9;
		//context.a2->technique = material->techniqueSet->remappedTechniqueSet->techniques[9];

		context.a2->vertexType = material->techniqueSet->remappedTechniqueSet->pad[0] + 2;

		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"setup pass");
		R_SetupPass(context);
		D3DPERF_EndEvent();

		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"vertex decl");
		R_UpdateVertexDecl(context.a2);
		Inverse_UpdateVertexDecl(context.a2);
		D3DPERF_EndEvent();

		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"vertex stable");
		R_SetupPassVertexShaderArgs(context);		
		D3DPERF_EndEvent();

		if (context.a2->shaderData->num3)
		{
			D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"pixel stable");
			R_SetPassPixelShaderStableArguments(context.a2->shaderData->num3, context);
			D3DPERF_EndEvent();
		}

		DWORD ad1 = (context.a2->shaderData->num4) + (8 * context.a2->shaderData->num1);

		if (context.a2->shaderData->num2)
		{
			D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"pass object");
			//__asm mov eax, ad1
			R_SetPassShaderObjectArguments(context.a2->shaderData->num2, context, ad1);
			D3DPERF_EndEvent();
		}

		ad1 = (context.a2->shaderData->num4);

		if (context.a2->shaderData->num1)
		{
			D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"pass prim");
			//__asm mov eax, ad1
			R_SetPassShaderPrimArguments(context.a2->shaderData->num1, context, ad1);
			D3DPERF_EndEvent();
		}

		IDirect3DDevice9* device = context.a2->device;

		/*XMFLOAT4X4 floatsGet;
		device->GetVertexShaderConstantF(0, (float*)&floatsGet, 4);

		XMFLOAT4X4* stuff = (XMFLOAT4X4*)(((char*)context.a1) + 640); // param 86; that's -76 = 10; 16 * 4 (matrix size) * 10 = 640!

		XMMATRIX vpMatrix((const float*)&floatsGet);* /

		XMFLOAT4X4 floatsGet2;
		device->GetVertexShaderConstantF(4, (float*)&floatsGet2, 4);

		XMFLOAT4X4* stuffs = (XMFLOAT4X4*)(((char*)context.a1) + 1152 + 128);

		XMMATRIX wMatrix((const float*)&floatsGet2);
		wMatrix = XMMatrixTranspose(wMatrix);

		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 0, 0, 0), L"DRAW STUFF");

		device->SetStreamSource(0, mesh->GetVertexBuffer(), 0, 32);
		device->SetIndices(mesh->GetIndexBuffer());

		XMMATRIX matrix = XMMatrixTranslation(10, 10, 10);
		//matrix = XMMatrixIdentity();
		//XMMatrixTranspose(matrix);
		//XMStoreFloat4x4(&floats, matrix);

		XMVECTOR meh;
		XMMATRIX lol = wMatrix * matrix;
		lol = XMMatrixTranspose(lol);

		device->SetVertexShaderConstantF(4, (float*)&lol, 4);

		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mesh->numVertices, 0, mesh->numTriangles);

		// vertices for debug lines
		/*GfxPackedVertex* debugVerts = new GfxPackedVertex[(invDebugLines.size() * 2) + 10];

		for (int i = 0; i < (invDebugLines.size() * 2); i++)
		{
			debugVerts[i].color = invDebugLines[i / 2].color;
			debugVerts[i].normal[0] = 0.f;
			debugVerts[i].normal[1] = 0.f;
			debugVerts[i].normal[2] = 1.f;
			debugVerts[i].texCoords[0] = 0.f;
			debugVerts[i].texCoords[1] = 0.f;

			debugVerts[i].x = invDebugLines[i / 2].from[0];
			debugVerts[i].y = invDebugLines[i / 2].from[1];
			debugVerts[i].z = invDebugLines[i / 2].from[2];

			i++;

			debugVerts[i].color = invDebugLines[i / 2].color;
			debugVerts[i].normal[0] = 0.f;
			debugVerts[i].normal[1] = 0.f;
			debugVerts[i].normal[2] = 1.f;
			debugVerts[i].texCoords[0] = 0.f;
			debugVerts[i].texCoords[1] = 0.f;

			debugVerts[i].x = invDebugLines[i / 2].to[0];
			debugVerts[i].y = invDebugLines[i / 2].to[1];
			debugVerts[i].z = invDebugLines[i / 2].to[2];
		}

		device->DrawPrimitiveUP(D3DPT_LINELIST, invDebugLines.size(), debugVerts, sizeof(GfxPackedVertex));

		delete[] debugVerts;* /

		D3DPERF_EndEvent();*/
	}
}

void __declspec(naked) R_DrawOpaqueHookStub()
{
	__asm
	{
		mov viewInfo, esi

		jmp R_DrawOpaqueHookFunc
	}
}

StompHook loadLevelZoneHook;
DWORD loadLevelZoneHookLoc = 0x42C2CC;

void __declspec(naked) LoadLevelZoneHookStub()
{
	__asm
	{
		call Inverse_Load

		push 4A95B0h
		retn
	}
}

void PatchMW2_Inverse()
{
	loadLevelZoneHook.initialize(loadLevelZoneHookLoc, LoadLevelZoneHookStub);
	loadLevelZoneHook.installHook();

	opaqueDrawCallHook.initialize(opaqueDrawCallHookLoc, R_DrawOpaqueHookStub);
	opaqueDrawCallHook.installHook();

	//*(BYTE*)0x541E40 = 0xC3; // disable BSP surface drawing func #1?
	//*(BYTE*)0x547300 = 0xC3; // ^

	// don't draw bsp surfaces (pretess case?)
	memset((void*)0x553DB5, 0x90, 18);
	memset((void*)0x553FB8, 0x90, 18);

	InvCM_Patch();
}
#else
void PatchMW2_Inverse()
{
}
#endif