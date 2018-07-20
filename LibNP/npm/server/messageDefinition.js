// This file has been automatically generated.
var schema = new (require('protobuf').Schema)((require('fs').readFileSync(__dirname + '/proto.desc')));
var server = require('./server').NPServer;

var parsers =
{
	1000: function(data)
	{
		return schema.HelloMessage.parse(data);
	},

	1001: function(data)
	{
		return schema.AuthenticateWithKeyMessage.parse(data);
	},

	1002: function(data)
	{
		return schema.AuthenticateWithDetailsMessage.parse(data);
	},

	1003: function(data)
	{
		return schema.AuthenticateWithTokenMessage.parse(data);
	},

	1004: function(data)
	{
		return schema.AuthenticateValidateTicketMessage.parse(data);
	},

	1005: function(data)
	{
		return schema.AuthenticateKickUserMessage.parse(data);
	},

	1006: function(data)
	{
		return schema.AuthenticateExternalStatusMessage.parse(data);
	},

	1010: function(data)
	{
		return schema.AuthenticateResultMessage.parse(data);
	},

	1011: function(data)
	{
		return schema.AuthenticateUserGroupMessage.parse(data);
	},

	1012: function(data)
	{
		return schema.AuthenticateValidateTicketResultMessage.parse(data);
	},

	1021: function(data)
	{
		return schema.AuthenticateRegisterServerMessage.parse(data);
	},

	1022: function(data)
	{
		return schema.AuthenticateRegisterServerResultMessage.parse(data);
	},

	1101: function(data)
	{
		return schema.StorageGetPublisherFileMessage.parse(data);
	},

	1111: function(data)
	{
		return schema.StoragePublisherFileMessage.parse(data);
	},

	1102: function(data)
	{
		return schema.StorageGetUserFileMessage.parse(data);
	},

	1112: function(data)
	{
		return schema.StorageUserFileMessage.parse(data);
	},

	1103: function(data)
	{
		return schema.StorageWriteUserFileMessage.parse(data);
	},

	1113: function(data)
	{
		return schema.StorageWriteUserFileResultMessage.parse(data);
	},

	1104: function(data)
	{
		return schema.StorageSendRandomStringMessage.parse(data);
	},

	1201: function(data)
	{
		return schema.FriendsSetSteamIDMessage.parse(data);
	},

	1202: function(data)
	{
		return schema.FriendsGetProfileDataMessage.parse(data);
	},

	1203: function(data)
	{
		return schema.FriendsGetProfileDataResultMessage.parse(data);
	},

	1211: function(data)
	{
		return schema.FriendsRosterMessage.parse(data);
	},

	1212: function(data)
	{
		return schema.FriendsPresenceMessage.parse(data);
	},

	1213: function(data)
	{
		return schema.FriendsSetPresenceMessage.parse(data);
	},

	1214: function(data)
	{
		return schema.FriendsGetUserAvatarMessage.parse(data);
	},

	1215: function(data)
	{
		return schema.FriendsGetUserAvatarResultMessage.parse(data);
	},

	1301: function(data)
	{
		return schema.ServersCreateSessionMessage.parse(data);
	},

	1302: function(data)
	{
		return schema.ServersCreateSessionResultMessage.parse(data);
	},

	1303: function(data)
	{
		return schema.ServersGetSessionsMessage.parse(data);
	},

	1304: function(data)
	{
		return schema.ServersGetSessionsResultMessage.parse(data);
	},

	1305: function(data)
	{
		return schema.ServersUpdateSessionMessage.parse(data);
	},

	1306: function(data)
	{
		return schema.ServersUpdateSessionResultMessage.parse(data);
	},

	1307: function(data)
	{
		return schema.ServersDeleteSessionMessage.parse(data);
	},

	1308: function(data)
	{
		return schema.ServersDeleteSessionResultMessage.parse(data);
	},

	2001: function(data)
	{
		return schema.CloseAppMessage.parse(data);
	},

	2002: function(data)
	{
		return schema.MessagingSendDataMessage.parse(data);
	},

};

var serializers =
{
	1000: function(data)
	{
		return schema.HelloMessage.serialize(data);
	},

	1001: function(data)
	{
		return schema.AuthenticateWithKeyMessage.serialize(data);
	},

	1002: function(data)
	{
		return schema.AuthenticateWithDetailsMessage.serialize(data);
	},

	1003: function(data)
	{
		return schema.AuthenticateWithTokenMessage.serialize(data);
	},

	1004: function(data)
	{
		return schema.AuthenticateValidateTicketMessage.serialize(data);
	},

	1005: function(data)
	{
		return schema.AuthenticateKickUserMessage.serialize(data);
	},

	1006: function(data)
	{
		return schema.AuthenticateExternalStatusMessage.serialize(data);
	},

	1010: function(data)
	{
		return schema.AuthenticateResultMessage.serialize(data);
	},

	1011: function(data)
	{
		return schema.AuthenticateUserGroupMessage.serialize(data);
	},

	1012: function(data)
	{
		return schema.AuthenticateValidateTicketResultMessage.serialize(data);
	},

	1021: function(data)
	{
		return schema.AuthenticateRegisterServerMessage.serialize(data);
	},

	1022: function(data)
	{
		return schema.AuthenticateRegisterServerResultMessage.serialize(data);
	},

	1101: function(data)
	{
		return schema.StorageGetPublisherFileMessage.serialize(data);
	},

	1111: function(data)
	{
		return schema.StoragePublisherFileMessage.serialize(data);
	},

	1102: function(data)
	{
		return schema.StorageGetUserFileMessage.serialize(data);
	},

	1112: function(data)
	{
		return schema.StorageUserFileMessage.serialize(data);
	},

	1103: function(data)
	{
		return schema.StorageWriteUserFileMessage.serialize(data);
	},

	1113: function(data)
	{
		return schema.StorageWriteUserFileResultMessage.serialize(data);
	},

	1104: function(data)
	{
		return schema.StorageSendRandomStringMessage.serialize(data);
	},

	1201: function(data)
	{
		return schema.FriendsSetSteamIDMessage.serialize(data);
	},

	1202: function(data)
	{
		return schema.FriendsGetProfileDataMessage.serialize(data);
	},

	1203: function(data)
	{
		return schema.FriendsGetProfileDataResultMessage.serialize(data);
	},

	1211: function(data)
	{
		return schema.FriendsRosterMessage.serialize(data);
	},

	1212: function(data)
	{
		return schema.FriendsPresenceMessage.serialize(data);
	},

	1213: function(data)
	{
		return schema.FriendsSetPresenceMessage.serialize(data);
	},

	1214: function(data)
	{
		return schema.FriendsGetUserAvatarMessage.serialize(data);
	},

	1215: function(data)
	{
		return schema.FriendsGetUserAvatarResultMessage.serialize(data);
	},

	1301: function(data)
	{
		return schema.ServersCreateSessionMessage.serialize(data);
	},

	1302: function(data)
	{
		return schema.ServersCreateSessionResultMessage.serialize(data);
	},

	1303: function(data)
	{
		return schema.ServersGetSessionsMessage.serialize(data);
	},

	1304: function(data)
	{
		return schema.ServersGetSessionsResultMessage.serialize(data);
	},

	1305: function(data)
	{
		return schema.ServersUpdateSessionMessage.serialize(data);
	},

	1306: function(data)
	{
		return schema.ServersUpdateSessionResultMessage.serialize(data);
	},

	1307: function(data)
	{
		return schema.ServersDeleteSessionMessage.serialize(data);
	},

	1308: function(data)
	{
		return schema.ServersDeleteSessionResultMessage.serialize(data);
	},

	2001: function(data)
	{
		return schema.CloseAppMessage.serialize(data);
	},

	2002: function(data)
	{
		return schema.MessagingSendDataMessage.serialize(data);
	},

};

var ids =
{
	RPCHelloMessage: 1000,
	RPCAuthenticateWithKeyMessage: 1001,
	RPCAuthenticateWithDetailsMessage: 1002,
	RPCAuthenticateWithTokenMessage: 1003,
	RPCAuthenticateValidateTicketMessage: 1004,
	RPCAuthenticateKickUserMessage: 1005,
	RPCAuthenticateExternalStatusMessage: 1006,
	RPCAuthenticateResultMessage: 1010,
	RPCAuthenticateUserGroupMessage: 1011,
	RPCAuthenticateValidateTicketResultMessage: 1012,
	RPCAuthenticateRegisterServerMessage: 1021,
	RPCAuthenticateRegisterServerResultMessage: 1022,
	RPCStorageGetPublisherFileMessage: 1101,
	RPCStoragePublisherFileMessage: 1111,
	RPCStorageGetUserFileMessage: 1102,
	RPCStorageUserFileMessage: 1112,
	RPCStorageWriteUserFileMessage: 1103,
	RPCStorageWriteUserFileResultMessage: 1113,
	RPCStorageSendRandomStringMessage: 1104,
	RPCFriendsSetSteamIDMessage: 1201,
	RPCFriendsGetProfileDataMessage: 1202,
	RPCFriendsGetProfileDataResultMessage: 1203,
	RPCFriendsRosterMessage: 1211,
	RPCFriendsPresenceMessage: 1212,
	RPCFriendsSetPresenceMessage: 1213,
	RPCFriendsGetUserAvatarMessage: 1214,
	RPCFriendsGetUserAvatarResultMessage: 1215,
	RPCServersCreateSessionMessage: 1301,
	RPCServersCreateSessionResultMessage: 1302,
	RPCServersGetSessionsMessage: 1303,
	RPCServersGetSessionsResultMessage: 1304,
	RPCServersUpdateSessionMessage: 1305,
	RPCServersUpdateSessionResultMessage: 1306,
	RPCServersDeleteSessionMessage: 1307,
	RPCServersDeleteSessionResultMessage: 1308,
	RPCCloseAppMessage: 2001,
	RPCMessagingSendDataMessage: 2002,
};

server.handlers[1000] = function(id, data)
{
	require('./handlers/RPCHelloMessage').call(this, data, id);
};

server.handlers[1001] = function(id, data)
{
	require('./handlers/RPCAuthenticateWithKeyMessage').call(this, data, id);
};

server.handlers[1002] = function(id, data)
{
	require('./handlers/RPCAuthenticateWithDetailsMessage').call(this, data, id);
};

server.handlers[1003] = function(id, data)
{
	require('./handlers/RPCAuthenticateWithTokenMessage').call(this, data, id);
};

server.handlers[1004] = function(id, data)
{
	require('./handlers/RPCAuthenticateValidateTicketMessage').call(this, data, id);
};

server.handlers[1005] = function(id, data)
{
	require('./handlers/RPCAuthenticateKickUserMessage').call(this, data, id);
};

server.handlers[1006] = function(id, data)
{
	require('./handlers/RPCAuthenticateExternalStatusMessage').call(this, data, id);
};

server.handlers[1010] = function(id, data)
{
	require('./handlers/RPCAuthenticateResultMessage').call(this, data, id);
};

server.handlers[1011] = function(id, data)
{
	require('./handlers/RPCAuthenticateUserGroupMessage').call(this, data, id);
};

server.handlers[1012] = function(id, data)
{
	require('./handlers/RPCAuthenticateValidateTicketResultMessage').call(this, data, id);
};

server.handlers[1021] = function(id, data)
{
	require('./handlers/RPCAuthenticateRegisterServerMessage').call(this, data, id);
};

server.handlers[1022] = function(id, data)
{
	require('./handlers/RPCAuthenticateRegisterServerResultMessage').call(this, data, id);
};

server.handlers[1101] = function(id, data)
{
	require('./handlers/RPCStorageGetPublisherFileMessage').call(this, data, id);
};

server.handlers[1111] = function(id, data)
{
	require('./handlers/RPCStoragePublisherFileMessage').call(this, data, id);
};

server.handlers[1102] = function(id, data)
{
	require('./handlers/RPCStorageGetUserFileMessage').call(this, data, id);
};

server.handlers[1112] = function(id, data)
{
	require('./handlers/RPCStorageUserFileMessage').call(this, data, id);
};

server.handlers[1103] = function(id, data)
{
	require('./handlers/RPCStorageWriteUserFileMessage').call(this, data, id);
};

server.handlers[1113] = function(id, data)
{
	require('./handlers/RPCStorageWriteUserFileResultMessage').call(this, data, id);
};

server.handlers[1104] = function(id, data)
{
	require('./handlers/RPCStorageSendRandomStringMessage').call(this, data, id);
};

server.handlers[1201] = function(id, data)
{
	require('./handlers/RPCFriendsSetSteamIDMessage').call(this, data, id);
};

server.handlers[1202] = function(id, data)
{
	require('./handlers/RPCFriendsGetProfileDataMessage').call(this, data, id);
};

server.handlers[1203] = function(id, data)
{
	require('./handlers/RPCFriendsGetProfileDataResultMessage').call(this, data, id);
};

server.handlers[1211] = function(id, data)
{
	require('./handlers/RPCFriendsRosterMessage').call(this, data, id);
};

server.handlers[1212] = function(id, data)
{
	require('./handlers/RPCFriendsPresenceMessage').call(this, data, id);
};

server.handlers[1213] = function(id, data)
{
	require('./handlers/RPCFriendsSetPresenceMessage').call(this, data, id);
};

server.handlers[1214] = function(id, data)
{
	require('./handlers/RPCFriendsGetUserAvatarMessage').call(this, data, id);
};

server.handlers[1215] = function(id, data)
{
	require('./handlers/RPCFriendsGetUserAvatarResultMessage').call(this, data, id);
};

server.handlers[1301] = function(id, data)
{
	require('./handlers/RPCServersCreateSessionMessage').call(this, data, id);
};

server.handlers[1302] = function(id, data)
{
	require('./handlers/RPCServersCreateSessionResultMessage').call(this, data, id);
};

server.handlers[1303] = function(id, data)
{
	require('./handlers/RPCServersGetSessionsMessage').call(this, data, id);
};

server.handlers[1304] = function(id, data)
{
	require('./handlers/RPCServersGetSessionsResultMessage').call(this, data, id);
};

server.handlers[1305] = function(id, data)
{
	require('./handlers/RPCServersUpdateSessionMessage').call(this, data, id);
};

server.handlers[1306] = function(id, data)
{
	require('./handlers/RPCServersUpdateSessionResultMessage').call(this, data, id);
};

server.handlers[1307] = function(id, data)
{
	require('./handlers/RPCServersDeleteSessionMessage').call(this, data, id);
};

server.handlers[1308] = function(id, data)
{
	require('./handlers/RPCServersDeleteSessionResultMessage').call(this, data, id);
};

server.handlers[2001] = function(id, data)
{
	require('./handlers/RPCCloseAppMessage').call(this, data, id);
};

server.handlers[2002] = function(id, data)
{
	require('./handlers/RPCMessagingSendDataMessage').call(this, data, id);
};


exports.parsers = parsers;
exports.serializers = serializers;
exports.ids = ids;
exports.schema = schema;