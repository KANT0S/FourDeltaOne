#include "StdInc.h"
#include "DW.h"
#include "bdLSGServer.h"

#include <time.h>

class bdFileResult : public bdResult
{
private:
	uint8_t* buffer;
	size_t size;
public:
	bdFileResult(uint8_t* b, size_t s)
		: buffer(b), size(s)
	{

	}

	virtual void deserialize(bdByteBuffer& buffer)
	{

	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeBlob((char*)this->buffer, size);
	}
};

void bdStorage::uploadUserFile(bdServiceServer* server, bdByteBuffer& data)
{
	char filename[512];
	bool stuff; char* filedata; int filelen;
	data.readString(filename, sizeof(filename));
	data.readBoolean(&stuff);
	data.readBlob(&filedata, &filelen);

	NPID npid = 0;
	data.readUInt64(&npid);

	if (!npid)
	{
		NP_GetNPID(&npid);
	}

	Trace("bdStorage", "writing user file %s", filename);

	//NPID myNPID;
	//NP_GetNPID(&myNPID);

	NPAsync<NPWriteUserFileResult>* async = NP_WriteUserFile(filename, npid, (uint8_t*)filedata, filelen);

	if (!async->Wait(5000))
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	auto reply = server->makeReply(0);
	reply->send();
}

void bdStorage::getPublisherFile(bdServiceServer* server, bdByteBuffer& data)
{
	char filename[512];
	data.readString(filename, sizeof(filename));

	static uint8_t fileBuffer[131072];

	Trace("dwstorage", "fetching publisher file %s", filename);

	NPAsync<NPGetPublisherFileResult>* async = NP_GetPublisherFile(filename, fileBuffer, sizeof(fileBuffer));
	NPGetPublisherFileResult* result = async->Wait(7500);

	if (!result)
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	Trace("dwstorage", "result %d, size %d", result->result, result->fileSize);

	if (result->result == GetFileResultOK)
	{
		bdFileResult fileResult(result->buffer, result->fileSize);

		auto reply = server->makeReply(0);
		reply->addResult(&fileResult);
		reply->send();
	}
	else
	{
		auto reply = server->makeReply(0x3E8);
		reply->send();
	}
}

void bdStorage::getUserFile(bdServiceServer* server, bdByteBuffer& data)
{
	char filename[512];
	data.readString(filename, sizeof(filename));

	NPID npid = 0;
	data.readUInt64(&npid);

	if (!npid)
	{
		NP_GetNPID(&npid);
	}

	static uint8_t fileBuffer[131072];

	Trace("dwstorage", "fetching user file %s", filename);

	NPAsync<NPGetUserFileResult>* async = NP_GetUserFile(filename, npid, fileBuffer, sizeof(fileBuffer));
	NPGetUserFileResult* result = async->Wait(5000);

	if (!result)
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	Trace("dwstorage", "result %d, size %d", result->result, result->fileSize);

	if (result->result == GetFileResultOK)
	{
		bdFileResult fileResult(result->buffer, result->fileSize);

		auto reply = server->makeReply(0);
		reply->addResult(&fileResult);
		reply->send();
	}
	else if (result->result == GetFileResultNotFound)
	{
		auto reply = server->makeReply(0x3E8);
		reply->send();
	}
	else
	{
		auto reply = server->makeReply(2);
		reply->send();
	}
}

void bdStorage::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
		case 1:
			uploadUserFile(server, data);
			break;
		case 3:
			getUserFile(server, data);
			break;
		case 7:
			getPublisherFile(server, data);
			break;
		default:
			Trace("bdStorage", "unknown call (%i)", subType);
			break;
	}
}