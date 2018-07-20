// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: Memory scanner
//
// 
// Initial author: JohnD
// Started: 2011-09-08
// ==========================================================

#include "StdInc.h"
#include "Hooking.h"

// dynamic areas, which get added during runtime when hooks are placed, 
// or we add them manually during the init functions
#define SCANMAX	80
DWORD		SCANkey=0;				// key which addresses are xored, so plain addresses are not visible
int			SCANmax=0;
tMEMSCAN	SCANmem[SCANMAX];		// room for SCANMAX scansave gives message when exceeded

int		CI_framecounter=0;

DWORD	Mem2Hash(BYTE *mem, int length);

DWORD	Mem2Hash(BYTE *mem, int length)
{
	DWORD dwHash;
	dwHash = 0;
	do
	{
		dwHash = _rotr( (DWORD)dwHash, 13 );
		dwHash += *mem++;
	} while( --length );
	return dwHash;
SCANMARK
}

int	ScanMemory(tMEMSCAN *mem, int st, int maxno)
{
	DWORD from, until, hash;
	int i;
	for (i=st; i<maxno; i++) {
		from  = mem->from  ^ SCANkey;
		until = mem->until ^ SCANkey;
#ifdef Developer
		if (mem->hash==0) {				// initial scan, so show encrypted output for dev
			hash=Mem2Hash((PBYTE)from,(int)(until-from+1));
			DEBUGprintf(("%d { 0x%08x, 0x%08x , 0x%08x }, \n",i,from,until,hash ));
			mem->hash=hash;
		} else {
			hash=Mem2Hash((PBYTE)from,(int)(until-from+1));
		}
#else
		hash=Mem2Hash((PBYTE)from,(int)(until-from+1));
#endif
		if (hash!=mem->hash) {
			DEBUGprintf(("scan %2d: %08x - %08x -> %08x %08x DETECTED !!!\n",i,from,until,hash,mem->hash));
			break;
		}
		mem=(tMEMSCAN *)((DWORD)mem+sizeof(tMEMSCAN));		// next entry
	}
	return i;
SCANMARK
}

void WINAPI CI_scanner(void *dum)
{
	int n;
	while (1) {
		__asm {
			push 45000					// wait time
			mov  eax, pSleep			// specify function address
			call pCallDllFunction		// jump inside
		}
		DEBUGprintf(("scanning\n"));
		// scan our aCI code
		n=ScanMemory(SCANmem,0,IW4start);
		if (n != IW4start) {
			CI_SendStatus(CI_ID_GEN_ACIMOD+n+1);
		}
		// scan the MW2 code
		n=ScanMemory(SCANmem,IW4start,SCANmax);
		if (n != SCANmax) {
			CI_SendStatus(CI_ID_GEN_MW2MOD+n+1);
		}
		if (CI_framecounter==0) {
			CI_SendStatus(CI_ID_GEN_CIFRAME);
		}
		if (CI_D3D9!=0) {
			CI_SendStatus(CI_ID_GEN_D3D9);
		}
		CI_framecounter=0;
	}
SCANMARK
}


// Add an asm region to our list to be scanned
void ScanSave(DWORD adr, int len, DWORD hash)
{
	if ((SCANmax+1)<(SCANMAX-1)) {
	    SCANmem[SCANmax+1].from=0;
	    SCANmem[SCANmax+1].until=0;
	    SCANmem[SCANmax+1].hash=0;
		if (len==0) for (len=1; len<300; len++)   if (*(PDWORD)(adr+len)==0xcccccccc) break;
	    SCANmem[SCANmax].until=(adr+len-1)^SCANkey;
	    SCANmem[SCANmax].from=adr ^ SCANkey;
		SCANmem[SCANmax].hash=(hash)?hash:Mem2Hash((PBYTE)adr,len);
		DEBUGprintf(("ScanSave %2d : %08x %3d  0x%08x\n",SCANmax,adr,len,SCANmem[SCANmax].hash));
		SCANmax++;
	} else
		DEBUGprintf(("ScanSave max reached !!\n"));
	return;
SCANMARK
}

void ScanInit()
{
    DEBUGprintf(("ScanInit\n"));
    SCANkey=((s_rand()%0xffff)<<16)+s_rand();
	for (int i=0; i<SCANMAX; i++) {
	    SCANmem[i].from=s_rand();
	    SCANmem[i].until=s_rand();
	    SCANmem[i].hash=s_rand();
	}
    SCANmax=0;
    SCANmem[0].from=0;
	ScanSave((DWORD)ScanSave);
	ScanSave((DWORD)ScanMemory);
	ScanSave((DWORD)CI_scanner);
	ScanSave((DWORD)Mem2Hash);
	ScanSave((DWORD)InstallCallHook);
	ScanSave((DWORD)InstallJumpHook);
	return;
SCANMARK
}