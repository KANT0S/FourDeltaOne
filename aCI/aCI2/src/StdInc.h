
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include "Hooking.h"
#include "strcrypt.h"

// only in Developer build mode there is debug output or strings
#ifdef Developer
void DEBUGmsg(char *fmt, ...);
void DEBUGdump(char *id,DWORD adr, int size);

#define DEBUGprintf(x)	DEBUGmsg x
#define DEBUGmem(x)	DEBUGdump x

#else
#define DEBUGprintf(x)	while (0)
#define DEBUGmem(x)	while (0)
#endif


// this way the code and rdata segment is merged with the data segment and it will be rwx
#pragma comment(linker,"/merge:.rdata=.data")
#pragma comment(linker,"/merge:.text=.data")

#define CI_ID_IW4_IMPULSEHACK			30001
#define CI_ID_IW4_ENTRY_IH				30002 // entry point most commonly used by impulse hack
#define CI_ID_IW4_AIMTARGET_GETTAGPOS	30003
#define CI_ID_IW4_xAIW3_StrWidth        30004

#define	CI_ID_GEN_CIFRAME				40001
#define	CI_ID_GEN_D3D9					40002
#define	CI_ID_GEN_ACIMOD				41000  // keep room for SCANmax ids...
#define	CI_ID_GEN_MW2MOD				42000  // keep room for SCANmax ids...

#define CI_ID_HWID_ERROR				50002  // if HWID != MAC or == "nope"

void CI_SendStatus(int status);
void StringCrypt(char *inout,int len, bool encrypt);
char *GetCommandLinePid(int pid);

void NP_HeartBeat();
char* getMAC();

//void CI_CompareDetection(void* address, const char* match, size_t length, int reason);

// string arrays which gets encrypted on load
extern char *IW4str[];
extern char *GENstr[];

extern DWORD	lastFrameCheck;
extern DWORD	lastScanCheck;
extern DWORD	maxcalladr;

extern DWORD	FONTcalladr;
extern DWORD	BONEPOScalladr;
extern DWORD	CGTRACEcalladr;
extern int		CI_framecounter;
extern int		CI_D3D9;
extern char*    macAddress;
typedef struct {
	DWORD from;
	DWORD until;
	DWORD hash;
} tMEMSCAN;

DWORD		Mem2Hash(BYTE *mem, int length);
void		ScanSave(DWORD adr, int len=0, DWORD hash=0);
int			ScanMemory(tMEMSCAN *mem, int st, int maxno);
void WINAPI	CI_scanner(void *dum);
void		ScanInit();
extern int	SCANmax,IW4start;


#define SCANMARK \
	__asm _emit	0xcc \
	__asm _emit	0xcc \
	__asm _emit	0xcc \
	__asm _emit	0xcc 


// FROM DLLfunctions
typedef DWORD (WINAPI *GetModuleHandle_t)(char *);
typedef DWORD (WINAPI *GetTickCount_t)();
typedef VOID  (WINAPI *Sleep_t)(DWORD dwMilliseconds);
typedef VOID  (WINAPI *CreateThread_t)(LPSECURITY_ATTRIBUTES,size_t,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
typedef DWORD (WINAPI *GetModuleFileNameA_t)(HMODULE hModule,char *lpFilename, DWORD nSize);
typedef BOOL  (WINAPI *GetVolumeInformationA_t)(
      LPCSTR lpRootPathName,
      LPSTR lpVolumeNameBuffer,
          DWORD nVolumeNameSize,
     LPDWORD lpVolumeSerialNumber,
     LPDWORD lpMaximumComponentLength,
     LPDWORD lpFileSystemFlags,
     LPSTR lpFileSystemNameBuffer,
          DWORD nFileSystemNameSize
    );

extern GetModuleHandle_t	pGetModuleHandle;					// gets filles in main.cpp
extern GetTickCount_t		pGetTickCount;						// filed with DLLfuncinit();
extern Sleep_t				pSleep;
extern CreateThread_t		pCreateThread;
extern GetModuleFileNameA_t	pGetModuleFileName;
extern GetVolumeInformationA_t	pGetVolumeInformation;

void     pCallDllFunction();
extern   int				CI_CGd3d9;
UINT_PTR GetProcAddressPE(UINT_PTR uiBaseAddress, char *name);
void	*getFuncAddress(char *dllnm, char *funcnm);
void     DLLfuncinit();
