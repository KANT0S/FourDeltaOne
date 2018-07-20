// ==========================================================
// blah
// 
// Initial author: NTAuthority
// Started: 2011-04-27
// ==========================================================

#pragma once
unsigned int InstallCallHook(unsigned int callAddress, unsigned int targetFunction);
unsigned int InstallJumpHook(unsigned int callAddress, unsigned int targetFunction);