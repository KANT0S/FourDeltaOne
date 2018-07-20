// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: General header file
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#pragma once

//#define CIDEV

#ifdef CIDEV
void DBGprintf(const char* format, ...);
#define DBGprint(x, ...) DBGprintf(x, __VA_ARGS__)
#else
#define DBGprint(x, ...) 
#endif

// OS header files
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef NTSYSCALLAPI
#define NTSYSCALLAPI extern "C" 

#define _vsnprintf(...) false
#define _vsnwprintf(...) false
#define getwc(...) 0
#define WEOF 0

#include <ntndk.h>
#include <strsafe.h>

#include <stdint.h>

#include <VMProtectSDK.h>
/*#define VMProtectBeginMutation(x) 
#define VMProtectEnd() 
#define VMProtectBeginVirtualization(x)
#define VMProtectBeginUltra(x)
#define VMProtectIsDebuggerPresent() false*/

// type definitions
typedef void (__cdecl * PVOIDFUNC)();

// CI header files
#include "hooking.h"
#include "systemapi.h"
#include "strcrypt.h"
#include "strutils.h"
#include "sysutils.h"

// allocation functions
#define halloc(size) HeapAlloc(GetProcessHeap(), 0, size)
#define hrealloc(memory, size) HeapReAlloc(GetProcessHeap(), 0, memory, size)
#define hfree(memory) HeapFree(GetProcessHeap(), 0, memory)

int
	b64_ntop(uint8_t const *src, size_t srclength, char *target, size_t targsize);