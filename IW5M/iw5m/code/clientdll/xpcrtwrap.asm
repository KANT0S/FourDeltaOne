;Copyright (c) 2012 Mike Ryan

;Permission is hereby granted, free of charge, to any person obtaining a copy of this software
;and associated documentation files (the "Software"), to deal in the Software without restriction,
;including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
;and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
;subject to the following conditions:

;The above copyright notice and this permission notice shall be included in all copies or substantial
;portions of the Software.

;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
;LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
;IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
;WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
;OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

__ML_64 = OPATTR rax

IF __ML_64
ELSE

.model flat, C 

AfxGetTickCount64 PROTO STDCALL
AfxIsValidLocaleName PROTO STDCALL :DWORD
AfxLCMapStringEx PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
AfxCompareStringEx PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
AfxGetLocaleInfoEx PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD
AfxGetUserDefaultLocaleName PROTO STDCALL :DWORD,:DWORD
AfxEnumSystemLocalesEx PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD

AfxFlsAlloc PROTO STDCALL :DWORD
AfxFlsFree PROTO STDCALL :DWORD
AfxFlsSetValue PROTO STDCALL :DWORD,:DWORD
AfxFlsGetValue PROTO STDCALL :DWORD

AfxGetCurrentProcessorNumber PROTO STDCALL
AfxFlushProcessWriteBuffers PROTO STDCALL
AfxCreateSemaphoreExW PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
AfxGetTimeFormatEx PROTO STDCALL  :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
AfxGetDateFormatEx PROTO STDCALL  :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
AfxSetThreadStackGuarantee PROTO STDCALL :DWORD

AfxInitializeCriticalSectionEx PROTO STDCALL :DWORD,:DWORD,:DWORD
AfxCreateSymbolicLinkA PROTO STDCALL :DWORD,:DWORD,:DWORD
AfxCreateSymbolicLinkW PROTO STDCALL :DWORD,:DWORD,:DWORD
AfxGetLogicalProcessorInformation PROTO STDCALL :DWORD,:DWORD
AfxInitOnceExecuteOnce PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD

AfxGetVersionExW PROTO STDCALL :DWORD

AfxCloseThreadpoolTimer PROTO STDCALL :DWORD
AfxCloseThreadpoolWait PROTO STDCALL :DWORD
AfxCreateThreadpoolTimer PROTO STDCALL :DWORD,:DWORD,:DWORD
AfxCreateThreadpoolWait PROTO STDCALL :DWORD,:DWORD,:DWORD
AfxFreeLibraryWhenCallbackReturns PROTO STDCALL :DWORD,:DWORD
AfxSetThreadpoolTimer PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD
AfxSetThreadpoolWait PROTO STDCALL :DWORD,:DWORD,:DWORD
AfxWaitForThreadpoolTimerCallbacks PROTO STDCALL :DWORD,:DWORD
 
.data
 __imp__GetTickCount64@0 dd AfxGetTickCount64
 __imp__IsValidLocaleName@4 dd AfxIsValidLocaleName
 __imp__LCMapStringEx@36 dd AfxLCMapStringEx
 __imp__CompareStringEx@36 dd AfxCompareStringEx
 __imp__GetLocaleInfoEx@16 dd AfxGetLocaleInfoEx
 __imp__GetUserDefaultLocaleName@8 dd AfxGetUserDefaultLocaleName
 __imp__EnumSystemLocalesEx@16 dd AfxEnumSystemLocalesEx

 __imp__FlsAlloc@4 dd AfxFlsAlloc
 __imp__FlsFree@4 dd AfxFlsFree
 __imp__FlsSetValue@8 dd AfxFlsSetValue
 __imp__FlsGetValue@4 dd AfxFlsGetValue

 __imp__GetCurrentProcessorNumber@0 dd AfxGetCurrentProcessorNumber
 __imp__FlushProcessWriteBuffers@0 dd AfxFlushProcessWriteBuffers
 __imp__CreateSemaphoreExW@24 dd AfxCreateSemaphoreExW
 __imp__GetTimeFormatEx@24 dd AfxGetTimeFormatEx
 __imp__GetDateFormatEx@28 dd AfxGetDateFormatEx
 __imp__SetThreadStackGuarantee@4 dd AfxSetThreadStackGuarantee

 __imp__InitializeCriticalSectionEx@12 dd AfxInitializeCriticalSectionEx
 __imp__CreateSymbolicLinkA@12 dd AfxCreateSymbolicLinkA
 __imp__CreateSymbolicLinkW@12 dd AfxCreateSymbolicLinkW
 __imp__GetLogicalProcessorInformation@8 dd AfxGetLogicalProcessorInformation
 __imp__InitOnceExecuteOnce@16 dd AfxInitOnceExecuteOnce

 __imp__GetVersionExW@4 dd AfxGetVersionExW

 __imp__CloseThreadpoolTimer@4 dd AfxCloseThreadpoolTimer 
 __imp__CloseThreadpoolWait@4 dd AfxCloseThreadpoolWait 
 __imp__CreateThreadpoolTimer@12 dd AfxCreateThreadpoolTimer
 __imp__CreateThreadpoolWait@12 dd AfxCreateThreadpoolWait
 __imp__FreeLibraryWhenCallbackReturns@8 dd	AfxFreeLibraryWhenCallbackReturns 
 __imp__SetThreadpoolTimer@16 dd AfxSetThreadpoolTimer
 __imp__SetThreadpoolWait@12 dd	AfxSetThreadpoolWait
 __imp__WaitForThreadpoolTimerCallbacks@8 dd AfxWaitForThreadpoolTimerCallbacks

 EXTERNDEF __imp__GetTickCount64@0 : QWORD
 EXTERNDEF __imp__IsValidLocaleName@4 : DWORD
 EXTERNDEF __imp__LCMapStringEx@36 : DWORD
 EXTERNDEF __imp__CompareStringEx@36 : DWORD
 EXTERNDEF __imp__GetLocaleInfoEx@16 : DWORD
 EXTERNDEF __imp__GetUserDefaultLocaleName@8 : DWORD
 EXTERNDEF __imp__EnumSystemLocalesEx@16 : DWORD

 EXTERNDEF __imp__FlsAlloc@4 : DWORD
 EXTERNDEF __imp__FlsFree@4 : DWORD
 EXTERNDEF __imp__FlsSetValue@8 : DWORD
 EXTERNDEF __imp__FlsGetValue@4 : DWORD 

 EXTERNDEF __imp__GetCurrentProcessorNumber@0 : DWORD
 EXTERNDEF __imp__FlushProcessWriteBuffers@0 :
 EXTERNDEF __imp__CreateSemaphoreExW@24 : DWORD
 EXTERNDEF __imp__GetTimeFormatEx@24 : DWORD
 EXTERNDEF __imp__GetDateFormatEx@28 : DWORD
 EXTERNDEF __imp__SetThreadStackGuarantee@4 : DWORD

 EXTERNDEF __imp__InitializeCriticalSectionEx@12 : DWORD
 EXTERNDEF __imp__CreateSymbolicLinkA@12 : BYTE
 EXTERNDEF __imp__CreateSymbolicLinkW@12 : BYTE
 EXTERNDEF __imp__GetLogicalProcessorInformation@8 : DWORD
 EXTERNDEF __imp__InitOnceExecuteOnce@16 : DWORD

 EXTERNDEF __imp__GetVersionExW@4 : DWORD
 
 EXTERNDEF __imp__CloseThreadpoolTimer@4 : 
 EXTERNDEF __imp__CloseThreadpoolWait@4 :
 EXTERNDEF __imp__CreateThreadpoolTimer@12 : DWORD
 EXTERNDEF __imp__CreateThreadpoolWait@12  : DWORD
 EXTERNDEF __imp__FreeLibraryWhenCallbackReturns@8 :
 EXTERNDEF __imp__SetThreadpoolTimer@16 :
 EXTERNDEF __imp__SetThreadpoolWait@12 :
 EXTERNDEF __imp__WaitForThreadpoolTimerCallbacks@8 :

.code

ENDIF 

end



 
