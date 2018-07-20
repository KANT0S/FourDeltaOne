#include "StdInc.h"
#include "DW.h"
#include "bdLSGServer.h"

void bdMessaging::sendGlobalInstantMessage(bdServiceServer* server, bdByteBuffer& data)
{
	NPID onlineID;
	char* msg; int msglen;

	data.readUInt64(&onlineID);
	data.readBlob(&msg, &msglen);

	uint8_t tempMsg[16384];
	tempMsg[0] = 1;
	memcpy(&tempMsg[1], msg, msglen);

	NP_SendMessage(onlineID, (uint8_t*)tempMsg, msglen + 1);

	Trace("bdMessaging", "sent instant message to %llx", onlineID);	

	auto reply = server->makeReply(0);
	reply->send();
}

void bdMessaging::sendInstantMessage(bdServiceServer* server, bdByteBuffer& data)
{
	NPID onlineID;
	char* msg; int msglen;

	data.readBlob(&msg, &msglen);
	data.readUInt64(&onlineID);

	uint8_t tempMsg[16384];
	tempMsg[0] = 2;
	memcpy(&tempMsg[1], msg, msglen);

	NP_SendMessage(onlineID, (uint8_t*)tempMsg, msglen + 1);

	Trace("bdMessaging", "sent im to %llx", onlineID);	
}

void bdMessaging::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
	case 14:
		sendGlobalInstantMessage(server, data);
		break;
	case 18:
		sendInstantMessage(server, data);
		break;
	}
}