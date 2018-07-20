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
#define NUM_RPC_MESSAGE_TYPES 37 

// message class definitions
class RPCHelloMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<HelloMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCHelloMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1000 };
	
	HelloMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCHelloMessage* Create();
};
class RPCAuthenticateWithKeyMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateWithKeyMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateWithKeyMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1001 };
	
	AuthenticateWithKeyMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateWithKeyMessage* Create();
};
class RPCAuthenticateWithDetailsMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateWithDetailsMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateWithDetailsMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1002 };
	
	AuthenticateWithDetailsMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateWithDetailsMessage* Create();
};
class RPCAuthenticateWithTokenMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateWithTokenMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateWithTokenMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1003 };
	
	AuthenticateWithTokenMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateWithTokenMessage* Create();
};
class RPCAuthenticateValidateTicketMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateValidateTicketMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateValidateTicketMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1004 };
	
	AuthenticateValidateTicketMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateValidateTicketMessage* Create();
};
class RPCAuthenticateKickUserMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateKickUserMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateKickUserMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1005 };
	
	AuthenticateKickUserMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateKickUserMessage* Create();
};
class RPCAuthenticateExternalStatusMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateExternalStatusMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateExternalStatusMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1006 };
	
	AuthenticateExternalStatusMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateExternalStatusMessage* Create();
};
class RPCAuthenticateResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1010 };
	
	AuthenticateResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateResultMessage* Create();
};
class RPCAuthenticateUserGroupMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateUserGroupMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateUserGroupMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1011 };
	
	AuthenticateUserGroupMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateUserGroupMessage* Create();
};
class RPCAuthenticateValidateTicketResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateValidateTicketResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateValidateTicketResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1012 };
	
	AuthenticateValidateTicketResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateValidateTicketResultMessage* Create();
};
class RPCAuthenticateRegisterServerMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateRegisterServerMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateRegisterServerMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1021 };
	
	AuthenticateRegisterServerMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateRegisterServerMessage* Create();
};
class RPCAuthenticateRegisterServerResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<AuthenticateRegisterServerResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCAuthenticateRegisterServerResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1022 };
	
	AuthenticateRegisterServerResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCAuthenticateRegisterServerResultMessage* Create();
};
class RPCStorageGetPublisherFileMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<StorageGetPublisherFileMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCStorageGetPublisherFileMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1101 };
	
	StorageGetPublisherFileMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCStorageGetPublisherFileMessage* Create();
};
class RPCStoragePublisherFileMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<StoragePublisherFileMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCStoragePublisherFileMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1111 };
	
	StoragePublisherFileMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCStoragePublisherFileMessage* Create();
};
class RPCStorageGetUserFileMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<StorageGetUserFileMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCStorageGetUserFileMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1102 };
	
	StorageGetUserFileMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCStorageGetUserFileMessage* Create();
};
class RPCStorageUserFileMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<StorageUserFileMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCStorageUserFileMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1112 };
	
	StorageUserFileMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCStorageUserFileMessage* Create();
};
class RPCStorageWriteUserFileMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<StorageWriteUserFileMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCStorageWriteUserFileMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1103 };
	
	StorageWriteUserFileMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCStorageWriteUserFileMessage* Create();
};
class RPCStorageWriteUserFileResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<StorageWriteUserFileResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCStorageWriteUserFileResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1113 };
	
	StorageWriteUserFileResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCStorageWriteUserFileResultMessage* Create();
};
class RPCStorageSendRandomStringMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<StorageSendRandomStringMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCStorageSendRandomStringMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1104 };
	
	StorageSendRandomStringMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCStorageSendRandomStringMessage* Create();
};
class RPCFriendsSetSteamIDMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsSetSteamIDMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsSetSteamIDMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1201 };
	
	FriendsSetSteamIDMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsSetSteamIDMessage* Create();
};
class RPCFriendsGetProfileDataMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsGetProfileDataMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsGetProfileDataMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1202 };
	
	FriendsGetProfileDataMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsGetProfileDataMessage* Create();
};
class RPCFriendsGetProfileDataResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsGetProfileDataResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsGetProfileDataResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1203 };
	
	FriendsGetProfileDataResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsGetProfileDataResultMessage* Create();
};
class RPCFriendsRosterMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsRosterMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsRosterMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1211 };
	
	FriendsRosterMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsRosterMessage* Create();
};
class RPCFriendsPresenceMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsPresenceMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsPresenceMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1212 };
	
	FriendsPresenceMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsPresenceMessage* Create();
};
class RPCFriendsSetPresenceMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsSetPresenceMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsSetPresenceMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1213 };
	
	FriendsSetPresenceMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsSetPresenceMessage* Create();
};
class RPCFriendsGetUserAvatarMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsGetUserAvatarMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsGetUserAvatarMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1214 };
	
	FriendsGetUserAvatarMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsGetUserAvatarMessage* Create();
};
class RPCFriendsGetUserAvatarResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<FriendsGetUserAvatarResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCFriendsGetUserAvatarResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1215 };
	
	FriendsGetUserAvatarResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCFriendsGetUserAvatarResultMessage* Create();
};
class RPCServersCreateSessionMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersCreateSessionMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersCreateSessionMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1301 };
	
	ServersCreateSessionMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersCreateSessionMessage* Create();
};
class RPCServersCreateSessionResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersCreateSessionResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersCreateSessionResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1302 };
	
	ServersCreateSessionResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersCreateSessionResultMessage* Create();
};
class RPCServersGetSessionsMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersGetSessionsMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersGetSessionsMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1303 };
	
	ServersGetSessionsMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersGetSessionsMessage* Create();
};
class RPCServersGetSessionsResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersGetSessionsResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersGetSessionsResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1304 };
	
	ServersGetSessionsResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersGetSessionsResultMessage* Create();
};
class RPCServersUpdateSessionMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersUpdateSessionMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersUpdateSessionMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1305 };
	
	ServersUpdateSessionMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersUpdateSessionMessage* Create();
};
class RPCServersUpdateSessionResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersUpdateSessionResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersUpdateSessionResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1306 };
	
	ServersUpdateSessionResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersUpdateSessionResultMessage* Create();
};
class RPCServersDeleteSessionMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersDeleteSessionMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersDeleteSessionMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1307 };
	
	ServersDeleteSessionMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersDeleteSessionMessage* Create();
};
class RPCServersDeleteSessionResultMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<ServersDeleteSessionResultMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCServersDeleteSessionResultMessage()
	{
		_payload = NULL;
	}

	enum { Type = 1308 };
	
	ServersDeleteSessionResultMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCServersDeleteSessionResultMessage* Create();
};
class RPCCloseAppMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<CloseAppMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCCloseAppMessage()
	{
		_payload = NULL;
	}

	enum { Type = 2001 };
	
	CloseAppMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCCloseAppMessage* Create();
};
class RPCMessagingSendDataMessage : public INPRPCMessage
{
private:
	NPRPCBuffer<MessagingSendDataMessage> _buffer;
	
	uint8_t* _payload;
public:
	RPCMessagingSendDataMessage()
	{
		_payload = NULL;
	}

	enum { Type = 2002 };
	
	MessagingSendDataMessage* GetBuffer();

	virtual void Deserialize(const uint8_t* message, size_t length);
	virtual uint8_t* Serialize(size_t* length, uint32_t id);
	
	virtual int GetType();
	
	virtual void Free();
	virtual void FreePayload();
	static RPCMessagingSendDataMessage* Create();
};
