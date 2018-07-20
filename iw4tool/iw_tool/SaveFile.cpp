#include "StdInc.h"
#include "Save.h"
#include "zlib.h"
#include <stdio.h>

void Save_Init()
{
	g_save.curStream = 3;
	g_save.streamStackIdx = 0;
	g_save.streamStack[0] = 3;
	//g_save.streamOffsets[3] = (count * 8) + 8 + 48;
	g_save.streamOffsets[3] = 40;
}

void Save_DoThatThing(int count)
{
	FILE* tempFile = fopen("zone.tmp", "wb");
	int zHeader[12];
	memset(zHeader, 0, sizeof(zHeader));
	fwrite(zHeader, 12, 4, tempFile);

	fwrite(&count, 1, 4, tempFile);

	for (int i = 0; i < count; i++)
	{
		int tHeader[2];
		tHeader[0] = -1;
		tHeader[1] = ASSET_TYPE_TECHSET;

		fwrite(tHeader, 4, 2, tempFile);
	}

	int neg1 = -1;
	fwrite(&neg1, 4, 1, tempFile);
	
	fwrite(g_save.stream, g_save.streamUsed, 1, tempFile);

	int off = ftell(tempFile);
	fseek(tempFile, 0, SEEK_SET);

	zHeader[0] = off - 40;
	zHeader[5] = (int)(off * 1.3);
	zHeader[2] = (int)(off * 0.4);

	fwrite(zHeader, 12, 4, tempFile);
	fclose(tempFile);

	char* fileThing = new char[off];
	char* outThing = new char[off];
	tempFile = fopen("zone.tmp", "rb");
	fread(fileThing, 1, off, tempFile);
	fclose(tempFile);

	/*z_stream strm;
	memset(&strm, 0, sizeof(strm));
	deflateInit(&strm, 9);

	strm.avail_in = off;
	strm.next_in = (Bytef*)fileThing;

	strm.avail_out = off;
	strm.next_out = (Bytef*)outThing;
	
	deflate(&strm, Z_NO_FLUSH);

	deflateEnd(&strm);*/

	uLong outLen = off;
	compress2((Bytef*)outThing, &outLen, (Bytef*)fileThing, off, Z_BEST_COMPRESSION);

	tempFile = fopen("zone\\english\\iw4_wc_shaders.ff", "wb");
	char custHeader[] = { 'I', 'W', 'f', 'f', 'u', '1', '0', '0', 0x15, 0x01, 0x00, 0x00, 0x01, 0xbe, 0x4b, 0xca, 0x01, 0x20, 0x6d, 0xd7, 0x2e };

	fwrite(custHeader, 1, sizeof(custHeader), tempFile);
	fwrite(outThing, 1, outLen, tempFile);

	fclose(tempFile);

	delete[] fileThing;
	delete[] outThing;
}

void Save_SaveFile(const char* name, int* size)
{
	char fname[256];
	sprintf_s(fname, sizeof(fname), "%s.tmp", name);

	FILE* tempFile = fopen(fname, "wb");
	int zHeader[10];
	memset(zHeader, 0, sizeof(zHeader));
	fwrite(zHeader, 10, 4, tempFile);

	fwrite(g_save.stream, g_save.streamUsed, 1, tempFile);

	int off = ftell(tempFile);
	fseek(tempFile, 0, SEEK_SET);

	// TODO: replace this with actual sizes of stream blocks
	zHeader[0] = off - 40;
	zHeader[8] = (int)(g_save.streamSizes[6] * 1.2);
	zHeader[9] = (int)(g_save.streamSizes[7] * 1.2);
	zHeader[5] = (int)(off * 1.3);
	zHeader[2] = (int)(off * 0.4);

	fwrite(zHeader, 10, 4, tempFile);
	fclose(tempFile);	

	*size = off;
}

void Save_CompressFile(const char* name, int off)
{
	char fname[256];
	sprintf_s(fname, sizeof(fname), "%s.tmp", name);

	char* fileThing = new char[off];
	char* outThing = new char[off];
	FILE* tempFile = fopen(fname, "rb");
	fread(fileThing, 1, off, tempFile);
	fclose(tempFile);

	uLong outLen = off;
	int err = compress2((Bytef*)outThing, &outLen, (Bytef*)fileThing, off, Z_BEST_COMPRESSION);

	if (err != Z_OK)
	{
		Sys_Error("compression error %d from zlib", err);
	}

	sprintf_s(fname, sizeof(fname), "zone\\english\\%s.ff", name);

	tempFile = fopen(fname, "wb");
	char custHeader[] = { 'I', 'W', 'f', 'f', 'u', '1', '0', '0', 0x15, 0x01, 0x00, 0x00, 0x01, 0xbe, 0x4b, 0xca, 0x01, 0x20, 0x6d, 0xd7, 0x2e };

	SYSTEMTIME systemTime;
	FILETIME fileTime;
	GetSystemTime(&systemTime);
	SystemTimeToFileTime(&systemTime, &fileTime);

	// iw's filetimes are inverted high/low
	*(DWORD*)(&custHeader[13]) = fileTime.dwHighDateTime;
	*(DWORD*)(&custHeader[17]) = fileTime.dwLowDateTime;

	fwrite(custHeader, 1, sizeof(custHeader), tempFile);
	fwrite(outThing, 1, outLen, tempFile);

	fclose(tempFile);

	delete[] fileThing;
	delete[] outThing;
}

void Save_LinkFile(const char* name, XAssetEntry* entries, int numEntries)
{
	// print 'link...'
	printf("link... ");

	// append a raw file with an identifying mark
	RawFile* tagRawFile = new RawFile;
	tagRawFile->name = const_cast<char*>(name);
	tagRawFile->compressedSize = 0;
	tagRawFile->size = strlen(IWTOOL_TAG);
	tagRawFile->data = IWTOOL_TAG;

	XAssetEntry* finalEntries = new XAssetEntry[numEntries + 1];
	memcpy(finalEntries, entries, sizeof(XAssetEntry) * numEntries);

	finalEntries[numEntries].type = ASSET_TYPE_RAWFILE;
	finalEntries[numEntries].asset = tagRawFile;

	numEntries++;

	// mark script strings
	SL_MarkAssets(finalEntries, numEntries);

	// list script strings
	int numScriptStrings = g_save.scriptStringMap.size();
	char** scriptStrings = new char*[numScriptStrings + 1];
	scriptStrings[0] = 0;

	for (int i = 0; i < numScriptStrings; i++)
	{
		scriptStrings[i + 1] = const_cast<char*>(g_save.scriptStringMap[i].c_str());
	}

	// create the XAssetList struct
	XAssetList* list = new XAssetList;
	list->numScriptStrings = (numScriptStrings) ? (numScriptStrings + 1) : 0;
	list->scriptStrings = (numScriptStrings) ? scriptStrings : nullptr;
	list->numAssets = numEntries;
	list->assets = finalEntries;

	// initialize the stream and store the XAssets in the stream
	Save_Init();
	Save_XAssetList(list);
	
	// clean up the temp entries
	delete list;
	delete[] scriptStrings;
	delete[] finalEntries;
	delete tagRawFile;

	// save the file
	int size;

	printf("save... ");
	Save_SaveFile(name, &size);

	// and compress/pack
	printf("compress...\n");
	Save_CompressFile(name, size);
}