#include "StdInc.h"
#include "DW.h"
#include "bdLSGServer.h"

#include <time.h>

class bdTimeResult : public bdResult
{
private:
	time_t time;
public:
	bdTimeResult(time_t time)
		: time(time)
	{

	}

	virtual void deserialize(bdByteBuffer& buffer)
	{

	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeUInt32(time);
	}
};

void bdTitleUtilities::getServerTime(bdServiceServer* server)
{
	bdTimeResult timeResult(time(NULL));

	auto reply = server->makeReply(0);
	reply->addResult(&timeResult);
	reply->send();
}

void bdTitleUtilities::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
		case 6:
			getServerTime(server);
			break;
	}
}