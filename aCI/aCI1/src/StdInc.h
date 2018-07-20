#include <windows.h>
#include <stdio.h>
#include "Hooking.h"

#define CI_ID_T5_AIMTARGET_GETTAGPOS 10001
#define CI_ID_T5_PYTHON_SPH4X_ESP 10002

#define CI_ID_IW5_REGISTERFONT 20001
#define CI_ID_IW5_GIBSONFAKER 20002

#define CI_ID_IW4_IMPULSEHACK 30001
#define CI_ID_IW4_ENTRY_IH 30002 // entry point most commonly used by impulse hack
#define CI_ID_IW4_AIMTARGET_GETTAGPOS 30003

void CI_SendStatus(int status);
WCHAR* GetCommandLinePid(int pid);

void CI_CompareDetection(void* address, const char* match, size_t length, int reason);