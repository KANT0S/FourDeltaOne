#include "StdInc.h"
#include "DW.h"

void dw_build_lsg_ticket(char* buf, char* key)
{
	memset(buf, 0, 128);
	memcpy(buf, key, 24);
}

void dw_build_game_ticket(char* buf, char* key, int gameID)
{
	dw_game_ticket* ticket = (dw_game_ticket*)buf;
	memset(ticket, 0xA, sizeof(dw_game_ticket));

	ticket->signature[0] = 0xDE;
	ticket->signature[1] = 0xAD;
	ticket->signature[2] = 0xBD;
	ticket->signature[3] = 0xEF;

	ticket->licenseType = 0;
	ticket->gameID = gameID;
	ticket->userID = 0x1100001DEADC0DE;
	ticket->nick[0] = 'a';
	ticket->nick[1] = 0;

	memcpy(ticket->key, key, sizeof(ticket->key));
}

/*void dw_im_received(NPID from, const uint8_t* buf, uint32_t buflen)
{
	dwMessage msg(nullptr, 2, false);
	msg.byteBuffer.writeUInt32(21);
	msg.byteBuffer.writeUInt64(from);
	msg.byteBuffer.writeString("me");
	msg.byteBuffer.writeBlob((const char*)buf, buflen);
	msg.send(true);

	Trace("dwhandler", "received IM from %llx", from);
}*/