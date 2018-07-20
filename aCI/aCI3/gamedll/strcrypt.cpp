// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: String segment deobfuscation functions.
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#include "aci3.h"

static char g_strKey[16] = { 0x01, 'K', 'E', 'Y', 'T', 'O', 'M', 'Y', 'H', 'E', 'A', 'R', 'T', 'L', 'Y', '#' };

#pragma code_seg(push, ".ci")
namespace ci
{
	const char* StringObfuscation::deobfuscateString(const char* string)
	{
		return (string + 0xBABE);
	}

	const wchar_t* StringObfuscation::deobfuscateString(const wchar_t* string)
	{
		return (string + 0xC0DE);
	}
}
#pragma code_seg(pop, ".ci")