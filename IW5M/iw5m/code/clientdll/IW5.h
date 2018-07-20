// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: IW5 function definitions
//
// Initial author: NTAuthority
// Started: 2012-01-29
// ==========================================================

#pragma once

typedef enum
{
	DVAR_TYPE_BOOL		= 0,
	DVAR_TYPE_FLOAT		= 1,
	DVAR_TYPE_FLOAT_2	= 2,
	DVAR_TYPE_FLOAT_3	= 3,
	DVAR_TYPE_FLOAT_4	= 4,
	DVAR_TYPE_INT		= 5,
	DVAR_TYPE_ENUM		= 6,
	DVAR_TYPE_STRING	= 7,
	DVAR_TYPE_COLOR		= 8,
} dvar_type;

union dvar_value_t {
	char*	string;
	int		integer;
	float	value;
	bool	boolean;
	float	vec2[2];
	float	vec3[3];
	float	vec4[4];
	BYTE	color[4];
};

union dvar_maxmin_t {
	int i;
	float f;
};

typedef struct dvar_t
{
	const char*		name;
	short			flags;
	char			pad1[2];
	char			type;
	char			pad2[3];
	dvar_value_t	current;
	dvar_value_t	latched;
	dvar_value_t	default;
	dvar_maxmin_t min;
	dvar_maxmin_t max;
} dvar_t;

typedef struct  
{
	int unknown1;
	int unknown2;
	char* data;
	int unknown3;
	int maxsize; // 16
	int cursize;
	int unknown4;
	int readcount; // 28
	char pad[12];
} msg_t;

typedef struct
{
	char pad[12];
} cmd_function_s;

extern bool isClient;

// netadr_t
typedef enum {
	NA_BOT,
	NA_BAD,					// an address lookup failed
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	//NA_IP6,
} netadrtype_t;

typedef enum {
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct {
	netadrtype_t	type;

	BYTE	ip[4];

	unsigned short	port;

	BYTE	ipx[10];
} netadr_t;

struct Image
{
	void* texture;
	char unknown2;
	char a3;
	char a2;
	char unknown3;
	char unknown4;
	char unknown5;
	char unknown6;
	char a4;
	char unknown7[16];
	char* name;
};

struct MaterialImage
{
	unsigned int typeHash; // asset hash of type
	char firstCharacter; // first character of image name
	char secondLastCharacter; // second-last character of image name (maybe only in CoD4?!)
	unsigned char unknown; // maybe 0xE2
	char unknown2; // likely 0x00
	Image* image; // Image* actually
};

struct Material
{
	const char* name; // 0
	unsigned short flags; // 4 // 0x2F00 for instance
	unsigned char animationX; // 6 // amount of animation frames in X
	unsigned char animationY; // 7 // amount of animation frames in Y
	char unknown1[4]; // 8 // 0x00
	unsigned int rendererIndex; // 12 // only for 3D models
	char unknown9[8]; // 16
	unsigned int unknown2; // 24 // 0xFFFFFFFF
	unsigned int unknown3; // 28 // 0xFFFFFF00
	char unknown4[46]; // 32 // 0xFF
	char numMaps; // 72/78 // 0x01, possibly 'map count' (zone code confirms)
	char unknown5; // 73/79 // 0x00
	char unknownCount2; // 74/80 // 0x01, maybe map count actually
	char unknown6; // 0x03
	unsigned short unknown7; // 0x04
	void* techniqueSet; // '2d' techset
	MaterialImage* maps; // map references
	unsigned int unknown8;
	void* stateMap; // might be NULL, need to test
};

typedef struct gentity_s
{
	char pad[628];
} gentity_t;

extern gentity_t* g_entities;

enum scriptType_e
{
	SCRIPT_NONE = 0,
	SCRIPT_OBJECT = 1,
	SCRIPT_STRING = 2,
	SCRIPT_VECTOR = 4,
	SCRIPT_FLOAT = 5,
	SCRIPT_INTEGER = 6
};

typedef struct  
{
	union
	{
		void* entity;
		float number;
		unsigned short string;
		float* vector;
		int integer;
	};
	scriptType_e type;
} VariableValue;

typedef void (__cdecl * MSG_Init_t)(msg_t* msg, char* data, int maxsize);
extern MSG_Init_t MSG_Init;

typedef void* (__cdecl * DB_FindXAssetHeader_t)(int type, const char* filename, bool);
extern DB_FindXAssetHeader_t DB_FindXAssetHeader;

typedef void (__cdecl * Dvar_SetCommand_t)(const char* var, const char* value);
extern Dvar_SetCommand_t Dvar_SetCommand;

typedef void (__cdecl * Cbuf_AddText_t)(int controller, const char* command);
extern Cbuf_AddText_t Cbuf_AddText;

typedef int (__cdecl * FS_ReadFile_t)(const char* filename, void** buffer);
extern FS_ReadFile_t FS_ReadFile;

typedef void (__cdecl * Con_Print_t)(int controller, int channel, const char* string, int a4, int a5, float a6);
extern Con_Print_t Con_Print;

typedef dvar_t* (__cdecl * Dvar_FindVar_t)(const char* name);
extern Dvar_FindVar_t Dvar_FindVar;

typedef void (__cdecl * Com_Error_t)(int type, const char* format, ...);
extern Com_Error_t Com_Error;

typedef void (__cdecl * Scr_AddInt_t)(int integer);
extern Scr_AddInt_t Scr_AddInt;

typedef void (__cdecl * Scr_AddFloat_t)(float value);
extern Scr_AddFloat_t Scr_AddFloat;

typedef void (__cdecl * Scr_AddString_t)(const char* string);
extern Scr_AddString_t Scr_AddString;

typedef void (__cdecl * Scr_AddEntityNum_t)(int entityNum, int type);
extern Scr_AddEntityNum_t Scr_AddEntityNum;

typedef short (__cdecl * SL_GetString_t)(const char* string, int a2);
extern SL_GetString_t SL_GetString;

typedef void (__cdecl * Scr_NotifyNum_t)(int number, int type, short notify, int numArgs);
extern Scr_NotifyNum_t Scr_NotifyNum;

typedef void (__cdecl * Scr_AddVector_t)(float* vector);
extern Scr_AddVector_t Scr_AddVector;

typedef char* (__cdecl * SL_ConvertToString_t)(short);
extern SL_ConvertToString_t SL_ConvertToString;

typedef int (__cdecl * Scr_GetNumParam_t)();
extern Scr_GetNumParam_t Scr_GetNumParam;

typedef const char* (__cdecl * Scr_GetString_t)(int index);
extern Scr_GetString_t Scr_GetString;

typedef void (__cdecl * Cmd_AddCommand_t)(const char*, void (*)(), cmd_function_s*);
extern Cmd_AddCommand_t Cmd_AddCommand;

typedef void (__cdecl * SV_GameSendServerCommand_t)(int targetEntity, int a2, const char* command);
extern SV_GameSendServerCommand_t SV_GameSendServerCommand;

typedef void (__cdecl * Cmd_TokenizeString_t)(char* token);
extern Cmd_TokenizeString_t Cmd_TokenizeString;

typedef void (__cdecl * Cmd_EndTokenizedString_t)();
extern Cmd_EndTokenizedString_t Cmd_EndTokenizedString;

typedef void (__cdecl * Scr_NotifyLevel_t)(short notify, int numArgs);
extern Scr_NotifyLevel_t Scr_NotifyLevel;

typedef void (__cdecl * FS_Printf_t)(int file, char* fmt, ...);
extern FS_Printf_t FS_Printf;

typedef char* (__cdecl * Key_KeynumToString_t)(int key, int a2);
extern Key_KeynumToString_t Key_KeynumToString;

typedef struct
{
	char name[32];
	int index;
	int license;
} dlcmap_t;

// inline cmd functions
extern DWORD* cmd_id;
extern DWORD* cmd_argc;
extern DWORD** cmd_argv;

extern DWORD* cmd_id_sv;
extern DWORD* cmd_argc_sv;
extern DWORD** cmd_argv_sv;

extern DWORD* scr_numParam;

inline int	Cmd_Argc( void )
{
	return cmd_argc[*cmd_id];
}

inline char *Cmd_Argv( int arg )
{
	if ( (unsigned)arg >= cmd_argc[*cmd_id] ) {
		return "";
	}
	return (char*)(cmd_argv[*cmd_id][arg]);	
}

inline int Cmd_Argc_sv( void )
{
	return cmd_argc_sv[*cmd_id_sv];
}

inline char *Cmd_Argv_sv( int arg )
{
	if ( (unsigned)arg >= cmd_argc_sv[*cmd_id_sv] ) {
		return "";
	}
	return (char*)(cmd_argv_sv[*cmd_id_sv][arg]);	
}

void Com_Printf(int channel, const char* format, ...);

bool Scriptability_OnSay(int client, char* name, char** text, int team);

bool Scriptability_ServerCommand(const char* command);

bool Scriptability_ClientCommand(const char* command, int entity);