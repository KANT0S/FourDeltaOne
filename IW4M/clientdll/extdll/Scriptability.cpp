#include "StdInc.h"
#include "Script.h"
#include "Mono.h"

#include <algorithm>
#include <string>

// global stuff we might need
#ifdef COMPILING_IW5M
DWORD* scr_retArgs = (DWORD*)0x1F3E414;
VariableValue** scr_stack = (VariableValue**)0x1F3E410;
#endif
#ifdef COMPILING_IW4M
DWORD* scr_retArgs = (DWORD*)0x2040D08;
VariableValue** scr_stack = (VariableValue**)0x2040D00;

DWORD* scr_numParam = (DWORD*)0x2040D0C;

#define Cmd_Argc_sv Cmd_ArgcSV
#define Cmd_Argv_sv Cmd_ArgvSV

#define Cmd_AddCommand(a, b, c) Cmd_AddCommand(a, b, c, 0)
#endif
// end global stuff

#ifdef BUILDING_EXTDLL
//static MonoDomain* rootDomain;
static MonoDomain* scriptDomain;
static MonoAssembly* scriptManagerAssembly;
MonoImage* scriptManagerImage;
static MonoMethod* scriptExitMethod;
static MonoMethod* loadScriptMethod;
static MonoMethod* runFrameMethod;
static MonoMethod* notifyMethod;
static MonoMethod* callMethod;
static MonoMethod* sayMethod;
static MonoMethod* serverCommandMethod;
static MonoMethod* clientCommandMethod;
static MonoMethod* scriptInitLevelMethod;
static MonoMethod* webRequestMethod;
static MonoMethod* parsePlaylistsMethod;
static MonoMethod* rotateMapMethod;

//static bool monoStarted = false;
bool scriptStarted = false;

MonoString* GI_Cmd_Argv(int arg);
void GI_Print(MonoString* str);
void GI_Cbuf_AddText(MonoString* str);
void GI_NotifyLevel(MonoString* notifyType, int numArgs);
void GI_SV_GameSendServerCommand(int entid, int id, MonoString* message);
void GI_Cmd_TokenizeString(MonoString* token);
void GI_NotifyNum(int entref, MonoString* notifyType, int numArgs);
void GI_PushString(MonoString* string);
MonoString* GI_NotifyType();
MonoString* GI_Cmd_Argv_sv(int arg);
MonoString* GI_Dvar_InfoString_Big(int flag);
MonoString* GI_GetString(int index);
MonoString* GI_GetHTTPHeader(int num);
MonoArray* GI_ReadHTTPBody(int length);
MonoString* GI_GetDvar(MonoString* name, MonoString* defaultValue);
MonoArray* GI_ReadFile(MonoString* filenameStr);

int count = 0;
wchar_t wide[1024];
char skinny[1024];

#define BUFFER_COUNT		4
#define BUFFER_SIZE		131072

static wchar_t wideBuffer[BUFFER_COUNT][BUFFER_SIZE];
static int nextWBufferIndex = 0;
static char skinnyBuffer[BUFFER_COUNT][BUFFER_SIZE];
static int nextSBufferIndex = 0;

MonoString* GetMonoStringFromMultiByteString(const char* mbStr)
{
	if (strlen(mbStr) > BUFFER_SIZE) return NULL;

	size_t sizeNeeded = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbStr, -1, wideBuffer[nextWBufferIndex], BUFFER_SIZE);
	if (sizeNeeded > sizeof(wchar_t) * BUFFER_SIZE) return NULL;

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbStr, -1, wideBuffer[nextWBufferIndex], BUFFER_SIZE);

	MonoString* retval = mono_string_new_utf16(scriptDomain, (mono_unichar2*)wideBuffer[nextWBufferIndex], wcslen(wideBuffer[nextWBufferIndex]));
	nextWBufferIndex = (nextWBufferIndex + 1) % BUFFER_COUNT;

	return retval;
}
char* GetMultiByteStringFromMonoString(MonoString* mStr)
{
	if (mono_string_length(mStr) > BUFFER_SIZE) return NULL;

	wchar_t* str_w = (wchar_t*)mono_string_to_utf16(mStr);
	size_t sizeNeeded = WideCharToMultiByte(CP_ACP, 0, str_w, -1, NULL, NULL, NULL, NULL);
	if (sizeNeeded > BUFFER_SIZE)
	{
		mono_free(str_w);
		return NULL;
	}

	WideCharToMultiByte(CP_ACP, 0, str_w, -1, skinnyBuffer[nextSBufferIndex], BUFFER_SIZE, 0, 0);
	mono_free(str_w);

	char* retval = skinnyBuffer[nextSBufferIndex];
	nextSBufferIndex = (nextSBufferIndex + 1) % BUFFER_COUNT;
	
	return retval;
}

static DWORD tlsConnection;
static DWORD tlsReqInfo;

void CreateScriptDomain()
{
	Mono_EnsureInit();

	tlsConnection = TlsAlloc();
	tlsReqInfo = TlsAlloc();

	//scriptDomain = mono_domain_create();
	scriptDomain = mono_domain_create_appdomain("InfinityScript", NULL);
	mono_domain_set(scriptDomain, true);

	scriptManagerAssembly = mono_domain_assembly_open(scriptDomain, "InfinityScript.dll");

	if (!scriptManagerAssembly)
	{
		Com_Error(0, "Could not load the InfinityScript.dll assembly.");
	}

	scriptManagerImage = mono_assembly_get_image(scriptManagerAssembly);
	g_scriptability->scriptManagerImage = scriptManagerImage;

	bool methodSearchSuccess = true;
	MonoMethodDesc * description;

	#define method_search(name, method) description = mono_method_desc_new(name, 1); \
			method = mono_method_desc_search_in_image(description, scriptManagerImage); \
			mono_method_desc_free(description); \
			methodSearchSuccess = methodSearchSuccess && method != NULL

	MonoMethod* scriptInitMethod;
	method_search("InfinityScript.SHManager:InitializeOnce", scriptInitMethod);
	method_search("InfinityScript.SHManager:InitializeLevel", scriptInitLevelMethod);

	method_search("InfinityScript.SHManager:Shutdown", scriptExitMethod);
	method_search("InfinityScript.SHManager:LoadScript", loadScriptMethod);
	method_search("InfinityScript.SHManager:RunFrame", runFrameMethod);
	method_search("InfinityScript.SHManager:HandleNotify", notifyMethod);
	method_search("InfinityScript.SHManager:HandleCall", callMethod);
	method_search("InfinityScript.SHManager:HandleSay", sayMethod);
	method_search("InfinityScript.SHManager:HandleServerCommand", serverCommandMethod);
	method_search("InfinityScript.SHManager:HandleClientCommand", clientCommandMethod);
	method_search("InfinityScript.SHManager:HandleWebRequest", webRequestMethod);
	
	method_search("InfinityScript.PlaylistManager:ParsePlaylists", parsePlaylistsMethod);
	method_search("InfinityScript.PlaylistManager:RotateMap", rotateMapMethod);

	mono_add_internal_call("InfinityScript.GameInterface::Cmd_Argv", GI_Cmd_Argv);
	mono_add_internal_call("InfinityScript.GameInterface::Print", GI_Print);
	mono_add_internal_call("InfinityScript.GameInterface::Cbuf_AddText", GI_Cbuf_AddText);
	mono_add_internal_call("InfinityScript.GameInterface::Script_NotifyLevel", GI_NotifyLevel);
	mono_add_internal_call("InfinityScript.GameInterface::SV_GameSendServerCommand", GI_SV_GameSendServerCommand);
	mono_add_internal_call("InfinityScript.GameInterface::Cmd_TokenizeString", GI_Cmd_TokenizeString);
	mono_add_internal_call("InfinityScript.GameInterface::Script_NotifyNum", GI_NotifyNum);
	mono_add_internal_call("InfinityScript.GameInterface::Script_PushString", GI_PushString);
	mono_add_internal_call("InfinityScript.GameInterface::Notify_Type", GI_NotifyType);
	mono_add_internal_call("InfinityScript.GameInterface::Cmd_Argv_sv", GI_Cmd_Argv_sv);
	mono_add_internal_call("InfinityScript.GameInterface::Dvar_InfoString_Big", GI_Dvar_InfoString_Big);
	mono_add_internal_call("InfinityScript.GameInterface::Script_GetString", GI_GetString);
	mono_add_internal_call("InfinityScript.GameInterface::GetHTTPHeader", GI_GetHTTPHeader);
	mono_add_internal_call("InfinityScript.GameInterface::ReadHTTPBody", GI_ReadHTTPBody);
	mono_add_internal_call("InfinityScript.GameInterface::GetDvar", GI_GetDvar);
	mono_add_internal_call("InfinityScript.GameInterface::ReadFile", GI_ReadFile);

	if (!methodSearchSuccess)
	{
		Com_Error(0, "Could not find one or more InfinityScript.SHManager methods.");
	}

	MonoObject* exc = NULL;
	mono_runtime_invoke(scriptInitMethod, NULL, NULL, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}
}

void InitScripts();

void InitScriptDomain()
{
	// re-enable dropping weapons
	//*(WORD*)0x47D53B = 0x0B75;

	// stuff
	if (!scriptStarted)
	{
		//if (!strcmp(GetStringConvar("mapname"), "ui_viewer_mp"))
		if (!strcmp(GetStringConvar("mapname"), "ui_viewer_mp") || true)
		{
			__asm
			{
#ifdef COMPILING_IW5M
				mov eax, 4D97B0h
#endif
#ifdef COMPILING_IW4M
				mov eax, 47F640h
#endif
				call eax
			}

			return;
		}

		//mono_domain_unload(scriptDomain);
		//mono_domain_set(rootDomain, true);

		CreateScriptDomain();

		scriptStarted = true;
	}
	
	MonoObject* exc = NULL;
	mono_runtime_invoke(scriptInitLevelMethod, NULL, NULL, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}

	InitScripts();

	__asm
	{
#ifdef COMPILING_IW5M
		mov eax, 4D97B0h
#endif
#ifdef COMPILING_IW4M
		mov eax, 47F640h
#endif
		call eax
	}
}

struct enabledScript
{
	std::string name;
	bool temporary;

	bool operator == (const enabledScript& other) const
	{
		return (name == other.name);
	}
};

static std::list<enabledScript> enabledScripts;
static bool g_inDSRParse;

void InitScripts()
{
	for (std::list<enabledScript>::iterator iter = enabledScripts.begin(); iter != enabledScripts.end(); iter++)
	{
		std::string name = (*iter).name;
		MonoString* string = GetMonoStringFromMultiByteString(name.c_str());
		
		void* args[1];
		args[0] = string;

		MonoObject* exc = NULL;
		mono_runtime_invoke(loadScriptMethod, NULL, args, &exc);

		if (exc)
		{
			OutputExceptionToDebugger(exc);
		}
	}
}

void Script_SetLoad_f()
{
	if (Cmd_Argc() != 2)
	{
		Com_Printf(0, "usage: loadScript [name]\n");
		return;
	}

	const char* scriptName = Cmd_Argv(1);
	
	// iterate through and see if we find a similarly-named script already
	for (std::list<enabledScript>::iterator iter = enabledScripts.begin(); iter != enabledScripts.end(); iter++)
	{
		if ((*iter).name == scriptName)
		{
			if (!g_inDSRParse)
			{
				(*iter).temporary = false;
			}

			return;
		}
	}

	enabledScript script;
	script.temporary = g_inDSRParse;
	script.name = scriptName;
	enabledScripts.push_front(script);
}

void Script_SetUnload_f()
{
	if (Cmd_Argc() != 2)
	{
		Com_Printf(0, "usage: unloadScript [name]\n");
		return;
	}

	const char* scriptName = Cmd_Argv(1);

	std::list<enabledScript>::iterator iter = enabledScripts.begin();

	while (iter != enabledScripts.end())
	{
		if ((*iter).name == scriptName)
		{
			enabledScript script = *iter++;
			enabledScripts.remove(script);
		}
		else
		{
			iter++;
		}
	}
}

void RemoveTemporaryScripts()
{
	std::list<enabledScript>::iterator iter = enabledScripts.begin();

	while (iter != enabledScripts.end())
	{
		if ((*iter).temporary)
		{
			enabledScript script = *iter++;
			enabledScripts.remove(script);
		}
		else
		{
			iter++;
		}
	}
}

#ifdef COMPILING_IW5M
void ExecuteDSRHook(int a1, int a2, const char* filename)
{
	RemoveTemporaryScripts();

	g_inDSRParse = true;
	
	__asm
	{
		push filename
		push a2
		push a1
		mov eax, 4BFED0h
		call eax
		add esp, 0Ch
	}

	g_inDSRParse = false;
}
#endif

void ProcessScripts(void* dest, void* source, size_t size)
{
	memcpy(dest, source, size);

	if (!scriptStarted) return;

	MonoObject* exc = NULL;
	mono_runtime_invoke(runFrameMethod, NULL, NULL, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}

	/*DWORD scrRunCurrentThreads = 0x4E19D0;

	__asm
	{
		call scrRunCurrentThreads
	}*/
}

int tempEntRef = (int)g_entities;//0x18FBB28;
#ifdef COMPILING_IW5M
DWORD storeTempEntRef_Retn = 0x480E87;
#endif
#ifdef COMPILING_IW4M
DWORD storeTempEntRef_Retn = 0x418608;
#endif
void __declspec(naked) StoreTempEntRef()
{
	__asm mov eax, [esp+4]
	__asm mov tempEntRef, eax

	g_scriptability->tempEntRef = tempEntRef;

#ifdef COMPILING_IW5M
	__asm sub esp, 10h
	__asm mov ecx, [esp+34h]
#endif
#ifdef COMPILING_IW4M
	__asm
	{
		sub esp, 0Ch
		push edi
		mov edi, [esp + 14h]
	}
#endif

	__asm jmp storeTempEntRef_Retn
}

//static VariableValue* notifyStack;
//static int notifyNumArgs;
//static const char* notifyType;

void NotifyScript(int entity, unsigned short type, VariableValue* stack)
{
	if (!scriptStarted) return;

	g_scriptability->notifyStack = stack;

	const char* string = SL_ConvertToString(type);
	int numArgs = 0;

	if (stack->type != 8)
	{
		for (VariableValue* value = stack; value->type != 8; value--)
		{
			numArgs++;
		}
	}

	g_scriptability->notifyNumArgs = numArgs;
	g_scriptability->notifyType = string;

	void* args[1];
	args[0] = &entity;

	MonoObject* exc = NULL;
	mono_runtime_invoke(notifyMethod, NULL, args, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}
}

#ifdef COMPILING_IW4M
#define VM_Notify_LOC 0x61E1D0
#endif
#ifdef COMPILING_IW5M
#define VM_Notify_LOC 0x4DCDD0
#endif

void __declspec(naked) NotifyHookStub()
{
	__asm
	{
		mov eax, [esp + 0Ch]
		push eax
		mov eax, [esp + 0Ch]
		push eax
		mov eax, [esp + 0Ch]
		push eax
		call NotifyScript
		add esp, 0Ch

#ifdef COMPILING_IW4M
		push    ebp
		mov     ebp, esp
		and     esp, 0FFFFFFF8h

		push 61E1D6h
		retn
#endif
#ifdef COMPILING_IW5M
		push    ebp
		mov     ebp, esp
		and     esp, 0FFFFFFF8h

		mov eax, 4DCDD6h
		jmp eax
#endif
	}
}

typedef short (__cdecl * Scr_ExecEntThreadNum_t)(int number, int type, int funcHandle, int numArgs);

#ifdef COMPILING_IW5M
Scr_ExecEntThreadNum_t Scr_ExecEntThreadNum = (Scr_ExecEntThreadNum_t)0x4E1910;
#endif

#ifdef COMPILING_IW4M
Scr_ExecEntThreadNum_t Scr_ExecEntThreadNum = (Scr_ExecEntThreadNum_t)0x441510;
#endif

short ExecEntThreadHook(int* entity, DWORD funcHandle, int numArgs)
{
	if (!scriptStarted) return Scr_ExecEntThreadNum(*entity, 0, funcHandle, numArgs);

	int entityNum = *entity;

	int funcNum = -1;

#ifdef COMPILING_IW4M
	DWORD* codeCallbacks = (DWORD*)0x1A9C5FC;
#endif

#ifdef COMPILING_IW5M
	DWORD* codeCallbacks = (DWORD*)0x1B1B1C4;
#endif

	if (funcHandle == codeCallbacks[0]) // StartGameType
	{
		funcNum = 0;
	}
	else if (funcHandle == codeCallbacks[1]) // PlayerConnect
	{
		funcNum = 1;
	}
	else if (funcHandle == codeCallbacks[2]) // PlayerDisconnect
	{
		funcNum = 2;
	}
	else if (funcHandle == codeCallbacks[3]) // PlayerDamage
	{
		funcNum = 3;
	}
	else if (funcHandle == codeCallbacks[4]) // PlayerKilled
	{
		funcNum = 4;
	}
	else if (funcHandle == codeCallbacks[5]) // VehicleDamage
	{
		funcNum = 5;
	}

	g_scriptability->notifyNumArgs = numArgs;
	g_scriptability->notifyStack = *scr_stack;

	void* args[2];
	args[0] = &entityNum;
	args[1] = &funcNum;

	DWORD oldRetArgs = *scr_retArgs;
	*(DWORD*)scr_retArgs = 0;

	MonoObject* exc = NULL;
	mono_runtime_invoke(callMethod, NULL, args, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}

	*(DWORD*)scr_retArgs = oldRetArgs;

	return Scr_ExecEntThreadNum(entityNum, 0, funcHandle, numArgs);
}

void ExitLevelHook()
{
	if (!scriptStarted) return;

	MonoObject* exc = NULL;
	mono_runtime_invoke(scriptExitMethod, NULL, NULL, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}
}

#ifdef COMPILING_IW4M
void __declspec(naked) ExitLevelHookStub()
{
	__asm
	{
		pop esi
		pop ebp
		jmp ExitLevelHook
	}
}
#endif

bool Scriptability_OnSay(int client, char* name, char** textptr, int team);

void InitScriptability()
{
	static cmd_function_s loadScript;
	static cmd_function_s unloadScript;
	Cmd_AddCommand("loadScript", Script_SetLoad_f, &loadScript);
	Cmd_AddCommand("unloadScript", Script_SetUnload_f, &unloadScript);

#ifdef COMPILING_IW5M
	call(0x489F88, ProcessScripts, PATCH_CALL);
	call(0x48A73F, InitScriptDomain, PATCH_CALL);
	call(0x4DCDD0, NotifyHookStub, PATCH_JUMP); // VM_Notify
	call(0x4A6800, ExecEntThreadHook, PATCH_JUMP); // Scr_ExecEntThread
	call(0x4E4265, ExecuteDSRHook, PATCH_CALL);

	call(0x48A865, ExitLevelHook, PATCH_JUMP);
	call(0x480E80, StoreTempEntRef, PATCH_JUMP);
	nop(0x4DD344, 2); // skip bad entities
#endif

#ifdef COMPILING_IW4M
	call(0x415DE5, ProcessScripts, PATCH_CALL);
	call(0x48EFF4, InitScriptDomain, PATCH_CALL);
	call(0x61E1D0, NotifyHookStub, PATCH_JUMP);
	call(0x48F640, ExecEntThreadHook, PATCH_JUMP);

	call(0x406D0B, ExitLevelHookStub, PATCH_JUMP);
	call(0x418600, StoreTempEntRef, PATCH_JUMP);

	nop(0x61E69B, 2); // skipping 'bad entities' (ones where no objects are passed?) for notify
#endif
}
bool Scriptability_OnSay(int client, char* name, char** textptr, int team)
{
	if (!scriptStarted) return true;

	char* text = *textptr;

	MonoString* nameStr = GetMonoStringFromMultiByteString(name);
	MonoString* textStr = GetMonoStringFromMultiByteString(text);

	void* args[4];
	args[0] = &client;
	args[1] = nameStr;
	args[2] = &textStr;
	args[3] = &team;

	MonoObject* exc = NULL;
	MonoObject* boolean = mono_runtime_invoke(sayMethod, NULL, args, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}

	text = GetMultiByteStringFromMonoString(textStr);
	*textptr = text;

	bool* retval = (bool*)mono_object_unbox(boolean);
	return !(*retval);
}

bool Scriptability_ServerCommand(const char* command)
{
	if (serverCommandMethod != NULL)
	{	
		MonoString* cmdStr = GetMonoStringFromMultiByteString(command);
		if (cmdStr != NULL)
		{
			MonoObject* exc = NULL;

			void* args[1];
			args[0] = cmdStr;

			MonoObject* boolean = mono_runtime_invoke(serverCommandMethod, NULL, args, &exc);

			if (exc)
			{
				OutputExceptionToDebugger(exc);
			}

			bool* retval = (bool*)mono_object_unbox(boolean);
			return *retval;
		}
	}
	return false;
}

bool Scriptability_ClientCommand(const char* command, int client)
{
	if (clientCommandMethod != NULL)
	{
		MonoString* cmdStr = GetMonoStringFromMultiByteString(command);
		if (cmdStr != NULL)
		{
			MonoObject* exc = NULL;

			void* args[2];
			args[0] = cmdStr;
			args[1] = &client;

			MonoObject* boolean = mono_runtime_invoke(clientCommandMethod, NULL, args, &exc);

			if (exc)
			{
				OutputExceptionToDebugger(exc);
			}
			bool* retval = (bool*)mono_object_unbox(boolean);
			return *retval;
		}
	}
	return false;
}

// game interface funcs
MonoString* GI_GetString(int index)
{
	char* retval = SL_ConvertToString((g_scriptability->notifyStack[-index]).string);
	MonoString* cmdStr = GetMonoStringFromMultiByteString(retval);
	if (cmdStr != NULL)  return cmdStr;
	else return mono_string_new(scriptDomain, "");
}

MonoString* GI_Cmd_Argv(int arg)
{
	char* str = Cmd_Argv(arg);
	MonoString* cmdStr = GetMonoStringFromMultiByteString(str);
	if (cmdStr != NULL) return cmdStr;
	else return mono_string_new(scriptDomain, "");
}

MonoString* GI_Cmd_Argv_sv(int arg)
{
	char* str = Cmd_Argv_sv(arg);
	MonoString* cmdStr = GetMonoStringFromMultiByteString(str);
	if (cmdStr != NULL) return cmdStr;
	else return mono_string_new(scriptDomain, "");
}

void GI_Print(MonoString* str)
{
	char* mbStr = GetMultiByteStringFromMonoString(str);
	if (mbStr == NULL) return;

	Com_Printf(0, "%s", mbStr);
}

void GI_Cbuf_AddText(MonoString* str)
{
	char* mbStr = GetMultiByteStringFromMonoString(str);
	if (mbStr == NULL) return;

	Cbuf_AddText(0, mbStr);
}

void GI_NotifyLevel(MonoString* notifyType, int numArgs)
{
	char* mbStr = GetMultiByteStringFromMonoString(notifyType);
	if (mbStr == NULL) return;

	short notifyTypeStr = SL_GetString(mbStr, 0);
	Scr_NotifyLevel(notifyTypeStr, numArgs);
}

void GI_NotifyNum(int entref, MonoString* notifyType, int numArgs)
{
	char* mbStr = GetMultiByteStringFromMonoString(notifyType);
	if (mbStr == NULL) return;

	short notifyTypeStr = SL_GetString(mbStr, 0);
	Scr_NotifyNum(entref & 0xFFFF, entref >> 16, notifyTypeStr, numArgs);
}

void GI_SV_GameSendServerCommand(int entid, int id, MonoString* message)
{
	char* mbStr = GetMultiByteStringFromMonoString(message);
	if (mbStr == NULL) return;

#ifdef COMPILING_IW5M
	if (id == -1) // SAY COMMAND
		id = 84;
#endif

#ifdef COMPILING_IW4M
	if (id == -1) // SAY COMMAND
		id = 104;
#endif

	SV_GameSendServerCommand(entid, 0, va("%c \"%s\"", id, mbStr));
}


void GI_Cmd_TokenizeString(MonoString* token)
{
	char* mbStr = GetMultiByteStringFromMonoString(token);
	if (mbStr != NULL) Cmd_TokenizeString(mbStr);
}

void GI_PushString(MonoString* string)
{
	char* mbStr = GetMultiByteStringFromMonoString(string);
	if (mbStr != NULL) Scr_AddString(mbStr);
}

MonoString* GI_NotifyType()
{
	MonoString* mStr = GetMonoStringFromMultiByteString(g_scriptability->notifyType);
	if (mStr) return mStr;
	else return mono_string_new(scriptDomain, "");
}

MonoString* GI_Dvar_InfoString_Big(int flag)
{
#ifdef COMPILING_IW5M
	DWORD func = 0x4CE100;
#endif

#ifdef COMPILING_IW4M
	DWORD func = 0x4D98A0;
#endif

	const char* retval = "";

	__asm
	{
		push flag
		push 0
		call func
		add esp, 8h
		mov retval, eax
	}
	
	MonoString* mStr = GetMonoStringFromMultiByteString(retval);
	if (mStr) return mStr;
	else return mono_string_new(scriptDomain, "");
}
#endif

// TODO: this is not threadsafe at all!
//static mg_request_info* mgReqInfo;
//static mg_connection* mgConn;

void Scriptability_HandleWebRequest(mg_connection* conn, const mg_request_info* request_info)
{
	mono_thread_attach(scriptDomain);

	TlsSetValue(tlsConnection, conn);
	TlsSetValue(tlsReqInfo, (void*)request_info);

	void* args[4];

	MonoString* method = GetMonoStringFromMultiByteString(request_info->request_method);
	args[0] = method;

	MonoString* uri = GetMonoStringFromMultiByteString(request_info->uri);
	args[1] = uri;
	
	MonoString* qstring = GetMonoStringFromMultiByteString(request_info->query_string ? request_info->query_string : "");
	args[2] = qstring;
	
	int numHeaders = request_info->num_headers;
	args[3] = &numHeaders;

	int remoteIP = request_info->remote_ip;
	args[4] = &remoteIP;

	MonoObject* exc = NULL;
	MonoObject* boolean = mono_runtime_invoke(webRequestMethod, NULL, args, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
		return;
	}

	MonoArray* data = (MonoArray*)boolean;
	char* array = mono_array_addr(data, char, 0);
	int length = mono_array_length(data);

	mg_write(conn, array, length);
}

MonoString* GI_GetHTTPHeader(int num)
{
	mg_request_info* mgReqInfo = (mg_request_info*)TlsGetValue(tlsReqInfo);

	if (num > mgReqInfo->num_headers)
	{
		return nullptr;
	}

	return GetMonoStringFromMultiByteString(va("%s: %s", mgReqInfo->http_headers[num].name, mgReqInfo->http_headers[num].value));
}

MonoArray* GI_ReadHTTPBody(int length)
{
	mg_connection* mgConn = (mg_connection*)TlsGetValue(tlsConnection);

	char* byteBuffer = new char[length];
	int bytesRead = mg_read(mgConn, byteBuffer, length);

	MonoArray* data = mono_array_new(scriptDomain, mono_get_byte_class(), bytesRead);
	char* monoBuffer = mono_array_addr(data, char, 0);

	memcpy(monoBuffer, byteBuffer, bytesRead);

	delete[] byteBuffer;

	return data;
}

MonoString* GI_GetDvar(MonoString* name, MonoString* defaultValue)
{
	dvar_t* dvar = Dvar_FindVar(GetMultiByteStringFromMonoString(name));

	if (dvar == NULL)
	{
		return defaultValue;
	}

	switch (dvar->type)
	{
		case DVAR_TYPE_BOOL:
			return GetMonoStringFromMultiByteString(dvar->current.boolean ? "True" : "False");
		case DVAR_TYPE_FLOAT:
			return GetMonoStringFromMultiByteString(va("%g", dvar->current.value));
		case DVAR_TYPE_INT:
		case DVAR_TYPE_ENUM:
			return GetMonoStringFromMultiByteString(va("%i", dvar->current.integer));
		case DVAR_TYPE_STRING:
			return GetMonoStringFromMultiByteString(dvar->current.string);
		default:
			return defaultValue;
	}
}

MonoArray* GI_ReadFile(MonoString* filenameStr)
{
	char* buffer;
	int length = FS_ReadFile(GetMultiByteStringFromMonoString(filenameStr), &buffer);

	if (length < 0)
	{
		return NULL;
	}

	MonoArray* data = mono_array_new(scriptDomain, mono_get_byte_class(), length);
	char* monoBuffer = mono_array_addr(data, char, 0);

	memcpy(monoBuffer, buffer, length);

	FS_FreeFile(buffer);

	return data;
}

void Scriptability_ParsePlaylists(const char* playlists)
{
	if (!scriptStarted)
	{
		//mono_domain_unload(scriptDomain);
		//mono_domain_set(rootDomain, true);

		CreateScriptDomain();

		scriptStarted = true;
	}

	MonoString* playlistStr = GetMonoStringFromMultiByteString(playlists);
	if (playlistStr != NULL)
	{
		MonoObject* exc = NULL;

		void* args[1];
		args[0] = playlistStr;

		mono_runtime_invoke(parsePlaylistsMethod, NULL, args, &exc);

		if (exc)
		{
			OutputExceptionToDebugger(exc);
		}
	}
}

void Scriptability_RotateMap()
{
	if (!scriptStarted)
	{
		//mono_domain_unload(scriptDomain);
		//mono_domain_set(rootDomain, true);

		CreateScriptDomain();

		scriptStarted = true;
	}

	MonoObject* exc = NULL;

	mono_runtime_invoke(rotateMapMethod, NULL, NULL, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}
}