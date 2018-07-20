// This file has been automatically generated.

using NPx;
using System;

namespace NPx
{
	public partial class RPCHelloMessage : NPRPCMessage<HelloMessage>
	{
		public override int Type
		{
			get
			{
				return 1000;
			}
		}
	}
	public partial class RPCAuthenticateWithKeyMessage : NPRPCMessage<AuthenticateWithKeyMessage>
	{
		public override int Type
		{
			get
			{
				return 1001;
			}
		}
	}
	public partial class RPCAuthenticateWithDetailsMessage : NPRPCMessage<AuthenticateWithDetailsMessage>
	{
		public override int Type
		{
			get
			{
				return 1002;
			}
		}
	}
	public partial class RPCAuthenticateWithTokenMessage : NPRPCMessage<AuthenticateWithTokenMessage>
	{
		public override int Type
		{
			get
			{
				return 1003;
			}
		}
	}
	public partial class RPCAuthenticateValidateTicketMessage : NPRPCMessage<AuthenticateValidateTicketMessage>
	{
		public override int Type
		{
			get
			{
				return 1004;
			}
		}
	}
	public partial class RPCAuthenticateKickUserMessage : NPRPCMessage<AuthenticateKickUserMessage>
	{
		public override int Type
		{
			get
			{
				return 1005;
			}
		}
	}
	public partial class RPCAuthenticateExternalStatusMessage : NPRPCMessage<AuthenticateExternalStatusMessage>
	{
		public override int Type
		{
			get
			{
				return 1006;
			}
		}
	}
	public partial class RPCAuthenticateResultMessage : NPRPCMessage<AuthenticateResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1010;
			}
		}
	}
	public partial class RPCAuthenticateUserGroupMessage : NPRPCMessage<AuthenticateUserGroupMessage>
	{
		public override int Type
		{
			get
			{
				return 1011;
			}
		}
	}
	public partial class RPCAuthenticateValidateTicketResultMessage : NPRPCMessage<AuthenticateValidateTicketResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1012;
			}
		}
	}
	public partial class RPCAuthenticateRegisterServerMessage : NPRPCMessage<AuthenticateRegisterServerMessage>
	{
		public override int Type
		{
			get
			{
				return 1021;
			}
		}
	}
	public partial class RPCAuthenticateRegisterServerResultMessage : NPRPCMessage<AuthenticateRegisterServerResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1022;
			}
		}
	}
	public partial class RPCStorageGetPublisherFileMessage : NPRPCMessage<StorageGetPublisherFileMessage>
	{
		public override int Type
		{
			get
			{
				return 1101;
			}
		}
	}
	public partial class RPCStoragePublisherFileMessage : NPRPCMessage<StoragePublisherFileMessage>
	{
		public override int Type
		{
			get
			{
				return 1111;
			}
		}
	}
	public partial class RPCStorageGetUserFileMessage : NPRPCMessage<StorageGetUserFileMessage>
	{
		public override int Type
		{
			get
			{
				return 1102;
			}
		}
	}
	public partial class RPCStorageUserFileMessage : NPRPCMessage<StorageUserFileMessage>
	{
		public override int Type
		{
			get
			{
				return 1112;
			}
		}
	}
	public partial class RPCStorageWriteUserFileMessage : NPRPCMessage<StorageWriteUserFileMessage>
	{
		public override int Type
		{
			get
			{
				return 1103;
			}
		}
	}
	public partial class RPCStorageWriteUserFileResultMessage : NPRPCMessage<StorageWriteUserFileResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1113;
			}
		}
	}
	public partial class RPCStorageSendRandomStringMessage : NPRPCMessage<StorageSendRandomStringMessage>
	{
		public override int Type
		{
			get
			{
				return 1104;
			}
		}
	}
	public partial class RPCFriendsSetSteamIDMessage : NPRPCMessage<FriendsSetSteamIDMessage>
	{
		public override int Type
		{
			get
			{
				return 1201;
			}
		}
	}
	public partial class RPCFriendsGetProfileDataMessage : NPRPCMessage<FriendsGetProfileDataMessage>
	{
		public override int Type
		{
			get
			{
				return 1202;
			}
		}
	}
	public partial class RPCFriendsGetProfileDataResultMessage : NPRPCMessage<FriendsGetProfileDataResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1203;
			}
		}
	}
	public partial class RPCFriendsRosterMessage : NPRPCMessage<FriendsRosterMessage>
	{
		public override int Type
		{
			get
			{
				return 1211;
			}
		}
	}
	public partial class RPCFriendsPresenceMessage : NPRPCMessage<FriendsPresenceMessage>
	{
		public override int Type
		{
			get
			{
				return 1212;
			}
		}
	}
	public partial class RPCFriendsSetPresenceMessage : NPRPCMessage<FriendsSetPresenceMessage>
	{
		public override int Type
		{
			get
			{
				return 1213;
			}
		}
	}
	public partial class RPCFriendsGetUserAvatarMessage : NPRPCMessage<FriendsGetUserAvatarMessage>
	{
		public override int Type
		{
			get
			{
				return 1214;
			}
		}
	}
	public partial class RPCFriendsGetUserAvatarResultMessage : NPRPCMessage<FriendsGetUserAvatarResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1215;
			}
		}
	}
	public partial class RPCServersCreateSessionMessage : NPRPCMessage<ServersCreateSessionMessage>
	{
		public override int Type
		{
			get
			{
				return 1301;
			}
		}
	}
	public partial class RPCServersCreateSessionResultMessage : NPRPCMessage<ServersCreateSessionResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1302;
			}
		}
	}
	public partial class RPCServersGetSessionsMessage : NPRPCMessage<ServersGetSessionsMessage>
	{
		public override int Type
		{
			get
			{
				return 1303;
			}
		}
	}
	public partial class RPCServersGetSessionsResultMessage : NPRPCMessage<ServersGetSessionsResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1304;
			}
		}
	}
	public partial class RPCServersUpdateSessionMessage : NPRPCMessage<ServersUpdateSessionMessage>
	{
		public override int Type
		{
			get
			{
				return 1305;
			}
		}
	}
	public partial class RPCServersUpdateSessionResultMessage : NPRPCMessage<ServersUpdateSessionResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1306;
			}
		}
	}
	public partial class RPCServersDeleteSessionMessage : NPRPCMessage<ServersDeleteSessionMessage>
	{
		public override int Type
		{
			get
			{
				return 1307;
			}
		}
	}
	public partial class RPCServersDeleteSessionResultMessage : NPRPCMessage<ServersDeleteSessionResultMessage>
	{
		public override int Type
		{
			get
			{
				return 1308;
			}
		}
	}
	public partial class RPCCloseAppMessage : NPRPCMessage<CloseAppMessage>
	{
		public override int Type
		{
			get
			{
				return 2001;
			}
		}
	}
	public partial class RPCMessagingSendDataMessage : NPRPCMessage<MessagingSendDataMessage>
	{
		public override int Type
		{
			get
			{
				return 2002;
			}
		}
	}

	public class NPMessageFactory
	{
		public static INPRPCMessage CreateMessage(int type)
		{
			switch (type)
			{
				case 1000:
					return new RPCHelloMessage();
				case 1001:
					return new RPCAuthenticateWithKeyMessage();
				case 1002:
					return new RPCAuthenticateWithDetailsMessage();
				case 1003:
					return new RPCAuthenticateWithTokenMessage();
				case 1004:
					return new RPCAuthenticateValidateTicketMessage();
				case 1005:
					return new RPCAuthenticateKickUserMessage();
				case 1006:
					return new RPCAuthenticateExternalStatusMessage();
				case 1010:
					return new RPCAuthenticateResultMessage();
				case 1011:
					return new RPCAuthenticateUserGroupMessage();
				case 1012:
					return new RPCAuthenticateValidateTicketResultMessage();
				case 1021:
					return new RPCAuthenticateRegisterServerMessage();
				case 1022:
					return new RPCAuthenticateRegisterServerResultMessage();
				case 1101:
					return new RPCStorageGetPublisherFileMessage();
				case 1111:
					return new RPCStoragePublisherFileMessage();
				case 1102:
					return new RPCStorageGetUserFileMessage();
				case 1112:
					return new RPCStorageUserFileMessage();
				case 1103:
					return new RPCStorageWriteUserFileMessage();
				case 1113:
					return new RPCStorageWriteUserFileResultMessage();
				case 1104:
					return new RPCStorageSendRandomStringMessage();
				case 1201:
					return new RPCFriendsSetSteamIDMessage();
				case 1202:
					return new RPCFriendsGetProfileDataMessage();
				case 1203:
					return new RPCFriendsGetProfileDataResultMessage();
				case 1211:
					return new RPCFriendsRosterMessage();
				case 1212:
					return new RPCFriendsPresenceMessage();
				case 1213:
					return new RPCFriendsSetPresenceMessage();
				case 1214:
					return new RPCFriendsGetUserAvatarMessage();
				case 1215:
					return new RPCFriendsGetUserAvatarResultMessage();
				case 1301:
					return new RPCServersCreateSessionMessage();
				case 1302:
					return new RPCServersCreateSessionResultMessage();
				case 1303:
					return new RPCServersGetSessionsMessage();
				case 1304:
					return new RPCServersGetSessionsResultMessage();
				case 1305:
					return new RPCServersUpdateSessionMessage();
				case 1306:
					return new RPCServersUpdateSessionResultMessage();
				case 1307:
					return new RPCServersDeleteSessionMessage();
				case 1308:
					return new RPCServersDeleteSessionResultMessage();
				case 2001:
					return new RPCCloseAppMessage();
				case 2002:
					return new RPCMessagingSendDataMessage();
			}
			
			throw new ArgumentException(string.Format("No message handler for type {0} is defined.", type));
		}
		
		public static INPRPCMessage CreateMessage(Type type)
		{
			switch (type.Name)
			{
				case "HelloMessage":
					return new RPCHelloMessage();
				case "AuthenticateWithKeyMessage":
					return new RPCAuthenticateWithKeyMessage();
				case "AuthenticateWithDetailsMessage":
					return new RPCAuthenticateWithDetailsMessage();
				case "AuthenticateWithTokenMessage":
					return new RPCAuthenticateWithTokenMessage();
				case "AuthenticateValidateTicketMessage":
					return new RPCAuthenticateValidateTicketMessage();
				case "AuthenticateKickUserMessage":
					return new RPCAuthenticateKickUserMessage();
				case "AuthenticateExternalStatusMessage":
					return new RPCAuthenticateExternalStatusMessage();
				case "AuthenticateResultMessage":
					return new RPCAuthenticateResultMessage();
				case "AuthenticateUserGroupMessage":
					return new RPCAuthenticateUserGroupMessage();
				case "AuthenticateValidateTicketResultMessage":
					return new RPCAuthenticateValidateTicketResultMessage();
				case "AuthenticateRegisterServerMessage":
					return new RPCAuthenticateRegisterServerMessage();
				case "AuthenticateRegisterServerResultMessage":
					return new RPCAuthenticateRegisterServerResultMessage();
				case "StorageGetPublisherFileMessage":
					return new RPCStorageGetPublisherFileMessage();
				case "StoragePublisherFileMessage":
					return new RPCStoragePublisherFileMessage();
				case "StorageGetUserFileMessage":
					return new RPCStorageGetUserFileMessage();
				case "StorageUserFileMessage":
					return new RPCStorageUserFileMessage();
				case "StorageWriteUserFileMessage":
					return new RPCStorageWriteUserFileMessage();
				case "StorageWriteUserFileResultMessage":
					return new RPCStorageWriteUserFileResultMessage();
				case "StorageSendRandomStringMessage":
					return new RPCStorageSendRandomStringMessage();
				case "FriendsSetSteamIDMessage":
					return new RPCFriendsSetSteamIDMessage();
				case "FriendsGetProfileDataMessage":
					return new RPCFriendsGetProfileDataMessage();
				case "FriendsGetProfileDataResultMessage":
					return new RPCFriendsGetProfileDataResultMessage();
				case "FriendsRosterMessage":
					return new RPCFriendsRosterMessage();
				case "FriendsPresenceMessage":
					return new RPCFriendsPresenceMessage();
				case "FriendsSetPresenceMessage":
					return new RPCFriendsSetPresenceMessage();
				case "FriendsGetUserAvatarMessage":
					return new RPCFriendsGetUserAvatarMessage();
				case "FriendsGetUserAvatarResultMessage":
					return new RPCFriendsGetUserAvatarResultMessage();
				case "ServersCreateSessionMessage":
					return new RPCServersCreateSessionMessage();
				case "ServersCreateSessionResultMessage":
					return new RPCServersCreateSessionResultMessage();
				case "ServersGetSessionsMessage":
					return new RPCServersGetSessionsMessage();
				case "ServersGetSessionsResultMessage":
					return new RPCServersGetSessionsResultMessage();
				case "ServersUpdateSessionMessage":
					return new RPCServersUpdateSessionMessage();
				case "ServersUpdateSessionResultMessage":
					return new RPCServersUpdateSessionResultMessage();
				case "ServersDeleteSessionMessage":
					return new RPCServersDeleteSessionMessage();
				case "ServersDeleteSessionResultMessage":
					return new RPCServersDeleteSessionResultMessage();
				case "CloseAppMessage":
					return new RPCCloseAppMessage();
				case "MessagingSendDataMessage":
					return new RPCMessagingSendDataMessage();
			}
			
			throw new ArgumentException(string.Format("No message handler for type {0} is defined.", type.Name));
		}
	}
}