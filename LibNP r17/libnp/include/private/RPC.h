// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: header file for RPC.cpp
//
// Initial author: NTAuthority
// Started: 2011-06-28
// ==========================================================

#pragma once

// message class
class IRPCBuffer
{
public:
	virtual void Deserialize(const uint8_t* buffer, size_t length) = 0;
	virtual std::string Serialize() = 0;
};

template <class T>
class NPRPCBuffer : public IRPCBuffer
{
private:
	T _buffer;
public:
	T* GetPayload()
	{
		return &_buffer;
	}

	virtual void Deserialize(const uint8_t* buffer, size_t length)
	{
		_buffer.ParseFromArray(buffer, length);
	}

	virtual std::string Serialize()
	{
		std::string str;
		_buffer.SerializeToString(&str);
		return str;
	}
};

// message header
struct rpc_message_header_s
{
	uint32_t signature;
	uint32_t length;
	uint32_t type;
    uint32_t id;
};

// dispatch handler callback
class INPRPCMessage;
typedef void (* DispatchHandlerCB)(INPRPCMessage*);

// initialize the RPC system
bool RPC_Init();

// shut down the RPC system
void RPC_Shutdown();

// register a dispatch handler
void RPC_RegisterDispatch(uint32_t type, DispatchHandlerCB callback);

// send a message
void RPC_SendMessage(INPRPCMessage* message);
NPAsync<INPRPCMessage>* RPC_SendMessageAsync(INPRPCMessage* message);

// increment and return a new sequence ID
int RPC_GenerateID();

// initialize authenticate service RPC components
void Authenticate_Init();

// initialize messaging service RPC components
void Messaging_Init();

// run RPC frame
void RPC_RunFrame();