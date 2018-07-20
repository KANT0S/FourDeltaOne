#include "StdInc.h"
#include <shlobj.h>

#define MAX_STABLE_NAMES 32768
static const char* sTableNames[MAX_STABLE_NAMES];
static int currentSTable = 0;

typedef struct stringTable_s {
	char* fileName;
	int columns;
	int rows;
	char** data;
} stringTable_t;

stringTable_t* stringTable;

void AddStringTableEntry(const char* name)
{
	if (currentSTable < MAX_STABLE_NAMES)
	{
		sTableNames[currentSTable] = name;
		currentSTable++;
	}
}

void DumpStringTables()
{
	for (int i = 0; i < currentSTable; i++)
	{
		const char* name = sTableNames[i];
		stringTable_t* stringTable;

		if (stringTable = (stringTable_t*)DB_FindXAssetHeader(40, name, false))
		{
			char filename[512];
			char dir[512];
			sprintf(filename, "%s/%s", "raw", name);

			GetCurrentDirectoryA(sizeof(dir), dir);
			strcat(dir, "/");
			strcat(dir, filename);
			*(strrchr(dir, '/')) = '\0';

			size_t strl = strlen(dir);

			for (size_t i = 0; i < strl; i++)
			{
				if (dir[i] == '/') dir[i] = '\\';
			}

			SHCreateDirectoryExA(NULL, dir, NULL);

			FILE* file = fopen(filename, "w");

			if (file)
			{
				int currentColumn = 0;
				int currentRow = 0;
				int total = stringTable->columns * stringTable->rows;

				for (int i = 0; i < total; i++) {
					char* current = stringTable->data[i * 2];

					fprintf(file, "%s", current);

					bool isNext = ((i + 1) % stringTable->columns) == 0;

					if (isNext) {
						fprintf(file, "\n");
					} else {
						fprintf(file, ",");
					}

					fflush(file);
				}

				fclose(file);
			}
		}
	}

	currentSTable = 0;
}