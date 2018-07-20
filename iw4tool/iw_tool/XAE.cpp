#include "StdInc.h"

int freadint(FILE* file);
char* freadstr(FILE* file);

typedef short (__cdecl * SL_GetString_t)(const char* string);
extern SL_GetString_t SL_GetString;

XAnimParts* XAE_Load(const char* name)
{
	FILE* file = fopen(va("X:\\aiw\\%s.xae", name), "rb");
	XAnimParts* anim = new XAnimParts;

	fread(anim, sizeof(XAnimParts), 1, file);

	//memcpy(((char*)anim) + 17, ((char*)anim) + 18, 13);

	anim->name = freadstr(file);

	// tag list
	anim->tagList = new short[anim->numTags1];

	for (int i = 0; i < anim->numTags1; i++)
	{
		anim->tagList[i] = SL_GetString(freadstr(file));
	}

	if (freadint(file))
	{

		anim->tagList2 = new XAnimTL[anim->numTags2];
		//anim->numTags2 = (anim->pad2 & 0xFF);

		for (int i = 0; i < (anim->numTags2); i++)
		{
			fread(&anim->tagList2[i], sizeof(XAnimTL), 1, file);

			const char* str = freadstr(file);
			anim->tagList2[i].tagName = SL_GetString(str);
		}

		//anim->pad2 &= 0xFFFFFF00;
		//anim->tagList2 = 0;
	}

	if (anim->unk1)
	{
		anim->unk1 = new char[anim->numUnk1];

		fread(anim->unk1, 1, anim->numUnk1, file);
	}

	if (anim->unk2)
	{
		anim->unk2 = new char[anim->numUnk2 * 2];

		fread(anim->unk2, 2, anim->numUnk2, file);
	}

	if (anim->unk3)
	{
		anim->unk3 = new char[anim->numUnk3 * 4];

		fread(anim->unk3, 4, anim->numUnk3, file);
	}

	if (anim->unk6)
	{
		anim->unk6 = new char[anim->numUnk6 * 2];

		fread(anim->unk6, 2, anim->numUnk6, file);
	}

	if (anim->unk4)
	{
		anim->unk4 = new char[anim->numUnk4];

		fread(anim->unk4, 1, anim->numUnk4, file);
	}

	if (anim->unk5)
	{
		anim->unk5 = new char[anim->numUnk5 * 4];

		fread(anim->unk5, 4, anim->numUnk5, file);
	}

	if (anim->deltaParts)
	{
		anim->deltaParts = 0; // sneaky sneaky
	}

	if (anim->indices)
	{
		//anim->numIndices = 0;
		anim->indices = 0;
	}

	fclose(file);
	return anim;
}