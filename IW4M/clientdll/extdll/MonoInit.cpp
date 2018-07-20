#include "StdInc.h"
#include "Mono.h"

bool monoStarted = false;
MonoDomain* rootDomain;

void Mono_EnsureInit()
{
	if (!monoStarted)
	{
		mono_set_dirs("scripts\\lib", "scripts\\cfg");

		rootDomain = mono_jit_init_version("IW5M", "v4.0.30319");

		char* args[2];
		args[0] = "--soft-breakpoints";
		args[1] = "--debugger-agent=transport=dt_socket,address=127.0.0.1:10000";

		mono_jit_parse_options(2, args);

		SetUnhandledExceptionFilter(g_scriptability->cbExceptionFilter);

		monoStarted = true;
	}
}

void OutputExceptionToDebugger(MonoObject* exc)
{
	MonoClass* eclass = mono_object_get_class(exc);

	if (eclass)
	{
		MonoProperty* prop = mono_class_get_property_from_name(eclass, "Message");
		MonoMethod* getter = mono_property_get_get_method(prop);
		MonoString* msg = (MonoString*)mono_runtime_invoke(getter, exc, NULL, NULL);

		prop = mono_class_get_property_from_name(eclass, "InnerException");
		getter = mono_property_get_get_method(prop);
		MonoObject* inner = (MonoObject*)mono_runtime_invoke(getter, exc, NULL, NULL);

/*		MonoMethod* tostring = mono_class_get_method_from_name(eclass, "ToString", 0);

		MonoString* msg = (MonoString*)mono_runtime_invoke(tostring, exc, NULL, NULL);*/

		OutputDebugString(va("Exception: %s\n", mono_string_to_utf8(msg)));
		Com_Printf(0, "Unhandled exception: %s\n", mono_string_to_utf8(msg));

		if (inner)
		{
			OutputExceptionToDebugger(inner);
		}

		Com_Error(0, "Unhandled exception: %s", mono_string_to_utf8(msg));
	}
}