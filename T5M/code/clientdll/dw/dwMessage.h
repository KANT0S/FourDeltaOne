#pragma once

#include "bdBitBuffer.h"
#include "bdByteBuffer.h"

class bdServer;
class bdServiceServer;

#include <windows.h>

class dwMessage
{
private:
	bool _isBit;
	char _type;

	BYTE* _bytes;

	bdServer* server;

public:
	bdBitBuffer bitBuffer;
	bdByteBuffer byteBuffer;

	dwMessage(bdServer* server, char type, bool isBit);
	~dwMessage();

	void send(bool encrypted);
};

class bdResult
{
public:
	virtual void serialize(bdByteBuffer& buffer) = 0;
	virtual void deserialize(bdByteBuffer& buffer) = 0;
};

class dwReply
{
private:
	dwMessage* message;

	int errorCode;
	std::list<bdResult*> results;

	bdServiceServer* server;

public:
	dwReply(bdServiceServer* server, int errorCode);
	~dwReply();

	void addResult(bdResult* result);
	void send();
};