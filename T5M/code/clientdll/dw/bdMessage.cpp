#include "StdInc.h"
#include "DW.h"

dwMessage::dwMessage(bdServer* server, char type, bool isBit)
{
	_type = type;
	_isBit = isBit;

	_bytes = new BYTE[131072];
	memset(_bytes, 0, 131072);

	this->server = server;

	if (isBit)
	{
		bitBuffer.init((char*)_bytes, 131072);
	}
	else
	{
		BYTE initData[5] = { 0xef, 0xbe, 0xad, 0xde, type };

		byteBuffer.unmanageBytes();
		byteBuffer.init((char*)_bytes, 131072);
		byteBuffer.write(5, initData);
	}
}

dwMessage::~dwMessage()
{
	delete[] _bytes;
}

void dwMessage::send(bool encrypted)
{
	BYTE* tempBytes = NULL;
	int tempLength = 0;
	int length = (_isBit) ? bitBuffer.getLength() : byteBuffer.getLength();

	if (!encrypted)
	{
		tempBytes = new BYTE[length + 6];
		tempLength = length + 6;

		*(int*)tempBytes = length + 2;
		tempBytes[4] = 0;
		tempBytes[5] = _type;

		memcpy(tempBytes + 6, _bytes, length);
	}
	else
	{
		if (length % 8)
		{
			length += (8 - (length % 8));
		}

		tempBytes = new BYTE[length + 9];
		tempLength = length + 9;

		*(int*)tempBytes = length + 5;
		tempBytes[4] = 1;
		*(int*)(tempBytes + 5) = 0x13371337;

		BYTE iv[24];
		BYTE key[24];

		dw_calculate_iv(0x13371337, iv);
		dw_get_global_key(key);

		dw_encrypt_data((char*)_bytes, iv, key, (char*)(tempBytes + 9), length);
	}

	//dw_queue_packet((char*)tempBytes, tempLength);
	server->enqueuePacket((char*)tempBytes, tempLength);

	delete[] tempBytes;
}

dwReply::dwReply(bdServiceServer* server, int errorCode)
{
	this->errorCode = errorCode;
	this->server = server;

	this->message = new dwMessage(server, 1, false);
}

dwReply::~dwReply()
{
	delete this->message;
}

void dwReply::addResult(bdResult* result)
{
	results.push_back(result);
}

void dwReply::send()
{
	message->byteBuffer.writeUInt64(0x8000000000000001);
	message->byteBuffer.writeUInt32(errorCode);

	if (!errorCode)
	{
		message->byteBuffer.writeByte(server->curService->getType());
		message->byteBuffer.writeUInt32(results.size());
		message->byteBuffer.writeUInt32(results.size());

		for (std::list<bdResult*>::const_iterator i = results.begin(); i != results.end(); i++)
		{
			(*i)->serialize(message->byteBuffer);
		}
	}

	message->send(true);
}