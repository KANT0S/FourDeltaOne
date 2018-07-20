// ==========================================================
// secretSchemes
// 
// Component: steam_api
// Sub-component: clientdll
// Purpose: Branding for the BlackOps SP client.
//
// Initial author: Terminator
// Started: 2012-05-08
// ==========================================================

#include "StdInc.h"

void PatchSP_Branding()
{
#ifdef USE_STUPID_BRANDING
	// change default clan tag
	//strcpy((char*)0xA78F98, "connect \"\\cg_predictItems\\1\\cl_punkbuster\\0\\cl_anonymous\\0\\color\\4\\head\\default\\model\\multi\\snaps\\20\\rate\\5000\\name\\%s\\clanAbbrev\\iNT\\bdOnlineUserID\\%s\\protocol\\%d\\qport\\%d\"");
    
	// window title
	strcpy((char*)0x9D839C, "Call of Duty: secretSchemes");
    
	// change console text
	strcpy((char*)0x9C3FC4, "COD_SECRETSCHEMES_SP");
	
	// change the motd text in the main menu
	strcpy((char*)0x9D375C, "secretSchemes"); // TICKER_MESSAGE_CATEGORY_COD_CAPS (COD)
#endif
}