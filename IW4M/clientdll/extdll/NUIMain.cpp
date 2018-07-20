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
#include <MMSystem.h>

#ifdef WE_DO_WANT_NUI
#include "NUI.h"
#include <d3d9.h>

//extern HWND g_nuiHWND;
//extern int g_nuiWidth;
//extern int g_nuiHeight;

static int g_roundedWidth;
static int g_roundedHeight;

//GfxImage* g_nuiImage;

nui_s g_nui;

class NUIClient : public CefClient, public CefLifeSpanHandler, public CefRenderHandler, public CefDisplayHandler
{
public:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		if (!g_nui.browser)
		{
			g_nui.browser = browser.get();
			browser->AddRef();
		}
	}

	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler()
	{
		return this;
	}

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler()
	{
		return this;
	}

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler()
	{
		return this;
	}

	virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
	{
		std::string sourceStr = source.ToString();
		std::string messageStr = message.ToString();

		Com_Printf(0, "%s:%i, %s\n", sourceStr.c_str(), line, messageStr.c_str());

		return false;
	}

	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
	{
		rect.Set(0, 0, g_nuiDraw->nuiWidth, g_nuiDraw->nuiHeight);

		return true;
	}

	virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
	{
		if (type == PET_VIEW)
		{
			EnterCriticalSection(&g_nui.renderBufferLock);

			int timeBegin = timeGetTime();

			LeaveCriticalSection(&g_nui.renderBufferLock);

			for (RectList::const_iterator iter = dirtyRects.begin(); iter != dirtyRects.end(); iter++)
			{
				CefRect rect = *iter;
				
				/*for (int x = rect.x; x < (rect.x + rect.width); x++)
				{
					for (int y = rect.y; y < (rect.y + rect.height); y++)
					{
						int* src = &((int*)(buffer))[(y * width) + x];
						int* dest = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + x];

						*dest = *src;
					}
				}*/

				for (int y = rect.y; y < (rect.y + rect.height); y++)
				{
					int* src = &((int*)(buffer))[(y * width) + rect.x];
					int* dest = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];

					memcpy(dest, src, (rect.width * 4));
				}

				g_nui.dirtyRects.push(rect);
			}

			g_nui.renderBufferDirty = true;

			int duration = timeGetTime() - timeBegin;

			//OutputDebugString(va("[%d] onpaint time %dms\n", Com_Milliseconds(), duration));

			//LeaveCriticalSection(&g_nui.renderBufferLock);
		}
	}

	IMPLEMENT_REFCOUNTING(NUIClient);
};

class NUIApp : public CefApp, public CefRenderProcessHandler
{
private:
	CefRefPtr<CefV8Handler> extensionHandler;

public:
	NUIApp()
	{
		extensionHandler = new NUIExtensionHandler();
	}

	virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
	{
		registrar->AddCustomScheme("nui", true, false, false);
	}

	virtual void OnWebKitInitialized()
	{
		std::string nui_code =
			"var nui;"
			"if (!nui)"
			"  nui = {};"
			"(function() {"
			"  nui.runExpression = function(str) {"
			"    native function runExpression(str);"
			"    return runExpression(str);"
			"  };"
			"  nui.getStatus = function(str) {"
			"    native function getStatus(str);"
			"    return getStatus(str);"
			"  };"
			"  nui.runUITask = function(task, arg) {"
			"    native function runUITask(task, arg);"
			"    return runUITask(task, arg);"
			"  };"
			"  nui.getLoadProgress = function() {"
			"    native function getLoadProgress();"
			"    return getLoadProgress();"
			"  };"
			"  nui.getDvar = function(name, defa) {"
			"    native function getDvar(name, defa);"
			"    return getDvar(name, defa);"
			"  };"
			"  nui.tableQuery = function(table, column, value) {"
			"    native function tableQuery(table, column, value);"
			"    return tableQuery(table, column, value);"
			"  };"
			"})();";

		CefRegisterExtension("v8/nui", nui_code, extensionHandler);
		
		/*char path[MAX_PATH];
		GetCurrentDirectory(sizeof(path), path);

		char pluginPath[MAX_PATH];
		sprintf_s(pluginPath, sizeof(pluginPath), "%s/%s", path, "zone/");
		//CefAddWebPluginDirectory(va("%s/%s", path, "zone"));
		CefAddWebPluginPath(pluginPath);
		CefRefreshWebPlugins();*/
	}

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
	{
		CefRefPtr<CefV8Value> window = context->GetGlobal();

		window->SetValue("registerUICallback", CefV8Value::CreateFunction("nuiCallback", extensionHandler), V8_PROPERTY_ATTRIBUTE_READONLY);
	}

	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
	{

	}

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
	{
		return this;
	}

	IMPLEMENT_REFCOUNTING(NUIApp);
};

void NUI_Shutdown()
{
	delete g_nui.browser;
	g_nui.browser = nullptr;

	CefShutdown();
}

void NUI_CreateBrowser()
{
	CefMainArgs args(GetModuleHandle(NULL));
	CefRefPtr<CefApp> app(new NUIApp());

	CefSettings cSettings;
	cSettings.multi_threaded_message_loop = true;
	cSettings.single_process = true;
	cSettings.remote_debugging_port = 13172;
	cef_string_utf16_set(L"GameData", 8, &cSettings.locales_dir_path, true);
	cef_string_utf16_set(L"en-US", 5, &cSettings.locale, true);

	CefInitialize(args, cSettings, app.get());
	CefRegisterSchemeHandlerFactory("nui", "game", new NUISchemeHandlerFactory());

	CefRefPtr<CefClient> client(new NUIClient());

	CefWindowInfo info;
	info.SetAsOffScreen(GetDesktopWindow()); // ?!
	info.SetTransparentPainting(true);

	CefBrowserSettings settings;

	CefBrowserHost::CreateBrowser(info, client, "nui://game/index.html", settings);
	//CefBrowserHost::CreateBrowser(info, client, "http://anthonycalzadilla.com/css3-ATAT/index.html", settings);

	call(0x6B8ABB, NUI_Shutdown, PATCH_CALL);
}

/*
struct RendererInitInfo
{
	HWND window;
	int refreshRate;
	bool fullscreen;
	int xpos;
	int ypos;
	int width;
	int height;
	int width2;
	int height2;
	int aaSamples;
};

static DWORD _initD3DBase = 0x5073F0;
static DWORD _setInitInfo = 0x5077F0;
static DWORD _createWindow = 0x5075F0;

void NUI_CreateWindow()
{
	RendererInitInfo info;

	__asm
	{
		call _initD3DBase

		push esi
		lea esi, info

		call _setInitInfo
		call _createWindow

		pop esi
	}

	g_nui.hGameWindow = info.window;

	GetWindowRect(g_nui.hGameWindow, &g_nui.gameRect);

	ShowWindow(g_nui.hGameWindow, SW_SHOW);
}
*/

void NUI_Reload_f()
{
	if (g_nui.browser)
	{
		g_nui.browser->ReloadIgnoreCache();
	}
}

void NUI_ReloadPage_f()
{
	if (g_nui.browser)
	{
		g_nui.browser->GetMainFrame()->ExecuteJavaScript(va("CodeCallback_ReloadPage(\"%s\");", Cmd_Argv(1)), "nui://game/native.js", 1);
	}
}

#pragma optimize("", off)
void Image_Setup(GfxImage* image, short width, short height, short depth, unsigned int flags, int format)
{
	DWORD func = 0x54AF50;

	__asm
	{
		push edi
		mov eax, image
		mov di, width
		push format
		push flags
		push 0
		push depth
		push height
		call func
		add esp, 14h
		pop edi
	}
}
#pragma optimize("", on)

Material* Material_Register(const char* filename);

static int roundUp(int number, int multiple)
{
	if ((number % multiple) == 0)
	{
		return number;
	}

	int added = number + multiple;

	return (added) - (added % multiple);
}

void NUI_ClearServerList()
{
	InvokeNUICallback("clearServers", CefV8ValueList());
}

void NUI_AddServerToList_(const CefString& adr, const CefString& info)
{
	CefV8ValueList args;
	CefRefPtr<CefV8Value> adrValue = CefV8Value::CreateString(adr);
	CefRefPtr<CefV8Value> infoValue = CefV8Value::CreateString(info);
	args.push_back(adrValue);
	args.push_back(infoValue);

	InvokeNUICallback("addServer", args);
}

static double HighPercentage(const char* string)
{
	int totalLetters = 0;
	int totalHigh = 0;

	while (*string)
	{
		if (*string >= 'A' && *string <= 'z')
		{
			totalLetters++;
		}
		else if (*string < 0)
		{
			totalLetters++;
			totalHigh++;
		}

		string++;
	}

	return (totalHigh / (double)totalLetters);
}

void NUI_AddServerToList(netadr_t adr, const char* info)
{
	// we take the mod name (fs_game) and the hostname, and see if either includes a lot of high characters, in which case it's 1251 (rus)
	const char* gameName = &Info_ValueForKey(info, "fs_game")[5];
	const char* hostName = Info_ValueForKey(info, "hostname");

	int codepage = 1252;
	double highCharsGame = HighPercentage(gameName);
	double highCharsHost = HighPercentage(hostName);

	if (highCharsGame > 0.7 || highCharsHost > 0.7)
	{
		codepage = 1251;
	}

	static wchar_t wideInfo[8192];
	MultiByteToWideChar(codepage, 0, info, -1, wideInfo, sizeof(wideInfo) / sizeof(wchar_t));

	CefString adrString = NET_AdrToString(adr);
	CefStringUTF16 infoString = wideInfo;
	
	CefPostTask(TID_RENDERER, NewCefRunnableFunction(&NUI_AddServerToList_, adrString, infoString));
}

/*void NUI_TestExtract()
{
	int fh;
	int length = FS_FOpenFileRead("sound/music/ui_lobby_answering_the_call.mp3", &fh, true);

	char buffer[15384];
	int len;

	FILE* out = fopen("hi.mp3", "wb");

	while ((len = FS_Read(buffer, sizeof(buffer), fh)) > 0)
	{
		fwrite(buffer, 1, len, out);
		fflush(out);
	}

	fclose(out);

	FS_FCloseFile(fh);
	ExitProcess(1);
}*/

char* Auth_GetUsername();

void NUI_Init()
{
	Com_Printf(0, "Initializing NUI...\n");

	// testing stuff
	//NUI_TestExtract();

	if (!g_nui.initialized)
	{
		InitializeCriticalSection(&g_nui.renderBufferLock);

		NUI_CreateBrowser();

		static cmd_function_t reloadCmd;
		static cmd_function_t reloadPagesCmd;
		Cmd_AddCommand("reload", NUI_Reload_f, &reloadCmd, 0);
		Cmd_AddCommand("reloadPage", NUI_ReloadPage_f, &reloadPagesCmd, 0);

		NUITask_Init();
		
		g_nui.initialized = true;
	}

	g_nuiDraw->nuiImage = &g_nui.nuiImage;
	Image_Setup(&g_nui.nuiImage, g_nuiDraw->nuiWidth, g_nuiDraw->nuiHeight, 1, 0x1000003, D3DFMT_A8R8G8B8);

	g_roundedHeight = roundUp(g_nuiDraw->nuiHeight, 16);
	g_roundedWidth = roundUp(g_nuiDraw->nuiWidth, 16);

	Com_Printf(0, "Created NUI texture (expected size %dx%d)\n", g_roundedWidth, g_roundedHeight);

	g_nui.material = g_clientDLL->RegisterMaterial("nui");

	if (g_nui.renderBuffer)
	{
		delete[] g_nui.renderBuffer;
	}

	g_nui.renderBuffer = new char[4 * g_roundedWidth * g_roundedHeight];

	g_nuiDraw->AddServerToList = NUI_AddServerToList;
	g_nuiDraw->ClearServerList = NUI_ClearServerList;
	g_nuiDraw->HandleStatusResponse = NUI_HandleStatusResponse;

	Dvar_SetCommand("name", g_clientDLL->GetUsername());
	// make sure this doesn't get duplicated too much
	Dvar_SetCommand("steamid", va("%i", g_clientDLL->GetUserID()));
}

void NUI_UpdateFrame()
{
	if (g_nui.renderBufferDirty)
	{
		int timeBegin = timeGetTime();

		D3DLOCKED_RECT lockedRect;
		((IDirect3DTexture9*)g_nui.nuiImage.texture)->LockRect(0, &lockedRect, NULL, 0);
		//memcpy(lockedRect.pBits, g_nui.renderBuffer, g_roundedWidth * g_nuiDraw->nuiHeight * 4);

		//for (std::list<CefRect>::const_iterator it = g_nui.dirtyRects.begin(); it != g_nui.dirtyRects.end(); it++)
		while (!g_nui.dirtyRects.empty())
		{
			EnterCriticalSection(&g_nui.renderBufferLock);
			CefRect rect = g_nui.dirtyRects.front();
			g_nui.dirtyRects.pop();
			LeaveCriticalSection(&g_nui.renderBufferLock);

			for (int y = rect.y; y < (rect.y + rect.height); y++)
			{
				//int* src = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				//int* dest = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				//int* dest = &((int*)(lockedRect.pBits))[(y * g_roundedWidth) + rect.x];

				int* src = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				//int* dest = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				int* dest = &((int*)(lockedRect.pBits))[(y * (lockedRect.Pitch / 4)) + rect.x];

				memcpy(dest, src, (rect.width * 4));

				//_VEC_memcpy()
			}
		}

		((IDirect3DTexture9*)g_nui.nuiImage.texture)->UnlockRect(0);

		int duration = timeGetTime() - timeBegin;

		//OutputDebugString(va("[%d] update time %dms\n", Com_Milliseconds(), duration));

		/*D3DLOCKED_RECT lockedRect;

		for (std::list<CefRect>::const_iterator it = g_nui.dirtyRects.begin(); it != g_nui.dirtyRects.end(); it++)
		{
			CefRect rect = *it;

			RECT d3dRect;
			d3dRect.top = rect.y;
			d3dRect.bottom = rect.y + rect.height;
			d3dRect.left = rect.x;
			d3dRect.right = rect.x + rect.width;

			((IDirect3DTexture9*)g_nui.nuiImage.texture)->LockRect(0, &lockedRect, &d3dRect, 0);

			for (int y = rect.y; y < (rect.y + rect.height); y++)
			{
				//int* src = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				//int* dest = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				//int* dest = &((int*)(lockedRect.pBits))[(y * g_roundedWidth) + rect.x];

				int* src = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				//int* dest = &((int*)(g_nui.renderBuffer))[(y * g_roundedWidth) + rect.x];
				int* dest = &((int*)(lockedRect.pBits))[((y - rect.y) * (lockedRect.Pitch / 4))];

				memcpy(dest, src, (rect.width * 4));
			}

			((IDirect3DTexture9*)g_nui.nuiImage.texture)->UnlockRect(0);
		}*/

		g_nui.renderBufferDirty = false;

		//LeaveCriticalSection(&g_nui.renderBufferLock);
	}
}

typedef void (__cdecl * R_AddCmdDrawStretchPic_t)(float x, float y, float width, float height, float u1, float v1, float u2, float v2, float* color, Material* material);
R_AddCmdDrawStretchPic_t R_AddCmdDrawStretchPic = (R_AddCmdDrawStretchPic_t)0x509770;

typedef float* (__cdecl * _getScrPlace_t)(int controllerNum);
_getScrPlace_t _getScrPlace = (_getScrPlace_t)0x4F8940;

typedef void (__cdecl * UI_DrawHandlePic_t)(float* placement, float x, float y, float width, float height, int, int, int, Material*);
UI_DrawHandlePic_t UI_DrawHandlePic = (UI_DrawHandlePic_t)0x4D0EA0;

void NUI_Draw()
{
	float colorWhite[4];
	colorWhite[0] = 1;
	colorWhite[1] = 1;
	colorWhite[2] = 1;
	colorWhite[3] = 1;

	// false until the cinematic has been played
	static bool hasBeenColored = false;

	// not fully connected to a server, nor is a cinematic playing
	if (*(DWORD*)0xB2C540 != 1 && *(DWORD*)0xB2C540 != 2 && *(DWORD*)0xB2C540 != 9)
	{
		if (hasBeenColored)
		{
			R_AddCmdDrawStretchPic(0, 0, g_nuiDraw->nuiWidth, g_nuiDraw->nuiHeight, 0, 0, 1, 1, colorWhite, (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "loadscreen_mp_estate_tropical"));
		}
	}
	else
	{
		hasBeenColored = true;
	}

	R_AddCmdDrawStretchPic(0, 0, g_nuiDraw->nuiWidth, g_nuiDraw->nuiHeight, 0, 0, 1, 1, colorWhite, g_nui.material);

	// draw a cursor, if applicable
	if (*(DWORD*)0x62E2874)
	{
		if (!Dvar_FindVar("cl_bypassMouseInput")->current.boolean)
		{
			if (*(DWORD*)0x62E3EF8 != 6)
			{
				if (*(DWORD*)0xB2C538 & 16) // cls.keyCatchers
				{
					float* placement = _getScrPlace(0);

					float width = (placement[0] * 24) / placement[2];
					float height = (placement[1] * 24) / placement[3];

					float x = *(float*)0x62E2868 - (width / 2);
					float y = *(float*)0x62E286C - (height / 2);

					UI_DrawHandlePic(placement, x, y, width, height, 4, 4, 0, *(Material**)0x62E4B9C);
				}
			}
		}
	}
}

typedef void (__cdecl * SV_SpawnServer_t)(const char* map, bool dontLoadZone, bool migrating, bool cheats);
SV_SpawnServer_t SV_SpawnServer = (SV_SpawnServer_t)0x4A7120;

typedef bool (__cdecl * Sys_IsDatabaseReady_t)();
Sys_IsDatabaseReady_t Sys_IsDatabaseReady = (Sys_IsDatabaseReady_t)0x4CA4A0;

void CL_ConnectInstantly(netadr_t adr)
{
	*(DWORD*)0xB2C540 = 3; // client state
	*(DWORD*)0xA1E878 = *(DWORD*)0x1CDE614; // net encoding key?

	*(netadr_t*)0xA1E888 = adr;
	*(int*)0xA1E89C = -99999;

	*(DWORD*)0xA1E8A0 = 0; // connection attempts
	*(DWORD*)0xB2C69C = 0; // timeout stuff?

	// clear keycatchers
	//*(DWORD*)0xB2C538 &= 1;
}

static dvar_t*& com_sv_running = *(dvar_t**)0x1AD7934;

void NUI_Frame()
{
	if (Sys_IsDatabaseReady())
	{
		if (!strcmp(*(char**)0x1AA6480, "maps/mp/mp_ui_viewer.d3dbsp"))
		//if (!strcmp(*(char**)0x1AA6480, "maps/mp/mp_bog_sh.d3dbsp"))
		//if (DB_FindXAssetHeader(ASSET_TYPE_COL_MAP_MP, "maps/mp/mp_ui_viewer.d3dbsp") != nullptr) // is this even valid?
		{
			if (!com_sv_running->current.boolean)
			{
				netadr_t adr;
				memset(&adr, 0, sizeof(adr));
				adr.type = NA_LOOPBACK;

				Dvar_SetCommand("xblive_privatematch", "1");
				Dvar_SetCommand("g_gametype", "menu");

				SV_SpawnServer("ui_viewer_mp", true, false, true);
				//SV_SpawnServer("mp_bog_sh", true, false, true);
				CL_ConnectInstantly(adr);
			}
		}
	}

	NUITask_Frame();

	if (g_nui.browser)
	{
		float mouseX = *(float*)0x62E2868;
		float mouseY = *(float*)0x62E286C;

		if (mouseX != g_nui.lastX || mouseY != g_nui.lastY)
		{
			g_nui.lastX = mouseX;
			g_nui.lastY = mouseY;

			CefMouseEvent event;
			event.x = (mouseX * (g_nuiDraw->nuiWidth / 640.0f));
			event.y = (mouseY * (g_nuiDraw->nuiHeight / 480.0f));

			CefRefPtr<CefBrowserHost> host = g_nui.browser->GetHost();
			host->SendMouseMoveEvent(event, false);
		}
	}
}

void NUI_KeyEvent(int key, int down)
{
	if (!g_nui.browser)
	{
		return;
	}

	CefRefPtr<CefBrowserHost> host = g_nui.browser->GetHost();

	if (key >= 200 && key <= 202)
	{
		// mouse click event
		CefMouseEvent event;
		event.x = (g_nui.lastX * (g_nuiDraw->nuiWidth / 640.0f));
		event.y = (g_nui.lastY * (g_nuiDraw->nuiHeight / 480.0f));

		CefBrowserHost::MouseButtonType type;

		switch (key)
		{
			case 200:
				type = CefBrowserHost::MouseButtonType::MBT_LEFT;
				break;
			case 201:
				type = CefBrowserHost::MouseButtonType::MBT_RIGHT;
				break;
			case 202:
				type = CefBrowserHost::MouseButtonType::MBT_MIDDLE;
				break;
		}

		host->SendMouseClickEvent(event, type, !down, 1);
	}
	else if (key == 205 || key == 206)
	{
		// mouse wheel event
		CefMouseEvent event;
		event.x = (g_nui.lastX * (g_nuiDraw->nuiWidth / 640.0f));
		event.y = (g_nui.lastY * (g_nuiDraw->nuiHeight / 480.0f));

		host->SendMouseWheelEvent(event, 0, (key == 205) ? 3 : -3);
	}
	else
	{
		// todo: key events
	}
}

void NUI_OpenPage(const char* page)
{
	if (g_nui.browser)
	{
		g_nui.browser->GetMainFrame()->ExecuteJavaScript(va("CodeCallback_OpenPage(\"%s\");", page), "nui://game/native.js", 1);
	}
}

void NUI_ExecuteJS(const char* js)
{
	if (g_nui.browser)
	{
		g_nui.browser->GetMainFrame()->ExecuteJavaScript(js, "nui://game/native.js", 1);
	}
}

bool overrideCloseAll = false;

void NUI_CloseAllPages()
{
	// we ignore a 'close all' request for ui_viewer_mp the first time
	if (!strcmp(GetStringConvar("mapname"), "ui_viewer_mp") && !overrideCloseAll)
	{
		return;
	}

	overrideCloseAll = false;

	if (g_nui.browser)
	{
		g_nui.browser->GetMainFrame()->ExecuteJavaScript("CodeCallback_CloseAllPages();", "nui://game/native.js", 1);
	}
}

struct mapArena_t
{
	char uiName[32];
	char mapName[16];
	char pad[2768];
};

static int* numArenas = (int*)0x62E6930;
//static mapArena_t* arenas = (mapArena_t*)0x62E6934;

struct gameTypeName_t
{
	char gameType[12];
	char uiName[32];
};

static int* numGameTypes = (int*)0x62E50A0;
static gameTypeName_t* gameTypes = (gameTypeName_t*)0x62E50A4;

typedef char* (__cdecl * LocalizeMapString_t)(char*);
LocalizeMapString_t LocalizeMapString = (LocalizeMapString_t)0x44BB30;

void NUI_UpdateArenas()
{
	newMapArena_t* arenas = *(newMapArena_t**)0x420717;

	EnterV8Context();

	CefRefPtr<CefV8Value> rootObj = CefV8Value::CreateObject(nullptr);

	CefRefPtr<CefV8Value> mapArray = CefV8Value::CreateObject(nullptr);
	for (int i = 0; i < *numArenas; i++)
	{
		mapArray->SetValue(arenas[i].mapName, CefV8Value::CreateString(LocalizeMapString(arenas[i].uiName)), V8_PROPERTY_ATTRIBUTE_NONE);
	}

	rootObj->SetValue("maps", mapArray, V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Value> typeArray = CefV8Value::CreateObject(nullptr);
	for (int i = 0; i < *numGameTypes; i++)
	{
		typeArray->SetValue(gameTypes[i].gameType, CefV8Value::CreateString(LocalizeMapString(gameTypes[i].uiName)), V8_PROPERTY_ATTRIBUTE_NONE);
	}

	rootObj->SetValue("gametypes", typeArray, V8_PROPERTY_ATTRIBUTE_NONE);

	CefV8ValueList args;
	args.push_back(rootObj);

	InvokeNUICallback("arenas", args);

	LeaveV8Context();
}
#endif