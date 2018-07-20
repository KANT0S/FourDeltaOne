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

//#define MASTER_SERVER "77.21.192.163"
#define MASTER_SERVER "iw5.prod.fourdeltaone.net"

#define _CRT_SECURE_NO_WARNINGS

// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#include <strsafe.h>
#include <stdint.h>

// LibNP
#include <libnp.h>

// C/C++ headers
#include <map>
#include <vector>
#include <list>

// OSW headers
#define NO_STEAM // to reduce header amount needed
#include "CCallback.h"
#include "ISteamClient011.h"
#include "ISteamContentServer002.h"
#include "ISteamUser016.h"
#include "ISteamFriends009.h"
#include "ISteamApps004.h"
#include "ISteamGameServer010.h"
#include "ISteamMasterServerUpdater001.h"
#include "ISteamMatchmaking008.h"
#include "ISteamNetworking005.h"
#include "ISteamRemoteStorage002.h"
#include "ISteamUtils005.h"
#include "ISteamUserStats010.h"
#include "ISteamMatchmakingServers002.h"

// custom headers
#include "buildnumber.h"
#include "SteamBase.h"
#include "Utils.h"
#include "Hooking.h"
#include "IW5.h"

// blah
char* Auth_GetUsername();
char* Auth_GetObfuscatedUsername();
char* Auth_GetSessionID();
int Auth_GetUserID();
char* Auth_GetCountry();

extern bool g_isDedicated;