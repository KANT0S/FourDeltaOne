// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: standard include file for system headers
//
// Initial author: NTAuthority
// Started: 2010-08-29
// ==========================================================

#pragma once

#define _CRT_SECURE_NO_WARNINGS

// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C/C++ headers
#define _SCL_SECURE_NO_WARNINGS

#include <map>
#include <vector>
#include <string>

#include <stdint.h>

void Sys_RunInit();

// safe string functions
#define STRSAFE_NO_DEPRECATE
#define STRSAFE_NO_CCH_FUNCTIONS
#include <tchar.h>
#include <strsafe.h>

#undef sprintf_s
#define sprintf_s StringCbPrintf
//#define sprintf_s(buf, size, format, ...) StringCbPrintf(buf, size, format, __VA_ARGS__)

#undef strcat_s
#define strcat_s StringCbCat
//#define strcat_s(dst, size, src) StringCbCat(dst, size, src)

#undef strcpy_s
#define strcpy_s StringCbCopy