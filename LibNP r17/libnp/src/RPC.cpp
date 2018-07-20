// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: network transport code; remote calls and 
//          notifications.
//
// Initial author: NTAuthority
// Started: 2011-06-28
// ==========================================================

#include "StdInc.h"
#include <WS2tcpip.h>
#include "RPCAsync.h"
#include "hello.pb.h"

// rpc state variables
struct rpc_dispatch_handler_s
{
	uint32_t type;
	uint32_t id;
	DispatchHandlerCB callback;
};

static struct rpc_state_s
{
	// client socket
	SOCKET socket;
	WSAEVENT hSocketEvent;

	// message parsing
	uint32_t totalBytes;
	uint32_t readBytes;
	uint32_t messageType;
	uint32_t messageID;
	uint8_t* messageBuffer;

	// connected flag
	bool connected;
	bool wasConnected;
	bool reconnecting;

	// message dispatching
	std::vector<rpc_dispatch_handler_s> dispatchHandlers;
	std::vector<NPRPCAsync*> freeNext;
	std::map<int, NPRPCAsync*> asyncHandlers;

	// thread management
	HANDLE hThread;
	HANDLE hShutdownEvent;

    // global message id
    LONG sendMessageID;
} g_rpc;

// code
void Authenticate_Reauthenticate();

static DWORD WINAPI RPC_HandleReconnect(LPVOID param)
{
	while (!g_rpc.connected)
	{
		Log_Print("Reconnecting to RPC...\n");

		Sleep(5000);

		RPC_Shutdown();
		RPC_Init();
		if (NP_Connect(g_np.serverHost, g_np.serverPort))
		{
			Authenticate_Reauthenticate();
		}
	}

	g_rpc.reconnecting = false;

	return 0;
}

void RPC_Reconnect()
{
	if (!g_rpc.wasConnected)
	{
		return;
	}

	if (g_rpc.reconnecting)
	{
		return;
	}

	g_rpc.connected = false;
	g_np.authenticated = false;
	g_rpc.reconnecting = true;
	CreateThread(NULL, 0, RPC_HandleReconnect, NULL, NULL, NULL);
}

int RPC_GenerateID()
{
    InterlockedIncrement(&g_rpc.sendMessageID);
    return g_rpc.sendMessageID;
}

static void RPC_DispatchMessage(INPRPCMessage* message)
{
	uint32_t type = message->GetType();

	Log_Print("Dispatching RPC message with ID %d and type %d.\n", g_rpc.messageID, type);

	for (std::vector<rpc_dispatch_handler_s>::iterator i = g_rpc.dispatchHandlers.begin(); i != g_rpc.dispatchHandlers.end(); i++)
	{
		if (i->type == type)
		{
			Log_Print("Dispatching RPC message to dispatch handler.\n");
			i->callback(message);
		}
	}

	if (g_rpc.messageID > 0 && g_rpc.asyncHandlers[g_rpc.messageID] == NULL)
	{
		Sleep(100);
	}

	if (g_rpc.asyncHandlers[g_rpc.messageID] != NULL)
	{
		Log_Print("Dispatching RPC message to async handler.\n");

		NPRPCAsync* async = g_rpc.asyncHandlers[g_rpc.messageID];
		async->SetResult(message);
		g_rpc.freeNext.push_back(async);
	}
}

static void RPC_HandleMessage()
{
	Log_Print("RPC_HandleMessage: type %d\n", g_rpc.messageType);

	for (int i = 0; i < NUM_RPC_MESSAGE_TYPES; i++)
	{
		if (g_rpcMessageTypes[i].type == g_rpc.messageType)
		{
			INPRPCMessage* message = g_rpcMessageTypes[i].handler();
			message->Deserialize(g_rpc.messageBuffer, g_rpc.totalBytes);
			RPC_DispatchMessage(message);
			message->Free();
		}
	}

	delete[] g_rpc.messageBuffer;
}

static int testLen;
static uint8_t testBuffer[2048];

static uint32_t facefeed = 0xFACEFEED;

// reads a message from the RPC socket
static int RPC_ReadMessage()
{
	uint8_t buffer[2048];
	int len = recv(g_rpc.socket, (char*)buffer, sizeof(buffer), 0);
	//int len = testLen;
	//memcpy(buffer, testBuffer, sizeof(buffer));

	if (len > 0)
	{
		uint8_t* origin = buffer;
		uint32_t read = len;

		while (read > 0)
		{
			// if we've not read any prior part of this packet before
			if (g_rpc.readBytes == 0)
			{
				rpc_message_header_s* message = (rpc_message_header_s*)origin;
				
				if (message->signature == 0xFACEFEED)
				{
					send(g_rpc.socket, (const char*)&facefeed, 4, 0);
					return false;
				}
				
				if (message->signature != 0xDEADC0DE)
				{
					Log_Print("Signature (0x%x) doesn't match\n", message->signature);
					return false;
				}

				// set up size/buffer
				g_rpc.totalBytes = message->length;
				g_rpc.readBytes = 0;

				g_rpc.messageBuffer = new uint8_t[message->length];
				g_rpc.messageType = message->type;
				g_rpc.messageID = message->id;

				memset(g_rpc.messageBuffer, 0, message->length);

				// skip the header
				origin = &origin[sizeof(rpc_message_header_s)];
				read -= sizeof(rpc_message_header_s);
			}

			int copyLen = min(read, (g_rpc.totalBytes - g_rpc.readBytes));
			memcpy(&g_rpc.messageBuffer[g_rpc.readBytes], origin, copyLen);
			g_rpc.readBytes += copyLen;
			read -= copyLen;
			origin += copyLen;

			if (g_rpc.readBytes >= g_rpc.totalBytes)
			{
				g_rpc.readBytes = 0;

				// handle the message here
				RPC_HandleMessage();
			}
		}

		return FD_WRITE;
	}

	if (len == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			return FD_CLOSE;
		}
	}

	if (len == 0)
	{
		return FD_CLOSE;
	}

	return 0;
}

/*static void RPC_SendMessage(int type, IRPCMessage* message)
{
	std::string str = message->Serialize();
	uint32_t datalen = str.length();
	uint32_t buflen = sizeof(rpc_message_header_s) + datalen;

	// allocate a response buffer and copy data to it
	uint8_t* buffer = new uint8_t[buflen];
	const char* data = str.c_str();
	memcpy(&buffer[sizeof(rpc_message_header_s)], data, datalen);

	// set the response header data
	rpc_message_header_s* header = (rpc_message_header_s*)buffer;
	header->signature = 0xDEADC0DE;
	header->length = datalen;
	header->type = type;

	// send to the socket
	send(g_rpc.socket, (const char*)buffer, buflen, 0);

	// free the buffer
	delete[] buffer;
}*/

#define RPC_EVENT_SHUTDOWN (WAIT_OBJECT_0)
#define RPC_EVENT_SOCKET (WAIT_OBJECT_0 + 1)

static uint32_t RPC_WaitForEvent()
{
	HANDLE waitHandles[] =
	{
		g_rpc.hShutdownEvent, // shutdown event should be first, should other events be signaled at the same time a shutdown occurs
		g_rpc.hSocketEvent
	};

	return WaitForMultipleObjects(2, waitHandles, FALSE, 0);
}

static uint32_t RPC_DetermineEvent()
{
	WSANETWORKEVENTS networkEvents;
	WSAEnumNetworkEvents(g_rpc.socket, g_rpc.hSocketEvent, &networkEvents);

	if (networkEvents.lNetworkEvents & FD_CLOSE)
	{
		return FD_CLOSE;
	}
	else if (networkEvents.lNetworkEvents & FD_READ)
	{
		return FD_READ;
	}

	return 0;
}

static bool RPC_RunThread()
{
	// wait for an event to occur
	/*uint32_t event = RPC_WaitForEvent();

	if (event == WAIT_TIMEOUT)
	{
		return true;
	}

	if (event == RPC_EVENT_SHUTDOWN)
	{
		// pass the shutdown event through
		Log_Print("Shutting down RPC thread\n");
		return true;
	}

	// socket events
	if (event == RPC_EVENT_SOCKET)
	{
		uint32_t eventType = RPC_DetermineEvent();

		if (eventType == FD_READ)
		{
			if (RPC_ReadMessage())
			{
				RPC_HandleMessage();
			}
		}

		if (eventType == FD_CLOSE)
		{
			RPC_Reconnect();
			return true;
		}
	}

	if (event == WAIT_FAILED)
	{
		Log_Print("WaitForMultipleObjects failed: 0x%x\n", GetLastError());
	}*/

	int result = 0;

	do
	{
		result = RPC_ReadMessage();

		if (result == FD_READ)
		{
			RPC_HandleMessage();
		}
	} while (result == FD_WRITE);

	if (result == FD_CLOSE)
	{
		RPC_Reconnect();
	}

	return true;
}

static void RPC_HandleHello(INPRPCMessage* message)
{
	RPCHelloMessage* hello = (RPCHelloMessage*)message;
	Log_Print("got %d %d %s %s\n", hello->GetBuffer()->number(), hello->GetBuffer()->number2(), hello->GetBuffer()->name().c_str(), hello->GetBuffer()->stuff().c_str());
}

static void RPC_HandleClose(INPRPCMessage* message)
{
	RPCCloseAppMessage* close = (RPCCloseAppMessage*)message;

	ExitProcess(0x76767676);
}

bool RPC_Init()
{
	/*testLen = 40;
	memcpy(testBuffer, "\xde\xc0\xad\xde\x04\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xaa\xaa\xaa\xaa"
					   "\xde\xc0\xad\xde\x08\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\xaa\xaa\xaa\xaa", 40);

	RPC_ReadMessage();

	testLen = 24;
	memcpy(testBuffer, "\xbb\xbb\xbb\xbb"
					   "\xde\xc0\xad\xde\x04\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xaa\xaa\xaa\xaa", 24);

	RPC_ReadMessage();

	ExitProcess(0);*/

	Log_Print("Initializing RPC\n");

	// startup Winsock
	WSADATA data;
	int result;

	if ((result = WSAStartup(MAKEWORD(2, 2), &data)))
	{
		Log_Print("Couldn't initialize Winsock (0x%x)\n", result);
		return false;
	}

	// create RPC socket
	g_rpc.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (g_rpc.socket == INVALID_SOCKET)
	{
		Log_Print("Couldn't create RPC socket\n");
		RPC_Shutdown();
		return false;
	}

	// create RPC socket event
	//g_rpc.hSocketEvent = WSACreateEvent();
	g_rpc.hShutdownEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// blah
	//RPC_RegisterDispatch(RPCHelloMessage::Type, RPC_HandleHello);
	RPC_RegisterDispatch(RPCCloseAppMessage::Type, RPC_HandleClose);

	return true;
}

void RPC_Shutdown()
{
	Log_Print("Shutting down RPC\n");

	if (g_rpc.hThread)
	{
		if (g_rpc.hShutdownEvent)
		{
			SetEvent(g_rpc.hShutdownEvent);
			WaitForSingleObject(g_rpc.hThread, INFINITE);
		}

		TerminateThread(g_rpc.hThread, 1);

		CloseHandle(g_rpc.hThread);
		CloseHandle(g_rpc.hShutdownEvent);

		g_rpc.hThread = NULL;
		g_rpc.hShutdownEvent = NULL;
	}

	if (g_rpc.socket)
	{
		closesocket(g_rpc.socket);
		g_rpc.socket = NULL;
	}

	if (g_rpc.hSocketEvent)
	{
		WSACloseEvent(g_rpc.hSocketEvent);
		g_rpc.hSocketEvent = NULL;
	}

	g_rpc.dispatchHandlers.clear();

	WSACleanup();
}

void RPC_RegisterDispatch(uint32_t type, DispatchHandlerCB callback)
{
	rpc_dispatch_handler_s handler;
	memset(&handler, 0, sizeof(handler));
	handler.callback = callback;
	handler.type = type;

	g_rpc.dispatchHandlers.push_back(handler);
}

void RPC_SendMessage(INPRPCMessage* message, int id)
{
	// serialize the message
	size_t len;
	uint8_t* data = message->Serialize(&len, id);

	// send to the socket
	send(g_rpc.socket, (const char*)data, len, 0);
	
	// free the data pointer
	message->FreePayload();

	// log it
	Log_Print("Sending RPC message with ID %d.\n", id);
}

void RPC_SendMessage(INPRPCMessage* message)
{
	RPC_SendMessage(message, 0);
}

NPAsync<INPRPCMessage>* RPC_SendMessageAsync(INPRPCMessage* message)
{
	int id = RPC_GenerateID();

	NPRPCAsync* async = new NPRPCAsync();
	g_rpc.asyncHandlers[id] = async;

	RPC_SendMessage(message, id);

	Log_Print("Sending async RPC message with ID %d.\n", id);

	return async;
}

void RPC_RunFrame()
{
	// poll the socket
	RPC_RunThread();

	// free to-free items
	for (std::vector<NPRPCAsync*>::iterator i = g_rpc.freeNext.begin(); i != g_rpc.freeNext.end(); i++)
	{
		(*i)->Free();
	}

	g_rpc.freeNext.clear();
}

LIBNP_API bool LIBNP_CALL NP_Connect(const char* serverAddr, uint16_t port)
{
	if (g_rpc.connected)
	{
		return true;
	}

	// store server name and port
	strncpy(g_np.serverHost, serverAddr, sizeof(g_np.serverHost));
	g_np.serverPort = port;

	// code to connect to some server
	hostent* hostEntity = gethostbyname(serverAddr);

	if (hostEntity == NULL)
	{
		Log_Print("Could not look up %s: %d.\n", serverAddr, WSAGetLastError());
		return false;
	}

	sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = *(ULONG*)hostEntity->h_addr_list[0];
	server.sin_port = htons(port);

	if (connect(g_rpc.socket, (sockaddr*)&server, sizeof(sockaddr)))
	{
		Log_Print("Connecting failed: %d\n", WSAGetLastError());
		return false;
	}

	//g_rpc.hSocketEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//WSAEventSelect(g_rpc.socket, g_rpc.hSocketEvent, FD_READ | FD_CLOSE);

	u_long nonBlocking = TRUE;
	ioctlsocket(g_rpc.socket, FIONBIO, &nonBlocking);

	BOOL noDelay = TRUE;
	setsockopt(g_rpc.socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelay, sizeof(BOOL));

	g_rpc.wasConnected = true;
	g_rpc.connected = true;

	return true;
}