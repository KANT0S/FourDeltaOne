// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: authentication methods
//
// Initial author: NTAuthority
// Started: 2011-07-16
// ==========================================================

#include "StdInc.h"

enum EAuthenticationType
{
	AuthenticationTypeDetails,
	AuthenticationTypeLicenseKey,
	AuthenticationTypeToken
};

static struct  
{
	// state for reconnection
	EAuthenticationType authType;

	char username[1024];
	char password[1024];
	char licenseKey[1024];
	char authToken[1024];
} g_auth;

static void AuthenticateCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<NPAuthenticateResult>* asyncResult = (NPAsyncImpl<NPAuthenticateResult>*)async->GetUserData();
	RPCAuthenticateResultMessage* result = (RPCAuthenticateResultMessage*)async->GetResult();

	// handle result object
	NPAuthenticateResult* authResult = new NPAuthenticateResult();
	authResult->id = result->GetBuffer()->npid();
	authResult->result = (EAuthenticateResult)result->GetBuffer()->result();

	if (result->GetBuffer()->sessiontoken().length() == sizeof(authResult->sessionToken))
	{
		memcpy(authResult->sessionToken, result->GetBuffer()->sessiontoken().c_str(), sizeof(authResult->sessionToken));
	}

	// handle internal authentication
	if (authResult->result == AuthenticateResultOK)
	{
		g_np.npID = authResult->id;
		g_np.authenticated = true;
		memcpy(g_np.sessionToken, authResult->sessionToken, sizeof(authResult->sessionToken));

		// TODO: make some 'event handler' system
		if (g_auth.authType != AuthenticationTypeLicenseKey)
		{
			Friends_Connect();	
		}
	}
	else
	{
		g_np.npID = 0;
		g_np.authenticated = false;
	}

	// set the auth result
	asyncResult->SetResult(authResult);
}

LIBNP_API NPAsync<NPAuthenticateResult>* LIBNP_CALL NP_AuthenticateWithToken(const char* authToken)
{
	RPCAuthenticateWithTokenMessage* request = new RPCAuthenticateWithTokenMessage();
	request->GetBuffer()->set_token(authToken);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPAuthenticateResult>* result = new NPAsyncImpl<NPAuthenticateResult>();
	async->SetCallback(AuthenticateCB, result);

	request->Free();

	// set auth type
	g_auth.authType = AuthenticationTypeToken;
	strncpy(g_auth.authToken, authToken, sizeof(g_auth.authToken));

	return result;
}

LIBNP_API NPAsync<NPAuthenticateResult>* LIBNP_CALL NP_AuthenticateWithLicenseKey(const char* licenseKey)
{
	RPCAuthenticateWithKeyMessage* request = new RPCAuthenticateWithKeyMessage();
	request->GetBuffer()->set_licensekey(licenseKey);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPAuthenticateResult>* result = new NPAsyncImpl<NPAuthenticateResult>();
	async->SetCallback(AuthenticateCB, result);

	request->Free();

	// set auth type
	g_auth.authType = AuthenticationTypeLicenseKey;
	strncpy(g_auth.licenseKey, licenseKey, sizeof(g_auth.licenseKey));

	return result;
}

LIBNP_API NPAsync<NPAuthenticateResult>* LIBNP_CALL NP_AuthenticateWithDetails(const char* username, const char* password)
{
	RPCAuthenticateWithDetailsMessage* request = new RPCAuthenticateWithDetailsMessage();
	request->GetBuffer()->set_username(username);
	request->GetBuffer()->set_password(password);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPAuthenticateResult>* result = new NPAsyncImpl<NPAuthenticateResult>();
	async->SetCallback(AuthenticateCB, result);

	request->Free();

	// set auth type
	g_auth.authType = AuthenticationTypeDetails;
	strncpy(g_auth.username, username, sizeof(g_auth.username));
	strncpy(g_auth.password, password, sizeof(g_auth.password));

	return result;
}

static void RegisterServerCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<NPRegisterServerResult>* asyncResult = (NPAsyncImpl<NPRegisterServerResult>*)async->GetUserData();
	RPCAuthenticateRegisterServerResultMessage* result = (RPCAuthenticateRegisterServerResultMessage*)async->GetResult();

	// handle result object
	std::string licenseKey = result->GetBuffer()->licensekey();

	NPRegisterServerResult* authResult = new NPRegisterServerResult();
	authResult->result = (EAuthenticateResult)result->GetBuffer()->result();
	authResult->serverID = result->GetBuffer()->serverid();
	strcpy_s(authResult->licenseKey, licenseKey.c_str());

	// set the async result
	asyncResult->SetResult(authResult);
}

LIBNP_API NPAsync<NPRegisterServerResult>* LIBNP_CALL NP_RegisterServer(const char* configPath)
{
	RPCAuthenticateRegisterServerMessage* request = new RPCAuthenticateRegisterServerMessage();
	request->GetBuffer()->set_configpath(configPath);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPRegisterServerResult>* result = new NPAsyncImpl<NPRegisterServerResult>();
	async->SetCallback(RegisterServerCB, result);

	request->Free();

	return result;
}

static void ValidateUserTicketCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<NPValidateUserTicketResult>* asyncResult = (NPAsyncImpl<NPValidateUserTicketResult>*)async->GetUserData();
	RPCAuthenticateValidateTicketResultMessage* result = (RPCAuthenticateValidateTicketResultMessage*)async->GetResult();

	// handle result object
	NPValidateUserTicketResult* validateResult = new NPValidateUserTicketResult();
	validateResult->id = result->GetBuffer()->npid();
	validateResult->result = (EValidateUserTicketResult)result->GetBuffer()->result();
	validateResult->groupID = result->GetBuffer()->groupid();

	// set the auth result
	asyncResult->SetResult(validateResult);
}

LIBNP_API NPAsync<NPValidateUserTicketResult>* LIBNP_CALL NP_ValidateUserTicket(const void* ticket, size_t ticketSize, uint32_t clientIP, NPID clientID)
{
	std::string ticketString((const char*)ticket, ticketSize);

	RPCAuthenticateValidateTicketMessage* request = new RPCAuthenticateValidateTicketMessage();
	request->GetBuffer()->set_clientip(clientIP);
	request->GetBuffer()->set_npid(clientID);
	request->GetBuffer()->set_ticket(ticketString);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPValidateUserTicketResult>* result = new NPAsyncImpl<NPValidateUserTicketResult>();
	async->SetCallback(ValidateUserTicketCB, result);

	request->Free();

	return result;
}

void CI_Initialize(char version);
void CI_Shutdown();

LIBNP_API bool LIBNP_CALL NP_GetUserTicket(void* buffer, size_t bufferSize, NPID targetServer)
{
	if (!g_np.authenticated)
	{
		return false;
	}

	if (bufferSize < sizeof(NPAuthenticateTicket))
	{
		return false;
	}

	NPAuthenticateTicket* ticket = (NPAuthenticateTicket*)buffer;
	ticket->version = 1;
	NP_GetNPID(&ticket->clientID);
	ticket->serverID = targetServer;
	ticket->time = 0;

	CI_Initialize(0x9C);
	CI_Initialize(0x9D);
	CI_Initialize(0x9E);

	return true;
}

LIBNP_API void LIBNP_CALL NP_TerminateGameConnection()
{
	CI_Shutdown();
}

LIBNP_API bool LIBNP_CALL NP_GetNPID(NPID* pID)
{
	if (g_np.authenticated)
	{
		*pID = g_np.npID;
		return true;
	}

	return false;
}

LIBNP_API int LIBNP_CALL NP_GetUserGroup()
{
	return g_np.groupID;
}

void Authenticate_SetUserGroup(INPRPCMessage* message)
{
	RPCAuthenticateUserGroupMessage* result = (RPCAuthenticateUserGroupMessage*)message;
	g_np.groupID = result->GetBuffer()->groupid();
}

static void (__cdecl * kickCB)(NPID, const char*);
static void (__cdecl * authCB)(EExternalAuthState);

void Authenticate_KickUser(INPRPCMessage* message)
{
	RPCAuthenticateKickUserMessage* result = (RPCAuthenticateKickUserMessage*)message;
	
	if (kickCB)
	{
		kickCB(result->GetBuffer()->npid(), result->GetBuffer()->reasonstring().c_str());
	}
}

LIBNP_API void LIBNP_CALL NP_RegisterKickCallback(void (__cdecl * callback)(NPID, const char*))
{
	kickCB = callback;
}

void Authenticate_EAHandler(INPRPCMessage* message)
{
	RPCAuthenticateExternalStatusMessage* result = (RPCAuthenticateExternalStatusMessage*)message;

	if (authCB)
	{
		authCB((EExternalAuthState)result->GetBuffer()->status());
	}
}

LIBNP_API void LIBNP_CALL NP_RegisterEACallback(void (__cdecl * callback)(EExternalAuthState))
{
	authCB = callback;

	authCB(ExternalAuthStateUnverified);
}

void Authenticate_Init()
{
	RPC_RegisterDispatch(RPCAuthenticateUserGroupMessage::Type, &Authenticate_SetUserGroup);
	RPC_RegisterDispatch(RPCAuthenticateKickUserMessage::Type, &Authenticate_KickUser);
	RPC_RegisterDispatch(RPCAuthenticateExternalStatusMessage::Type, &Authenticate_EAHandler);
}

void Authenticate_Reauthenticate()
{
	switch (g_auth.authType)
	{
		case AuthenticationTypeDetails:
			NP_AuthenticateWithDetails(g_auth.username, g_auth.password);
			break;
		case AuthenticationTypeLicenseKey:
			NP_AuthenticateWithLicenseKey(g_auth.licenseKey);
			break;
		case AuthenticationTypeToken:
			NP_AuthenticateWithToken(g_auth.authToken);
			break;
	}
}

void CI_InitializeGameDLL(const char* dllName);

LIBNP_API void* LIBNP_CALL NP_LoadGameModule(int version)
{
	char buffer[32];
	_snprintf(buffer, sizeof(buffer), "NetRT.%i", version);
	buffer[31] = '\0';

	CI_InitializeGameDLL(buffer);

	__try
	{
		void* retval = 0;

		RaiseException(0xCEAD0001, 0, 0, NULL);

		__asm
		{
			mov retval, eax
		}

		return retval;
	}
	__except ((GetExceptionCode() == 0xCEAD0001) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return NULL;
	}
}