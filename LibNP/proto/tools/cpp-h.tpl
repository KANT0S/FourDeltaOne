// This file has been automatically generated.

#pragma once

// RPC message base class
class INPRPCMessage
{
public:
	virtual void Deserialize(const uint8_t* message, size_t length) = 0;
	virtual uint8_t* Serialize(size_t* length, uint32_t id) = 0;
	
	virtual int GetType() = 0;
	
	virtual void Free() = 0;
	virtual void FreePayload() = 0;
};

// RPC message parsing callbacks
typedef INPRPCMessage* (* CreateMessageCB)();

struct rpc_message_type_s
{
	uint32_t type;
	CreateMessageCB handler;
};

extern rpc_message_type_s g_rpcMessageTypes[];
#define NUM_RPC_MESSAGE_TYPES <?php echo count($messages) ?> 

// message class definitions
<?php
foreach ($messages as $message)
{
?>
class <?php echo $message['name'] ?> : public INPRPCMessage
{
private:
	NPRPCBuffer<<?php echo $message['message'] ?>> _buffer;
	
	uint8_t* _payload;
public:
	<?php echo $message['name'] ?>()
	{
		_payload = NULL;
	}

	enum { Type = <?php echo intval($message['type']) ?> };
	
	<?php echo $message['message'] ?>* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static <?php echo $message['name'] ?>* Create();
};
<?php
}
?>