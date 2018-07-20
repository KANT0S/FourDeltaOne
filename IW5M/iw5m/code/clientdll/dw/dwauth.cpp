#include "StdInc.h"
#include "dw.h"
#include "bdBitBuffer.h"
#include "dwMessage.h"

void dw_handle_auth_message_server(const char* buf, int len)
{
	bool unknownBool;
	unsigned int randomNumber;
	unsigned int gameID;
	char keyBase[8];

	bdBitBuffer data((char*)buf, len);

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

	char encryptedGameTicket[128];
	dw_encrypt_data(gameTicket, iv, (BYTE*)DEDI_KEY_HASH, encryptedGameTicket, sizeof(encryptedGameTicket));

	dwMessage reply(13, true);
	reply.bitBuffer.setUseDataTypes(false);
	reply.bitBuffer.writeBoolean(false);
	reply.bitBuffer.writeUInt32((keyBase[0] == DEDI_KEY_HASH[0]) ? 700 : 706);
	reply.bitBuffer.writeUInt32(0xDEADC0DE);
	reply.bitBuffer.writeBytes(128, (BYTE*)encryptedGameTicket);
	reply.bitBuffer.writeBytes(128, (BYTE*)lsgTicket);
	reply.send(false);
}

void dw_handle_auth_message_steam(const char* buf, int len)
{
	bool unknownBool;
	unsigned int randomNumber;
	unsigned int gameID;
	char keyBase[8];

	bdBitBuffer data((char*)buf, len);

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

	dwMessage reply(29, true);
	reply.bitBuffer.setUseDataTypes(false);
	reply.bitBuffer.writeBoolean(false);
	reply.bitBuffer.writeUInt32(700);
	reply.bitBuffer.writeUInt32(0xDEADC0DE);
	reply.bitBuffer.writeBytes(128, (BYTE*)encryptedGameTicket);
	reply.bitBuffer.writeBytes(128, (BYTE*)lsgTicket);
	reply.send(false);
}

void dw_handle_auth_message_register_server(const char* buf, int len)
{
	bool unknownBool;
	unsigned int randomNumber;
	unsigned int gameID;
	char rsaKey[140];

	bdBitBuffer data((char*)buf, len);

	data.setUseDataTypes(false);
	data.readBoolean(&unknownBool);
	data.setUseDataTypes(true);

	data.readUInt32(&randomNumber);
	data.readUInt32(&gameID);

	data.read(1120, rsaKey);

	Trace("dwauth", "got dedi register message: randomNumber %i, gameID %i", randomNumber, gameID);

	BYTE iv[24];
	dw_calculate_iv(0xDEADC0DE, iv);

	char gameTicket[128];
	char lsgTicket[128];

	dw_build_game_ticket(gameTicket, GLOBAL_KEY, gameID);
	dw_build_lsg_ticket(lsgTicket, GLOBAL_KEY);

	char encryptedGameTicket[128];
	dw_encrypt_data(gameTicket, iv, (BYTE*)DEDI_KEY_HASH, encryptedGameTicket, sizeof(encryptedGameTicket));

	dwMessage reply(25, true);
	reply.bitBuffer.setUseDataTypes(false);
	reply.bitBuffer.writeBoolean(false);
	reply.bitBuffer.writeUInt32(700);
	reply.bitBuffer.writeUInt32(0xDEADC0DE);
	reply.bitBuffer.writeBytes(128, (BYTE*)encryptedGameTicket);
	reply.bitBuffer.writeBytes(128, (BYTE*)lsgTicket);
	reply.bitBuffer.writeBytes(86, (BYTE*)DEDI_KEY_OB);
	reply.bitBuffer.writeInt32(1234);
	reply.send(false);
}

void dw_handle_auth_message(int type, const char* buf, int len)
{
	switch (type)
	{
		case 12:
			dw_handle_auth_message_server(buf, len);
			break;
		case 26:
			dw_handle_auth_message_register_server(buf, len);
			break;
		case 28:
			dw_handle_auth_message_steam(buf, len);
			break;
	}
}

void dw_handle_lobby_message(int type, const char* buf, int len)
{
	if (type == 7)
	{
		bdBitBuffer data((char*)buf, len);
		bool unknownBool;
		unsigned int gameID;
		unsigned int randomNumber;
		char ticket[128];

		data.setUseDataTypes(false);
		data.readBoolean(&unknownBool);
		data.setUseDataTypes(true);

		data.readUInt32(&gameID);
		data.readUInt32(&randomNumber);

		data.readBytes(128, (BYTE*)ticket);

		Trace("dwauth", "setting global key");

		dw_set_global_key((BYTE*)ticket); // ticket starts with the global key
	}
}