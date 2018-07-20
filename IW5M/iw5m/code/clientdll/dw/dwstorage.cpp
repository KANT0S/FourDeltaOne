#include "StdInc.h"
#include "dw.h"
#include "dwMessage.h"

void dw_storage_get_publisher_file(bdByteBuffer& data)
{
	char filename[512];
	data.readString(filename, sizeof(filename));

	static uint8_t fileBuffer[131072];

	Trace("dwstorage", "fetching publisher file %s", filename);

	NPAsync<NPGetPublisherFileResult>* async = NP_GetPublisherFile(filename, fileBuffer, sizeof(fileBuffer));
	NPGetPublisherFileResult* result = async->Wait();

	Trace("dwstorage", "result %d, size %d", result->result, result->fileSize);

	if (result->result == GetFileResultOK)
	{
		dwMessage reply(1, false);
		reply.byteBuffer.writeUInt64(0x8000000000000001);
		reply.byteBuffer.writeUInt32(0);
		reply.byteBuffer.writeByte(7);
		reply.byteBuffer.writeUInt32(1);
		reply.byteBuffer.writeUInt32(1);
		reply.byteBuffer.writeBlob((char*)result->buffer, result->fileSize);
		reply.send(true);
	}
	else
	{
		dwMessage reply(1, false);
		reply.byteBuffer.writeUInt64(0x8000000000000001);
		reply.byteBuffer.writeUInt32(0x3E8);
		reply.send(true);
	}
}

void dw_storage_get_user_file(bdByteBuffer& data)
{
	char filename[512];
	data.readString(filename, sizeof(filename));

	static uint8_t fileBuffer[131072];

	Trace("dwstorage", "fetching user file %s", filename);

	NPID myNPID;
	NP_GetNPID(&myNPID);

	NPAsync<NPGetUserFileResult>* async = NP_GetUserFile(filename, myNPID, fileBuffer, sizeof(fileBuffer));
	NPGetUserFileResult* result = async->Wait();

	Trace("dwstorage", "result %d, size %d", result->result, result->fileSize);

	if (result->result == GetFileResultOK)
	{
		dwMessage reply(1, false);
		reply.byteBuffer.writeUInt64(0x8000000000000001);
		reply.byteBuffer.writeUInt32(0);
		reply.byteBuffer.writeByte(7);
		reply.byteBuffer.writeUInt32(1);
		reply.byteBuffer.writeUInt32(1);
		reply.byteBuffer.writeBlob((char*)result->buffer, result->fileSize);
		reply.send(true);
	}
	else if (result->result == GetFileResultNotFound)
	{
		dwMessage reply(1, false);
		reply.byteBuffer.writeUInt64(0x8000000000000001);
		reply.byteBuffer.writeUInt32(0x3E8);
		reply.send(true);
	}
	else
	{
		dwMessage reply(1, false);
		reply.byteBuffer.writeUInt64(0x8000000000000001);
		reply.byteBuffer.writeUInt32(2);
		reply.send(true);
	}
}

void dw_storage_upload_user_file(bdByteBuffer& data)
{
	char filename[512];
	bool stuff; char* filedata; int filelen;
	data.readString(filename, sizeof(filename));
	data.readBoolean(&stuff);
	data.readBlob(&filedata, &filelen);

	Trace("dwstorage", "writing user file %s", filename);

	NPID myNPID;
	NP_GetNPID(&myNPID);

	NPAsync<NPWriteUserFileResult>* async = NP_WriteUserFile(filename, myNPID, (uint8_t*)filedata, filelen);

	dwMessage reply(1, false);
	reply.byteBuffer.writeUInt64(0x8000000000000001);
	reply.byteBuffer.writeUInt32(0);
	reply.byteBuffer.writeByte(1);
	reply.byteBuffer.writeUInt32(0);
	reply.byteBuffer.writeUInt32(0);
	reply.send(true);
}

void dw_handle_storage_message(int type, const char* buf, int len)
{
	if (type == 10)
	{
		bdByteBuffer data((char*)buf, len);
		char subtype = 0;
		data.readByte(&subtype);

		switch (subtype)
		{
			case 1:
				dw_storage_upload_user_file(data);
				break;
			case 3:
				dw_storage_get_user_file(data);
				break;
			case 7:
				dw_storage_get_publisher_file(data);
				break;
			default:
				Trace("dwstorage", "call %i", subtype);
				break;
		}
	}
}