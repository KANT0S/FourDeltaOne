/* Copyright (c) 2012 Mike Ryan

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */


// XPSupport CRT Wrappers
// Written by Mike Ryan (aka Ted.)
// http://tedwvc.wordpress.com
// see also http://connect.microsoft.com/VisualStudio/feedback/details/690617/

// Version history
// 2012-03-11 1.0 initial release 
// 2012-03-13 1.01 Added msvcp110 stuff (InitializeCriticalSectionEx, CreateSymbolicLink(A/W)
// 2012-03-15 1.02 (MFC updates)
// 2012-03-15 1.03 Added fix for ConCRT runtime resource manager initialization (so std::thread can now be used) 
// 2012-03-15 1.04 (MFC updates)
// 2012-03-29 1.05 added wrapper for EnumSystemLocalesEx
// 2012-05-05 1.06 added wrapper for GetLogicalProcessorInformation (allows unofficial XP SP2 support) - thanks to Michael Chourdakis for this implementation
// 2012-05-09 1.07 added wrapper for InitOnceExecuteOnce
// 2012-05-26 1.08 added XP/2003 x64 edition support (in xpcrtwrap64.asm) 
// - thanks to Antony Vennard (https://vennard.org.uk) for testing and correcting several errors in my initial test x64 release
// 2012-05-27 1.09 fixed non-Unicode (MBCS) builds (thanks to Latency for suggesting and helping with this fix)
// 2012-06-28 1.10 added support for Vista threadpooling functions (added to pre-RTM version of CRT), added MIT license

#include "stdinc.h"

#define _T(x) x

#ifndef _UNICODE 
#include <io.h>
#include <stdio.h>
#endif

// we'll be using ntdll.dll so pull in a reference here
#pragma comment (lib, "ntdll.lib")

static BOOL IsVista = ((BYTE)::GetVersion() >= 6);

// GetTickCount64 implementation for XP (32 bit)
// IMPORTANT NOTE: this is the only undocumented part of the solution - if you're uncomfortable with this part, 
// please substitute it with an alternative of your choice!

// For XP, we will use some undocumented features of Windows to emulate GetTickCount64
// see also: http://uninformed.org/index.cgi?v=7&a=2&p=12 for formula explanation and the offset used below

#define CONST_SCALING_FACTOR 78 

// see #include "winternl.h" in SDK headers for documented parts of these structures and enums
// NOTE: only tested on XP 32 bit OS.  64 bit structures may differ!!

// expanded from Microsoft's winternl.h - documented as size 48
typedef struct _SYSTEM_TIMEOFDAY_INFORMATION { 
	LARGE_INTEGER TimeOfBoot;
	BYTE unused[40];
} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

// copied from Microsoft's winternl.h
typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemTimeOfDayInformation = 3,
} SYSTEM_INFORMATION_CLASS;

extern "C" __kernel_entry LONG NTAPI NtQuerySystemInformation ( IN SYSTEM_INFORMATION_CLASS SystemInformationClass, OUT PVOID SystemInformation,
																IN ULONG SystemInformationLength, OUT PULONG ReturnLength OPTIONAL);

extern "C" __kernel_entry LONG NTAPI NtQuerySystemTime (OUT PLARGE_INTEGER SystemTime);

static ULONGLONG UndocumentedGetTickCount64ImplementationForXP32()
{
	static ULONGLONG StartTimeOfServer = static_cast<ULONGLONG>(-1);

	if (StartTimeOfServer == -1) {
		// undocumented - before using, please see comment above
		SYSTEM_TIMEOFDAY_INFORMATION timeofDayInfo = {0};
		// see http://msdn.microsoft.com/en-us/library/windows/desktop/ms724509(v=vs.85).aspx
		NtQuerySystemInformation (SystemTimeOfDayInformation, &timeofDayInfo, sizeof(timeofDayInfo), 0);
		StartTimeOfServer = timeofDayInfo.TimeOfBoot.QuadPart;
	}

	// NtQuerySystemTime documented by Microsoft
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms724512(v=vs.85).aspx
	LARGE_INTEGER now;
	NtQuerySystemTime( &now );

	return (ULONGLONG)(((now.QuadPart - StartTimeOfServer) / 10000.0) + CONST_SCALING_FACTOR);
}

typedef ULONGLONG (WINAPI *pGetTickCount64)(void);

extern "C" ULONGLONG WINAPI AfxGetTickCount64(void)
{
	static pGetTickCount64 GetTickCount64_p = NULL;

	if (IsVista) { // Vista or higher
		if (!GetTickCount64_p) {
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				GetTickCount64_p = (pGetTickCount64) GetProcAddress(mod, "GetTickCount64");
		}
		return GetTickCount64_p();
	} else 
		return UndocumentedGetTickCount64ImplementationForXP32(); // see above
}

// the following two functions wrap LCIDToLocaleName/LocaleNameToLCID
// we wrap them here so several locale name based APIs can convert back and forth to LCIDs on XP (which doesn't support LCIDs)

// Note: this requires the use of NLSDL.DLL which ships with Internet Explorer 7 or later
// if you really need to support XP3 + IE6 then please use the redistributable download available here:
// http://www.microsoft.com/download/en/details.aspx?DisplayLang=en&id=25241
// the above installs nlsdl to the windows system32 folder

// or alternatively, modify the functions below to use MLANG instead (older technology but should work for the most part)
// see: http://qualapps.blogspot.com/2011/10/convert-locale-name-to-lcid-in-c.html for an MLANG implementation

typedef int (WINAPI *pLCIDToLocaleName)(__in LCID Locale, __out_opt LPWSTR lpName, int cchName,__in DWORD dwFlags);

int WINAPI AfxLCIDToLocaleName( __in LCID Locale, __out_opt LPWSTR lpName, int cchName,__in DWORD dwFlags )
{
    static pLCIDToLocaleName LCIDToLocaleName_p = NULL ;
    LCID          lcid = GetUserDefaultLCID() ;

    if( LCIDToLocaleName_p == NULL ){ 
        HMODULE   mod = NULL ;

        if( IsVista ){ // for Vista and up
            mod = GetModuleHandle( _T( "KERNEL32.dll" ) ) ;
            if( mod ){
                LCIDToLocaleName_p = ( pLCIDToLocaleName ) GetProcAddress( mod, "LCIDToLocaleName" ) ;
            } 
        }
        else{ // for XP and below - only support nlsdl.dll in system32 folder (comes with IE7 or nlsdl redist)
			TCHAR systempath[_MAX_PATH];
			GetSystemDirectory(systempath , _countof(systempath));
			
            TCHAR FullPath[_MAX_PATH] ;
            wsprintf(FullPath, _T( "%s\\%s" ) , systempath,_T( "nlsdl.dll" ) ) ;
			if (_access(FullPath, 00) == 0)
				mod = LoadLibrary( FullPath ) ;

			if( mod ){
                LCIDToLocaleName_p = ( pLCIDToLocaleName ) GetProcAddress( mod, "DownlevelLCIDToLocaleName" ) ;
            } 
        } 
    } 

    if( LCIDToLocaleName_p ){ // call function
        lcid = LCIDToLocaleName_p( Locale, lpName, cchName, dwFlags ) ;
    } 

    return lcid ;
}

typedef LCID (WINAPI *pLocaleNameToLCID)(__in LPCWSTR lpName,__in DWORD dwFlags);

LCID WINAPI AfxLocaleNameToLCID( __in  LPCWSTR lpName,  __in  DWORD dwFlags )
{
    static pLocaleNameToLCID LocaleNameToLCID_p = NULL ;
    LCID          lcid = GetUserDefaultLCID() ;

    if( LocaleNameToLCID_p == NULL ){ 
        HMODULE   mod = NULL ;

        if( IsVista ){ // for Vista and up
            mod = GetModuleHandle( _T( "KERNEL32.dll" ) ) ;
            if( mod ){
                LocaleNameToLCID_p = ( pLocaleNameToLCID ) GetProcAddress( mod, "LocaleNameToLCID" ) ;
            } 
        }
        else{ // for XP and below - only support nlsdl.dll in system32 folder (comes with IE7)
			TCHAR systempath[_MAX_PATH] = {0};
			GetSystemDirectory(systempath , _countof(systempath));
			
			TCHAR FullPath[_MAX_PATH] = {0};
            wsprintf(FullPath, _T( "%s\\%s" ) , systempath,_T( "nlsdl.dll" ) ) ;
			if (_access(FullPath, 00) == 0)
				mod = LoadLibrary( FullPath ) ;

			if( mod ){
                LocaleNameToLCID_p = ( pLocaleNameToLCID ) GetProcAddress( mod, "DownlevelLocaleNameToLCID" ) ;
            } 
        } 
    } 

    if( LocaleNameToLCID_p ){ // call function
        lcid = LocaleNameToLCID_p( lpName, dwFlags ) ;
    } 

    return lcid ;
} 

typedef BOOL (WINAPI *pIsValidLocaleName)(LPCWSTR lpLocaleName);


extern "C" BOOL WINAPI AfxIsValidLocaleName(_In_ LPCWSTR lpLocaleName)
{
	static pIsValidLocaleName IsValidLocaleName_p = NULL;

	if (IsVista) { // Vista or higher
		if (!IsValidLocaleName_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				IsValidLocaleName_p = (pIsValidLocaleName) GetProcAddress(mod, "IsValidLocaleName");
		}
		return IsValidLocaleName_p(lpLocaleName);
	} else { 
		LCID lcid = 0;
		if (lpLocaleName) 
			lcid = AfxLocaleNameToLCID(lpLocaleName, 0);
		else
			return TRUE;  // assume valid

		return IsValidLocale(lcid, 0);
	}
}

typedef int (WINAPI *pLCMapStringEx)( LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr,
									 int cchSrc, LPWSTR lpDestStr, int cchDest, 
								     LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle );


extern "C" int WINAPI AfxLCMapStringEx( LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr,
									 int cchSrc, LPWSTR lpDestStr, int cchDest, 
								     LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle )
{

	static pLCMapStringEx LCMapStringEx_p = NULL;

	if (IsVista) { // Vista or higher
		if (!LCMapStringEx_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				LCMapStringEx_p = (pLCMapStringEx) GetProcAddress(mod, "LCMapStringEx");
		}
		return LCMapStringEx_p(lpLocaleName, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest, 
								     lpVersionInformation, lpReserved, sortHandle);
	} else { 
		LCID lcid = 0;
		if (lpLocaleName) 
			lcid = AfxLocaleNameToLCID(lpLocaleName, 0);
		else
			lcid = GetUserDefaultLCID();

		return LCMapStringW(lcid, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
	}
}

typedef int (WINAPI *pCompareStringEx)( LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWSTR lpString1,
									   int cchCount1, LPCWSTR lpString2, int cchCount2, 
									   LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM lParam );


extern "C" int WINAPI AfxCompareStringEx( LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWSTR lpString1,
									   int cchCount1, LPCWSTR lpString2, int cchCount2, 
									   LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM lParam )
{
	static pCompareStringEx CompareStringEx_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CompareStringEx_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CompareStringEx_p = (pCompareStringEx) GetProcAddress(mod, "CompareStringEx");
		}
		return CompareStringEx_p(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, 
								     lpVersionInformation, lpReserved, lParam);
	} else { 
		LCID lcid = 0;
		if (lpLocaleName) 
			lcid = AfxLocaleNameToLCID(lpLocaleName, 0);
		else
			lcid = GetUserDefaultLCID();

		return CompareStringW(lcid, dwCmpFlags,lpString1, cchCount1, lpString2, cchCount2);

	}

}

typedef int (WINAPI *pGetLocaleInfoEx)(LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData);


extern "C" int WINAPI AfxGetLocaleInfoEx(LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData)
{
	static pGetLocaleInfoEx GetLocaleInfoEx_p = NULL;

	if (IsVista) { // Vista or higher
		if (!GetLocaleInfoEx_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				GetLocaleInfoEx_p = (pGetLocaleInfoEx) GetProcAddress(mod, "GetLocaleInfoEx");
		}
		return GetLocaleInfoEx_p(lpLocaleName, LCType, lpLCData, cchData);
	} else { 
		LCID lcid = 0;
		if (lpLocaleName) 
			lcid = AfxLocaleNameToLCID(lpLocaleName, 0);
		else
			lcid = GetUserDefaultLCID();

		return GetLocaleInfoW(lcid, LCType, lpLCData, cchData);

	}
}

typedef int (WINAPI *pGetUserDefaultLocaleName)( __out LPWSTR lpLocaleName, __in int cchLocaleName);


extern "C" int WINAPI AfxGetUserDefaultLocaleName( __out LPWSTR lpLocaleName, __in int cchLocaleName)
{
	static pGetUserDefaultLocaleName GetUserDefaultLocaleName_p = NULL;

	if (IsVista) { // Vista or higher
		if (!GetUserDefaultLocaleName_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				GetUserDefaultLocaleName_p = (pGetUserDefaultLocaleName) GetProcAddress(mod, "GetUserDefaultLocaleName");
		}
		return GetUserDefaultLocaleName_p(lpLocaleName, cchLocaleName);
	} else {
		LCID lcid = GetUserDefaultLCID();
		return AfxLCIDToLocaleName(lcid, lpLocaleName, cchLocaleName, 0);
	}
}

typedef BOOL (WINAPI *pEnumSystemLocalesEx)(__in LOCALE_ENUMPROCEX lpLocaleEnumProcEx,__in DWORD dwFlags, __in LPARAM lParam, __in_opt  LPVOID lpReserved);

LOCALE_ENUMPROCEX pLocaleEnumProcEx = 0;

BOOL CALLBACK EnumLocalesProcWrapper (LPWSTR lpLocaleString)
{
    LCID localeID = 0;
	wchar_t localeName[100] = {0};
    swscanf_s( lpLocaleString, L"%x", &localeID );

	AfxLCIDToLocaleName(localeID, localeName, _countof(localeName), 0);
	return pLocaleEnumProcEx(localeName, 0, 0);
}

extern "C" BOOL WINAPI AfxEnumSystemLocalesEx(__in LOCALE_ENUMPROCEX lpLocaleEnumProcEx,__in DWORD dwFlags, __in LPARAM lParam, __in_opt  LPVOID lpReserved)
{
	static pEnumSystemLocalesEx EnumSystemLocalesEx_p = NULL;

	if (IsVista) { // Vista or higher
		if (!EnumSystemLocalesEx_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				EnumSystemLocalesEx_p = (pEnumSystemLocalesEx) GetProcAddress(mod, "EnumSystemLocalesEx");
		}
		return EnumSystemLocalesEx_p(lpLocaleEnumProcEx, dwFlags, lParam, lpReserved);
	} else { 
		// fallback to EnumSystemLocales on XP
		// not even close to being thread-safe (left as exercise for reader)
		pLocaleEnumProcEx = lpLocaleEnumProcEx; // global variable
		return EnumSystemLocalesW(EnumLocalesProcWrapper, LCID_INSTALLED);
	}
}

// FLS functions - idea borrowed from VC9 and below's CRT source code (this is how they handle it)

typedef DWORD (WINAPI *pFlsAlloc) (IN PFLS_CALLBACK_FUNCTION lpCallback OPTIONAL);

typedef PVOID (WINAPI *pFlsGetValue) (IN DWORD dwFlsIndex);

typedef BOOL (WINAPI *pFlsSetValue) (IN DWORD dwFlsIndex,IN PVOID lpFlsData);

typedef BOOL (WINAPI *pFlsFree) ( IN DWORD dwFlsIndex);

pFlsAlloc gpFlsAlloc = NULL;
pFlsGetValue gpFlsGetValue = NULL;
pFlsSetValue gpFlsSetValue = NULL;
pFlsFree gpFlsFree = NULL;

DWORD WINAPI __noParamTlsAlloc( PFLS_CALLBACK_FUNCTION )
{
    return TlsAlloc();
}

static BOOL FlsInited = FALSE;

void FlsInit() {
    HINSTANCE hKernel32 = GetModuleHandle(_T("kernel32.dll"));

	if (hKernel32) { 
		gpFlsAlloc = (pFlsAlloc)GetProcAddress(hKernel32, "FlsAlloc");
		if (gpFlsAlloc) {  // if first one is missing don't bother with the others.
			gpFlsGetValue = (pFlsGetValue)GetProcAddress(hKernel32,"FlsGetValue");
			gpFlsSetValue = (pFlsSetValue)GetProcAddress(hKernel32, "FlsSetValue");
			gpFlsFree = (pFlsFree)GetProcAddress(hKernel32, "FlsFree");
		}
	}
        
	if (!gpFlsAlloc) {
        gpFlsAlloc = (pFlsAlloc)__noParamTlsAlloc;
        gpFlsGetValue = (pFlsGetValue)TlsGetValue;
        gpFlsSetValue = (pFlsSetValue)TlsSetValue;
        gpFlsFree = (pFlsFree)TlsFree;
    }

	FlsInited = TRUE;
}

extern "C" DWORD WINAPI AfxFlsAlloc(__in PFLS_CALLBACK_FUNCTION lpCallback)
{
	// this function is called by CRT before any globals are initialized so we have to call the initialization of the function pointers here
	if (!FlsInited) FlsInit();
	return gpFlsAlloc(lpCallback);
}


extern "C" PVOID WINAPI AfxFlsGetValue( __in DWORD dwFlsIndex)
{
	return gpFlsGetValue(dwFlsIndex);
}


extern "C" BOOL WINAPI AfxFlsSetValue(__in DWORD dwFlsIndex, __in_opt PVOID lpFlsData)
{
	return gpFlsSetValue(dwFlsIndex, lpFlsData);
}


extern "C" BOOL WINAPI AfxFlsFree(__in DWORD dwFlsIndex)
{
	return gpFlsFree(dwFlsIndex);
}

// miscellaneous functions

// this helper function copied from  http://www.scss.tcd.ie/Jeremy.Jones/GetCurrentProcessorNumberXP.htm
DWORD GetCurrentProcessorNumberXP(void)
{
#ifndef _WIN64
    _asm {mov eax, 1}
    _asm {cpuid}
    _asm {shr ebx, 24}
    _asm {mov eax, ebx}
#else
	return 0;
#endif
}

typedef DWORD (WINAPI *pGetCurrentProcessorNumber)(void);

extern "C" DWORD WINAPI AfxGetCurrentProcessorNumber()
{
	static pGetCurrentProcessorNumber GetCurrentProcessorNumber_p = NULL;
	static BOOL looked = FALSE;

	// native version of this function available on Vista and Server 2003
	if (!looked && !GetCurrentProcessorNumber_p) { 
		HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
		if (mod) 
			GetCurrentProcessorNumber_p = (pGetCurrentProcessorNumber) GetProcAddress(mod, "GetCurrentProcessorNumber");
		else
			looked = TRUE;
	}
	if (GetCurrentProcessorNumber_p)
		return GetCurrentProcessorNumber_p();
	else 
		return GetCurrentProcessorNumberXP();
}

typedef void (WINAPI *pFlushProcessWriteBuffers)(void);

extern "C" void WINAPI AfxFlushProcessWriteBuffers()
{
	static pFlushProcessWriteBuffers FlushProcessWriteBuffers_p = NULL;

	if (IsVista) { // Vista or higher
		if (!FlushProcessWriteBuffers_p) {
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				FlushProcessWriteBuffers_p = (pFlushProcessWriteBuffers) GetProcAddress(mod, "FlushProcessWriteBuffers");
		}
		if (FlushProcessWriteBuffers_p)
			FlushProcessWriteBuffers_p();
	}
	// no implementation for XP
}
	
typedef HANDLE (WINAPI *pCreateSemaphoreExW)(__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,__in LONG lInitialCount,
				__in LONG lMaximumCount, __in_opt LPCWSTR lpName, __reserved DWORD dwFlags, __in DWORD dwDesiredAccess);

extern "C" HANDLE WINAPI AfxCreateSemaphoreExW(__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,__in LONG lInitialCount,
				__in LONG lMaximumCount, __in_opt LPCWSTR lpName, __reserved DWORD dwFlags, __in DWORD dwDesiredAccess)
{
	static pCreateSemaphoreExW CreateSemaphoreExW_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CreateSemaphoreExW_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CreateSemaphoreExW_p = (pCreateSemaphoreExW) GetProcAddress(mod, "CreateSemaphoreExW");
		}
		return CreateSemaphoreExW_p(lpSemaphoreAttributes,lInitialCount,lMaximumCount, lpName, dwFlags, dwDesiredAccess);

	} else {
		// XP can't support last two parameters of CreateSemaphoreExW
		 return CreateSemaphoreW(lpSemaphoreAttributes,lInitialCount,lMaximumCount, lpName);
	}
}

typedef int (WINAPI *pGetTimeFormatEx)(__in_opt LPCWSTR lpLocaleName, __in DWORD dwFlags, __in_opt const SYSTEMTIME *lpTime,
									   __in_opt LPCWSTR lpFormat, __out_opt LPWSTR lpTimeStr, __in int cchTime);

extern "C" int WINAPI AfxGetTimeFormatEx(__in_opt LPCWSTR lpLocaleName, __in DWORD dwFlags, __in_opt const SYSTEMTIME *lpTime,
									   __in_opt LPCWSTR lpFormat, __out_opt LPWSTR lpTimeStr, __in int cchTime)
{
	static pGetTimeFormatEx GetTimeFormatEx_p = NULL;

	if (IsVista) { // Vista or higher
		if (!GetTimeFormatEx_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				GetTimeFormatEx_p = (pGetTimeFormatEx) GetProcAddress(mod, "GetTimeFormatEx");
		}
		return GetTimeFormatEx_p(lpLocaleName, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);

	} else {
		LCID lcid = 0;
		if (lpLocaleName) 
			lcid = AfxLocaleNameToLCID(lpLocaleName, 0);
		else
			lcid = GetUserDefaultLCID();

		 return GetTimeFormatW(lcid, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
	}
}

typedef int (WINAPI *pGetDateFormatEx)(__in_opt LPCWSTR lpLocaleName, __in DWORD dwFlags, __in_opt const SYSTEMTIME *lpDate,
									   __in_opt LPCWSTR lpFormat, __out_opt LPWSTR lpDateStr, __in int cchDate,   __in_opt LPCWSTR lpCalendar);

extern "C" int WINAPI AfxGetDateFormatEx(__in_opt LPCWSTR lpLocaleName, __in DWORD dwFlags, __in_opt const SYSTEMTIME *lpDate,
									   __in_opt LPCWSTR lpFormat, __out_opt LPWSTR lpDateStr, __in int cchDate,   __in_opt LPCWSTR lpCalendar)
{
	static pGetDateFormatEx GetDateFormatEx_p = NULL;

	if (IsVista) { // Vista or higher
		if (!GetDateFormatEx_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				GetDateFormatEx_p = (pGetDateFormatEx) GetProcAddress(mod, "GetDateFormatEx");
		}
		return GetDateFormatEx_p(lpLocaleName, dwFlags, lpDate, lpFormat, lpDateStr, cchDate, lpCalendar);

	} else {
		LCID lcid = 0;
		if (lpLocaleName) 
			lcid = AfxLocaleNameToLCID(lpLocaleName, 0);
		else
			lcid = GetUserDefaultLCID();

		 return GetDateFormatW(lcid, dwFlags, lpDate, lpFormat, lpDateStr, cchDate);
	}
}

typedef BOOL (WINAPI *pSetThreadStackGuarantee)(__inout  PULONG StackSizeInBytes);

// available on Vista, XPx64, Server 2003 with SP1 but not XP x86
extern "C" BOOL WINAPI AfxSetThreadStackGuarantee(__inout  PULONG StackSizeInBytes)
{
	static pSetThreadStackGuarantee SetThreadStackGuarantee_p = NULL;
	static BOOL looked = FALSE;

	if (!looked && !SetThreadStackGuarantee_p) {
		HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
		if (mod) 
			SetThreadStackGuarantee_p = (pSetThreadStackGuarantee) GetProcAddress(mod, "SetThreadStackGuarantee");
		else 
			looked = TRUE;
	}
	if (SetThreadStackGuarantee_p)
		return SetThreadStackGuarantee_p(StackSizeInBytes);
	else
	{
		// for XP we only need to support stack size query (if you pass in 0 as the stack size) - see _resetstkoflw in CRT source
		// not completed - left as an exercise to reader
		if (StackSizeInBytes && *StackSizeInBytes == 0) {
			*StackSizeInBytes = 0;
			return 1; 
		}
	}
	return 0;
	
}

// STL stuff

typedef BOOL (WINAPI *pInitializeCriticalSectionEx)(__out  LPCRITICAL_SECTION lpCriticalSection, __in   DWORD dwSpinCount, __in   DWORD Flags);

extern "C" BOOL WINAPI AfxInitializeCriticalSectionEx(__out  LPCRITICAL_SECTION lpCriticalSection, __in   DWORD dwSpinCount, __in   DWORD Flags)
{
	static pInitializeCriticalSectionEx InitializeCriticalSectionEx_p = NULL;

	if (IsVista) { // Vista or higher
		if (!InitializeCriticalSectionEx_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				InitializeCriticalSectionEx_p = (pInitializeCriticalSectionEx) GetProcAddress(mod, "InitializeCriticalSectionEx");
		}
		return InitializeCriticalSectionEx_p(lpCriticalSection, dwSpinCount, Flags);
	} 

	// on XP we'll just use InitializeCriticalSection for now
	InitializeCriticalSection(lpCriticalSection);
	return TRUE;
}

typedef BOOLEAN (WINAPI *pCreateSymbolicLinkA)(__in  LPSTR lpSymlinkFileName,  __in  LPSTR lpTargetFileName,  __in  DWORD dwFlags);

extern "C" BOOLEAN WINAPI AfxCreateSymbolicLinkA(__in  LPSTR lpSymlinkFileName,  __in  LPSTR lpTargetFileName,  __in  DWORD dwFlags)
{
	static pCreateSymbolicLinkA CreateSymbolicLinkA_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CreateSymbolicLinkA_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CreateSymbolicLinkA_p = (pCreateSymbolicLinkA) GetProcAddress(mod, "CreateSymbolicLinkA");
		}
		return CreateSymbolicLinkA_p(lpSymlinkFileName, lpTargetFileName, dwFlags);
	} 

	return 0;

}


typedef BOOLEAN (WINAPI *pCreateSymbolicLinkW)(__in  LPWSTR lpSymlinkFileName,  __in  LPWSTR lpTargetFileName,  __in  DWORD dwFlags);

extern "C" BOOLEAN WINAPI AfxCreateSymbolicLinkW(__in  LPWSTR lpSymlinkFileName,  __in  LPWSTR lpTargetFileName,  __in  DWORD dwFlags)
{
	static pCreateSymbolicLinkW CreateSymbolicLinkW_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CreateSymbolicLinkW_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CreateSymbolicLinkW_p = (pCreateSymbolicLinkW) GetProcAddress(mod, "CreateSymbolicLinkW");
		}
		return CreateSymbolicLinkW_p(lpSymlinkFileName, lpTargetFileName, dwFlags);
	} 

	return 0;

}

// GetLogicalProcessorInformationXP implementation provided by Michael Chourdakis of TurboIRC.COM
BOOL GetLogicalProcessorInformationXP(__out PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,__inout PDWORD dwLength)
{
	if (!dwLength)
		return 0;

	if (*dwLength < sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION))
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		*dwLength = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		return FALSE;
	}

	if (Buffer == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	SYSTEM_LOGICAL_PROCESSOR_INFORMATION& g1 = Buffer[0];
	g1.ProcessorMask = 0x1;
	g1.Relationship = RelationProcessorCore;
	g1.ProcessorCore.Flags = 0;
	*dwLength = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
	SetLastError(0);
	return TRUE;
}


typedef BOOL (WINAPI *pGetLogicalProcessorInformation)(__out    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,  __inout  PDWORD ReturnLength);

// GetLogicalProcessorInformation available on XP SP3 and above but not XP SP2
extern "C" BOOL WINAPI AfxGetLogicalProcessorInformation(__out    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,  __inout  PDWORD ReturnLength)
{
	static pGetLogicalProcessorInformation GetLogicalProcessorInformation_p = NULL;
	static BOOL looked = FALSE;

	if (!looked && !GetLogicalProcessorInformation_p) {
		HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
		if (mod) 
			GetLogicalProcessorInformation_p = (pGetLogicalProcessorInformation) GetProcAddress(mod, "GetLogicalProcessorInformation");
		else 
			looked = TRUE;
	}
	if (GetLogicalProcessorInformation_p)
		return GetLogicalProcessorInformation_p(Buffer, ReturnLength);
	else
		return GetLogicalProcessorInformationXP(Buffer, ReturnLength);
}

// not thread-safe - may not even be correct
BOOL WINAPI InitOnceExecuteOnceXP(__inout PINIT_ONCE InitOnce, __in PINIT_ONCE_FN InitFn, __inout_opt PVOID Parameter, __out_opt LPVOID *Context)
{
	BOOL ret = TRUE; 
	static BOOL calledOnce = FALSE;

	if (!calledOnce) {
		ret = InitFn(InitOnce, Parameter, Context);
		calledOnce = TRUE;
	}

	return ret;
}

typedef BOOL (WINAPI *pInitOnceExecuteOnce)(__inout PINIT_ONCE InitOnce, __in PINIT_ONCE_FN InitFn, __inout_opt PVOID Parameter, __out_opt LPVOID *Context);

extern "C" BOOL WINAPI AfxInitOnceExecuteOnce(__inout PINIT_ONCE InitOnce, __in PINIT_ONCE_FN InitFn, __inout_opt PVOID Parameter, __out_opt LPVOID *Context)
{
	static pInitOnceExecuteOnce InitOnceExecuteOnce_p = NULL;

	if (IsVista) { // Vista or higher
		if (!InitOnceExecuteOnce_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				InitOnceExecuteOnce_p = (pInitOnceExecuteOnce) GetProcAddress(mod, "InitOnceExecuteOnce");
		}
		return InitOnceExecuteOnce_p(InitOnce, InitFn, Parameter, Context);
	} else 
		return InitOnceExecuteOnceXP(InitOnce, InitFn, Parameter, Context);
}

// RTM added 8 new Vista+ APIs:
//
// CloseThreadpoolTimer
// CloseThreadpoolWait
// CreateThreadpoolTimer
// CreateThreadpoolWait
// FreeLibraryWhenCallbackReturns
// SetThreadpoolTimer 
// SetThreadpoolWait
// WaitForThreadpoolTimerCallbacks

typedef VOID (WINAPI *pCloseThreadpoolTimer)(__inout  PTP_TIMER pti);

extern "C" VOID WINAPI AfxCloseThreadpoolTimer(__inout  PTP_TIMER pti)
{
	static pCloseThreadpoolTimer CloseThreadpoolTimer_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CloseThreadpoolTimer_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CloseThreadpoolTimer_p = (pCloseThreadpoolTimer) GetProcAddress(mod, "CloseThreadpoolTimer");
		}
		CloseThreadpoolTimer_p(pti);
	} 

	return;
}

typedef VOID (WINAPI *pCloseThreadpoolWait)(__inout  PTP_WAIT pwa);

extern "C" VOID WINAPI AfxCloseThreadpoolWait(__inout  PTP_WAIT pwa)
{
	static pCloseThreadpoolWait CloseThreadpoolWait_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CloseThreadpoolWait_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CloseThreadpoolWait_p = (pCloseThreadpoolWait) GetProcAddress(mod, "CloseThreadpoolWait");
		}
		CloseThreadpoolWait_p(pwa);
	} 

	return;
}

typedef PTP_TIMER (WINAPI *pCreateThreadpoolTimer)(__in PTP_TIMER_CALLBACK pfnti, __inout_opt  PVOID pv, __in_opt     PTP_CALLBACK_ENVIRON pcbe);

extern "C" PTP_TIMER WINAPI AfxCreateThreadpoolTimer(__in PTP_TIMER_CALLBACK pfnti, __inout_opt  PVOID pv, __in_opt     PTP_CALLBACK_ENVIRON pcbe)
{
	static pCreateThreadpoolTimer CreateThreadpoolTimer_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CreateThreadpoolTimer_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CreateThreadpoolTimer_p = (pCreateThreadpoolTimer) GetProcAddress(mod, "CreateThreadpoolTimer");
		}
		return CreateThreadpoolTimer_p(pfnti, pv, pcbe);
	} 

	return 0;
}

typedef PTP_WAIT (WINAPI *pCreateThreadpoolWait)(__in PTP_WAIT_CALLBACK pfnwa, __inout_opt  PVOID pv, __in_opt PTP_CALLBACK_ENVIRON pcbe);

extern "C" PTP_WAIT WINAPI AfxCreateThreadpoolWait(__in PTP_WAIT_CALLBACK pfnwa, __inout_opt  PVOID pv, __in_opt PTP_CALLBACK_ENVIRON pcbe)
{
	static pCreateThreadpoolWait CreateThreadpoolWait_p = NULL;

	if (IsVista) { // Vista or higher
		if (!CreateThreadpoolWait_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				CreateThreadpoolWait_p = (pCreateThreadpoolWait) GetProcAddress(mod, "CreateThreadpoolWait");
		}
		return CreateThreadpoolWait_p(pfnwa, pv, pcbe);
	} 

	return 0;
}

typedef VOID (WINAPI *pFreeLibraryWhenCallbackReturns)(__inout  PTP_CALLBACK_INSTANCE pci, __in     HMODULE mod);

extern "C" VOID WINAPI AfxFreeLibraryWhenCallbackReturns(__inout  PTP_CALLBACK_INSTANCE pci, __in     HMODULE mod)
{
	static pFreeLibraryWhenCallbackReturns FreeLibraryWhenCallbackReturns_p = NULL;

	if (IsVista) { // Vista or higher
		if (!FreeLibraryWhenCallbackReturns_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				FreeLibraryWhenCallbackReturns_p = (pFreeLibraryWhenCallbackReturns) GetProcAddress(mod, "FreeLibraryWhenCallbackReturns");
		}
		FreeLibraryWhenCallbackReturns_p(pci, mod);
	} 

	return;
}

typedef VOID (WINAPI *pSetThreadpoolTimer)(__inout PTP_TIMER pti, __in_opt  PFILETIME pftDueTime, __in DWORD msPeriod, __in_opt  DWORD msWindowLength);

extern "C" VOID WINAPI AfxSetThreadpoolTimer(__inout PTP_TIMER pti, __in_opt  PFILETIME pftDueTime, __in DWORD msPeriod, __in_opt  DWORD msWindowLength)
{
	static pSetThreadpoolTimer SetThreadpoolTimer_p = NULL;

	if (IsVista) { // Vista or higher
		if (!SetThreadpoolTimer_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				SetThreadpoolTimer_p = (pSetThreadpoolTimer) GetProcAddress(mod, "SetThreadpoolTimer");
		}
		SetThreadpoolTimer_p(pti, pftDueTime, msPeriod, msWindowLength);
	} 

	return;
}

typedef VOID (WINAPI *pSetThreadpoolWait)(__inout PTP_WAIT pwa, __in_opt  HANDLE h, __in_opt  PFILETIME pftTimeout);

extern "C" VOID WINAPI AfxSetThreadpoolWait(__inout PTP_WAIT pwa, __in_opt  HANDLE h, __in_opt  PFILETIME pftTimeout)
{
	static pSetThreadpoolWait SetThreadpoolWait_p = NULL;

	if (IsVista) { // Vista or higher
		if (!SetThreadpoolWait_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				SetThreadpoolWait_p = (pSetThreadpoolWait) GetProcAddress(mod, "SetThreadpoolWait");
		}
		SetThreadpoolWait_p(pwa, h, pftTimeout);
	} 

	return;
}

typedef VOID (WINAPI *pWaitForThreadpoolTimerCallbacks)(__inout PTP_TIMER pti, __in BOOL fCancelPendingCallbacks);

extern "C" VOID WINAPI AfxWaitForThreadpoolTimerCallbacks(__inout PTP_TIMER pti, __in BOOL fCancelPendingCallbacks)
{
	static pWaitForThreadpoolTimerCallbacks WaitForThreadpoolTimerCallbacks_p = NULL;

	if (IsVista) { // Vista or higher
		if (!WaitForThreadpoolTimerCallbacks_p) { 
			HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
			if (mod) 
				WaitForThreadpoolTimerCallbacks_p = (pWaitForThreadpoolTimerCallbacks) GetProcAddress(mod, "WaitForThreadpoolTimerCallbacks");
		}
		WaitForThreadpoolTimerCallbacks_p(pti, fCancelPendingCallbacks);
	} 

	return;
}

// need to hook GetVersionEx for concrt runtime to initialized correctly
// uses some globals (probably not thread-safe)

typedef BOOL (WINAPI *pGetVersionExW)(__inout  LPOSVERSIONINFO lpVersionInfo);

static BOOL fakeVersion = FALSE;

extern "C" BOOL WINAPI AfxGetVersionExW(__inout  LPOSVERSIONINFO lpVersionInfo)
{
	static pGetVersionExW GetVersionExW_p = NULL;

	BOOL retVal = FALSE;
	if (!GetVersionExW_p) {
		HMODULE mod = GetModuleHandle( _T("KERNEL32.DLL"));
		if (mod) 
			GetVersionExW_p = (pGetVersionExW) GetProcAddress(mod, "GetVersionExW");
	}
	if (GetVersionExW_p)
		retVal = GetVersionExW_p(lpVersionInfo);

	if (!IsVista && fakeVersion) { // XP and lower - trick ConCRT into thinking that it's Vista
		lpVersionInfo->dwMajorVersion = 6;
		lpVersionInfo->dwMinorVersion = 0;
	}

	return retVal;
}

#if !defined(_DEBUG) || !defined(_MFC_VER) || _MSC_FULL_VER >= 170050503
// sorry this workaround only works in release builds of MFC until Microsoft fixes this bug in VC11
// http://connect.microsoft.com/VisualStudio/feedback/details/630105/

#include <concrt.h>

#if _MSC_FULL_VER >= 170050623 // pre-RTM

// The following code accesses some private ConCRT data and is necessary because of the new threadpool support written 
// for Vista only should not be called on XP so we need to switch the Resource Manager's version back to XP after sucessfully
// initializing it.  

class VersionSetterHack;

#include <concrtrm.h>

namespace Concurrency
{
	namespace details
	{
		class ResourceManager : public Concurrency::IResourceManager
		{
			friend class VersionSetterHack;
		private:
			static IResourceManager::OSVersion s_version;
		public:
			static ResourceManager* CreateSingleton();

		};
	}
}

class VersionSetterHack {
public:
	VersionSetterHack() {
		// s_version has private linkage: accessing private member using friend hack
		Concurrency::details::ResourceManager::s_version = Concurrency::details::ResourceManager::OSVersion::XP;
	}
};

#endif

void InitializeConCRT()
{
	fakeVersion = TRUE;

	// the following function loads the resource manager using a temporary fake version (Vista) by hacking GetVersionEx
	Concurrency::details::_GetConcurrency();

#if _MSC_FULL_VER >= 170050623  // pre-RTM

	if (!IsVista) {  
		// this needs to be done before setting back to XP because of an assertion checking for Vista
		Concurrency::details::ResourceManager::CreateSingleton(); 

		// On XP OS reset version back to XP so ConCRT fallbacks will be used instead of Vista threadpooling functions 
		VersionSetterHack versionSet; 
	}

#endif

	fakeVersion = FALSE;
}

class ForceConCRTInit 
{
public:
	ForceConCRTInit() { InitializeConCRT(); }
};

// this gets called before main() so allows ConCRT Resource Manager to be initialized early
ForceConCRTInit init;
#endif
