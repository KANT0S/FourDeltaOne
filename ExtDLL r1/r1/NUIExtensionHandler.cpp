// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: CEF extension handler
//
// Initial author: NTAuthority
// Started: 2013-01-22
// ==========================================================

#include "StdInc.h"

#ifdef WE_DO_WANT_NUI
#include "NUI.h"
#include <unordered_map>
#include <queue>

int PC_LoadSourceString(const char* string);
int PC_FreeSource(int handle);

struct UITask
{
	std::string task;
	std::string argument;

	int taskID;
};

static CRITICAL_SECTION _uiTaskLock;
static int _uiTaskID;
static std::queue<UITask> _uiTasks;

void NUITask_Init()
{
	InitializeCriticalSection(&_uiTaskLock);
}

struct TaskDefinition
{
	const char* type;
	void (*func)(int* menus, const char* source, char** parseSource);
};

typedef void (__cdecl * UI_RunScript_t)(int menus, char** parseSource, const char* what);
UI_RunScript_t UI_RunScript = (UI_RunScript_t)0x45EC00;

void NUITask_Process(UITask* task)
{
	const char* taskName = task->task.c_str();
	const char* argument = task->argument.c_str();

	TaskDefinition* tasks = (TaskDefinition*)0x746A88;

	for (int i = 0; i < 0x3B; i++)
	{
		if (!_stricmp(tasks[i].type, taskName))
		{
			int menus = 0x62E2858;
			tasks[i].func((int*)menus, (const char*)0xDEAD1234, (char**)&argument);
			break;
		}
	}

	UI_RunScript(0, (char**)&argument, taskName);

	g_nui.browser->GetMainFrame()->ExecuteJavaScript(va("CodeCallback_UITaskDone(%i);", task->taskID), "nui://game/native.js", 1);
}

void NUITask_Frame()
{
	if (!_uiTasks.empty())
	{
		do 
		{
			EnterCriticalSection(&_uiTaskLock);

			UITask task = _uiTasks.front();
			_uiTasks.pop();

			LeaveCriticalSection(&_uiTaskLock);

			NUITask_Process(&task);
		} while (!_uiTasks.empty());
	}
}

typedef menuExpression_t* (__cdecl * UI_CompileExpression_t)(int handle, int maxTokens);
UI_CompileExpression_t UI_CompileExpression = (UI_CompileExpression_t)0x413050;

menuExpression_t* NUI_CompileExpression(const char* exp)
{
	int handle = PC_LoadSourceString(exp);
	menuExpression_t* retval = UI_CompileExpression(handle, 200);

	PC_FreeSource(handle);

	return retval;
}

typedef const char* (__cdecl * RunExpression_t)(int uiContext, menuExpression_t* expression);
RunExpression_t RunExpression = (RunExpression_t)0x475360;

const char* NUI_RunExpression(menuExpression_t* exp)
{
	*(DWORD*)0x1CDE80C = GetCurrentThreadId();

	const char* result = RunExpression(0x62E2858, exp);

	return result;
}

typedef const char* (__cdecl * Dvar_GetStringValue_t)(const char* dvarName, const char* default);
Dvar_GetStringValue_t Dvar_GetStringValue = (Dvar_GetStringValue_t)0x43AC20;

typedef double (__cdecl * UI_GetLoadingProgress_t)();
UI_GetLoadingProgress_t UI_GetLoadingProgress = (UI_GetLoadingProgress_t)0x468380;

extern HANDLE rtHandle;

typedef std::unordered_map<std::string, menuExpression_t*>::iterator _expressionCacheIt;
static std::unordered_map<std::string, menuExpression_t*> _expressionCache;

bool NUIExtensionHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
	bool handled = false;

	if (name == "runExpression")
	{
		if (!*(BYTE*)0x62E3EFC)
		{
			retval = CefV8Value::CreateUndefined();
			return true;
		}

		//retval = CefV8Value::CreateInt((int)rtHandle);
		CefString expression = arguments[0]->GetStringValue();
		std::string str = expression.ToString();
		const char* exp = str.c_str();

		_expressionCacheIt iter = _expressionCache.find(exp);

		if (iter != _expressionCache.end())
		{
			retval = CefV8Value::CreateString(NUI_RunExpression((*iter).second));
		}
		else
		{
			menuExpression_t* menuExp = NUI_CompileExpression(exp);

			_expressionCache[exp] = menuExp;

			retval = CefV8Value::CreateString(NUI_RunExpression(menuExp));
		}

		handled = true;
	}
	else if (name == "getLoadProgress")
	{
		retval = CefV8Value::CreateDouble(UI_GetLoadingProgress());

		handled = true;
	}
	else if (name == "runUITask")
	{
		CefString task = arguments[0]->GetStringValue();
		CefString arg = arguments[1]->GetStringValue();

		std::string taskStr = task.ToString();
		std::string argStr = arg.ToString();

		UITask taskData;
		taskData.task = taskStr;
		taskData.argument = argStr;
		taskData.taskID = ++_uiTaskID;

		EnterCriticalSection(&_uiTaskLock);
		_uiTasks.push(taskData);
		LeaveCriticalSection(&_uiTaskLock);

		retval = CefV8Value::CreateInt(_uiTaskID);

		handled = true;
	}
	else if (name == "getDvar")
	{
		CefString dvarName = arguments[0]->GetStringValue();
		CefString dvarDefault = (arguments[1]->IsUndefined()) ? "" : arguments[1]->GetStringValue();

		std::string dvarNameStr = dvarName.ToString();
		std::string dvarDefaultStr = dvarDefault.ToString();

		retval = CefV8Value::CreateString(Dvar_GetStringValue(dvarNameStr.c_str(), dvarDefaultStr.c_str()));

		handled = true;
	}

	return handled;
}
#endif