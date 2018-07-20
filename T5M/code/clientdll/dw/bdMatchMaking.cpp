#include "StdInc.h"
#include "DW.h"
#include "bdLSGServer.h"

#include <time.h>

class MatchMakingInfo : public bdResult
{
public:
	std::string address;
	uint32_t unknown1;
	uint32_t unknown2;

	// only for serialization
	std::string sessionID;
	uint32_t players;

	// T5-specific
	int netcodeVersion;
	std::string unknownBlob2;
	char hostname[260];
	uint64_t unknownLong1;
	char mapname[260];
	int gametype;
	int playlist;
	int unknown6;
	int unknown7;
	int unknown8;
	uint32_t licenseType;
	int unknownA;
	int unknownB;
	int maxPlayers;
	int unknownD;
	uint64_t onlineID;
	int wager;
	int unknownF;
	char unknownString3[260];
	uint64_t unknownLong3;
	uint32_t statDDL;

	MatchMakingInfo()
	{
		unknown1 = 0;
		unknown2 = 0;

		players = 0;
	}

	virtual void deserialize(bdByteBuffer& buffer)
	{
		deserialize(buffer, false);
	}

	virtual void deserialize(bdByteBuffer& buffer, bool clientPart)
	{
		buffer.readBlob(address);

		if (clientPart)
		{
			buffer.readBlob(sessionID);
		}

		buffer.readUInt32(&unknown1);
		buffer.readUInt32(&unknown2);

		if (clientPart)
		{
			buffer.readUInt32(&players);
		}

		// T5
		buffer.readInt32(&netcodeVersion);
		buffer.readBlob(unknownBlob2);
		buffer.readString(hostname, sizeof(hostname));
		buffer.readUInt64(&unknownLong1);
		buffer.readString(mapname, sizeof(mapname));
		buffer.readInt32(&gametype);
		buffer.readInt32(&playlist);
		buffer.readInt32(&unknown6);
		buffer.readInt32(&unknown7);
		buffer.readInt32(&unknown8);
		buffer.readUInt32(&licenseType);
		buffer.readInt32(&unknownA);
		buffer.readInt32(&unknownB);
		buffer.readInt32(&maxPlayers);
		buffer.readInt32(&unknownD);
		buffer.readUInt64(&onlineID);
		buffer.readInt32(&wager);
		buffer.readInt32(&unknownF);
		buffer.readString(unknownString3, sizeof(unknownString3));
		buffer.readUInt64(&unknownLong3);
		buffer.readUInt32(&statDDL);
	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		serialize(buffer, false);
	}

	virtual void serialize(bdByteBuffer& buffer, bool clientPart)
	{
		buffer.writeBlob(address);

		if (!clientPart)
		{
			buffer.writeBlob(sessionID);
		}

		buffer.writeUInt32(unknown1);
		buffer.writeUInt32(unknown2);

		if (!clientPart)
		{
			buffer.writeUInt32(players);
		}

		// T5
		buffer.writeInt32(netcodeVersion);
		buffer.writeBlob(unknownBlob2);
		buffer.writeString(hostname);
		buffer.writeUInt64(unknownLong1);
		buffer.writeString(mapname);
		buffer.writeInt32(gametype);
		buffer.writeInt32(playlist);
		buffer.writeInt32(unknown6);
		buffer.writeInt32(unknown7);
		buffer.writeInt32(unknown8);
		buffer.writeUInt32(licenseType);
		buffer.writeInt32(unknownA);
		buffer.writeInt32(unknownB);
		buffer.writeInt32(maxPlayers);
		buffer.writeInt32(unknownD);
		buffer.writeUInt64(onlineID);
		buffer.writeInt32(wager);
		buffer.writeInt32(unknownF);
		buffer.writeString(unknownString3);
		buffer.writeUInt64(unknownLong3);
		buffer.writeUInt32(statDDL);
	}
};

class bdSessionId : public bdResult
{
public:
	uint64_t sessionID;

	bdSessionId(uint64_t sessionID)
		: sessionID(sessionID)
	{

	}

	virtual void deserialize(bdByteBuffer& buffer)
	{

	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeBlob((char*)&sessionID, 8);
	}
};

void bdMatchMaking::updateNPInfo(MatchMakingInfo& info)
{
	sinfo.address = 0;
	sinfo.port = 3074;
	sinfo.maxplayers = info.maxPlayers;
	sinfo.players = 0;

	NP_GetNPID(&sinfo.npid);

	// hack to set the online id properly
	info.onlineID = sinfo.npid;

	bdByteBuffer minfo;
	info.serialize(minfo);

	size_t outLen;
	char* base64 = base64_encode(minfo.getBytes(), minfo.getLength(), &outLen);

	char base64Data[4096];
	memcpy(base64Data, base64, outLen);
	base64Data[outLen] = 0;

	sinfo.data.Set("mmInfo", base64Data);
	sinfo.data.Set("statDDL", va("%d", info.statDDL));
	sinfo.data.Set("netcodeVersion", va("%d", info.netcodeVersion));
	sinfo.data.Set("serverType", va("%d", info.wager));
	free(base64);
}

void bdMatchMaking::createSession(bdServiceServer* server, bdByteBuffer& message)
{
	MatchMakingInfo info;
	info.deserialize(message);

	updateNPInfo(info);

	auto async = NP_CreateSession(&sinfo);
	NPCreateSessionResult* result = async->Wait(5000);

	if (result == nullptr)
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	npSID = result->sid;

	bdSessionId sessID(npSID);

	auto reply = server->makeReply(0);
	reply->addResult(&sessID);
	reply->send();
}

void bdMatchMaking::deleteSession(bdServiceServer* server, bdByteBuffer& message)
{
	std::string sid;
	message.readBlob(sid);

	NPSID npSID = *(NPSID*)sid.c_str();
	
	auto async = NP_DeleteSession(npSID);

	if (async->Wait(5000) == nullptr)
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	auto reply = server->makeReply(0);
	reply->send();
}

void bdMatchMaking::updateSession(bdServiceServer* server, bdByteBuffer& message)
{
	std::string sid;
	uint32_t players;
	MatchMakingInfo info;

	message.readBlob(sid);
	info.deserialize(message);

	updateNPInfo(info);

	auto async = NP_UpdateSession(npSID, &sinfo);

	if (async->Wait(5000) == nullptr)
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	bdSessionId sessID(npSID);

	auto reply = server->makeReply(0);
	reply->addResult(&sessID);
	reply->send();
}

void bdMatchMaking::updateSessionPlayers(bdServiceServer* server, bdByteBuffer& message)
{
	std::string sid;
	uint32_t players;
	MatchMakingInfo info;

	message.readBlob(sid);
	message.readUInt32(&players);
	info.deserialize(message);

	updateNPInfo(info);

	auto async = NP_UpdateSession(npSID, &sinfo);

	if (async->Wait(5000) == nullptr)
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	bdSessionId sessID(npSID);

	auto reply = server->makeReply(0);
	reply->addResult(&sessID);
	reply->send();
}

void bdMatchMaking::updateSessionsFromNP(bdServiceServer* server, NPDictionary& params)
{
	auto async = NP_RefreshSessions(params);

	if (async->Wait(5000) == nullptr)
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	auto reply = server->makeReply(0);

	MatchMakingInfo minfos[100];

	int numSessions = NP_GetNumSessions();
	for (int i = 0; i < numSessions; i++)
	{
		NPSessionInfo info;
		NP_GetSessionData(i, &info);

		std::string mmInfo = info.data.Get("mmInfo");
		size_t outSize;

		unsigned char* buffer = base64_decode(mmInfo.c_str(), mmInfo.size(), &outSize);

		bdByteBuffer infoBuffer((char*)buffer, outSize);

		std::string sessID((char*)&info.sid, sizeof(info.sid));

		minfos[i].deserialize(infoBuffer, true);
		minfos[i].sessionID = sessID;

		reply->addResult(&minfos[i]);

		free(buffer);
	}

	reply->send();
}

void bdMatchMaking::findSessionsPaged(bdServiceServer* server, bdByteBuffer& message)
{
	// base 'findSessionsPaged'
	uint32_t queryType;
	bool newPagingToken;
	std::string pagingToken;
	uint32_t resultsPerPage;

	message.readUInt32(&queryType);
	message.readBoolean(&newPagingToken);
	message.readBlob(pagingToken);
	message.readUInt32(&resultsPerPage);

	// geo query (type 6 in t5)
	uint32_t netcodeVersion;
	uint32_t unknown1;
	uint32_t serverType;
	uint32_t statDDL;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t country;

	message.readUInt32(&netcodeVersion);
	message.readDataType(0x14); // NaN?!
	message.readUInt32(&serverType);

	// rest is unknown and we don't care about for now

	// set up a NP query
	NPDictionary params;
	//params.Set("statDDL", va("%d", statDDL));
	params.Set("netcodeVersion", va("%d", netcodeVersion));
	params.Set("serverType", va("%d", serverType));

	updateSessionsFromNP(server, params);
}

void bdMatchMaking::inviteToSession(bdServiceServer* server, bdByteBuffer& data)
{
	NPID onlineID;
	std::string sid;
	std::string attachment;

	data.readBlob(sid);
	data.readBlob(attachment);

	while (data.readUInt64(&onlineID))
	{
		uint8_t tempMsg[4096];
		tempMsg[0] = 3;
		memcpy(&tempMsg[1], sid.c_str(), 8);
		memcpy(&tempMsg[9], attachment.c_str(), attachment.size());

		NP_SendMessage(onlineID, (uint8_t*)tempMsg, attachment.size() + 9);
	}

	auto reply = server->makeReply(0);
	reply->send();
}

void bdMatchMaking::findSessionFromID(bdServiceServer* server, bdByteBuffer& message)
{
	std::string sid;

	message.readBlob(sid);

	char sidStr[20];
	_snprintf(sidStr, sizeof(sidStr), "%016llx", *(NPSID*)sid.c_str());

	NPDictionary params;
	params.Set("sid", sidStr);

	updateSessionsFromNP(server, params);
}

void bdMatchMaking::findSessionsByEntityIDs(bdServiceServer* server, bdByteBuffer& data)
{
	std::vector<NPID> onlineIDs;
	NPID onlineID;

	while (data.readUInt64(&onlineID))
	{
		onlineIDs.push_back(onlineID);
	}

	std::string query = "[";
	bool first = true;

	for (auto& npID : onlineIDs)
	{
		query += va("%s\"%016llx\"", (first) ? "" : ", ", npID);

		first = false;
	}

	query += "]";

	NPDictionary params;
	params.Set("npid", query.c_str());

	updateSessionsFromNP(server, params);
}

void bdMatchMaking::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
		case 1:
			createSession(server, data);
			break;
		case 2:
			updateSession(server, data);
			break;
		case 3:
			deleteSession(server, data);
			break;
		case 4:
			findSessionFromID(server, data);
			break;
		case 8:
			inviteToSession(server, data);
			break;
		case 12:
			updateSessionPlayers(server, data);
			break;
		case 13:
			findSessionsPaged(server, data);
			break;
		case 14:
			findSessionsByEntityIDs(server, data);
			break;
		default:
			Trace("bdMatchMaking", "unhandled type %i", subType);
			break;
	}
}