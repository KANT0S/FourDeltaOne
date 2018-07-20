#include "StdInc.h"

typedef int (__cdecl * DB_GetXAssetSizeHandler_t)();

void** DB_XAssetPool = (void**)0x8B0080;
unsigned int* g_poolSize = (unsigned int*)0x8AFDA0;

DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x8AFB80;

void* ReallocateAssetPool(int type, unsigned int newSize)
{
	int elSize = DB_GetXAssetSizeHandlers[type]();
	void* poolEntry = malloc(newSize * elSize);
	DB_XAssetPool[type] = poolEntry;
	g_poolSize[type] = newSize;
	return poolEntry;
}

void LogXZoneLoadFunc(char* target, char* source, int length)
{
	_snprintf(target, length, source);

	Com_Printf(0, "Loading fastfile %s\n", target);
}

void PatchIW5_AssetReallocation()
{
	if (g_isDedicated)
	{
		DB_XAssetPool = (void**)0x78B0B8;
		g_poolSize = (unsigned int*)0x78ADD8;
		DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x78AC00;
	}

	ReallocateAssetPool(28, 128);

	// a bit irrelevant, though still related to the XAsset database
	if (g_isDedicated) // only dedis have a raw console
	{
		call(0x45629E, LogXZoneLoadFunc, PATCH_CALL);
	}
}