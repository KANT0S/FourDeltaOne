// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: main entry point for the aCI DLL
//
// Initial author  : NTAuthority
// Secondary author: JohnD :)
// Started: 2012-01-18
// ==========================================================

#include "StdInc.h"


char* cheat_message = "HELLO FELLOW CODE 'ENTHUSIASTS', I WOULD JUST LIKE TO BRING TO YOUR ATTENTION THAT SADLY THIS ANTI-CHEAT IS CURRENTLY UNDERDEVELOPED. FORTUNATELY THAT'S ABOUT TO CHANGE :)";

char *GENstr[]={
	"fal %d;%s",				// string 0, status and hwid: sure you want to have it like this ?
    "ntdll.dll",
	"NtQueryInformationProcess",
	0
};

PBYTE	pDllBase=0;
DWORD	lastFrameCheck=0;
DWORD	lastScanCheck=0;
DWORD   maxcalladr=0;

DWORD	FONTcalladr=0x01000;
DWORD	BONEPOScalladr=0x01000;
DWORD	CGTRACEcalladr=0x01000;

DWORD	lastCI=0;
DWORD   DLLhash=0;

typedef VOID  (WINAPI *NP_SendRandomString_t)(char *str);
NP_SendRandomString_t	pNP_SendRandomString=0;						// filled during Globalinit();

void GLOBAL_Init();
void IW4_Init();

BOOL EqualsMajorVersion(DWORD majorVersion)
{
    OSVERSIONINFOEX osVersionInfo;
    ::ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osVersionInfo.dwMajorVersion = majorVersion;
    ULONGLONG maskCondition = ::VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);
    return ::VerifyVersionInfo(&osVersionInfo, VER_MAJORVERSION, maskCondition);
}
BOOL EqualsMinorVersion(DWORD minorVersion)
{
    OSVERSIONINFOEX osVersionInfo;
    ::ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osVersionInfo.dwMinorVersion = minorVersion;
    ULONGLONG maskCondition = ::VerSetConditionMask(0, VER_MINORVERSION, VER_EQUAL);
    return ::VerifyVersionInfo(&osVersionInfo, VER_MINORVERSION, maskCondition);
}

BOOL WINAPI DllMain(
					__in  HINSTANCE hinstDLL,
					__in  DWORD fdwReason,
					__in  LPVOID lpvReserved
					)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (EqualsMajorVersion(6) && EqualsMinorVersion(3))
		{
			DEBUGprintf((0));
			DEBUGprintf(("Running Windows 8.1\n"));
			//return true;
			//Temporary 8.1 fix
		}
		
		pDllBase=(PBYTE) hinstDLL;			// remember our base address
		GLOBAL_Init();
		// game version is detected in init function
		DEBUGprintf(("Starting inits\n"));
		IW4_Init();
		DEBUGprintf(("cleanup\n"));
		FUNCclear((PBYTE)GLOBAL_Init);
		FUNCclear((PBYTE)IW4_Init);		// somehow this causes a crash
		DEBUGprintf(("wait for scan results\n"));
	}
	return true;
}


// encrypt all stringarrays
void GLOBAL_Init()
{
  int i;
  // initialize Developer console
  DEBUGprintf((0));
  DEBUGprintf(("aCI2 loaded at %08x\n",pDllBase));
  // initialize string crypting
  STRcryptInit();
  FUNCclear((PBYTE)STRcryptInit);
  for (i=0; GENstr[i]!=0; i++)	STRcrypt(GENstr[i],0);
  for (i=0; IW4str[i]!=0; i++)	STRcrypt(IW4str[i],0);

  ScanInit();
  // calculate hash of function code
  ScanSave((DWORD)CI_SendStatus);
  DLLfuncinit();
  FUNCclear((PBYTE)ScanInit);
  FUNCclear((PBYTE)DLLfuncinit);

  //hwid load
  macAddress = getMAC();
  FUNCclear((BYTE *)getMAC);

  // find the NP_ciSendRandomString function
  pNP_SendRandomString=(NP_SendRandomString_t)*(DWORD *)&pDllBase[0xd00];
  DEBUGprintf(("NP_ciSendRandomString at %08x\n",(DWORD)pNP_SendRandomString));
  if (pNP_SendRandomString) DEBUGmem(("NP func",(DWORD)pNP_SendRandomString,32));
  DLLhash=*(DWORD *)&pDllBase[0xd08];
  DEBUGprintf(("DLL hash=%08x\n",(DWORD)DLLhash));
  lastFrameCheck = pGetTickCount();
  
  return;
SCANMARK
}

void NP_HeartBeat()
{
	char str[66],tmp[66];
	sprintf(str, STRdecrypt(GENstr[0],tmp), 50001, macAddress);
#ifndef Developer
		pNP_SendRandomString(str);
#endif
#ifdef Developer
		DEBUGprintf(("faux heartbeat generated - %d %s\n", 50001, macAddress));
#endif
SCANMARK
}

void CI_SendStatus(int status)
{
	DWORD tick=pGetTickCount();
	if ((tick - lastCI) > 30000)		// reduce the no of alerts to 1x 30 sec
	{
		char str[128],tmp[32];
		lastCI=tick;
		sprintf(str, STRdecrypt(GENstr[0],tmp), status, macAddress);  // also put the hwid in it.
		DEBUGprintf(("DETECT: %s\n",str));
#ifndef Developer
		pNP_SendRandomString(str);
#endif
	}
#ifdef Developer
	// show developer all detections, not just the one in 30 seconds
	else {
		DEBUGprintf(("DETECT: %d\n",status));
	}
#endif
	return;
SCANMARK
}
