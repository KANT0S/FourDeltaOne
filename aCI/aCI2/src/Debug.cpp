// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: debug
// Purpose: generate debug output in Dev mode
//          otherwise make sure everything is stripped
//
// output can be generated with,   watch the double (( ))
//       DEBUGprintf(( "fieldval=%08x\n",adr ));
//
// if compiled in Normal mode all statements are
// stripped and strings values will not be in the exe/dll
//
// If you want the output somewhere else, just define
// some code in the DEBUGmsg() function
//
// Initial author: JohnD
// Started: 2011-08-31
// ==========================================================

#include "StdInc.h"
#include <stdio.h>

#ifdef Developer	

// define what kind of output you want
#define DEBUGCONSOLE
#define DEBUGFILE
#define DEBUGFN	"c:\\aCI2.txt"
FILE *DEBUGfp=0;
int   DEBUGcons=0;
// ============ Some general functions ================
// log everything in <basename>.txt
void DEBUGmsg(char *fmt, ...)
{
	// stdout output in seperate console and to a logfile
	if (!fmt) {
#ifdef DEBUGCONSOLE
		// Create a console window for logging (NoFate)
		if (AllocConsole()) {
			AttachConsole( GetCurrentProcessId() );
			freopen( "CON", "w", stdout ) ;
			SetConsoleTitle( "aIW - Developer" );
			// Resize console (max length)
			COORD cordinates = {80, 32766};
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleScreenBufferSize(handle, cordinates);
			DEBUGcons=1;
		}
#endif
#ifdef DEBUGFILE
		DEBUGfp = fopen(DEBUGFN,"w");
#endif
	} else {
		char str[513];
		va_list va_alist;
		va_start (va_alist, fmt);
		vsnprintf(str,512,fmt,va_alist);
		va_end (va_alist);
#ifdef DEBUGCONSOLE
		if (DEBUGcons)  printf(str);
#endif
#ifdef DEBUGFILE
		if (DEBUGfp) { 
			fprintf(DEBUGfp,str);
			fflush(DEBUGfp);
		}
#endif
	}
}

void DEBUGdump(char *id,DWORD adr, int size)
{
	int i;
	DWORD orgadr;
	char data[133];
	BYTE *ptr;

	orgadr=adr;
    do {
		ptr=(BYTE *)adr;
		sprintf(data,"%s %08x +%04x ",id,adr,adr-orgadr);
	    for (i=0; i<16; i++) sprintf(&data[strlen(data)],"%02x ",ptr[i]);
	    strcat(data," ");
	    for (i=0; i<16; i++) {
			if (ptr[i]>=0x20 && ptr[i]<124)
				sprintf(&data[strlen(data)],"%c",ptr[i]);
			else
				strcat(data,".");
		}
	    DEBUGmsg("%s\n",data);
		size-=16;
		adr +=16;
    } while (size>0);
}

#endif
