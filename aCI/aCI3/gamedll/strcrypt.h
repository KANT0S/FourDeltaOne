// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: String segment deobfuscation, header file.
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#pragma once

namespace ci
{
	class StringObfuscation
	{
	public:
		static void initialize();
		static const char* deobfuscateString(const char* string);
		static const wchar_t* deobfuscateString(const wchar_t* string);
	};
}

#define STRCRYPT_START	__pragma(const_seg(push, ".cstring$Z"))
#define STRCRYPT_ALIGN	__declspec(align(16))
#define STRCRYPT_END	__pragma(const_seg(pop))

#define STRCRYPT_STARTW	__pragma(const_seg(push, ".wstring$Z"))