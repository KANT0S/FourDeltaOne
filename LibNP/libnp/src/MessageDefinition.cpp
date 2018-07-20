// This file has been automatically generated.

#include "StdInc.h"
#include "MessageDefinition.h"

HelloMessage* RPCHelloMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCHelloMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCHelloMessage::Serialize(size_t* length, uint32_t id)
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

int RPCHelloMessage::GetType()
{
	return 1000;
}

void RPCHelloMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCHelloMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCHelloMessage* RPCHelloMessage::Create()
{
	return new RPCHelloMessage();
}

AuthenticateWithKeyMessage* RPCAuthenticateWithKeyMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateWithKeyMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateWithKeyMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateWithKeyMessage::GetType()
{
	return 1001;
}

void RPCAuthenticateWithKeyMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateWithKeyMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateWithKeyMessage* RPCAuthenticateWithKeyMessage::Create()
{
	return new RPCAuthenticateWithKeyMessage();
}

AuthenticateWithDetailsMessage* RPCAuthenticateWithDetailsMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateWithDetailsMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateWithDetailsMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateWithDetailsMessage::GetType()
{
	return 1002;
}

void RPCAuthenticateWithDetailsMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateWithDetailsMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateWithDetailsMessage* RPCAuthenticateWithDetailsMessage::Create()
{
	return new RPCAuthenticateWithDetailsMessage();
}

AuthenticateWithTokenMessage* RPCAuthenticateWithTokenMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateWithTokenMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateWithTokenMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateWithTokenMessage::GetType()
{
	return 1003;
}

void RPCAuthenticateWithTokenMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateWithTokenMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateWithTokenMessage* RPCAuthenticateWithTokenMessage::Create()
{
	return new RPCAuthenticateWithTokenMessage();
}

AuthenticateValidateTicketMessage* RPCAuthenticateValidateTicketMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateValidateTicketMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateValidateTicketMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateValidateTicketMessage::GetType()
{
	return 1004;
}

void RPCAuthenticateValidateTicketMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateValidateTicketMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateValidateTicketMessage* RPCAuthenticateValidateTicketMessage::Create()
{
	return new RPCAuthenticateValidateTicketMessage();
}

AuthenticateKickUserMessage* RPCAuthenticateKickUserMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateKickUserMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateKickUserMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateKickUserMessage::GetType()
{
	return 1005;
}

void RPCAuthenticateKickUserMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateKickUserMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateKickUserMessage* RPCAuthenticateKickUserMessage::Create()
{
	return new RPCAuthenticateKickUserMessage();
}

AuthenticateExternalStatusMessage* RPCAuthenticateExternalStatusMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateExternalStatusMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateExternalStatusMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateExternalStatusMessage::GetType()
{
	return 1006;
}

void RPCAuthenticateExternalStatusMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateExternalStatusMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateExternalStatusMessage* RPCAuthenticateExternalStatusMessage::Create()
{
	return new RPCAuthenticateExternalStatusMessage();
}

AuthenticateResultMessage* RPCAuthenticateResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateResultMessage::GetType()
{
	return 1010;
}

void RPCAuthenticateResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateResultMessage* RPCAuthenticateResultMessage::Create()
{
	return new RPCAuthenticateResultMessage();
}

AuthenticateUserGroupMessage* RPCAuthenticateUserGroupMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateUserGroupMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateUserGroupMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateUserGroupMessage::GetType()
{
	return 1011;
}

void RPCAuthenticateUserGroupMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateUserGroupMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateUserGroupMessage* RPCAuthenticateUserGroupMessage::Create()
{
	return new RPCAuthenticateUserGroupMessage();
}

AuthenticateValidateTicketResultMessage* RPCAuthenticateValidateTicketResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateValidateTicketResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateValidateTicketResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateValidateTicketResultMessage::GetType()
{
	return 1012;
}

void RPCAuthenticateValidateTicketResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateValidateTicketResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateValidateTicketResultMessage* RPCAuthenticateValidateTicketResultMessage::Create()
{
	return new RPCAuthenticateValidateTicketResultMessage();
}

AuthenticateRegisterServerMessage* RPCAuthenticateRegisterServerMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateRegisterServerMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateRegisterServerMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateRegisterServerMessage::GetType()
{
	return 1021;
}

void RPCAuthenticateRegisterServerMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateRegisterServerMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateRegisterServerMessage* RPCAuthenticateRegisterServerMessage::Create()
{
	return new RPCAuthenticateRegisterServerMessage();
}

AuthenticateRegisterServerResultMessage* RPCAuthenticateRegisterServerResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCAuthenticateRegisterServerResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCAuthenticateRegisterServerResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCAuthenticateRegisterServerResultMessage::GetType()
{
	return 1022;
}

void RPCAuthenticateRegisterServerResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCAuthenticateRegisterServerResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCAuthenticateRegisterServerResultMessage* RPCAuthenticateRegisterServerResultMessage::Create()
{
	return new RPCAuthenticateRegisterServerResultMessage();
}

StorageGetPublisherFileMessage* RPCStorageGetPublisherFileMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCStorageGetPublisherFileMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCStorageGetPublisherFileMessage::Serialize(size_t* length, uint32_t id)
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

int RPCStorageGetPublisherFileMessage::GetType()
{
	return 1101;
}

void RPCStorageGetPublisherFileMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCStorageGetPublisherFileMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCStorageGetPublisherFileMessage* RPCStorageGetPublisherFileMessage::Create()
{
	return new RPCStorageGetPublisherFileMessage();
}

StoragePublisherFileMessage* RPCStoragePublisherFileMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCStoragePublisherFileMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCStoragePublisherFileMessage::Serialize(size_t* length, uint32_t id)
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

int RPCStoragePublisherFileMessage::GetType()
{
	return 1111;
}

void RPCStoragePublisherFileMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCStoragePublisherFileMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCStoragePublisherFileMessage* RPCStoragePublisherFileMessage::Create()
{
	return new RPCStoragePublisherFileMessage();
}

StorageGetUserFileMessage* RPCStorageGetUserFileMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCStorageGetUserFileMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCStorageGetUserFileMessage::Serialize(size_t* length, uint32_t id)
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

int RPCStorageGetUserFileMessage::GetType()
{
	return 1102;
}

void RPCStorageGetUserFileMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCStorageGetUserFileMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCStorageGetUserFileMessage* RPCStorageGetUserFileMessage::Create()
{
	return new RPCStorageGetUserFileMessage();
}

StorageUserFileMessage* RPCStorageUserFileMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCStorageUserFileMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCStorageUserFileMessage::Serialize(size_t* length, uint32_t id)
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

int RPCStorageUserFileMessage::GetType()
{
	return 1112;
}

void RPCStorageUserFileMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCStorageUserFileMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCStorageUserFileMessage* RPCStorageUserFileMessage::Create()
{
	return new RPCStorageUserFileMessage();
}

StorageWriteUserFileMessage* RPCStorageWriteUserFileMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCStorageWriteUserFileMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCStorageWriteUserFileMessage::Serialize(size_t* length, uint32_t id)
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

int RPCStorageWriteUserFileMessage::GetType()
{
	return 1103;
}

void RPCStorageWriteUserFileMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCStorageWriteUserFileMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCStorageWriteUserFileMessage* RPCStorageWriteUserFileMessage::Create()
{
	return new RPCStorageWriteUserFileMessage();
}

StorageWriteUserFileResultMessage* RPCStorageWriteUserFileResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCStorageWriteUserFileResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCStorageWriteUserFileResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCStorageWriteUserFileResultMessage::GetType()
{
	return 1113;
}

void RPCStorageWriteUserFileResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCStorageWriteUserFileResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCStorageWriteUserFileResultMessage* RPCStorageWriteUserFileResultMessage::Create()
{
	return new RPCStorageWriteUserFileResultMessage();
}

StorageSendRandomStringMessage* RPCStorageSendRandomStringMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCStorageSendRandomStringMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCStorageSendRandomStringMessage::Serialize(size_t* length, uint32_t id)
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

int RPCStorageSendRandomStringMessage::GetType()
{
	return 1104;
}

void RPCStorageSendRandomStringMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCStorageSendRandomStringMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCStorageSendRandomStringMessage* RPCStorageSendRandomStringMessage::Create()
{
	return new RPCStorageSendRandomStringMessage();
}

FriendsSetSteamIDMessage* RPCFriendsSetSteamIDMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsSetSteamIDMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsSetSteamIDMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsSetSteamIDMessage::GetType()
{
	return 1201;
}

void RPCFriendsSetSteamIDMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsSetSteamIDMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsSetSteamIDMessage* RPCFriendsSetSteamIDMessage::Create()
{
	return new RPCFriendsSetSteamIDMessage();
}

FriendsGetProfileDataMessage* RPCFriendsGetProfileDataMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsGetProfileDataMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsGetProfileDataMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsGetProfileDataMessage::GetType()
{
	return 1202;
}

void RPCFriendsGetProfileDataMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsGetProfileDataMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsGetProfileDataMessage* RPCFriendsGetProfileDataMessage::Create()
{
	return new RPCFriendsGetProfileDataMessage();
}

FriendsGetProfileDataResultMessage* RPCFriendsGetProfileDataResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsGetProfileDataResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsGetProfileDataResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsGetProfileDataResultMessage::GetType()
{
	return 1203;
}

void RPCFriendsGetProfileDataResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsGetProfileDataResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsGetProfileDataResultMessage* RPCFriendsGetProfileDataResultMessage::Create()
{
	return new RPCFriendsGetProfileDataResultMessage();
}

FriendsRosterMessage* RPCFriendsRosterMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsRosterMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsRosterMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsRosterMessage::GetType()
{
	return 1211;
}

void RPCFriendsRosterMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsRosterMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsRosterMessage* RPCFriendsRosterMessage::Create()
{
	return new RPCFriendsRosterMessage();
}

FriendsPresenceMessage* RPCFriendsPresenceMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsPresenceMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsPresenceMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsPresenceMessage::GetType()
{
	return 1212;
}

void RPCFriendsPresenceMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsPresenceMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsPresenceMessage* RPCFriendsPresenceMessage::Create()
{
	return new RPCFriendsPresenceMessage();
}

FriendsSetPresenceMessage* RPCFriendsSetPresenceMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsSetPresenceMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsSetPresenceMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsSetPresenceMessage::GetType()
{
	return 1213;
}

void RPCFriendsSetPresenceMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsSetPresenceMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsSetPresenceMessage* RPCFriendsSetPresenceMessage::Create()
{
	return new RPCFriendsSetPresenceMessage();
}

FriendsGetUserAvatarMessage* RPCFriendsGetUserAvatarMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsGetUserAvatarMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsGetUserAvatarMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsGetUserAvatarMessage::GetType()
{
	return 1214;
}

void RPCFriendsGetUserAvatarMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsGetUserAvatarMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsGetUserAvatarMessage* RPCFriendsGetUserAvatarMessage::Create()
{
	return new RPCFriendsGetUserAvatarMessage();
}

FriendsGetUserAvatarResultMessage* RPCFriendsGetUserAvatarResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCFriendsGetUserAvatarResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCFriendsGetUserAvatarResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCFriendsGetUserAvatarResultMessage::GetType()
{
	return 1215;
}

void RPCFriendsGetUserAvatarResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCFriendsGetUserAvatarResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCFriendsGetUserAvatarResultMessage* RPCFriendsGetUserAvatarResultMessage::Create()
{
	return new RPCFriendsGetUserAvatarResultMessage();
}

ServersCreateSessionMessage* RPCServersCreateSessionMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersCreateSessionMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersCreateSessionMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersCreateSessionMessage::GetType()
{
	return 1301;
}

void RPCServersCreateSessionMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersCreateSessionMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersCreateSessionMessage* RPCServersCreateSessionMessage::Create()
{
	return new RPCServersCreateSessionMessage();
}

ServersCreateSessionResultMessage* RPCServersCreateSessionResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersCreateSessionResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersCreateSessionResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersCreateSessionResultMessage::GetType()
{
	return 1302;
}

void RPCServersCreateSessionResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersCreateSessionResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersCreateSessionResultMessage* RPCServersCreateSessionResultMessage::Create()
{
	return new RPCServersCreateSessionResultMessage();
}

ServersGetSessionsMessage* RPCServersGetSessionsMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersGetSessionsMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersGetSessionsMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersGetSessionsMessage::GetType()
{
	return 1303;
}

void RPCServersGetSessionsMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersGetSessionsMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersGetSessionsMessage* RPCServersGetSessionsMessage::Create()
{
	return new RPCServersGetSessionsMessage();
}

ServersGetSessionsResultMessage* RPCServersGetSessionsResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersGetSessionsResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersGetSessionsResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersGetSessionsResultMessage::GetType()
{
	return 1304;
}

void RPCServersGetSessionsResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersGetSessionsResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersGetSessionsResultMessage* RPCServersGetSessionsResultMessage::Create()
{
	return new RPCServersGetSessionsResultMessage();
}

ServersUpdateSessionMessage* RPCServersUpdateSessionMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersUpdateSessionMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersUpdateSessionMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersUpdateSessionMessage::GetType()
{
	return 1305;
}

void RPCServersUpdateSessionMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersUpdateSessionMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersUpdateSessionMessage* RPCServersUpdateSessionMessage::Create()
{
	return new RPCServersUpdateSessionMessage();
}

ServersUpdateSessionResultMessage* RPCServersUpdateSessionResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersUpdateSessionResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersUpdateSessionResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersUpdateSessionResultMessage::GetType()
{
	return 1306;
}

void RPCServersUpdateSessionResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersUpdateSessionResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersUpdateSessionResultMessage* RPCServersUpdateSessionResultMessage::Create()
{
	return new RPCServersUpdateSessionResultMessage();
}

ServersDeleteSessionMessage* RPCServersDeleteSessionMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersDeleteSessionMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersDeleteSessionMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersDeleteSessionMessage::GetType()
{
	return 1307;
}

void RPCServersDeleteSessionMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersDeleteSessionMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersDeleteSessionMessage* RPCServersDeleteSessionMessage::Create()
{
	return new RPCServersDeleteSessionMessage();
}

ServersDeleteSessionResultMessage* RPCServersDeleteSessionResultMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCServersDeleteSessionResultMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCServersDeleteSessionResultMessage::Serialize(size_t* length, uint32_t id)
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

int RPCServersDeleteSessionResultMessage::GetType()
{
	return 1308;
}

void RPCServersDeleteSessionResultMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCServersDeleteSessionResultMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCServersDeleteSessionResultMessage* RPCServersDeleteSessionResultMessage::Create()
{
	return new RPCServersDeleteSessionResultMessage();
}

CloseAppMessage* RPCCloseAppMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCCloseAppMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCCloseAppMessage::Serialize(size_t* length, uint32_t id)
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

int RPCCloseAppMessage::GetType()
{
	return 2001;
}

void RPCCloseAppMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCCloseAppMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCCloseAppMessage* RPCCloseAppMessage::Create()
{
	return new RPCCloseAppMessage();
}

MessagingSendDataMessage* RPCMessagingSendDataMessage::GetBuffer()
{
	return _buffer.GetPayload();
}

void RPCMessagingSendDataMessage::Deserialize(const uint8_t* message, size_t length)
{
	_buffer.Deserialize(message, length);
}

uint8_t* RPCMessagingSendDataMessage::Serialize(size_t* length, uint32_t id)
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

int RPCMessagingSendDataMessage::GetType()
{
	return 2002;
}

void RPCMessagingSendDataMessage::Free()
{
	if (_payload != NULL)
	{
		delete[] _payload;
	}
	
	delete this;
}

void RPCMessagingSendDataMessage::FreePayload()
{
	if (_payload != NULL)
	{
		delete[] _payload;
		_payload = NULL;
	}
}

RPCMessagingSendDataMessage* RPCMessagingSendDataMessage::Create()
{
	return new RPCMessagingSendDataMessage();
}


rpc_message_type_s g_rpcMessageTypes[] = 
{
	{ 1000, (CreateMessageCB)&RPCHelloMessage::Create },
	{ 1001, (CreateMessageCB)&RPCAuthenticateWithKeyMessage::Create },
	{ 1002, (CreateMessageCB)&RPCAuthenticateWithDetailsMessage::Create },
	{ 1003, (CreateMessageCB)&RPCAuthenticateWithTokenMessage::Create },
	{ 1004, (CreateMessageCB)&RPCAuthenticateValidateTicketMessage::Create },
	{ 1005, (CreateMessageCB)&RPCAuthenticateKickUserMessage::Create },
	{ 1006, (CreateMessageCB)&RPCAuthenticateExternalStatusMessage::Create },
	{ 1010, (CreateMessageCB)&RPCAuthenticateResultMessage::Create },
	{ 1011, (CreateMessageCB)&RPCAuthenticateUserGroupMessage::Create },
	{ 1012, (CreateMessageCB)&RPCAuthenticateValidateTicketResultMessage::Create },
	{ 1021, (CreateMessageCB)&RPCAuthenticateRegisterServerMessage::Create },
	{ 1022, (CreateMessageCB)&RPCAuthenticateRegisterServerResultMessage::Create },
	{ 1101, (CreateMessageCB)&RPCStorageGetPublisherFileMessage::Create },
	{ 1111, (CreateMessageCB)&RPCStoragePublisherFileMessage::Create },
	{ 1102, (CreateMessageCB)&RPCStorageGetUserFileMessage::Create },
	{ 1112, (CreateMessageCB)&RPCStorageUserFileMessage::Create },
	{ 1103, (CreateMessageCB)&RPCStorageWriteUserFileMessage::Create },
	{ 1113, (CreateMessageCB)&RPCStorageWriteUserFileResultMessage::Create },
	{ 1104, (CreateMessageCB)&RPCStorageSendRandomStringMessage::Create },
	{ 1201, (CreateMessageCB)&RPCFriendsSetSteamIDMessage::Create },
	{ 1202, (CreateMessageCB)&RPCFriendsGetProfileDataMessage::Create },
	{ 1203, (CreateMessageCB)&RPCFriendsGetProfileDataResultMessage::Create },
	{ 1211, (CreateMessageCB)&RPCFriendsRosterMessage::Create },
	{ 1212, (CreateMessageCB)&RPCFriendsPresenceMessage::Create },
	{ 1213, (CreateMessageCB)&RPCFriendsSetPresenceMessage::Create },
	{ 1214, (CreateMessageCB)&RPCFriendsGetUserAvatarMessage::Create },
	{ 1215, (CreateMessageCB)&RPCFriendsGetUserAvatarResultMessage::Create },
	{ 1301, (CreateMessageCB)&RPCServersCreateSessionMessage::Create },
	{ 1302, (CreateMessageCB)&RPCServersCreateSessionResultMessage::Create },
	{ 1303, (CreateMessageCB)&RPCServersGetSessionsMessage::Create },
	{ 1304, (CreateMessageCB)&RPCServersGetSessionsResultMessage::Create },
	{ 1305, (CreateMessageCB)&RPCServersUpdateSessionMessage::Create },
	{ 1306, (CreateMessageCB)&RPCServersUpdateSessionResultMessage::Create },
	{ 1307, (CreateMessageCB)&RPCServersDeleteSessionMessage::Create },
	{ 1308, (CreateMessageCB)&RPCServersDeleteSessionResultMessage::Create },
	{ 2001, (CreateMessageCB)&RPCCloseAppMessage::Create },
	{ 2002, (CreateMessageCB)&RPCMessagingSendDataMessage::Create },
};