// ==========================================================
// alterIWnet project
// 
// Sub-component: DLLfunctions
// Purpose: lookup function addresses using PE structures
//          and jump inside the function instead of normal calls
//
// Normally people hook DLL functions on the 1st 2 or 5 bytes
// or put a breakpoints on the first 4 bytes
// Object is to prevent touching the 1st 4 bytes.
//
// Initial author: JohnD
// Started: 2011-09-05
// ==========================================================
#include "stdinc.h"

// will get encrypted
char *sfunc[] = {
	"kernel32.dll",
	"GetTickCount",				// do not call this using pCallDllfuncion, starting asm differs
	"Sleep",
	"CreateThread",
	"CreateFileA",
	"GetModuleFileNameA",
	"d3d9.dll",
	"GetVolumeInformationA",
	0
};
typedef HANDLE (WINAPI* CreateFileA_t)(
  __in      LPCTSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
);


typedef struct DEVICE_NUMBER {
  DEVICE_TYPE	DeviceType;
  ULONG			DeviceNumber;
  ULONG			PartitionNumber;
} DEVICE_NUMBER, *PDEVICE_NUMBER;

GetModuleHandle_t		pGetModuleHandle=0;								// gets filles in main.cpp
GetTickCount_t			pGetTickCount=0;
Sleep_t					pSleep=0;
CreateThread_t			pCreateThread=0;
GetModuleFileNameA_t	pGetModuleFileName=0;
CreateFileA_t			pCreateFile=0;
GetVolumeInformationA_t	pGetVolumeInformation=0;

char					ModuleDir[MAX_PATH];
int						CI_D3D9=0;

#define DEREF(name)		*(UINT_PTR *)(name)
#define DEREF_64(name)	*(DWORD64 *)(name)
#define DEREF_32(name)	*(DWORD *)(name)
#define DEREF_16(name)	*(WORD *)(name)
#define DEREF_8(name)	*(BYTE *)(name)


// special function which jumps inside a DLL function
// skipping the first 5 bytes, which can have a possible hook
// Before using it, check if the standard code  is there....
void __declspec(naked) pCallDllFunction()
{
	__asm
	{						// mov edi,edi     placeholder for MS patch
		push ebp			// original code which we skip, total 5 bytes
		mov  ebp, esp
		// ---- above is standard code, now we skip 5 bytes and jump to it
		add  eax, 5			// skip possible hook
		push eax
		ret					// jump to the function
	}
}


// Retrieve the address of a module function without using kernel32 functions
UINT_PTR	GetProcAddressPE(UINT_PTR uiBaseAddress, char *name)
{
	// variables for processing the kernels export table
	UINT_PTR uiAddressArray;
	UINT_PTR uiNameArray;
	UINT_PTR uiExportDir;
	UINT_PTR uiNameOrdinals;
	UINT_PTR uiAddress;
	DWORD    dwHashValue, dwNoFunc, dwFuncHash;
	char     *fname;

	dwFuncHash=STRhash(name);
	// get the VA of the modules NT Header
	uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;
	// uiNameArray = the address of the modules export directory entry
	uiNameArray = (UINT_PTR)&((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];
	// get the VA of the export directory
	uiExportDir = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress );
	// get the VA for the array of name pointers
	uiNameArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNames );
	// get the VA for the array of name ordinals
	uiNameOrdinals = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNameOrdinals );
	// get the VA for the array of addresses
	uiAddressArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfFunctions );

	uiAddress=0;
	dwNoFunc = ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->NumberOfFunctions;
	// loop while we still have imports to find
	while (dwNoFunc--) {
		// compute the hash values for this function name
		fname = (char *)( uiBaseAddress + DEREF_32( uiNameArray ) );
		dwHashValue = STRhash(fname,strlen(fname));
		// printf("%08x %s\n",dwHashValue,(char *)( uiBaseAddress + DEREF_32( uiNameArray ) ));
		// if we have found a function we want we get its virtual address
		if( dwHashValue == dwFuncHash )
		{
			// use this functions name ordinal as an index into the array of name pointers
			uiAddressArray += ( DEREF_16( uiNameOrdinals ) * sizeof(DWORD) );
			uiAddress =	uiBaseAddress + DEREF_32( uiAddressArray);
		    break;
		}
		// get the next exported function name
		uiNameArray += sizeof(DWORD);
		// get the next exported function name ordinal
		uiNameOrdinals += sizeof(WORD);
	}
	return uiAddress;
}

/* MOVED TO HWID
// This is bad... now the GetVolumeInformation is visible in the DLL
// somebody can hook the GetVolumeInformation and pass you a different hwid which is pretty easy
//
// the volume serial is just based on a timestamp when the drive was formatted... 
// there are simple tools available to adjust it.
// however if you adjust it.. some license programs can stop working...
// perhaps better to take the real disk serial (not the volume serial)
int ds(){
	DWORD dwSerial;

	if (pGetVolumeInformation(TEXT("C:\\"), NULL, 0, &dwSerial, NULL, NULL,NULL,0))	{
		return static_cast <int>(dwSerial);
	} else {
		return 0;
	}
SCANMARK
}*/


// get function address using PE lookup
void *getFuncAddress(char *dllnm, char *funcnm)
{
	HINSTANCE   hMod;
	DWORD       dwRetVal;
	void        *adr=NULL;

	// GetModuleHandle
	__asm {
			push dllnm					// param 1
			mov  eax, pGetModuleHandle	// specify function address
			call pCallDllFunction		// GetModuleHandle
			mov dwRetVal, eax
	}
	hMod = (HINSTANCE)dwRetVal;			//GetModuleHandle(dllnm);		// get dll location
	if (hMod) {
		adr=(void *)GetProcAddressPE((UINT_PTR)hMod, funcnm);
		DEBUGprintf(("function %s at %08x\n",funcnm,adr));
	} else
		DEBUGprintf(("module %s not found\n",dllnm));
	return adr;
}

void DLLfuncinit()
{
    char	mod[41],func[41],tmp[MAX_PATH], *ptr;
    DWORD	dwRetVal,len;
	DEBUGprintf(("DLLinit\n"));
    for (int i=0; sfunc[i]!=0 ; i++)	STRcrypt(sfunc[i],0);

	// get real address of GetModuleHandle function, so we can use it later
	PBYTE adr=(PBYTE)GetModuleHandle;
	if (adr[0]==0xff && adr[1]==0x25) {										//  with /clr jmp [xxxxxx]
		pGetModuleHandle=(GetModuleHandle_t)*(PDWORD)*(PDWORD)&adr[2];		// yes pointer contains pointer to
	} else {									
		pGetModuleHandle=(GetModuleHandle_t)adr;
	}
	pGetTickCount = (GetTickCount_t)getFuncAddress(STRdecrypt(sfunc[0],mod),STRdecrypt(sfunc[1],func));
	pSleep        = (Sleep_t)       getFuncAddress(STRdecrypt(sfunc[0],mod),STRdecrypt(sfunc[2],func));
	pCreateThread = (CreateThread_t)getFuncAddress(STRdecrypt(sfunc[0],mod),STRdecrypt(sfunc[3],func));
	pCreateFile   = (CreateFileA_t )getFuncAddress(STRdecrypt(sfunc[0],mod),STRdecrypt(sfunc[4],func));
	pGetModuleFileName=(GetModuleFileNameA_t)getFuncAddress(STRdecrypt(sfunc[0],mod),STRdecrypt(sfunc[5],func));
	pGetVolumeInformation=(GetVolumeInformationA_t)getFuncAddress(STRdecrypt(sfunc[0],mod),STRdecrypt(sfunc[7],func));

	srand(pGetTickCount());				// initialize randomizer first
	// check for a d3d9.dll in our directory, should not be there
	len=pGetModuleFileName(NULL,ModuleDir,sizeof(ModuleDir));
	ModuleDir[len]=0;
	ptr=strrchr(ModuleDir,'\\');
	if (ptr) ptr[1]='\0';
	DEBUGprintf(("Basedir [%s]\n",ModuleDir));

	strcpy(tmp,ModuleDir);
	strcat(tmp,STRdecrypt(sfunc[6],func));					// d3d9.dll
	HANDLE hFile = pCreateFile( tmp, 0, FILE_SHARE_READ,NULL, OPEN_EXISTING, 0, NULL );    
	if( hFile != INVALID_HANDLE_VALUE ) {
		CI_D3D9=1;
		DEBUGprintf(("Found a d3d9 proxy dll\n"));
	}

	DEBUGprintf(("Scan start\n"));
	//	CreateThread(0,0,(LPTHREAD_START_ROUTINE)CI_scanner,0,0,0);
	if (pCreateThread) {
		__asm {
			push 0					// reserved
			push 0					// reserved
			push 0					// reserved
			push CI_scanner			// pass CIscanner function
			push 0					// reserved
			push 0					// reserved
			mov  eax, pCreateThread	// specify function address
			call pCallDllFunction	
			mov dwRetVal, eax
		}
	} else
		DEBUGprintf(("ERROR: CreateThread not resolved\n"));
	return;
SCANMARK
}