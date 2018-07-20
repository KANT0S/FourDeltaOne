#include "StdInc.h"
#include "DW.h"
#include "bdLSGServer.h"

class PublicProfileInfo : public bdResult
{
public:
	int profileInt;
	std::string profileData;
	NPID npID;

	PublicProfileInfo()
	{

	}

	virtual void deserialize(bdByteBuffer& buffer)
	{
		buffer.readInt32(&profileInt);
		buffer.readBlob(profileData);
	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeUInt64(npID);

		buffer.writeInt32(profileInt);
		buffer.writeBlob(profileData);
	}
};

void bdProfiles::getPublicInfos(bdServiceServer* server, bdByteBuffer& message)
{
	std::vector<NPID> npIDs;
	NPID npID;

	while (message.readUInt64(&npID))
	{
		npIDs.push_back(npID);
	}

	NPExtProfileData* outData = new NPExtProfileData[npIDs.size()];
	auto async = NP_GetExtProfileData(npIDs.size(), "t5pub", &npIDs[0], outData);

	auto result = async->Wait(5000);

	if (!result)
	{
		auto reply = server->makeReply(2);
		reply->send();

		delete[] outData;

		return;
	}

	auto reply = server->makeReply(0);
	static PublicProfileInfo infos[300];
	
	for (int i = 0; i < result->numResults; i++)
	{
		NPExtProfileData* row = &result->results[i];

		infos[i].npID = row->npID;		
		infos[i].profileInt = *(int*)(row->buffer);
		infos[i].profileData = std::string(&row->buffer[4], row->bufLength - 4);

		reply->addResult(&infos[i]);
	}

	reply->send();

	delete[] outData;
}

void bdProfiles::setPublicInfo(bdServiceServer* server, bdByteBuffer& message)
{
	PublicProfileInfo info;
	info.deserialize(message);

	static char buf[65535];
	*(int*)buf = info.profileInt;
	memcpy(&buf[4], info.profileData.c_str(), info.profileData.size());

	NPID npID;
	NP_GetNPID(&npID);

	auto async = NP_WriteUserFile("profilet5pub", npID, (uint8_t*)buf, info.profileData.size() + 4);

	if (!async->Wait(5000))
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	auto reply = server->makeReply(0);
	reply->send();
}

void bdProfiles::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
	case 1:
		getPublicInfos(server, data);
		break;
	case 3:
		setPublicInfo(server, data);
		break;
	default:
		Trace("bdProfiles", "unhandled type %i", subType);
		break;
	}
}