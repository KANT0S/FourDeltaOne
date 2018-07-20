// ==========================================================
// alterOps project
// 
// Component: t5cli
// Sub-component: clientdll
// Purpose: Authorization to the aO web service.
//
// Initial author: NTAuthority
// Started: 2011-08-21 (finally...)
// ==========================================================

#undef UNICODE
#define NTDDI_VERSION 0x06000000
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <tchar.h>
#include <wincred.h>
#include <Shlwapi.h>

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

bool Auth_DisplayLoginDialog(const wchar_t** usernameP, const wchar_t** passwordP)
{
	CREDUI_INFOW info;
	static WCHAR username[CREDUI_MAX_USERNAME_LENGTH * sizeof(WCHAR)] = { 0 };
	static WCHAR password[CREDUI_MAX_PASSWORD_LENGTH * sizeof(WCHAR)] = { 0 };
	DWORD uLen = CREDUI_MAX_USERNAME_LENGTH;
	DWORD pLen = CREDUI_MAX_PASSWORD_LENGTH;
	LPVOID outStuff;
	ULONG outSize = 0;
	ULONG outPackage = 0;
	BOOL save = false;
	memset(&info, 0, sizeof(info));
	info.cbSize = sizeof(info);
	info.pszCaptionText = L"Authentication for NPTest";
	info.pszMessageText = L"Please log in to use NPTest. Use your alterIW.net forum credentials.";
	//CredUIPromptForCredentials(&info, _T("Target"), NULL, NULL, username, CREDUI_MAX_USERNAME_LENGTH, password, CREDUI_MAX_PASSWORD_LENGTH, &save, CREDUI_FLAGS_GENERIC_CREDENTIALS | CREDUI_FLAGS_SHOW_SAVE_CHECK_BOX | CREDUI_FLAGS_ALWAYS_SHOW_UI | CREDUI_FLAGS_DO_NOT_PERSIST | CREDUI_FLAGS_EXCLUDE_CERTIFICATES);

	HRESULT result = CredUIPromptForWindowsCredentialsW(&info, 0, &outPackage, NULL, 0, &outStuff, &outSize, NULL, CREDUIWIN_GENERIC);

	if (result == ERROR_SUCCESS)
	{
		CredUnPackAuthenticationBufferW(0, outStuff, outSize, username, &uLen, NULL, 0, password, &pLen);

		*usernameP = username;
		*passwordP = password;
	}

	return (result == ERROR_SUCCESS);
}

void Auth_Error(const char* message)
{
	int btn;
	static wchar_t buf[32768];
	MultiByteToWideChar(CP_UTF8, 0, message, -1, buf, sizeof(buf));

/*	TaskDialog(NULL,
		NULL,
		L"alterOps",
		L"alterOps",
		buf,
		TDCBF_OK_BUTTON,
		TD_ERROR_ICON,
		&btn);*/
	MessageBoxW(NULL, buf, L"Oshibka", MB_OK | MB_ICONSTOP);
}

size_t DataReceived(void *ptr, size_t size, size_t nmemb, void *data)
{
	if ((strlen((char*)data) + (size * nmemb)) > 8192)
	{
		return (size * nmemb);
	}

	strncat((char*)data, (const char*)ptr, size * nmemb);
	return (size * nmemb);
}

int authUserID;
char authUsername[256];
char authSessionID[40];

char* Auth_GetUsername()
{
	return authUsername;
}

char* Auth_GetSessionID()
{
	return authSessionID;
}

int Auth_GetUserID()
{
	return authUserID;
}

bool Auth_ParseResultBuffer(const char* result)
{
	bool ok = false;
	static char buf[16384];
	strncpy(buf, result, sizeof(buf));

	int i = 0;
	char* tokens[16];
	char* tok = strtok(buf, "#");

	while (tok && (i < 16))
	{
		tokens[i++] = tok;
		tok = strtok(NULL, "#");
	}

	// ok
	if (tokens[0][0] == 'o')
	{
		ok = true;
	}

	if (!ok)
	{
		Auth_Error(tokens[1]);
	}
	else
	{
		authUserID = atoi(tokens[2]);
		strcpy(authSessionID, tokens[5]);
		strncpy(authUsername, tokens[3], sizeof(authUsername));
	}

	return ok;
}

// a funny thing is how this va() function could possibly come from leaked IW code.
#define VA_BUFFER_COUNT		4
#define VA_BUFFER_SIZE		4096

static char g_vaBuffer[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
static int g_vaNextBufferIndex = 0;

const char *va( const char *fmt, ... )
{
	va_list ap;
	char *dest = &g_vaBuffer[g_vaNextBufferIndex][0];
	g_vaNextBufferIndex = (g_vaNextBufferIndex + 1) % VA_BUFFER_COUNT;
	va_start(ap, fmt);
	vsprintf( dest, fmt, ap );
	va_end(ap);
	return dest;
}

bool Auth_PerformSessionLogin(const char* username, const char* password)
{
	curl_global_init(CURL_GLOBAL_ALL);

	CURL* curl = curl_easy_init();

	if (curl)
	{
		char url[255];
		_snprintf(url, sizeof(url), "http://%s/remauth.php", "aiw3.net");

		char buf[8192] = {0};
		char postBuf[8192];
		_snprintf(postBuf, sizeof(postBuf), "%s&&%s", username, password);

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DataReceived);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buf);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "alterOps");
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuf);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1);

		CURLcode code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		curl_global_cleanup();

		if (code == CURLE_OK)
		{
			return Auth_ParseResultBuffer(buf);
		}
		else
		{
			Auth_Error(va("Could not reach the aIW3.net server. Error code from CURL: %x.", code));
		}

		return false;
	}

	curl_global_cleanup();
	return false;
}

void Auth_VerifyIdentity()
{
	const wchar_t* wusername;
	const wchar_t* wpassword;
	bool canceled = false;
	
	/*while (!canceled)
	{
		canceled = !Auth_DisplayLoginDialog(&wusername, &wpassword);

		if (!canceled)
		{
			char username[CREDUI_MAX_USERNAME_LENGTH];
			char password[CREDUI_MAX_PASSWORD_LENGTH];
			WideCharToMultiByte(CP_UTF8, 0, wusername, -1, username, sizeof(username), NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, wpassword, -1, password, sizeof(password), NULL, NULL);

			bool result = Auth_PerformSessionLogin(username, password);
			
			if (result)
			{
				return;
			}
		}
	}*/

	bool result = Auth_PerformSessionLogin("conglomeration", "whyyoulookinghereboy?!");

	if (result)
	{
		return;
	}

	ExitProcess(0x8000D3AD);
}