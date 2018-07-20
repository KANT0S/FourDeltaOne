// ==========================================================
// "alterMW3" project
// 
// Component: code
// Sub-component: clientdll
// Purpose: Master server heartbeat/querying code
//
// Initial author: NTAuthority
// Started: 2012-01-28
// ==========================================================

#include "StdInc.h"
#include "GSServer.h"
#include <WS2tcpip.h>

static struct  
{
	// game port
	int gamePort;

	// is initialized?
	bool initialized;
	bool masterActive;

	// next heartbeat at
	DWORD nextHeartbeatTime;

	// socket stuff
	SOCKET serverSocket;
	sockaddr_in from;
} g_svs;

bool GSServer_Init(int gamePort, int queryPort)
{
	if (!queryPort)
	{
		return true;
	}

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err)
	{
		return false;
	}

	g_svs.serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (g_svs.serverSocket == INVALID_SOCKET)
	{
		Trace("GSServer", "socket() failed - %d", WSAGetLastError());
		return false;
	}

	int initialQueryPort = queryPort;
	bool bindPassed = false;
	while (queryPort < (initialQueryPort + 10))
	{
		sockaddr_in bindAddr;
		memset(&bindAddr, 0, sizeof(bindAddr));
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_port = htons(queryPort);

		dvar_t** net_ip = (dvar_t**)0x59244C0;

		//bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bindAddr.sin_addr.s_addr = inet_addr((*net_ip)->current.string);

		if (bind(g_svs.serverSocket, (sockaddr*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR)
		{
			Trace("GSServer", "bind() failed - %d", WSAGetLastError());

			if (WSAGetLastError() == WSAEADDRINUSE)
			{
				queryPort++;
				continue;
			}

			return false;
		}

		bindPassed = true;
		break;
	}

	if (!bindPassed)
	{
		return false;
	}

	ULONG nonBlocking = 1;
	ioctlsocket(g_svs.serverSocket, FIONBIO, &nonBlocking);

	g_svs.gamePort = gamePort;

	g_svs.initialized = true;
	return true;
}

void GSServer_PrintOOBInternal(int socket, int a2, int a3, int a4, int a5, int a6, int a7, char* buffer, sockaddr_in* addr)
{
	static char tempOOBBuffer[4096];
	tempOOBBuffer[0] = -1;
	tempOOBBuffer[1] = -1;
	tempOOBBuffer[2] = -1;
	tempOOBBuffer[3] = -1;

	memcpy(&tempOOBBuffer[4], buffer, strlen(buffer));

	sendto(g_svs.serverSocket, tempOOBBuffer, strlen(buffer) + 4, 0, (sockaddr*)addr, sizeof(*addr));
}

void GSServer_PrintOOB(int socket, int a2, int a3, int a4, int a5, int a6, int a7, char* buffer)
{
	GSServer_PrintOOBInternal(socket, a2, a3, a4, a5, a6, a7, buffer, &g_svs.from);
}

typedef void (__cdecl * SV_ConnectionlessPacket_t)(netadr_t from);
SV_ConnectionlessPacket_t SV_ConnectionlessPacket_ = (SV_ConnectionlessPacket_t)0x4EBC30;

void SV_ConnectionlessPacket(msg_t* msg, netadr_t from)
{
	__asm
	{
		push edi
		mov edi, msg
	}

	SV_ConnectionlessPacket_(from);

	__asm
	{
		pop edi
	}
}

StompHook netOOBPrintHook;
DWORD netOOBPrintHookLoc = 0x4D2350;

void GSServer_HandleOOB(char* buffer, int length)
{
    // install hook to catch output
    netOOBPrintHook.initialize("", 5, (PBYTE)netOOBPrintHookLoc);
    netOOBPrintHook.installHook((void(*)())GSServer_PrintOOB, true, false);

	// create a msg_t
	msg_t msg;
	MSG_Init(&msg, buffer, length);
	msg.cursize = length;

	// handle the packet
	netadr_t adr;
	*(int*)&adr.ip = *(int*)&g_svs.from.sin_addr;
	adr.port = g_svs.from.sin_port;
	adr.type = NA_IP;
	SV_ConnectionlessPacket(&msg, adr);

	// release hook
	netOOBPrintHook.releaseHook(false);
}

void GSServer_PollSocket()
{
	char buf[2048];
	memset(buf, 0, sizeof(buf));

	sockaddr_in from;
	memset(&from, 0, sizeof(from));

	int fromlen = sizeof(from);
	int len = recvfrom(g_svs.serverSocket, buf, 2048, 0, (sockaddr*)&from, &fromlen);

	if (len == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		if (error != WSAEWOULDBLOCK)
		{
			Trace("GSServer", "recv() failed - %d", error);
		}

		return;
	}

	g_svs.from = from;

	if (*(int*)buf == -1)
	{
		GSServer_HandleOOB(buf, len);
	}
}

#define	HEARTBEAT_MSEC	120 * 1000
#define	HEARTBEAT_GAME	"IW5"

void GSServer_Heartbeat()
{
	// if not time yet, don't send anything
	if (GetTickCount() < g_svs.nextHeartbeatTime)
	{
		return;
	}

	g_svs.nextHeartbeatTime = GetTickCount() + HEARTBEAT_MSEC;

	static bool lookedUp;
	static sockaddr_in masterIP;

	if (!lookedUp)
	{
		hostent* host = gethostbyname(MASTER_SERVER);

		if (!host)
		{
			Trace("GSServer", "gethostbyname() failed - %d", WSAGetLastError());
			return;
		}

		masterIP.sin_family = AF_INET;
		masterIP.sin_addr.s_addr = *(ULONG*)host->h_addr_list[0];
		masterIP.sin_port = htons(27950);

		lookedUp = true;
	}

	char message[128];
	_snprintf(message, sizeof(message), "\xFF\xFF\xFF\xFFheartbeatExt " HEARTBEAT_GAME " %d", g_svs.gamePort);

	sendto(g_svs.serverSocket, message, strlen(message), 0, (sockaddr*)&masterIP, sizeof(masterIP));
}

void GSServer_RunFrame()
{
	if (!g_svs.initialized)
	{
		return;
	}

	GSServer_PollSocket();

	if (g_svs.masterActive)
	{
		GSServer_Heartbeat();
	}
}

void GSServer_SetHeartbeatActive(bool active)
{
	if (/*active || */g_svs.masterActive)
	{
		g_svs.nextHeartbeatTime = 0;
		GSServer_Heartbeat();

		g_svs.nextHeartbeatTime = 0;
		GSServer_Heartbeat();
	}

	if (active)
	{
		g_svs.nextHeartbeatTime = 0;
	}

	g_svs.masterActive = active;
}