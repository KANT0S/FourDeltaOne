// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: NUI main header file
//
// Initial author: NTAuthority
// Started: 2013-01-22
// ==========================================================

#pragma once

#define USING_CEF_SHARED 1

#include <queue>

#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_runnable.h>

void NUI_Init();
void NUI_UpdateFrame();
void NUI_Draw();

void NUI_OpenPage(const char* name);
void NUI_CloseAllPages();

void NUI_ExecuteJS(const char* js);

void NUI_KeyEvent(int key, int down);

// NUI tasks
void NUITask_Init();
void NUITask_Frame();

// struct
extern struct nui_s
{
	bool initialized;
	HWND hGameWindow;
	RECT gameRect;
	CefBrowser* browser;

	bool renderBufferDirty;
	CRITICAL_SECTION renderBufferLock;
	char* renderBuffer;

	GfxImage nuiImage;
	Material* material;

	float lastX;
	float lastY;

	std::queue<CefRect> dirtyRects;
} g_nui;

// scheme handler factory
class NUISchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
	virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request);

	IMPLEMENT_REFCOUNTING(NUISchemeHandlerFactory);
};

class NUIExtensionHandler : public CefV8Handler
{
public:
	NUIExtensionHandler();

	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);

	void InvokeNUICallback(const CefString& name, const CefV8ValueList& arguments);

	void EnterV8Context();
	void ExitV8Context();

private:
	CefRefPtr<CefV8Value> _callbackFunction;
	CefRefPtr<CefV8Context> _callbackContext;

	IMPLEMENT_REFCOUNTING(NUIExtensionHandler);
};

// assertions
#include <assert.h>

void EnterV8Context();
void LeaveV8Context();
void InvokeNUICallback(const CefString& name, const CefV8ValueList& arguments);

#define REQUIRE_IO_THREAD()   assert(CefCurrentlyOn(TID_IO));

void NUI_UpdateArenas();

// NUIStatusResponse.cpp
void NUI_HandleStatusResponse(netadr_t from, msg_t* msg);