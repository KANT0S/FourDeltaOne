#include "StdInc.h"
#include "dw.h"
#include "dwMessage.h"
#include <queue>

static bool queuedPacketHere = false;

static void dw_handle_message(const char* buf, int len)
{
	BYTE ptype = 0xFF;
	bool encrypted = false;

	// encrypted?
	if (buf[0] == 1)
	{
		// yes
		encrypted = true;
		buf++;
		len--;

		// get IV seed
		unsigned int ivSeed = *(unsigned int*)buf;
		buf += 4;
		len -= 4;

		// calculate IV and decrypt
		BYTE iv[24];
		BYTE key[24];

		static char data[256 * 1024];
		dw_calculate_iv(ivSeed, iv);
		dw_get_global_key(key);

		dw_decrypt_data(buf, iv, key, data, len);

		buf = data;

		// skip 'hash'
		buf += 4;
		len -= 4;
	}
	else
	{
		buf++;
		len--;
	}

	// read packet type
	ptype = buf[0];

	buf++;
	len--;

	// dispatch packet to a handler
	Trace("dwhandler", "type %d, encrypted %u", ptype, encrypted);

	queuedPacketHere = false;

	if (!encrypted)
	{
		if (ptype == 28 || ptype == 12 || ptype == 26)
		{
			dw_handle_auth_message(ptype, buf, len);
		}
		else if (ptype == 7)
		{
			dw_handle_lobby_message(ptype, buf, len);
		}
	}
	else
	{
		if (ptype == 10)
		{
			dw_handle_storage_message(ptype, buf, len);
		}
		else if (ptype == 12 || ptype == 27 || ptype == 6)
		{
			dw_handle_tutils_message(ptype, buf, len);
		}

		if (!queuedPacketHere)
		{
			dwMessage reply(1, false);
			reply.byteBuffer.writeUInt64(0x8000000000000001);
			reply.byteBuffer.writeUInt32(2);
			reply.send(true);
		}
	}
}

std::queue<std::string> incomingQueue;
CRITICAL_SECTION incomingCS;

void dw_handle_packet(const char* buf, int buflen)
{
	Trace("dwhandler", "got a %d byte packet...", buflen);

	EnterCriticalSection(&incomingCS);
	std::string packet(buf, buflen);
	incomingQueue.push(packet);
	LeaveCriticalSection(&incomingCS);
}


static DWORD WINAPI dw_thread(LPVOID param)
{
	while (true)
	{
		Sleep(1);

		if (incomingQueue.empty())
		{
			continue;
		}

		EnterCriticalSection(&incomingCS);
		std::string packet = incomingQueue.front();
		incomingQueue.pop();
		LeaveCriticalSection(&incomingCS);

		const char* buf = packet.c_str();
		int buflen = packet.size();

		int pos = 0;

		while (pos < buflen)
		{
			int origin = 0;

			int totalBytes = *(int*)(buf + pos);

			if (totalBytes == 0xC8)
			{
				break;
			}

			if (totalBytes <= 0)
			{
				char emptyPacket[4] = { 0, 0, 0, 0 };
				dw_queue_packet(emptyPacket, sizeof(emptyPacket));

				break;
			}

			pos += 4;

			if (buf[pos] != 0xFF)
			{
				dw_handle_message(buf + pos, totalBytes);

				pos += totalBytes;
			}
		}
	}

	return 0;
}

std::queue<char> packetQueue;

bool dw_packet_available()
{
	return !packetQueue.empty();
}

CRITICAL_SECTION packetCS;

int dw_dequeue_packet(char* buf, int len)
{
	if (!packetQueue.empty())
	{
		EnterCriticalSection(&packetCS);

		int toPop = min(packetQueue.size(), len);

		for (int i = 0; i < toPop; i++)
		{
			buf[i] = packetQueue.front();
			packetQueue.pop();
		}

		//Trace("dwhandler", "popped %d bytes", toPop);

		LeaveCriticalSection(&packetCS);

		return toPop;
	}

	return 0;
}

void dw_queue_packet(char* buf, int len)
{
	EnterCriticalSection(&packetCS);

	for (int i = 0; i < len; i++)
	{
		packetQueue.push(buf[i]);
	}

	//Trace("dwhandler", "pushed %d bytes", len);

	LeaveCriticalSection(&packetCS);

	queuedPacketHere = true;
}

void dw_build_lsg_ticket(char* buf, char* key)
{
	memset(buf, 0, 128);
	memcpy(buf, key, 24);
}

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

void dw_build_game_ticket(char* buf, char* key, int gameID)
{
	dw_game_ticket* ticket = (dw_game_ticket*)buf;
	memset(ticket, 0xA, sizeof(dw_game_ticket));

	ticket->signature[0] = 0xDE;
	ticket->signature[1] = 0xAD;
	ticket->signature[2] = 0xBD;
	ticket->signature[3] = 0xEF;

	ticket->licenseType = 0;
	ticket->gameID = gameID;
	ticket->userID = 0x1100001DEADC0DE;
	ticket->nick[0] = 'a';
	ticket->nick[1] = 0;

	memcpy(ticket->key, key, sizeof(ticket->key));
}

void dw_im_received(NPID from, const uint8_t* buf, uint32_t buflen)
{
	dwMessage msg(2, false);
	msg.byteBuffer.writeUInt32(21);
	msg.byteBuffer.writeUInt64(from);
	msg.byteBuffer.writeString("me");
	msg.byteBuffer.writeBlob((const char*)buf, buflen);
	msg.send(true);

	Trace("dwhandler", "received IM from %llx", from);
}

void dw_init()
{
	InitializeCriticalSection(&incomingCS);
	InitializeCriticalSection(&packetCS);

	CreateThread(0, 0, dw_thread, 0, 0, 0);

	NP_RegisterMessageCallback(dw_im_received);
}