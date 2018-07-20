// ==========================================================
// alterOps project
// 
// Component: t5cli
// Sub-component: clientdll
// Purpose: General definitions for T5.
//
// Initial author: NTAuthority
// Started: 2012-01-18
// ==========================================================

#include "stdinc.h"

Dvar_RegisterString_t Dvar_RegisterString = (Dvar_RegisterString_t)0x42CB50;
Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x41AEA0;
Dvar_RegisterInt_t Dvar_RegisterInt = (Dvar_RegisterInt_t)0x41AEA0;
Dvar_FindVar_t Dvar_FindVar = (Dvar_FindVar_t)0x5C9790;

DW_ReadLicenseKey_t DW_ReadLicenseKey = (DW_ReadLicenseKey_t)0x648FA0;
Com_Error_t Com_Error = (Com_Error_t)0x61E640;
Cbuf_AddText_t Cbuf_AddText = (Cbuf_AddText_t)0x678FB0;
Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x5391D0;
Com_Printf_t Com_Printf = (Com_Printf_t)0x612A90;