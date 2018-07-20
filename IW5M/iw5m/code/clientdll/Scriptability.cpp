#include "StdInc.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/threads.h>

#include <algorithm>
#include <string>

static MonoDomain* rootDomain;
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

static bool monoStarted = false;

void OutputExceptionToDebugger(MonoObject* exc)
{
	MonoClass* eclass = mono_object_get_class(exc);

	if (eclass)
	{
		MonoProperty* prop = mono_class_get_property_from_name(eclass, "Message");
		MonoMethod* getter = mono_property_get_get_method(prop);
		MonoString* msg = (MonoString*)mono_runtime_invoke(getter, exc, NULL, NULL);

		Com_Error(0, "Unhandled exception: %s", mono_string_to_utf8(msg));
	}
}

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

int count = 0;
wchar_t wide[1024];
char skinny[1024];

#define BUFFER_COUNT		4
#define BUFFER_SIZE		2048

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

void CreateScriptDomain()
{
	if (!monoStarted)
	{
		mono_set_dirs("scripts\\lib", "scripts\\cfg");

		rootDomain = mono_jit_init_version("IW5M", "v4.0.30319");

		char* args[1];
		args[0] = "--soft-breakpoints";

		mono_jit_parse_options(1, args);

		monoStarted = true;
	}

	//scriptDomain = mono_domain_create();
	scriptDomain = mono_domain_create_appdomain("InfinityScript", NULL);
	mono_domain_set(scriptDomain, true);

	scriptManagerAssembly = mono_domain_assembly_open(scriptDomain, "InfinityScript.dll");

	if (!scriptManagerAssembly)
	{
		Com_Error(0, "Could not load the InfinityScript.dll assembly.");
	}

	scriptManagerImage = mono_assembly_get_image(scriptManagerAssembly);

	bool methodSearchSuccess = true;
	MonoMethodDesc * description;

	#define method_search(name, method) description = mono_method_desc_new(name, 1); \
			method = mono_method_desc_search_in_image(description, scriptManagerImage); \
			mono_method_desc_free(description); \
			methodSearchSuccess = methodSearchSuccess && method != NULL

	MonoMethod* scriptInitMethod;
	method_search("InfinityScript.SHManager:Initialize", scriptInitMethod);

	method_search("InfinityScript.SHManager:Shutdown", scriptExitMethod);
	method_search("InfinityScript.SHManager:LoadScript", loadScriptMethod);
	method_search("InfinityScript.SHManager:RunFrame", runFrameMethod);
	method_search("InfinityScript.SHManager:HandleNotify", notifyMethod);
	method_search("InfinityScript.SHManager:HandleCall", callMethod);
	method_search("InfinityScript.SHManager:HandleSay", sayMethod);
	method_search("InfinityScript.SHManager:HandleServerCommand", serverCommandMethod);
	method_search("InfinityScript.SHManager:HandleClientCommand", clientCommandMethod);

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
	if (monoStarted)
	{
		mono_domain_unload(scriptDomain);
		mono_domain_set(rootDomain, true);
	}

	CreateScriptDomain();
	InitScripts();

	__asm
	{
		mov eax, 4D97B0h
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

void ProcessScripts(void* dest, void* source, size_t size)
{
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

	memcpy(dest, source, size);
}

int tempEntRef = 0x18FBB28;
DWORD storeTempEntRef_Retn = 0x480E87;
void __declspec(naked) StoreTempEntRef()
{
	__asm mov eax, [esp+4]
	__asm mov tempEntRef, eax

	__asm sub esp, 10h
	__asm mov ecx, [esp+34h]

	__asm jmp storeTempEntRef_Retn
}

static VariableValue* notifyStack;
static int notifyNumArgs;
static const char* notifyType;

void NotifyScript(int entity, unsigned short type, VariableValue* stack)
{
	notifyStack = stack;

	const char* string = SL_ConvertToString(type);
	int numArgs = 0;

	if (stack->type != 8)
	{
		for (VariableValue* value = stack; value->type != 8; value--)
		{
			numArgs++;
		}
	}

	notifyNumArgs = numArgs;
	notifyType = string;

	void* args[1];
	args[0] = &entity;

	MonoObject* exc = NULL;
	mono_runtime_invoke(notifyMethod, NULL, args, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}
}

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

		push    ebp
		mov     ebp, esp
		and     esp, 0FFFFFFF8h

		mov eax, 4DCDD6h
		jmp eax
	}
}

typedef short (__cdecl * Scr_ExecEntThreadNum_t)(int number, int type, int funcHandle, int numArgs);
Scr_ExecEntThreadNum_t Scr_ExecEntThreadNum = (Scr_ExecEntThreadNum_t)0x4E1910;

short ExecEntThreadHook(int* entity, DWORD funcHandle, int numArgs)
{
	int entityNum = *entity;

	int funcNum = -1;

	if (funcHandle == *(DWORD*)0x1B1B1C4)
	{
		funcNum = 0;
	}
	else if (funcHandle == *(DWORD*)0x1B1B1C8)
	{
		funcNum = 1;
	}
	else if (funcHandle == *(DWORD*)0x1B1B1CC)
	{
		funcNum = 2;
	}
	else if (funcHandle == *(DWORD*)0x1B1B1D0)
	{
		funcNum = 3;
	}
	else if (funcHandle == *(DWORD*)0x1B1B1D4)
	{
		funcNum = 4;
	}
	else if (funcHandle == *(DWORD*)0x1B1B1D8)
	{
		funcNum = 5;
	}

	notifyNumArgs = numArgs;
	notifyStack = *(VariableValue**)0x1F3E410;

	void* args[2];
	args[0] = &entityNum;
	args[1] = &funcNum;

	DWORD oldRetArgs = *(DWORD*)0x1F3E414;
	*(DWORD*)0x1F3E414 = 0;

	MonoObject* exc = NULL;
	mono_runtime_invoke(callMethod, NULL, args, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}

	*(DWORD*)0x1F3E414 = oldRetArgs;

	return Scr_ExecEntThreadNum(entityNum, 0, funcHandle, numArgs);
}

void ExitLevelHook()
{
	MonoObject* exc = NULL;
	mono_runtime_invoke(scriptExitMethod, NULL, NULL, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}
}

void InitScriptability()
{
	static cmd_function_s loadScript;
	static cmd_function_s unloadScript;
	Cmd_AddCommand("loadScript", Script_SetLoad_f, &loadScript);
	Cmd_AddCommand("unloadScript", Script_SetUnload_f, &unloadScript);

	call(0x489F88, ProcessScripts, PATCH_CALL);
	call(0x48A73F, InitScriptDomain, PATCH_CALL);
	call(0x4DCDD0, NotifyHookStub, PATCH_JUMP); // VM_Notify
	call(0x4A6800, ExecEntThreadHook, PATCH_JUMP); // Scr_ExecEntThread
	call(0x4E4265, ExecuteDSRHook, PATCH_CALL);
	call(0x48A865, ExitLevelHook, PATCH_JUMP);
	call(0x480E80, StoreTempEntRef, PATCH_JUMP);
	nop(0x4DD344, 2); // skip bad entities
}
bool Scriptability_OnSay(int client, char* name, char** textptr, int team)
{
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
void Scriptability_HandleReturns()
{
	notifyNumArgs = *(DWORD*)0x1F3E414;
	notifyStack = *(VariableValue**)0x1F3E410;
}

extern "C" __declspec(dllexport) void GI_CleanReturnStack()
{
	DWORD oldNumParam = *scr_numParam;
	*scr_numParam = notifyNumArgs;

	__asm
	{
		mov eax, 4DC6C0h
		call eax
	}

	*scr_numParam = oldNumParam;
}

extern "C" __declspec(dllexport) void GI_GetVector(int index, float* vector)
{
	memcpy(vector, (notifyStack[-index]).vector, sizeof(float) * 3);
}

extern "C" __declspec(dllexport) int GI_GetInt(int index)
{
	return (notifyStack[-index]).integer;
}

MonoString* GI_GetString(int index)
{
	char* retval = SL_ConvertToString((notifyStack[-index]).string);
	MonoString* cmdStr = GetMonoStringFromMultiByteString(retval);
	if (cmdStr != NULL)  return cmdStr;
	else mono_string_new(scriptDomain, "");
}

extern "C" __declspec(dllexport) float GI_GetFloat(int index)
{
	return (notifyStack[-index]).number;
}

extern "C" __declspec(dllexport) int GI_ToEntRef(int obj)
{
	int entRef = obj;

	__asm
	{
		push entRef
		mov eax, 4D9650h
		call eax
		add esp, 4h
		mov entRef, eax
	}

	return entRef;
}

extern "C" __declspec(dllexport) int GI_GetEntRef(int index)
{
	int entRef = (notifyStack[-index]).integer;

	return GI_ToEntRef(entRef);
}

extern "C" __declspec(dllexport) int GI_GetType(int index)
{
	return (notifyStack[-index]).type;
}

extern "C" __declspec(dllexport) int GI_NotifyNumArgs()
{
	return notifyNumArgs;
}

extern "C" __declspec(dllexport) int GI_Cmd_Argc()
{
	return Cmd_Argc();
}

MonoString* GI_Cmd_Argv(int arg)
{
	char* str = Cmd_Argv(arg);
	MonoString* cmdStr = GetMonoStringFromMultiByteString(str);
	if (cmdStr != NULL) return cmdStr;
	else return mono_string_new(scriptDomain, "");
}

extern "C" __declspec(dllexport) int GI_Cmd_Argc_sv()
{
	return Cmd_Argc_sv();
}

MonoString* GI_Cmd_Argv_sv(int arg)
{
	char* str = Cmd_Argv_sv(arg);
	MonoString* cmdStr = GetMonoStringFromMultiByteString(str);
	if (cmdStr != NULL) return cmdStr;
	else return mono_string_new(scriptDomain, "");
}

extern "C" __declspec(dllexport) int GI_GetPing(int entity)
{
	return *(int*)(0x4A0CB08 + (0x1E1A2 * entity));
}

extern "C" __declspec(dllexport) int64_t GI_GetClientAddress(int client)
{
	unsigned int base = 0x49EB690 + (client * 493192);
	//netadr_t* adr = (netadr_t*)(base + 147068);
	netadr_t adr = *(netadr_t*)(base + 40);
	int ip = *(int*)adr.ip;
	int port = adr.port;
	return ((int64_t)ip << 32) + port;
}

extern "C" __declspec(dllexport) void GI_Cmd_EndTokenizedString()
{
	Cmd_EndTokenizedString();
}

extern "C" __declspec(dllexport) int GI_GetTempEntRef()
{
	if (tempEntRef == 0x18FBB28) return 0;
	return (tempEntRef - 0x18FBB28) / 628;
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

	if (id == -1) // SAY COMMAND
		id = 84;

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
	MonoString* mStr = GetMonoStringFromMultiByteString(notifyType);
	if (mStr) return mStr;
	else return mono_string_new(scriptDomain, "");
}

MonoString* GI_Dvar_InfoString_Big(int flag)
{
	DWORD func = 0x4CE100;
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