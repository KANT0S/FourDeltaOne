// ==========================================================
// alterOps project
// 
// Component: t5cli
// Sub-component: clientdll
// Purpose: Interestingly random suicide messages
//
// Initial author: NTAuthority
// Started: 2012-01-19
// ==========================================================

#include "stdinc.h"
#include <time.h>

CallHook localizeSuicideMessageHook;
DWORD localizeSuicideMessageHookLoc = 0x4EB6DE;

#define NUM_SUICIDE_MESSAGES 10

static const char* suicideMessages[] = 
{
        "Mistakes were made.",
        "made a fatal mistake.",
        "killed himself.",
        "couldn't take it anymore.",
        "was KIA.",
        "committed suicide.",
        "hung himself.",
        "took an arrow to the knee.",
		"left the world",
		"was destroyed - by someone who can pwn."
};

const char* LocalizeSuicideMessage(const char*)
{
        return suicideMessages[rand() % NUM_SUICIDE_MESSAGES];
}

void PatchT5_SuicideMessages()
{
        srand((unsigned int)time(NULL));

        localizeSuicideMessageHook.initialize("", (PBYTE)localizeSuicideMessageHookLoc);
        localizeSuicideMessageHook.installHook((void(*)())LocalizeSuicideMessage, false);
}