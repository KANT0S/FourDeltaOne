// ==========================================================
// T5M project
// 
// Component: client
// Sub-component: loopdw
// Purpose: "Steam" authentication service.
//
// Initial author: NTAuthority
// Started: 2013-04-15
// ==========================================================

#include "StdInc.h"
#include "DW.h"
#include "bdAuthServer.h"

void bdDediAuthService::callService(bdServiceServer* server, std::string& message)
{
	bool unknownBool;
	unsigned int randomNumber;
	unsigned int gameID;
	char keyBase[8];

	bdBitBuffer data(message);

	data.setUseDataTypes(false);
	data.readBoolean(&unknownBool);
	data.setUseDataTypes(true);

	data.readUInt32(&randomNumber);
	data.readUInt32(&gameID);

	data.read(64, keyBase);

	Trace("dwauth", "got dedi auth message: randomNumber %i, gameID %i", randomNumber, gameID);

	BYTE iv[24];
	dw_calculate_iv(0xDEADC0DE, iv);

	char gameTicket[128];
	char lsgTicket[128];

	dw_build_game_ticket(gameTicket, GLOBAL_KEY, gameID);
	dw_build_lsg_ticket(lsgTicket, GLOBAL_KEY);

	dw_game_ticket* ticket = (dw_game_ticket*)gameTicket;
	ticket->licenseType = 4;

	char encryptedGameTicket[128];
	dw_encrypt_data(gameTicket, iv, (BYTE*)DEDI_KEY_HASH, encryptedGameTicket, sizeof(encryptedGameTicket));

	//dwMessage reply(29, true);
	std::shared_ptr<dwMessage> reply = server->makeMessage(13, true);
	reply->bitBuffer.setUseDataTypes(false);
	reply->bitBuffer.writeBoolean(false);
	reply->bitBuffer.writeUInt32((keyBase[0] == DEDI_KEY_HASH[0]) ? 700 : 706);
	reply->bitBuffer.writeUInt32(0xDEADC0DE); // IV source, clearly
	reply->bitBuffer.writeBytes(128, (BYTE*)encryptedGameTicket);
	reply->bitBuffer.writeBytes(128, (BYTE*)lsgTicket);
	reply->send(false);
}