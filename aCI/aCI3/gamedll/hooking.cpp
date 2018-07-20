#include "aci3.h"

void _patch(void* pAddress, DWORD data, DWORD iSize)
{
	switch(iSize)
	{
	case 1: *(BYTE*)pAddress = (BYTE)data;
		break;
	case 2: *(WORD*)pAddress = (WORD)data;
		break;
	case 4: *(DWORD*)pAddress = (DWORD)data;
		break;
	}
}

void _nop(void* pAddress, DWORD size)
{
	memset(pAddress, 0x90, size);
	return;

	DWORD dwAddress = (DWORD)pAddress;
	if ( size % 2 )
	{
		*(BYTE*)pAddress = 0x90;
		dwAddress++;
	}
	if ( size - ( size % 2 ) )
	{
		DWORD sizeCopy = size - ( size % 2 );
		do
		{
			*(WORD*)dwAddress = 0xFF8B;
			dwAddress += 2;
			sizeCopy -= 2;
		}
		while ( sizeCopy );	
	}
}

void _call(void* pAddress, DWORD data, eCallPatcher bPatchType)
{
	switch ( bPatchType )
	{
	case PATCH_JUMP:
		*(BYTE*)pAddress = (BYTE)0xE9;
		break;

	case PATCH_CALL:
		*(BYTE*)pAddress = (BYTE)0xE8;
		break;

	default:
		break;
	}

	*(DWORD*)((DWORD)pAddress + 1) = (DWORD)data - (DWORD)pAddress - 5;
}

void _charptr(void* pAddress, const char* pChar)
{
	*(DWORD*)pAddress = (DWORD)pChar;
}