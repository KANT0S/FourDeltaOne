// ==========================================================
// alterIWnet project 
// 
// Component: xnp
// Sub-component: libnp
// Purpose: messaging methods
//
// Initial author: NTAuthority
// Started: 2012-07-11
// ==========================================================

#include "StdInc.h"

LIBNP_API void LIBNP_CALL NP_SendMessage(NPID npid, const uint8_t* data, uint32_t length)
{
	RPCMessagingSendDataMessage* request = new RPCMessagingSendDataMessage();
	request->GetBuffer()->set_npid(npid);
	request->GetBuffer()->set_data(data, length);

	RPC_SendMessage(request);

	request->Free();
}

static void (__cdecl * messageCB)(NPID, const uint8_t*, uint32_t);

LIBNP_API void LIBNP_CALL NP_RegisterMessageCallback(void (__cdecl * callback)(NPID, const uint8_t*, uint32_t))
{
	messageCB = callback;
}

static void Messaging_HandleMessage(INPRPCMessage* message)
{
	RPCMessagingSendDataMessage* msg = (RPCMessagingSendDataMessage*)message;

	if (messageCB)
	{
		std::string data = msg->GetBuffer()->data();
		messageCB(msg->GetBuffer()->npid(), (const uint8_t*)data.c_str(), data.size());
	}
}

void Messaging_Init()
{
	RPC_RegisterDispatch(RPCMessagingSendDataMessage::Type, Messaging_HandleMessage);
}