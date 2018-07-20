// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: common include file
//
// Initial author: NTAuthority
// Started: 2011-06-28
// ==========================================================

#pragma once

#define _CRT_SECURE_NO_WARNINGS

// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C/C++ headers
#include <string>
#include <vector>
#include <queue>

// code headers
#include "libnp.h"
#include "Utils.h"
#include "RPC.h"
#include "Friends.h"
#include "NPAsyncImpl.h"

// messages
#include "hello.pb.h"
#include "auth.pb.h"
#include "storage.pb.h"
#include "friends.pb.h"
#include "servers.pb.h"
#include "MessageDefinition.h"

// global storage
extern struct np_state_s
{
	// authentication service
	bool authenticated;
	NPID npID;
	char sessionToken[32];
	int groupID;

	// connection stuff
	char serverHost[1024];
	uint16_t serverPort;
} g_np;