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

#include "stdinc.h"

MSG_Init_t MSG_Init = (MSG_Init_t)0;
DB_FindXAssetHeader_t DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0;
Dvar_SetCommand_t Dvar_SetCommand = (Dvar_SetCommand_t)0x440FA0;
Cbuf_AddText_t Cbuf_AddText = (Cbuf_AddText_t)0x4C1030;
FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x512F40; // DS
Con_Print_t Con_Print = (Con_Print_t)0x417D20;
Dvar_FindVar_t Dvar_FindVar = (Dvar_FindVar_t)0;
Com_Error_t Com_Error = (Com_Error_t)0;
Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x4DE3A0;
Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x4DE1C0;
Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x4DE210;
Scr_AddEntityNum_t Scr_AddEntityNum = (Scr_AddEntityNum_t)0x4DE360;
Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x4DE4C0;
SL_GetString_t SL_GetString = (SL_GetString_t)0x4D8100;
Scr_NotifyNum_t Scr_NotifyNum = (Scr_NotifyNum_t)0x4DD3B0;
Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x537E70;
Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x4DE1B0;
Scr_GetString_t Scr_GetString = (Scr_GetString_t)0x4DDB70;
SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x4E8B80;
Cmd_TokenizeString_t Cmd_TokenizeString = (Cmd_TokenizeString_t)0x4BF680;
Cmd_EndTokenizedString_t Cmd_EndTokenizedString = (Cmd_EndTokenizedString_t)0x4BF6B0;
Scr_NotifyLevel_t Scr_NotifyLevel = (Scr_NotifyLevel_t)0x4DD3E0;
FS_Printf_t FS_Printf = (FS_Printf_t)0x438120;
Key_KeynumToString_t Key_KeynumToString = (Key_KeynumToString_t)0x4CCFF0;

bool g_isDedicated;

DWORD* cmd_id = (DWORD*)0;
DWORD* cmd_argc = (DWORD*)0;
DWORD** cmd_argv = (DWORD**)0;

DWORD* cmd_id_sv = (DWORD*)0x1B3EB98;
DWORD* cmd_argc_sv = (DWORD*)0x1B3EBDC;
DWORD** cmd_argv_sv = (DWORD**)0x1B3EBFC;

DWORD* scr_numParam = (DWORD*)0x1F3E418;

gentity_t* g_entities = (gentity_t*)0x18FBB28;

void Com_Printf(int channel, const char* format, ...)
{
	static char buffer[32768];
	va_list ap;
	va_start(ap, format);
	_vsnprintf(buffer, sizeof(buffer), format, ap);
	va_end(ap);

	buffer[sizeof(buffer) - 1] = '\0';

	if (g_isDedicated)
	{
		__asm
		{
			push offset buffer
			push channel
			mov eax, 4CB8A0h // Com_Print
			call eax
			add esp, 8h
		}
	}
	else
	{
		Con_Print(0, channel, buffer, 0, 0, 0.0f);
	}
}