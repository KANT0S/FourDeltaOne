// ==========================================================
// T5M project
// 
// Component: client
// Sub-component: loopdw
// Purpose: Implementation of the bdServer subclasses.
//
// Initial author: NTAuthority
// Started: 2013-04-15
// ==========================================================

#include "StdInc.h"
#include "DW.h"

void bdMessageServer::handlePacket(std::string& packet)
{
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
			enqueuePacket(emptyPacket, sizeof(emptyPacket));

			break;
		}

		pos += 4;

		if (buf[pos] != 0xFF)
		{
			std::string message(&buf[pos], totalBytes);
			handleMessage(message);

			pos += totalBytes;
		}
	}
}

void bdRequestServer::handleMessage(std::string& message)
{
	const char* buf = message.c_str();
	int len = message.size();

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

	queuedPacket = false;

	// dispatch packet to a handler
	Trace("bdRequestServer", "type %d, encrypted %u", ptype, encrypted);

	std::string request(buf, len);
	handleRequest(ptype, request);

	// if we didn't handle the request, send back some garbage error
	if (!queuedPacket && ptype != 7)
	{
		dwMessage reply(this, 1, false);
		reply.byteBuffer.writeUInt64(0x8000000000000001);
		reply.byteBuffer.writeUInt32(2);
		reply.send(true);
	}
}

void bdServiceServer::handleRequest(uint8_t type, std::string& message)
{
	std::map<uint8_t, bdService*>::const_iterator i = services.find(type);

	if (i != services.end())
	{
		curService = i->second;
		i->second->callService(this, message);
	}
}

void bdServiceServer::registerService(bdService* service)
{
	services[service->getType()] = service;
}

std::shared_ptr<dwMessage> bdServiceServer::makeMessage(char replyType, bool isBit)
{
	return std::make_shared<dwMessage>(this, replyType, isBit);
}

std::shared_ptr<dwReply> bdServiceServer::makeReply(int errorCode)
{
	return std::make_shared<dwReply>(this, errorCode);
}