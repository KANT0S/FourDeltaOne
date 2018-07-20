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

void bdSteamAuthService::callService(bdServiceServer* server, std::string& message)
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

	unsigned int ticketLength;
	data.readUInt32(&ticketLength);

	char ticket[1024];
	data.readBytes(ticketLength, (BYTE*)ticket);

	Trace("dwauth", "got steam auth message: randomNumber %i, gameID %i, tlen %i", randomNumber, gameID, ticketLength);

	BYTE iv[24];
	dw_calculate_iv(0xDEADC0DE, iv);

	char gameTicket[128];
	char lsgTicket[128];

	dw_build_game_ticket(gameTicket, GLOBAL_KEY, gameID);
	dw_build_lsg_ticket(lsgTicket, GLOBAL_KEY);

	char encryptedGameTicket[128];
	dw_encrypt_data(gameTicket, iv, (BYTE*)(ticket + 32), encryptedGameTicket, sizeof(encryptedGameTicket));

	//dwMessage reply(29, true);
	std::shared_ptr<dwMessage> reply = server->makeMessage(29, true);
	reply->bitBuffer.setUseDataTypes(false);
	reply->bitBuffer.writeBoolean(false);
	reply->bitBuffer.writeUInt32(700);
	reply->bitBuffer.writeUInt32(0xDEADC0DE); // IV source, clearly
	reply->bitBuffer.writeBytes(128, (BYTE*)encryptedGameTicket);
	reply->bitBuffer.writeBytes(128, (BYTE*)lsgTicket);
	reply->send(false);
}