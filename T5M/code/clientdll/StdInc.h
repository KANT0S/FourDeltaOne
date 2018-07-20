// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: standard include file for system headers
//
// Initial author: NTAuthority
// Started: 2010-08-29
// ==========================================================

#pragma once

#define VERSION "3.0"

#define VERSIONSTRING "T5M:" VERSION

#define ENV "prod"
//#define NP_SERVER "t5." ENV ".fourdeltaone.net"
#define NP_SERVER "77.21.194.119"

// ---------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS

// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// LibNP
#include <libnp.h>

// C/C++ headers
#define _SCL_SECURE_NO_WARNINGS

#include <map>
#include <vector>
#include <string>
#include <unordered_map>

// OSW headers
#define NO_STEAM // to reduce header amount needed
#include "CCallback.h"
#include "ISteamClient008.h"
#include "ISteamContentServer002.h"
#include "ISteamUser014.h"
#include "ISteamFriends007.h"
#include "ISteamApps003.h"
#include "ISteamGameServer010.h"
#include "ISteamMasterServerUpdater001.h"
#include "ISteamMatchmaking007.h"
#include "ISteamNetworking004.h"
#include "ISteamRemoteStorage002.h"
#include "ISteamUtils005.h"
#include "ISteamUserStats007.h"

// Steam base code
#include "SteamBase.h"

// Generic utilities
#include "Utils.h"

// T5 structures
#include "T5.h"

// Hooking
#include "Hooking.h"

// more names
#define MP_BASE (1 << 16)
#define SP_BASE (2 << 16)
#define GET_GAME_VERSION(x) ((x) & 0xFFFF)

#define IS_VALID_GAME(x) GET_GAME_VERSION((x))
#define MP_VERSION(x) ((x) | MP_BASE)
#define SP_VERSION(x) ((x) | SP_BASE)

void Sys_RunInit(int gameVersion);

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

// placeholder
#define Com_Printf(...)
//#define Com_Error(...)