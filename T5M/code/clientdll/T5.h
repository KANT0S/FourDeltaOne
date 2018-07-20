#pragma once

typedef enum
{
	DVAR_FLAG_NONE			= 0x0,			//no flags
	DVAR_FLAG_SAVED		    = 0x1,			//saves in config_mp.cfg for clients
	
	DVAR_FLAG_DEDISAVED		= 0x1000000,		//unknown
	DVAR_FLAG_NONEXISTENT	= 0xFFFFFFFF	//no such dvar
} dvar_flag;

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
	DVAR_TYPE_INT64		= 9
} dvar_type;

union dvar_value_t {
	char*	string;
	int		integer;
	float	value;
	bool	boolean;
	float	vec2[2];
	float	vec3[3];
	float	vec4[4];
	BYTE	color[4]; //to get float: multiply by 1/255
	__int64 integer64;
};

struct dvar_t
{
	const char* name;
	const char* description; // typically 0 in T5
	int hash;
	int flags;
	int type;
	int pad2;
	dvar_value_t current;

};

typedef void (__cdecl * Com_Error_t)(int type, const char* format, ...);
extern Com_Error_t Com_Error;

typedef dvar_t* (__cdecl * Dvar_FindVar_t)(const char* name);
extern Dvar_FindVar_t Dvar_FindVar;

typedef dvar_t* (__cdecl * Dvar_SetCommand_t)(const char* name, const char* value);
extern Dvar_SetCommand_t Dvar_SetCommand;

typedef dvar_t* (__cdecl * Dvar_RegisterString_t)(const char* name, const char* default, int, const char*);
extern Dvar_RegisterString_t Dvar_RegisterString;