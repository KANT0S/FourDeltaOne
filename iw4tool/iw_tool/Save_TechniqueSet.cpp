#include "StdInc.h"
#include "Save.h"

void Save_VertexDecl(char* var)
{
	char* vertexDecl = *(char**)var;
	char* outVar;

	if (!Save_GetPointer(vertexDecl, (DWORD*)var))
	{
		Save_PushStream(0);
		Save_Align(3);
		Save_Allocate(0x64, vertexDecl, &outVar);
		Save_PopStream();

		Save_XString((char**)(outVar + 0));

		*(DWORD*)var = -2;
	}
}

void Save_VertexShaderData(char* var)
{
	char* outVar;

	Save_Align(3);
	Save_Allocate(*(WORD*)(var + 8) * 4, *(void**)(var + 4), &outVar);
	*(DWORD*)(var + 4) = -1;
}

void Save_VertexShader(char* var)
{
	char* vertexShader = *(char**)var;
	char* outVar;

	if (!Save_GetPointer(vertexShader, (DWORD*)var))
	{
		Save_PushStream(0);
		Save_Align(3);
		Save_Allocate(0x10, vertexShader, &outVar);
		Save_PopStream();

		Save_XString((char**)(outVar + 0));

		Save_VertexShaderData(outVar + 4);

		*(DWORD*)var = -2;
	}
}

void Save_PixelShaderData(char* var)
{
	char* outVar;

	Save_Align(3);
	Save_Allocate(*(WORD*)(var + 8) * 4, *(void**)(var + 4), &outVar);
	*(DWORD*)(var + 4) = -1;
}

void Save_PixelShader(char* var)
{
	char* pixelShader = *(char**)var;
	char* outVar;

	if (!Save_GetPointer(pixelShader, (DWORD*)var))
	{
		Save_PushStream(0);
		Save_Align(3);
		Save_Allocate(0x10, pixelShader, &outVar);
		Save_PopStream();

		Save_XString((char**)(outVar + 0));

		Save_PixelShaderData(outVar + 4);

		*(DWORD*)var = -2;
	}
}

void Save_TechSetPassDataInt(char* var)
{
	int type = *(short*)(var);

	if (type == 1 || type == 7)
	{
		if (*(DWORD*)(var + 4))
		{
			char* outVar;
			Save_Align(3);
			Save_Allocate(16, *(void**)(var + 4), &outVar);

			*(DWORD*)(var + 4) = -1;
		}
	}
}

void Save_TechSetPassData(char* var, int num)
{
	char* passData = *(char**)var;
	char* outVar;

	if (!Save_GetPointer(passData, (DWORD*)var))
	{
		Save_Allocate(8 * num, passData, &outVar);

		for (int i = 0; i < num; i++)
		{
			Save_TechSetPassDataInt(outVar);
			outVar += 8;
		}

		*(DWORD*)var = -1;
	}
}

void Save_TechSetInternal3(char* var)
{
	Save_VertexDecl(var + 0);
	Save_VertexShader(var + 4);
	Save_PixelShader(var + 8);

	if (*(DWORD*)(var + 16))
	{
		Save_Align(3);
		Save_TechSetPassData(var + 16, var[12] + var[13] + var[14]);
	}
}

void Save_TechSetInternal2(char* var, short num)
{
	char* outVar;
	Save_Allocate(20 * num, var, &outVar);

	for (int i = 0; i < num; i++)
	{
		Save_TechSetInternal3(outVar);

		outVar += 20;
	}
}

void Save_TechSetInternal(char* var)
{
	for (int i = 0; i < 48; i++)
	{
		unsigned int* pointer = (unsigned int*)(var + (i * 4));

		if (*pointer)
		{
			char* outVar;

			if (!Save_GetPointer((char*)*pointer, (DWORD*)pointer, 3))
			{
				Save_Align(3);
				Save_Allocate(8, (void*)*pointer, (void**)&outVar);

				Save_TechSetInternal2((char*)((*pointer) + 8), *(short*)(outVar + 6));
				Save_XString((char**)(outVar + 0));

				*pointer = -1;
			}
		}
	}
}

void Save_MaterialTechniqueSet(void** var)
{
	char* techset = (char*)*var;

	if (!Save_GetPointer(techset, (DWORD*)var))
	{
		char* varTechset;
		Save_PushStream(0);
		Save_Allocate(0xcc, techset, (void**)&varTechset);
		*(DWORD*)(varTechset + 8) = 0;
		Save_PopStream();

		Save_XString((char**)(varTechset + 0));
		Save_TechSetInternal(varTechset + 12);

		*(DWORD*)var = -2;
	}
}
