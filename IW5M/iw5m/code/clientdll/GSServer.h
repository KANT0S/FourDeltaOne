// ==========================================================
// "alterMW3" project
// 
// Component: code
// Sub-component: clientdll
// Purpose: Master server heartbeat/querying code header
//
// Initial author: NTAuthority
// Started: 2012-01-28
// ==========================================================

#pragma once

bool GSServer_Init(int gamePort, int queryPort);
void GSServer_RunFrame();
void GSServer_SetHeartbeatActive(bool active);
void GSServer_PrintOOBInternal(int socket, int a2, int a3, int a4, int a5, int a6, int a7, char* buffer, sockaddr_in* addr);