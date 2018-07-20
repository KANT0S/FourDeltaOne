#include "StdInc.h"

save_state_s g_save;

void Save_Align(DWORD align);

bool Save_GetPointer(void* data, void* pointer, int align)
{
	if (g_save.pointerMap.find((DWORD)data) != g_save.pointerMap.end())
	{
		*(DWORD*)pointer = g_save.pointerMap[(DWORD)data];
		return true;
	}

	Save_Align(align);

	//g_save.pointerMap[(DWORD)data] = (g_save.curStream << 28) | (g_save.streamOffsets[3] + 1);
	g_save.pointerMap[(DWORD)data] = (0xF << 28) | (g_save.streamOffsets[3] + 1);
	return false;
}

int roundUp(int numToRound, int multiple)  
{  
	if(multiple == 0)  
	{  
		return numToRound;  
	}  

	int remainder = numToRound % multiple; 
	if (remainder == 0)
	{
		return numToRound; 
	}

	return numToRound + multiple - remainder; 
}  

void Save_Allocate(size_t size, void* data, void* outVar)
{
	if (size == 0)
	{
		return;
	}

	// allocate the stream
	if ((g_save.streamUsed + size) >= g_save.streamSize)
	{
		g_save.streamSize += size;
		g_save.streamSize = roundUp(g_save.streamSize, 32768 * 1024);

		g_save.stream = realloc(g_save.stream, g_save.streamSize);
	}

	*(void**)outVar = ((char*)g_save.stream) + g_save.streamUsed;
	g_save.streamUsed += size;
	//g_save.streamOffsets[g_save.curStream] += size;
	//g_save.streamOffsets[g_save.curStream] = roundUp(g_save.streamOffsets[g_save.curStream], 4);
	g_save.streamOffsets[3] += size;
	g_save.streamSizes[g_save.curStream] += roundUp(size, 4);

	memcpy(*(void**)outVar, data, size);
}

void Save_Align(DWORD align)
{
	DWORD pos = ~align & (align + g_save.streamSizes[g_save.curStream]);
	//g_save.streamOffsets[g_save.curStream] = pos;
	g_save.streamSizes[g_save.curStream] = pos;
}

void Save_PushStream(int next)
{
	g_save.streamStackIdx++;
	g_save.streamStack[g_save.streamStackIdx] = next;

	g_save.curStream = next;
}

void Save_PopStream()
{
	g_save.streamStackIdx--;

	g_save.curStream = g_save.streamStack[g_save.streamStackIdx];
}

void Save_XString(char** string)
{
	char* varXString;
	Save_Allocate(strlen(*string) + 1, *string, (void**)&varXString);

	*string = (char*)-1;
}

const char** g_assetTypes = (const char**)0x799278;

Sys_Error_t Sys_Error = (Sys_Error_t)0x43D570;
SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x4EC1D0;