// ==========================================================
// T5M project
// 
// Component: client
// Sub-component: loopdw
// Purpose: LoopDW global header file
//
// Initial author: NTAuthority
// Started: 2013-04-14
// ==========================================================

#pragma once
#include <queue>
#include <stdint.h>
#include <WS2tcpip.h>
#include <memory>

#include "dwMessage.h"

// hardcoded keys
#define DEDI_KEY "Hard work, by these words guarded. Please don't steal. -- NTA"
#define DEDI_KEY_HASH "\x94\x04\x2F\x8F\xE3\xDF\x0B\x54\x0D\xFF\xBA\x18\x05\x4F\x8C\xBA\x49\x17\x0A\x78\x5F\x8B\xBA\x25"
#define DEDI_KEY_OB "\x7C\x52\x34\x27\x62\x45\x2E\x31\x2D\x1E\x64\x55\x4B\x15\x4D\x5B\x21\x37\x52\x63\x33\x5E\x31\x52\x4A\x65\x57\x33\x51\x41\x27\x55\x53\x1C\x12\x69\x2A\x51\x22\x4B\x56\x11\x52\x59\x28\x10\x36\x10\x46\x36\x24\x51\x2F\x1B\x66\x68\x1E\x61\x0F\x67\x00\x32\x44\x39\x33\x45\x4B\x32\x34\x39\x35\x37\x38\x33\x4B\x44\x4B\x4E\x39\x32\x39\x33\x39\x44\x4B"

#define GLOBAL_KEY "\x56\x2f\x27\x61\xdf\xf9\x51\xd4\x30\xa7\x6a\xfb\xe7\xa1\x32\x73\x1e\x3d\xca\xa6\x6e\xd8\xa2\x40"

// global DW functionality
class bdServer;

void dw_init();
void dw_register_server(bdServer* server);

void dw_calculate_iv(unsigned int seed, BYTE* iv);
void dw_get_global_key(BYTE* key);
void dw_set_global_key(const BYTE* key);
void dw_decrypt_data(const char* ctext, BYTE* iv, BYTE* key, char* ptext, int len);
void dw_encrypt_data(const char* ptext, BYTE* iv, BYTE* key, char* ctext, int len);

void dw_build_game_ticket(char* buf, char* key, int gameID);
void dw_build_lsg_ticket(char* buf, char* key);

// provided by a game module
struct dw_entry
{
	int (__stdcall * connect)(SOCKET, const sockaddr*, int);
	int (__stdcall * send)(SOCKET, const char*, int, int);
	int (__stdcall * sendto)(SOCKET, const char*, int, int, const sockaddr*, int);
	int (__stdcall * recv)(SOCKET, char*, int, int);
	int (__stdcall * recvfrom)(SOCKET, char*, int, int, sockaddr*, int*);
	int (__stdcall * select)(int, fd_set*, fd_set*, fd_set*, const timeval*);
	int (__stdcall * closesocket)(SOCKET);

	hostent* (__stdcall * gethostbyname)(const char*);
};

void dw_patch_game(dw_entry* entryPoints);

// DW 'service' class
class bdServiceServer;

class bdService
{
public:
	// get the service type
	virtual uint8_t getType() = 0;

	// call the service
	virtual void callService(bdServiceServer* server, std::string& message) = 0;
};

// DW 'server' class
class bdServer
{
private:
	std::queue<std::string> incomingQueue;
	std::queue<uint8_t> outgoingQueue;

	CRITICAL_SECTION critSec;
	CRITICAL_SECTION inCritSec;

protected:
	// if we queued a packet during this thread iteration
	// -> to send error messages back to prevent desync
	bool queuedPacket;

	void enqueuePacket(char* buf, int len);

	friend class dwMessage;

public:
	bdServer();

	// get the hostname
	virtual const char* getHostName() = 0;

	// handle a packet
	virtual void handlePacket(std::string& packet) = 0;

	// queue a received packet
	void deferredHandlePacket(std::string& packet);

	// is a packet available?
	bool isPacketAvailable();

	// process the queues
	void processQueues();

	// dequeue a packet
	int dequeuePacket(char* buf, int len);
};

class bdMessageServer : public bdServer
{
public:
	virtual void handlePacket(std::string& packet);

	virtual void handleMessage(std::string& message) = 0;
};

class bdRequestServer : public bdMessageServer
{
public:
	virtual void handleMessage(std::string& message);

	virtual void handleRequest(uint8_t type, std::string& message) = 0;
};

class bdServiceServer : public bdRequestServer
{
private:
	std::map<uint8_t, bdService*> services;

protected:
	bdService* curService;

	friend class dwReply;

public:
	void registerService(bdService* service);

	virtual void handleRequest(uint8_t type, std::string& message);

	std::shared_ptr<dwMessage> makeMessage(char replyType, bool isBit);
	std::shared_ptr<dwReply> makeReply(int errorCode);
};

#pragma pack(push, 1)
struct dw_game_ticket
{
	char signature[4];
	char licenseType;
	unsigned int gameID;
	char stuff[16];
	__int64 userID;
	char nick[64];
	char key[24];
	char stuff2[7];
};
#pragma pack(pop)