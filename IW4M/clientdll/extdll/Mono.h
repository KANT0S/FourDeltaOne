#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/threads.h>

extern bool monoStarted;
extern MonoDomain* rootDomain;

void Mono_EnsureInit();
void OutputExceptionToDebugger(MonoObject* exc);