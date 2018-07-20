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

static NUIExtensionHandler* extHandler;

NUIExtensionHandler::NUIExtensionHandler()
{
	extHandler = this;
}

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

extern bool overrideCloseAll;

typedef void (__cdecl * UI_RunScript_t)(int menus, char** parseSource, const char* what);
UI_RunScript_t UI_RunScript = (UI_RunScript_t)0x45EC00;

void NUITask_Process(UITask* task)
{
	const char* taskName = task->task.c_str();
	const char* argument = task->argument.c_str();

	TaskDefinition* tasks = (TaskDefinition*)0x746A88;

	bool handledUIScript = false;

	if (strstr(argument, "connect"))
	{
		overrideCloseAll = true;
	}

	for (int i = 0; i < 0x3B; i++)
	{
		if (!_stricmp(tasks[i].type, taskName))
		{
			int menus = 0x62E2858;
			tasks[i].func((int*)menus, (const char*)0xDEAD1234, (char**)&argument);

			handledUIScript = true;
			break;
		}
	}

	if (!handledUIScript)
	{
		argument = task->argument.c_str();

		UI_RunScript(0, (char**)&argument, taskName);
	}

	argument = task->argument.c_str();

	if (!_stricmp(argument, "loadarenas"))
	{
		CefPostTask(TID_RENDERER, NewCefRunnableFunction(NUI_UpdateArenas));
	}

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

extern bool _workaroundThreadSync;

const char* NUI_RunExpression(menuExpression_t* exp)
{
	*(DWORD*)0x1CDE80C = GetCurrentThreadId();

	_workaroundThreadSync = true;
	const char* result = RunExpression(0x62E2858, exp);
	_workaroundThreadSync = false;

	return result;
}

typedef const char* (__cdecl * Dvar_GetStringValue_t)(const char* dvarName, const char* default);
Dvar_GetStringValue_t Dvar_GetStringValue = (Dvar_GetStringValue_t)0x43AC20;

typedef double (__cdecl * UI_GetLoadingProgress_t)();
UI_GetLoadingProgress_t UI_GetLoadingProgress = (UI_GetLoadingProgress_t)0x468380;

extern HANDLE rtHandle;

typedef std::unordered_map<std::string, menuExpression_t*>::iterator _expressionCacheIt;
static std::unordered_map<std::string, menuExpression_t*> _expressionCache;

struct stringTableValue_t
{
	char* string;
	int hash;
};

typedef struct stringTable_s {
	char* fileName;
	int columns;
	int rows;
	//char** data;
	stringTableValue_t* data;
} stringTable_t;

int StringTableHash(char* data)
{
	int hash = 0;

	while (*data != 0)
	{
		hash = tolower(*data) + (31 * hash);

		data++;
	}

	return hash;
}

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
	else if (name == "tableQuery")
	{
		std::list<CefRefPtr<CefV8Value>> results;

		std::string tableName = arguments[0]->GetStringValue();
		stringTable_t* table = (stringTable_t*)DB_FindXAssetHeader(ASSET_TYPE_STRINGTABLE, tableName.c_str());

		int columnNum = arguments[1]->GetIntValue();
		std::string value = arguments[2]->GetStringValue();

		if (columnNum < 0 || columnNum >= table->columns)
		{
			exception = "Column number is invalid.";
		}
		else
		{
			int32_t hashValue = StringTableHash((char*)value.c_str());

			for (int i = 0; i < table->rows; i++)
			{
				stringTableValue_t* tableValue = &table->data[(i * table->columns) + columnNum];

				if (tableValue->hash == hashValue)
				{
					if (!_stricmp(value.c_str(), tableValue->string))
					{
						// add the row to the results list
						CefRefPtr<CefV8Value> row = CefV8Value::CreateArray(table->columns);

						for (int j = 0; j < table->columns; j++)
						{
							row->SetValue(j, CefV8Value::CreateString(table->data[(i * table->columns) + j].string));
						}

						results.push_back(row);
					}
				}
			}

			CefRefPtr<CefV8Value> ret = CefV8Value::CreateArray(results.size());
			int ji = 0;

			for (auto& val : results)
			{
				ret->SetValue(ji, val);
				ji++;
			}

			retval = ret;
		}

		handled = true;
	}
	else if (name == "getLoadProgress")
	{
		retval = CefV8Value::CreateDouble(UI_GetLoadingProgress());

		handled = true;
	}
	else if (name == "getStatus")
	{
		std::string addr = arguments[0]->GetStringValue();
		netadr_t adr;

		NET_StringToAdr(addr.c_str(), &adr);

		NET_OutOfBandPrint(NS_CLIENT, adr, "getstatus 0");

		retval = CefV8Value::CreateNull();

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
	else if (name == "nuiCallback")
	{
		if (arguments.size() == 1 && arguments[0]->IsFunction())
		{
			_callbackContext = CefV8Context::GetCurrentContext();
			_callbackFunction = arguments[0];

			handled = true;
		}
	}

	return handled;
}

void NUIExtensionHandler::InvokeNUICallback(const CefString& name, const CefV8ValueList& arguments)
{
	if (!_callbackFunction.get() || !_callbackContext.get())
	{
		return;
	}

	//_callbackContext->Enter();

	CefV8ValueList newArguments;
	CefRefPtr<CefV8Value> nameValue = CefV8Value::CreateString(name);
	newArguments.push_back(nameValue);

	for (CefV8ValueList::const_iterator i = arguments.begin(); i != arguments.end(); i++)
	{
		newArguments.push_back(*i);
	}

	CefRefPtr<CefV8Value> retValue = _callbackFunction->ExecuteFunctionWithContext(_callbackContext, nullptr, newArguments);

	if (retValue.get() && retValue->HasException())
	{
		CefRefPtr<CefV8Exception> exception = retValue->GetException();

		Com_Printf(0, va("CEF exception: %s at %s:%i\n", exception->GetMessageA().c_str(), exception->GetScriptResourceName().c_str(), exception->GetSourceLine()));
	}

	//_callbackContext->Exit();
}

void NUIExtensionHandler::EnterV8Context()
{
	_callbackContext->Enter();
}

void NUIExtensionHandler::ExitV8Context()
{
	_callbackContext->Exit();
}

void EnterV8Context()
{
	extHandler->EnterV8Context();
}

void LeaveV8Context()
{
	extHandler->ExitV8Context();
}

void InvokeNUICallbackInternal(const CefString& string, const CefV8ValueList& arguments)
{
	extHandler->InvokeNUICallback(string, arguments);
}

void InvokeNUICallback(const CefString& name, const CefV8ValueList& arguments)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		InvokeNUICallbackInternal(name, arguments);
	}
	else
	{
		CefPostTask(TID_RENDERER, NewCefRunnableFunction(&InvokeNUICallbackInternal, name, arguments));
	}
}
#endif