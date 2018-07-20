// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: IW4M 'new client' functionality; main init code.
//
// Initial author: NTAuthority
// Started: 2013-01-22
// ==========================================================

#include "StdInc.h"

#ifdef WE_DO_WANT_NUI
#include "NUI.h"

char *MSG_ReadStringLine( msg_t *msg ) {
	static char	string[1024];
	int		l,c;

	l = 0;
	do {
		c = MSG_ReadByte(msg);		// use ReadByte so -1 is out of bounds
		if (c == -1 || c == 0 || c == '\n') {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' ) {
			c = '.';
		}
		// don't allow higher ascii values
		if ( c > 127 ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);

	string[l] = 0;

	return string;
}

struct ServerStatus
{
	CefStringUTF8 infoString;
	int numPlayers;

	struct
	{
		CefStringUTF8 playerLine;
	} players[MAX_CLIENTS];
};

void NUI_SendStatusResponse(const netadr_t& from, const ServerStatus& status)
{
	EnterV8Context();

	CefRefPtr<CefV8Value> rootObj = CefV8Value::CreateObject(nullptr);
	
	const char* scoreData = Info_ValueForKey(status.infoString.c_str(), "score");

	if (scoreData[0])
	{
		int scoreAllies = 0, scoreAxis = 0;
		NUITeam teamAllies = TEAM_ARAB, teamAxis = TEAM_MILITIA;
		sscanf(scoreData, "%d %d %d %d", &teamAllies, &scoreAllies, &teamAxis, &scoreAxis);

		rootObj->SetValue("allies", CefV8Value::CreateInt((int)teamAllies), V8_PROPERTY_ATTRIBUTE_NONE);
		rootObj->SetValue("axis", CefV8Value::CreateInt((int)teamAxis), V8_PROPERTY_ATTRIBUTE_NONE);

		rootObj->SetValue("alliesScore", CefV8Value::CreateInt((int)scoreAllies), V8_PROPERTY_ATTRIBUTE_NONE);
		rootObj->SetValue("axisScore", CefV8Value::CreateInt((int)scoreAxis), V8_PROPERTY_ATTRIBUTE_NONE);
	}

	const char* description = Info_ValueForKey(status.infoString.c_str(), "_Desc");

	if (description[0])
	{
		rootObj->SetValue("serverDesc", CefV8Value::CreateString(description), V8_PROPERTY_ATTRIBUTE_NONE);
	}

	rootObj->SetValue("addr", CefV8Value::CreateString(NET_AdrToString(from)), V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Value> playerArray = CefV8Value::CreateArray(status.numPlayers);

	for (int i = 0; i < status.numPlayers; i++)
	{
		const char* playerLine = status.players[i].playerLine.c_str();
		int score = 0, ping = 0;

		sscanf(playerLine, "%d %d", &score, &ping);

		const char* s = playerLine;

		s = strchr(s, ' ');
		if (s)
			s = strchr(s+1, ' ');
		if (s)
			s++;
		else
			s = "unknown";

		CefRefPtr<CefV8Value> data = CefV8Value::CreateObject(nullptr);
		data->SetValue("score", CefV8Value::CreateInt(score), V8_PROPERTY_ATTRIBUTE_NONE);
		data->SetValue("ping", CefV8Value::CreateInt(ping), V8_PROPERTY_ATTRIBUTE_NONE);
		data->SetValue("name", CefV8Value::CreateString(s), V8_PROPERTY_ATTRIBUTE_NONE);

		playerArray->SetValue(i, data);
	}

	rootObj->SetValue("players", playerArray, V8_PROPERTY_ATTRIBUTE_NONE);

	CefV8ValueList args;
	args.push_back(rootObj);

	InvokeNUICallback("statusResponse", args);

	LeaveV8Context();
}

void NUI_HandleStatusResponse(netadr_t from, msg_t* msg)
{
	ServerStatus status;
	status.infoString = MSG_ReadStringLine(msg);

	// likely exploit: sending more players than MAX_CLIENTS
	int i;
	const char* s;

	for (i = 0, s = MSG_ReadStringLine(msg); *s; s = MSG_ReadStringLine(msg), i++)
	{
		status.players[i].playerLine = s;
	}

	status.numPlayers = i;

	CefPostTask(TID_RENDERER, NewCefRunnableFunction(NUI_SendStatusResponse, from, status));
}
#endif