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
		if (!g_nui.browser.get())
		{
			g_nui.browser = browser;
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
			}

			g_nui.renderBufferDirty = true;

			LeaveCriticalSection(&g_nui.renderBufferLock);
		}
	}

	IMPLEMENT_REFCOUNTING(NUIClient);
};

class NUIApp : public CefApp, public CefRenderProcessHandler
{
public:
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
			"})();";

		CefRegisterExtension("v8/nui", nui_code, new NUIExtensionHandler());
		
		/*char path[MAX_PATH];
		GetCurrentDirectory(sizeof(path), path);

		char pluginPath[MAX_PATH];
		sprintf_s(pluginPath, sizeof(pluginPath), "%s/%s", path, "zone/");
		//CefAddWebPluginDirectory(va("%s/%s", path, "zone"));
		CefAddWebPluginPath(pluginPath);
		CefRefreshWebPlugins();*/
	}

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
	{
		return this;
	}

	IMPLEMENT_REFCOUNTING(NUIApp);
};

void NUI_CreateBrowser()
{
	CefMainArgs args(GetModuleHandle(NULL));
	CefRefPtr<CefApp> app(new NUIApp());

	CefSettings cSettings;
	cSettings.multi_threaded_message_loop = true;
	cSettings.single_process = true;
	cSettings.remote_debugging_port = 13172;
	cef_string_utf16_set(L"zone", 4, &cSettings.locales_dir_path, true);
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
	if (g_nui.browser.get())
	{
		g_nui.browser->ReloadIgnoreCache();
	}
}

void NUI_ReloadPage_f()
{
	if (g_nui.browser.get())
	{
		g_nui.browser->GetMainFrame()->ExecuteJavaScript(va("CodeCallback_ReloadPage(\"%s\");", Cmd_Argv(1)), "nui://game/native.js", 1);
	}
}

void Image_Setup(GfxImage* image, short width, short height, short depth, unsigned int flags, int format)
{
	DWORD func = 0x54AF50;

	__asm
	{
		push edi
		mov eax, image
		mov di, width
		push format
		push 0
		push flags
		push depth
		push height
		call func
		add esp, 14h
		pop edi
	}
}

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

char* Auth_GetUsername();

void NUI_Init()
{
	Com_Printf(0, "Initializing NUI...\n");

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

	g_nui.material = g_clientDLL->RegisterMaterial("nui");

	if (g_nui.renderBuffer)
	{
		delete[] g_nui.renderBuffer;
	}

	g_nui.renderBuffer = new char[4 * g_roundedWidth * g_roundedHeight];

	Dvar_SetCommand("name", g_clientDLL->GetUsername());
}

void NUI_UpdateFrame()
{
	if (g_nui.renderBufferDirty)
	{
		EnterCriticalSection(&g_nui.renderBufferLock);

		D3DLOCKED_RECT lockedRect;
		g_nui.nuiImage.texture->LockRect(0, &lockedRect, NULL, 0);
		memcpy(lockedRect.pBits, g_nui.renderBuffer, g_roundedWidth * g_nuiDraw->nuiHeight * 4);
		g_nui.nuiImage.texture->UnlockRect(0);

		g_nui.renderBufferDirty = false;

		LeaveCriticalSection(&g_nui.renderBufferLock);
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

					float width = (placement[0] * 32) / placement[2];
					float height = (placement[1] * 32) / placement[3];

					float x = *(float*)0x62E2868 - (width / 2);
					float y = *(float*)0x62E286C - (height / 2);

					UI_DrawHandlePic(placement, x, y, width, height, 4, 4, 0, *(Material**)0x62E4B9C);
				}
			}
		}
	}
}

void NUI_Frame()
{
	NUITask_Frame();

	if (g_nui.browser.get())
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
	if (!g_nui.browser.get())
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

		host->SendMouseWheelEvent(event, 0, (key == 205) ? 3 : 3);
	}
	else
	{
		// todo: key events
	}
}

void NUI_OpenPage(const char* page)
{
	if (g_nui.browser.get())
	{
		g_nui.browser->GetMainFrame()->ExecuteJavaScript(va("CodeCallback_OpenPage(\"%s\");", page), "nui://game/native.js", 1);
	}
}

void NUI_CloseAllPages()
{
	if (g_nui.browser.get())
	{
		g_nui.browser->GetMainFrame()->ExecuteJavaScript("CodeCallback_CloseAllPages();", "nui://game/native.js", 1);
	}
}
#endif