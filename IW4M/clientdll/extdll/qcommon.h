/*
===========================================================================

Wolfenstein: Enemy Territory GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Wolfenstein: Enemy Territory GPL Source Code (Wolf ET Source Code).  

Wolf ET Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolf ET Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolf ET Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Wolf: ET Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Wolf ET Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

// qcommon.h -- definitions common between client and server, but not game.or ref modules
#ifndef _QCOMMON_H_
#define _QCOMMON_H_

#include "cm_public.h"

//bani
#ifdef __GNUC__
#define _attribute( x ) __attribute__( x )
#else
#define _attribute( x )
#endif

//#define PRE_RELEASE_DEMO
#ifdef PRE_RELEASE_DEMO
#define PRE_RELEASE_DEMO_NODEVMAP
#endif							// PRE_RELEASE_DEMO

//============================================================================

/*
==============================================================

NET

==============================================================
*/
/*
==============================================================

VIRTUAL MACHINE

==============================================================
*/

typedef struct vm_s vm_t;

typedef enum
{
	VMI_NATIVE,
	VMI_BYTECODE,
	VMI_COMPILED
} vmInterpret_t;

typedef enum
{
	TRAP_MEMSET = 100,
	TRAP_MEMCPY,
	TRAP_STRNCPY,
	TRAP_SIN,
	TRAP_COS,
	TRAP_ATAN2,
	TRAP_SQRT,
	TRAP_MATRIXMULTIPLY,
	TRAP_ANGLEVECTORS,
	TRAP_PERPENDICULARVECTOR,
	TRAP_FLOOR,
	TRAP_CEIL,

	TRAP_TESTPRINTINT,
	TRAP_TESTPRINTFLOAT
} sharedTraps_t;

void            VM_Init(void);
vm_t           *VM_Create(const char *module, intptr_t(*systemCalls) (intptr_t *), vmInterpret_t interpret);

// module should be bare: "cgame", not "cgame.dll" or "vm/cgame.qvm"

void            VM_Free(vm_t * vm);
void            VM_Clear(void);
vm_t           *VM_Restart(vm_t * vm);

intptr_t QDECL  VM_Call(vm_t * vm, int callNum, ...);

void            VM_Debug(int level);

void           *VM_ArgPtr(intptr_t intValue);
void           *VM_ExplicitArgPtr(vm_t * vm, intptr_t intValue);

#define	VMA(x) VM_ArgPtr(args[x])
static ID_INLINE float _vmf(intptr_t x)
{
	floatint_t      fi;

	fi.i = (int)x;
	return fi.f;
}

#define	VMF(x)	_vmf(args[x])


/*
==============================================================

CMD

Command text buffering and command execution

==============================================================
*/

/*
==============================================================

CVAR

==============================================================
*/

/*

cvar_t variables are used to hold scalar or string variables that can be changed
or displayed at the console or prog code as well as accessed directly
in C code.

The user can access cvars from the console in three ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
set r_draworder 0	as above, but creates the cvar if not present

Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.

The are also occasionally used to communicated information between different
modules of the program.

*/

cvar_t         *Cvar_Get(const char *var_name, const char *value, int flags);

// creates the variable if it doesn't exist, or returns the existing one
// if it exists, the value will not be changed, but flags will be ORed in
// that allows variables to be unarchived without needing bitflags
// if value is "", the value will not override a previously set value.

void            Cvar_Register(vmCvar_t * vmCvar, const char *varName, const char *defaultValue, int flags);

// basically a slightly modified Cvar_Get for the interpreted modules

void            Cvar_Update(vmCvar_t * vmCvar);

// updates an interpreted modules' version of a cvar

void            Cvar_Set(const char *var_name, const char *value);

// will create the variable with no flags if it doesn't exist

void            Cvar_SetLatched(const char *var_name, const char *value);

// don't set the cvar immediately

void            Cvar_SetValue(const char *var_name, float value);

// expands value to a string and calls Cvar_Set

float           Cvar_VariableValue(const char *var_name);
int             Cvar_VariableIntegerValue(const char *var_name);

// returns 0 if not defined or non numeric

char           *Cvar_VariableString(const char *var_name);
void            Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize);

// returns an empty string if not defined
void            Cvar_LatchedVariableStringBuffer(const char *var_name, char *buffer, int bufsize);

// Gordon: returns the latched value if there is one, else the normal one, empty string if not defined as usual

void            Cvar_CommandCompletion(void (*callback) (const char *s));

// callback with each valid string

void            Cvar_Reset(const char *var_name);

void            Cvar_SetCheatState(void);

// reset all testing vars to a safe value

qboolean        Cvar_Command(void);

// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void            Cvar_WriteVariables(fileHandle_t f);

// writes lines containing "set variable value" for all variables
// with the archive flag set to true.

void            Cvar_Init(void);

char           *Cvar_InfoString(int bit);
char           *Cvar_InfoString_Big(int bit);

// returns an info string containing all the cvars that have the given bit set
// in their flags ( CVAR_USERINFO, CVAR_SERVERINFO, CVAR_SYSTEMINFO, etc )
void            Cvar_InfoStringBuffer(int bit, char *buff, int buffsize);

void            Cvar_Restart_f(void);

extern int      cvar_modifiedFlags;

// whenever a cvar is modifed, its flags will be OR'd into this, so
// a single check can determine if any CVAR_USERINFO, CVAR_SERVERINFO,
// etc, variables have been modified since the last check.  The bit
// can then be cleared to allow another change detection.

/*
==============================================================

FILESYSTEM

No stdio calls should be used by any part of the game, because
we need to deal with all sorts of directory and seperator char
issues.
==============================================================
*/

#ifndef PRE_RELEASE_DEMO
//#define BASEGAME "main"
#define BASEGAME "etmain"
#else
#define BASEGAME "ettest"
#endif

// referenced flags
// these are in loop specific order so don't change the order
#define FS_GENERAL_REF  0x01
#define FS_UI_REF       0x02
#define FS_CGAME_REF    0x04
#define FS_QAGAME_REF   0x08
// number of id paks that will never be autodownloaded from baseq3
#define NUM_ID_PAKS     9

#define MAX_FILE_HANDLES    64

#ifdef WIN32
#define Q_rmdir _rmdir
#else
#define Q_rmdir rmdir
#endif
/*
==============================================================

DOWNLOAD

==============================================================
*/

//#include "dl_public.h"

/*
==============================================================

Edit fields and command line history/completion

==============================================================
*/

#define MAX_EDIT_LINE   256
typedef struct
{
	int             cursor;
	int             scroll;
	int             widthInChars;
	char            buffer[MAX_EDIT_LINE];
} field_t;

void            Field_Clear(field_t * edit);
void            Field_CompleteCommand(field_t * edit);

/*
==============================================================

MISC

==============================================================
*/

// centralizing the declarations for cl_cdkey
// (old code causing buffer overflows)
extern char     cl_cdkey[34];
void            Com_AppendCDKey(const char *filename);
void            Com_ReadCDKey(const char *filename);

typedef struct gameInfo_s
{
	qboolean        spEnabled;
	int             spGameTypes;
	int             defaultSPGameType;
	int             coopGameTypes;
	int             defaultCoopGameType;
	int             defaultGameType;
	qboolean        usesProfiles;
} gameInfo_t;

extern gameInfo_t com_gameInfo;

// returnbed by Sys_GetProcessorId
#define CPUID_GENERIC           0	// any unrecognized processor

#define CPUID_AXP               0x10

#define CPUID_INTEL_UNSUPPORTED 0x20	// Intel 386/486
#define CPUID_INTEL_PENTIUM     0x21	// Intel Pentium or PPro
#define CPUID_INTEL_MMX         0x22	// Intel Pentium/MMX or P2/MMX
#define CPUID_INTEL_KATMAI      0x23	// Intel Katmai

#define CPUID_AMD_3DNOW         0x30	// AMD K6 3DNOW!

// TTimo
// centralized and cleaned, that's the max string you can send to a Com_Printf / Com_DPrintf (above gets truncated)
#define MAXPRINTMSG 4096

char           *CopyString(const char *in);
void            Info_Print(const char *s);

void            Com_BeginRedirect(char *buffer, int buffersize, void (*flush) (char *));
void            Com_EndRedirect(void);
// *INDENT-OFF*
int QDECL Com_VPrintf(const char *fmt, va_list argptr) _attribute((format(printf, 1, 0)));	// conforms to vprintf prototype for print callback passing
//	 void QDECL Com_Printf(const char *fmt, ...) _attribute((format(printf, 1, 2)));	// this one calls to Com_VPrintf now
//	 void QDECL Com_DPrintf(const char *fmt, ...) _attribute((format(printf, 1, 2)));
//	 void QDECL Com_Error(int code, const char *fmt, ...) _attribute((format(printf, 2, 3)));

typedef void (__cdecl * Com_Error_t)(int type, char* message, ...);
extern Com_Error_t Com_Error;

typedef void (__cdecl * Com_Printf_t)(int, const char*, ...);
extern Com_Printf_t Com_Printf;

// *INDENT-ON*
void            Com_Quit_f(void);
int             Com_EventLoop(void);
//int             Com_Milliseconds(void);	// will be journaled properly

unsigned int    Com_BlockChecksum(const void *buffer, int length);
unsigned int    Com_BlockChecksumKey(void *buffer, int length, int key);

// XreaL BEGIN
char           *Com_MD5File(const char *filename, int length, const char *prefix, int prefix_len);
char           *Com_MD5FileETCompat(const char *filename);
void            Com_RandomBytes(byte * string, int len);
// XreaL END

int             Com_Filter(char *filter, char *name, int casesensitive);
int             Com_FilterPath(char *filter, char *name, int casesensitive);
int             Com_RealTime(qtime_t * qtime);
qboolean        Com_SafeMode(void);

void            Com_StartupVariable(const char *match);
void            Com_SetRecommended();

// checks for and removes command line "+set var arg" constructs
// if match is NULL, all set commands will be executed, otherwise
// only a set with the exact name.  Only used during startup.

//bani - profile functions
void            Com_TrackProfile(char *profile_path);
qboolean        Com_CheckProfile(char *profile_path);
qboolean        Com_WriteProfile(char *profile_path);

extern cvar_t  *com_crashed;

extern cvar_t  *com_ignorecrash;	//bani

extern cvar_t  *com_pid;		//bani

extern cvar_t  *com_developer;
extern cvar_t  *com_dedicated;
extern cvar_t  *com_speeds;
extern cvar_t  *com_timescale;
extern cvar_t  *com_sv_running;
extern cvar_t  *com_cl_running;
extern cvar_t  *com_viewlog;	// 0 = hidden, 1 = visible, 2 = minimized
extern cvar_t  *com_version;
// XreaL BEGIN
extern cvar_t  *com_engine;
// XreaL END

//extern    cvar_t  *com_blood;
extern cvar_t  *com_buildScript;	// for building release pak files
extern cvar_t  *com_journal;
extern cvar_t  *com_cameraMode;
extern cvar_t  *com_logosPlaying;

// watchdog
extern cvar_t  *com_watchdog;
extern cvar_t  *com_watchdog_cmd;

// both client and server must agree to pause
extern cvar_t  *cl_paused;
extern cvar_t  *sv_paused;

// com_speeds times
extern int      time_game;
extern int      time_frontend;
extern int      time_backend;	// renderer backend time

extern int      com_frameTime;
extern int      com_frameMsec;
extern int      com_expectedhunkusage;
extern int      com_hunkusedvalue;

extern qboolean com_errorEntered;

extern fileHandle_t com_journalFile;
extern fileHandle_t com_journalDataFile;

typedef enum
{
	TAG_FREE,
	TAG_GENERAL,
	TAG_BOTLIB,
	TAG_RENDERER,
	TAG_SMALL,
	TAG_STATIC
} memtag_t;

/*

--- low memory ----
server vm
server clipmap
---mark---
renderer initialization (shaders, etc)
UI vm
cgame vm
renderer map
renderer models

---free---

temp file loading
--- high memory ---

*/

#if defined( _DEBUG ) && !defined( BSPC )
#define ZONE_DEBUG
#endif

#ifdef ZONE_DEBUG
#define Z_TagMalloc( size, tag )          Z_TagMallocDebug( size, tag, # size, __FILE__, __LINE__ )
#define Z_Malloc( size )                  Z_MallocDebug( size, # size, __FILE__, __LINE__ )
#define S_Malloc( size )                  S_MallocDebug( size, # size, __FILE__, __LINE__ )
void           *Z_TagMallocDebug(int size, int tag, char *label, char *file, int line);	// NOT 0 filled memory
void           *Z_MallocDebug(int size, char *label, char *file, int line);	// returns 0 filled memory
void           *S_MallocDebug(int size, char *label, char *file, int line);	// returns 0 filled memory
#else
void           *Z_TagMalloc(int size, int tag);	// NOT 0 filled memory
void           *Z_Malloc(int size);	// returns 0 filled memory
void           *S_Malloc(int size);	// NOT 0 filled memory only for small allocations
#endif
void            Z_Free(void *ptr);
void            Z_FreeTags(int tag);
void            Z_LogHeap(void);

void            Hunk_Clear(void);
void            Hunk_ClearToMark(void);
void            Hunk_SetMark(void);
qboolean        Hunk_CheckMark(void);

//void *Hunk_Alloc( int size );
// void *Hunk_Alloc( int size, ha_pref preference );
void            Hunk_ClearTempMemory(void);
void           *Hunk_AllocateTempMemory(int size);
void            Hunk_FreeTempMemory(void *buf);
int             Hunk_MemoryRemaining(void);
void            Hunk_SmallLog(void);
void            Hunk_Log(void);

void            Com_TouchMemory(void);

// commandLine should not include the executable name (argv[0])
void            Com_Init(char *commandLine);
void            Com_Frame(void);
void            Com_Shutdown(qboolean badProfile);


/*
==============================================================

CLIENT / SERVER SYSTEMS

==============================================================
*/

//
// client interface
//
void            CL_InitKeyCommands(void);

// the keyboard binding interface must be setup before execing
// config files, but the rest of client startup will happen later

void            CL_Init(void);
void            CL_ClearStaticDownload(void);
void            CL_Disconnect(qboolean showMainMenu);
void            CL_Shutdown(void);
void            CL_Frame(int msec);
qboolean        CL_GameCommand(void);
void            CL_KeyEvent(int key, qboolean down, unsigned time);

void            CL_CharEvent(int key);

// char events are for field typing, not game control

void            CL_MouseEvent(int dx, int dy, int time);

void            CL_JoystickEvent(int axis, int value, int time);

//void            CL_PacketEvent(netadr_t from, msg_t * msg);

void            CL_ConsolePrint(char *text);

void            CL_MapLoading(void);

// do a screen update before starting to load a map
// when the server is going to load a new map, the entire hunk
// will be cleared, so the client must shutdown cgame, ui, and
// the renderer

void            CL_ForwardCommandToServer(const char *string);

// adds the current command line as a clc_clientCommand to the client message.
// things like godmode, noclip, etc, are commands directed to the server,
// so when they are typed in at the console, they will need to be forwarded.

void            CL_CDDialog(void);

// bring up the "need a cd to play" dialog

void            CL_ShutdownAll(void);

// shutdown all the client stuff

void            CL_FlushMemory(void);

// dump all memory on an error

void            CL_StartHunkUsers(void);

// start all the client stuff using the hunk

void            CL_CheckAutoUpdate(void);
qboolean        CL_NextUpdateServer(void);
void            CL_GetAutoUpdate(void);

void            Key_WriteBindings(fileHandle_t f);

// for writing the config files

void            S_ClearSoundBuffer(qboolean killStreaming);	//----(SA)  modified

// call before filesystem access

void            SCR_DebugGraph(float value, int color);	// FIXME: move logging to common?


//
// server interface
//
void            SV_Init(void);
void            SV_Shutdown(char *finalmsg);
void            SV_Frame(int msec);
//void            SV_PacketEvent(netadr_t from, msg_t * msg);
qboolean        SV_GameCommand(void);


//
// UI interface
//
qboolean        UI_GameCommand(void);
qboolean        UI_usesUniqueCDKey();

/*
==============================================================

NON-PORTABLE SYSTEM SERVICES

==============================================================
*/

typedef enum
{
	AXIS_SIDE,
	AXIS_FORWARD,
	AXIS_UP,
	AXIS_ROLL,
	AXIS_YAW,
	AXIS_PITCH,
	MAX_JOYSTICK_AXIS
} joystickAxis_t;

typedef enum
{
	// bk001129 - make sure SE_NONE is zero
	SE_NONE = 0,				// evTime is still valid
	SE_KEY,						// evValue is a key code, evValue2 is the down flag
	SE_CHAR,					// evValue is an ascii char
	SE_MOUSE,					// evValue and evValue2 are reletive signed x / y moves
	SE_JOYSTICK_AXIS,			// evValue is an axis number and evValue2 is the current state (-127 to 127)
	SE_CONSOLE,					// evPtr is a char*
	SE_PACKET					// evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

typedef struct
{
	int             evTime;
	sysEventType_t  evType;
	int             evValue, evValue2;
	int             evPtrLength;	// bytes of data pointed to by evPtr, for journaling
	void           *evPtr;		// this must be manually freed if not NULL
} sysEvent_t;

sysEvent_t      Sys_GetEvent(void);

void            Sys_Init(void);
qboolean        Sys_IsNumLockDown(void);

void           *Sys_InitializeCriticalSection();
void            Sys_EnterCriticalSection(void *ptr);
void            Sys_LeaveCriticalSection(void *ptr);

char           *Sys_GetDLLName(const char *name);


// fqpath param added 2/15/02 by T.Ray - Sys_LoadDll is only called in vm.c at this time
// 
void           *QDECL Sys_LoadDll(const char *name, char *fqpath, intptr_t(QDECL ** entryPoint) (int, ...),
								  intptr_t(QDECL * systemcalls) (intptr_t, ...));
void            Sys_UnloadDll(void *dllHandle);

void            Sys_UnloadGame(void);
void           *Sys_GetGameAPI(void *parms);

void            Sys_UnloadCGame(void);
void           *Sys_GetCGameAPI(void);

void            Sys_UnloadUI(void);
void           *Sys_GetUIAPI(void);

// XreaL BEGIN
// RB: added generic DLL loading routines
void           *Sys_LoadDLLSimple(const char *name);
void		   *Sys_LoadFunction(void *dllHandle, const char *functionName);
char		   *Sys_DLLError();

// RB: added to link OS specific pointers to the renderer.dll space
void           *Sys_GetSystemHandles(void);
// XreaL END

char           *Sys_GetCurrentUser(void);

void QDECL      Sys_Error(const char *error, ...);
void            Sys_Quit(void);
char           *Sys_GetClipboardData(void);	// note that this isn't journaled...

void            Sys_Print(const char *msg);


// Sys_Milliseconds should only be used for profiling purposes,
// any game related timing information should come from event timestamps
int             Sys_Milliseconds(void);

// XreaL BEGIN
qboolean        Sys_RandomBytes(byte * string, int len);
// XreaL END

void            Sys_SnapVector(float *v);

// the system console is shown when a dedicated server is running
void            Sys_DisplaySystemConsole(qboolean show);

int             Sys_GetProcessorId(void);

void            Sys_BeginStreamedFile(fileHandle_t f, int readahead);
void            Sys_EndStreamedFile(fileHandle_t f);
int             Sys_StreamedRead(void *buffer, int size, int count, fileHandle_t f);
void            Sys_StreamSeek(fileHandle_t f, int offset, int origin);

void            Sys_ShowConsole(int level, qboolean quitOnClose);
void            Sys_SetErrorText(const char *text);

//void            Sys_SendPacket(int length, const void *data, netadr_t to);

//qboolean        Sys_StringToAdr(const char *s, netadr_t * a);

//Does NOT parse port numbers, only base addresses.

//qboolean        Sys_IsLANAddress(netadr_t adr);
void            Sys_ShowIP(void);

qboolean        Sys_CheckCD(void);

void            Sys_Mkdir(const char *path);
char           *Sys_Cwd(void);
char           *Sys_DefaultCDPath(void);
char           *Sys_DefaultBasePath(void);
char           *Sys_DefaultInstallPath(void);
char           *Sys_DefaultHomePath(void);

//char          **Sys_ListFiles(const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs);
//void            Sys_FreeFileList(char **list);

void            Sys_BeginProfiling(void);
void            Sys_EndProfiling(void);

qboolean        Sys_LowPhysicalMemory();
unsigned int    Sys_ProcessorCount();

// NOTE TTimo - on win32 the cwd is prepended .. non portable behaviour
void            Sys_StartProcess(char *exeName, qboolean doexit);	// NERVE - SMF
void            Sys_OpenURL(const char *url, qboolean doexit);	// NERVE - SMF
int             Sys_GetHighQualityCPU();
float           Sys_GetCPUSpeed(void);

#ifdef __linux__
// TTimo only on linux .. maybe on Mac too?
// will OR with the existing mode (chmod ..+..)
void            Sys_Chmod(char *file, int mode);
#endif

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */

#define NYT HMAX				/* NYT = Not Yet Transmitted */
#define INTERNAL_NODE ( HMAX + 1 )

typedef struct nodetype
{
	struct nodetype *left, *right, *parent;	/* tree structure */
	struct nodetype *next, *prev;	/* doubly-linked list */
	struct nodetype **head;		/* highest ranked node in block */
	int             weight;
	int             symbol;
} node_t;

#define HMAX 256				/* Maximum symbol */

typedef struct
{
	int             blocNode;
	int             blocPtrs;

	node_t         *tree;
	node_t         *lhead;
	node_t         *ltail;
	node_t         *loc[HMAX + 1];
	node_t        **freelist;

	node_t          nodeList[768];
	node_t         *nodePtrs[768];
} huff_t;

typedef struct
{
	huff_t          compressor;
	huff_t          decompressor;
} huffman_t;

/*void            Huff_Compress(msg_t * buf, int offset);
void            Huff_Decompress(msg_t * buf, int offset);
void            Huff_Init(huffman_t * huff);
void            Huff_addRef(huff_t * huff, byte ch);
int             Huff_Receive(node_t * node, int *ch, byte * fin);
void            Huff_transmit(huff_t * huff, int ch, byte * fout);
void            Huff_offsetReceive(node_t * node, int *ch, byte * fin, int *offset);
void            Huff_offsetTransmit(huff_t * huff, int ch, byte * fout, int *offset);
void            Huff_putBit(int bit, byte * fout, int *offset);
int             Huff_getBit(byte * fout, int *offset);*/

extern huffman_t clientHuffTables;

#define SV_ENCODE_START     4
#define SV_DECODE_START     12
#define CL_ENCODE_START     12
#define CL_DECODE_START     4

void            Com_GetHunkInfo(int *hunkused, int *hunkexpected);

// TTimo
// dll checksuming stuff, centralizing OS-dependent parts
// *_SHIFT is the shifting we applied to the reference string

#if defined( _WIN32 )

// qagame_mp_x86.dll
#define SYS_DLLNAME_QAGAME_SHIFT 6
#define SYS_DLLNAME_QAGAME "wgmgskesve~><4jrr"

// cgame_mp_x86.dll
#define SYS_DLLNAME_CGAME_SHIFT 2
#define SYS_DLLNAME_CGAME "eicogaoraz:80fnn"

// ui_mp_x86.dll
#define SYS_DLLNAME_UI_SHIFT 5
#define SYS_DLLNAME_UI "zndrud}=;3iqq"

#elif defined( __linux__ )

#if defined(__x86_64__)

// qagame.mp.x86_64.so
#define SYS_DLLNAME_QAGAME_SHIFT 6
#define SYS_DLLNAME_QAGAME "wgmgsk4sv4~><e<:4yu"

// cgame.mp.x86_64.so
#define SYS_DLLNAME_CGAME_SHIFT 2
#define SYS_DLLNAME_CGAME "eicog0or0z:8a860uq"

// ui.mp.x86_64.so
#define SYS_DLLNAME_UI_SHIFT 5
#define SYS_DLLNAME_UI "zn3ru3}=;d;93xt"

#else
// qagame.mp.i386.so
#define SYS_DLLNAME_QAGAME_SHIFT 6
#define SYS_DLLNAME_QAGAME "wgmgsk4sv4o9><4yu"

// cgame.mp.i386.so
#define SYS_DLLNAME_CGAME_SHIFT 2
#define SYS_DLLNAME_CGAME "eicog0or0k5:80uq"

// ui.mp.i386.so
#define SYS_DLLNAME_UI_SHIFT 5
#define SYS_DLLNAME_UI "zn3ru3n8=;3xt"

#endif

#elif __MACOS__

#ifdef _DEBUG
// qagame_d_mac
#define SYS_DLLNAME_QAGAME_SHIFT 6
#define SYS_DLLNAME_QAGAME "wgmgskejesgi"

// cgame_d_mac
#define SYS_DLLNAME_CGAME_SHIFT 2
#define SYS_DLLNAME_CGAME "eicogafaoce"

// ui_d_mac
#define SYS_DLLNAME_UI_SHIFT 5
#define SYS_DLLNAME_UI "zndidrfh"
#else
// qagame_mac
#define SYS_DLLNAME_QAGAME_SHIFT 6
#define SYS_DLLNAME_QAGAME "wgmgskesgi"

// cgame_mac
#define SYS_DLLNAME_CGAME_SHIFT 2
#define SYS_DLLNAME_CGAME "eicogaoce"

// ui_mac
#define SYS_DLLNAME_UI_SHIFT 5
#define SYS_DLLNAME_UI "zndrfh"
#endif

#else

#error unknown OS

#endif

#define LittleLong(x) x
#define LittleFloat(x) x

#endif							// _QCOMMON_H_
