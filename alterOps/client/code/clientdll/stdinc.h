// ==========================================================
// alterOps project
// 
// Component: client
// Sub-component: clientdll
// Purpose: Standard includes for the code.
//
// Initial author: NTAuthority
// Started: 2010-11-21
// ==========================================================

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define MASTER_SERVER "server.secretschemes.net"
#define KEY_DISABLED

// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// libnp
#include <libnp.h>

// C/C++ headers
#include <map>
#include <vector>

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

// custom headers
#include "SteamBase.h"
#include "Utils.h"
#include "Hooking.h"
#include "T5.h"

// blah
char* Auth_GetUsername();
char* Auth_GetSessionID();
int Auth_GetUserID();
char* Auth_GetCountry();

#include "buildnumber.h"

static bool banned = false;

#define lINFO 0
#define lWARN 1
#define lERROR 2
#define lSAPI 3
#define lDEBUG 4

void Logger(unsigned int lvl, char* caller, char* logline, ...);
static bool DebugMode = false;

#define CI_ID_T5_PYTHON_SPH4X_ESP 3001
#define CI_ID_T5_DEBUGGER_PRESENT 3002
#define CI_ID_T5_NORECOIL 3003
#define CI_ID_T5_CROSSHAIR 3004
#define CI_ID_T5_DVARUNLOCKER 3005