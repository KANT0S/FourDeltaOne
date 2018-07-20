// This file has been automatically generated.

#include "StdInc.h"
#include "MessageDefinition.h"

<?php
foreach ($messages as $message)
{
?>
<?php echo $message['message'] ?>* <?php echo $message['name'] ?>::GetBuffer()
{
	return _buffer.GetPayload();
}

void <?php echo $message['name'] ?>::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* <?php echo $message['name'] ?>::Serialize(size_t* length, uint32_t id)
{
	if (_payload)
	{
		return _payload;
	}

	std::string str = _buffer.Serialize();
	uint32_t type = this->GetType();
	uint32_t datalen = str.length();
	uint32_t buflen = sizeof(rpc_message_header_s) + datalen;

	// allocate a response buffer and copy data to it
	uint8_t* buffer = new uint8_t[buflen];
	const char* data = str.c_str();
	memcpy(&buffer[sizeof(rpc_message_header_s)], data, datalen);

	// set the response header data
	rpc_message_header_s* header = (rpc_message_header_s*)buffer;
	header->signature = 0xDEADC0DE;
	header->length = datalen;
	header->type = type;
	header->id = id;
	
	// set return stuff
	*length = buflen;
	_payload = buffer;
	
	return _payload;
}

int <?php echo $message['name'] ?>::GetType()
{
	return <?php echo intval($message['type']) ?>;
}

void <?php echo $message['name'] ?>::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void <?php echo $message['name'] ?>::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

<?php echo $message['name'] ?>* <?php echo $message['name'] ?>::Create()
{
	return new <?php echo $message['name'] ?>();
}

<?php
}
?>

rpc_message_type_s g_rpcMessageTypes[] = 
{
<?php
foreach ($messages as $message)
{
?>
	{ <?php echo $message['type'] ?>, (CreateMessageCB)&<?php echo $message['name'] ?>::Create },
<?php
}
?>
};