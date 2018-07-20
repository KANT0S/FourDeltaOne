// ==========================================================
// T5M project
// 
// Component: client
// Sub-component: loopdw
// Purpose: Implementation of the bdServer base class.
//
// Initial author: NTAuthority
// Started: 2013-04-15
// ==========================================================

#include "StdInc.h"
#include "DW.h"

bdServer::bdServer()
{
	InitializeCriticalSection(&critSec);
	InitializeCriticalSection(&inCritSec);
}

void bdServer::enqueuePacket(char* buf, int len)
{
	EnterCriticalSection(&critSec);

	for (int i = 0; i < len; i++)
	{
		outgoingQueue.push(buf[i]);
	}

	LeaveCriticalSection(&critSec);

	queuedPacket = true;
}

int bdServer::dequeuePacket(char* buf, int len)
{
	if (!outgoingQueue.empty())
	{
		EnterCriticalSection(&critSec);

		int toPop = min(outgoingQueue.size(), len);

		for (int i = 0; i < toPop; i++)
		{
			buf[i] = outgoingQueue.front();
			outgoingQueue.pop();
		}

		LeaveCriticalSection(&critSec);

		return toPop;
	}

	return 0;
}

bool bdServer::isPacketAvailable()
{
	return !outgoingQueue.empty();
}

void bdServer::processQueues()
{
	if (incomingQueue.empty())
	{
		return;
	}

	EnterCriticalSection(&inCritSec);
	std::string packet = incomingQueue.front();
	incomingQueue.pop();
	LeaveCriticalSection(&inCritSec);

	handlePacket(packet);
}

void bdServer::deferredHandlePacket(std::string& packet)
{
	EnterCriticalSection(&inCritSec);

	incomingQueue.push(packet);

	LeaveCriticalSection(&inCritSec);
}