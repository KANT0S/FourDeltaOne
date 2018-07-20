#include "StdInc.h"
#include "NUI.h"
#include "Mono.h"

#ifdef WE_DO_WANT_NUI
static MonoDomain* scriptDomain;
static MonoAssembly* scriptManagerAssembly;
static MonoImage* scriptManagerImage;

void GI_Print(MonoString* str);

char* GetMultiByteStringFromMonoString(MonoString* mStr);

void GI_ExecuteJS(MonoString* str)
{
	char* mbStr = GetMultiByteStringFromMonoString(str);
	if (mbStr == NULL) return;

	NUI_ExecuteJS(mbStr);
}

void CreateNUIDomain()
{
	Mono_EnsureInit();

	//scriptDomain = mono_domain_create();
	scriptDomain = mono_domain_create_appdomain("IW4NUI", NULL);
	mono_domain_set(scriptDomain, true);

	scriptManagerAssembly = mono_domain_assembly_open(scriptDomain, "scripts\\NUI.dll");

	if (!scriptManagerAssembly)
	{
		Com_Error(0, "Could not load the NUI.dll assembly.");
	}

	scriptManagerImage = mono_assembly_get_image(scriptManagerAssembly);

	bool methodSearchSuccess = true;
	MonoMethodDesc * description;

#define method_search(name, method) description = mono_method_desc_new(name, 1); \
	method = mono_method_desc_search_in_image(description, scriptManagerImage); \
	mono_method_desc_free(description); \
	methodSearchSuccess = methodSearchSuccess && method != NULL

	MonoMethod* scriptInitMethod;
	method_search("NUI.SHManager:InitializeOnce", scriptInitMethod);

	mono_add_internal_call("NUI.GameInterface::Print", GI_Print);
	mono_add_internal_call("NUI.GameInterface::ExecuteJS", GI_ExecuteJS);

	if (!methodSearchSuccess)
	{
		Com_Error(0, "Could not find one or more NUI.SHManager methods.");
	}

	MonoObject* exc = NULL;
	mono_runtime_invoke(scriptInitMethod, NULL, NULL, &exc);

	if (exc)
	{
		OutputExceptionToDebugger(exc);
	}
}
#else
void CreateNUIDomain()
{

}
#endif