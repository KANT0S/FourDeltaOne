#pragma once

// variables
extern const char** g_assetTypes;

// structures
struct XAssetEntry
{
	assetType_t type;
	void* asset;
};

struct XAssetList
{
	int numScriptStrings;
	char** scriptStrings;
	int numAssets;
	XAssetEntry* assets;
};

struct RawFile
{
	char* name;
	int compressedSize; // no trailing \0 in the string
	int size; // excludes the trailing \0
	char* data;
};

struct GfxImage
{
	char* texture;
	char unknown2;
	char a3;
	char a2;
	char unknown3;
	char unknown4;
	char unknown5;
	char unknown6;
	char a4;
	int dataLength1;
	int dataLength2;
	short width;
	short height;
	short depth;
	short unknown8;
	char* name;
};

struct MaterialTextureDef
{
	unsigned int typeHash; // asset hash of type
	char firstCharacter; // first character of image name
	char secondLastCharacter; // second-last character of image name (maybe only in CoD4?!)
	unsigned char unknown; // maybe 0xE2
	char unknown2; // likely 0x00
	GfxImage* image; // GfxImage* actually
};

struct Material
{
	char* name;
	unsigned short flags; // 0x2F00 for instance
	unsigned char animationX; // amount of animation frames in X
	unsigned char animationY; // amount of animation frames in Y
	char unknown1[4]; // 0x00
	unsigned int rendererIndex; // only for 3D models
	char unknown9[8];
	unsigned int unknown2; // 0xFFFFFFFF
	unsigned int unknown3; // 0xFFFFFF00
	char unknown4[40]; // 0xFF
	char numMaps; // 0x01, possibly 'map count' (zone code confirms)
	char unknown5; // 0x00
	char unknownCount2; // 0x01, maybe map count actually
	char unknown6; // 0x03
	unsigned int unknown7; // 0x04
	void* techniqueSet; // '2d' techset; +80
	MaterialTextureDef* maps; // map references
	char* unknown8;
	void* stateMap; // might be NULL, need to test
};

struct XSurfaceCTEntry
{
	char pad[24];
	int numNode;
	char* node; // el size 16
	int numLeaf;
	short* leaf;
};

struct XSurfaceCT
{
	int pad;
	int pad2;
	XSurfaceCTEntry* entry;
};

struct XSurface
{
	short pad;
	short numVertices;
	short numPrimitives;
	char streamHandle; // something to do with buffers
	char pad2;
	int pad3;
	void* indexBuffer;
	short blendNum1;
	short blendNum2;
	short blendNum3;
	short blendNum4;
	short* blendInfo;
	void* vertexBuffer; // +28
	int numCT;
	XSurfaceCT* ct;
	char pad5[24];
};

struct XModelSurfs
{
	char* name;
	XSurface* surfaces;
	int numSurfaces;
	char pad[24];
};

struct XSurfaceLod
{
	int pad;
	short numSurfacesInLod;
	short pad2;
	XModelSurfs* surfaces;
	char pad3[32];
};


struct cplane_t
{
	char pad[20];
};

struct cbrushside_t
{
	cplane_t* plane;
	int pad;
};

struct BrushWrapper
{
	char pad[24];
	short numPlaneSide;
	short pad2;
	cbrushside_t* side;
	char* edge;
	char pad3[24];
	int numEdge;
	cplane_t* plane;
};

struct PhysGeomInfo
{
	BrushWrapper* brush;
	char pad[64];
};

struct PhysCollmap
{
	char* name;
	int numInfo;
	PhysGeomInfo* info;
	char pad2[60];
};

struct PhysPreset
{
	char* name;
	char pad[24];
	char* unkString;
	char pad2[12];
};

struct XModel
{
	char* name; // +0
	char numBones; // +4
	char numSubBones;
	char numMaterials;
	char pad2;
	char pad3[28]; // +8
	short* boneNames; // +36
	char* boneUnknown1; // +40
	char* boneUnknown2; // +44, element size 8
	char* boneUnknown3; // +48, element size 12
	char* boneUnknown4; // +52
	char* boneUnknown5; // +56, element size 32
	Material** materials; // +60
	XSurfaceLod lods[4];
	int pad4; // +240
	char* stuffs; // +244
	int numStuffs; // +248
	int pad6;
	char* unknowns; // bone count, +256, element size 28
	char pad5[36];
	PhysPreset* physPreset;
	PhysCollmap* physCollmap;
}; // total size 304

struct XAnimTL
{
	short tagName; // script string
	short pad1;
	int pad2;
};

struct XAnimDeltaTransData
{
	char pad[24];
	char* unk;
}; // more data follows struct

struct XAnimDeltaTrans
{
	short deltaBase;
	short deltaBase2;
	union
	{
		float rawData[3];
		XAnimDeltaTransData data;
	};
};

struct XAnimDeltaQuatData
{
	char* data;
}; // more data follows end of struct

struct XAnimDeltaQuat
{
	short deltaBase;
	short deltaBase2;
	union
	{
		int rawData;
		XAnimDeltaQuatData data;
	};
};

struct XAnimDeltaUnkData
{
	char* data;
}; // more data follows end of struct, as usual

struct XAnimDeltaUnk
{
	short deltaBase;
	short deltaBase2;
	union
	{
		__int64 rawData;
		XAnimDeltaUnkData data;
	};
};

struct XAnimDeltaPart
{
	XAnimDeltaTrans* trans;
	XAnimDeltaQuat* quat;
	XAnimDeltaUnk* unk;
};

struct XAnimParts
{
	char* name; // +0
	short numUnk1; // +4
	short numUnk2; // +6
	short numUnk3; // +8
	short numUnk4; // +10
	short numUnk5; // +12
	short numIndices; // +14, it being > 255 changes a lot of stuff to short rather than byte
	char pad[10]; // +16
	unsigned char numTags1; // +26
	unsigned char numTags2; // +27
	char pad2[4];
	int numUnk6; // +32
	char pad3[12]; // +36
	short* tagList; // +48, script strings
	char* unk1; // +52
	char* unk2; // +56
	char* unk3; // +60
	char* unk6; // +64
	char* unk4; // +68
	char* unk5; // +72
	char* indices; // +76
	XAnimTL* tagList2; // +80
	XAnimDeltaPart* deltaParts; // +84
}; // size 88

struct VertexDecl
{
	const char* name;
	int unknown;
	char pad[28];
	IDirect3DVertexDeclaration9* declarations[16];
};

struct PixelShader
{
	const char* name;
	IDirect3DPixelShader9* shader;
	DWORD* bytecode;
	int flags;
};

struct VertexShader
{
	const char* name;
	IDirect3DVertexShader9* shader;
	DWORD* bytecode;
	int flags;
};

struct MaterialPass
{
	VertexDecl* vertexDecl;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	char pad[8];
};

struct MaterialTechnique
{
	int pad;
	short pad2;
	short numPasses;
	MaterialPass passes[1];
};

struct MaterialTechniqueSet
{
	const char* name;
	char pad[8];
	MaterialTechnique* techniques[48];
};

#include <unordered_map>
#include <vector>

extern struct save_state_s
{
	void* stream;
	size_t streamSize;
	size_t streamUsed;
	int streamOffsets[9];
	int streamSizes[9];
	char curStream;
	char streamStack[64];
	int streamStackIdx;
	std::vector<std::string> scriptStringMap;
	std::unordered_map<unsigned int, unsigned int> pointerMap;
} g_save;

// base Save_* functions (and strings)
bool Save_GetPointer(void* data, void* pointer, int align = 0);
void Save_Allocate(size_t size, void* data, void* outVar);
void Save_Align(DWORD align);
void Save_PushStream(int next);
void Save_PopStream();
void Save_XString(char** string);

// main linker function
void Save_LinkFile(const char* name, XAssetEntry* entries, int numEntries);

// XAsset
void Save_XAssetList(XAssetList* list);
short Save_ScriptString(const char* string);
void SL_MarkAssets(XAssetEntry* entries, int numEntries);

// RawFile
void Save_RawFile(RawFile** file);

// MaterialTechniqueSet, MaterialVertexShader, MaterialPixelShader and MaterialVertexDeclaration
void Save_MaterialTechniqueSet(void** techset);
void Save_VertexShader(char* var);
void Save_PixelShader(char* var);
void Save_VertexDecl(char* var);

// Material
void Save_Material(Material** material);

// XModel
void Save_XModel(XModel** var);
void SL_MarkXModel(XModel* model);

void Save_PhysPreset(PhysPreset** var);

// XAnimParts
void Save_XAnim(XAnimParts** var);
void SL_MarkXAnim(XAnimParts* anim);