// ==========================================================
// secretSchemes
// 
// Component: steam_api
// Sub-component: clientdll
// Purpose: Some simple branding for the T5 client.
//
// Initial author: Terminator
// Started: 2012-04-28
// ==========================================================

#include "StdInc.h"

void PatchT5_Branding()
{
#ifdef USE_STUPID_BRANDING
	// window title
	//strcpy((char*)0xA575A4, "Call of Duty: secretSchemes");
    
	// change console text
	strcpy((char*)0xA92684, "COD_SECRETSCHEMES_MP");
	
	//strcpy((char*)0xA59C0C, GetMOTD("")); // TICKER_MESSAGE_CATEGORY_CLAN_CAPS
	strcpy((char*)0xA8BA78, "secretSchemes"); // TICKER_MESSAGE_CATEGORY_COD_CAPS (COD)

	// server name changes
	strcpy((char*)0xA28DF4, "secretSchemesPublic"); // er, not sure about this?
	strcpy((char*)0xA2CA70, "secretSchemesPrivate"); // private match

	// this isn't how it's meant to be done but it's 04:23 and i want to sleep
	strcpy((char*)0xA52590, "Welcome to Call of Duty: secretSchemes!\n----- Initializing Renderer ----\n");

	// and that should be it?
#endif
}