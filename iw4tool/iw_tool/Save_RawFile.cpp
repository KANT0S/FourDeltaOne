#include "StdInc.h"
#include "Save.h"

void Save_RawFile(RawFile** var)
{
	RawFile* file = *var;

	if (!Save_GetPointer(file, (DWORD*)var))
	{
		RawFile* varFile;
		Save_PushStream(0);
		Save_Allocate(sizeof(RawFile), file, (void**)&varFile);
		Save_PopStream();

		Save_XString(&varFile->name);

		if (varFile->data)
		{
			void* outVar;

			if (varFile->compressedSize)
			{
				Save_Allocate(varFile->compressedSize, varFile->data, &outVar);
			}
			else
			{
				Save_Allocate(varFile->size + 1, varFile->data, &outVar);
			}

			varFile->data = (char*)-1;
		}

		*var = (RawFile*)-1;
	}
}