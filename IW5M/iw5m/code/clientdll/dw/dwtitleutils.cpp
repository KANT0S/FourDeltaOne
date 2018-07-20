#include "StdInc.h"
#include "dw.h"
#include "dwMessage.h"
#include <time.h>

void dw_tutils_get_server_time(bdByteBuffer& data)
{
	dwMessage reply(1, false);
	reply.byteBuffer.writeUInt64(0x8000000000000001);
	reply.byteBuffer.writeUInt32(0);
	reply.byteBuffer.writeByte(6);
	reply.byteBuffer.writeUInt32(1);
	reply.byteBuffer.writeUInt32(1);
	reply.byteBuffer.writeUInt32(time(NULL));
	reply.send(true);
}

void dw_dml_get_user_data(bdByteBuffer& data)
{
	dwMessage reply(1, false);
	reply.byteBuffer.writeUInt64(0x8000000000000001);
	reply.byteBuffer.writeUInt32(0);
	reply.byteBuffer.writeByte(8);
	reply.byteBuffer.writeUInt32(1);
	reply.byteBuffer.writeUInt32(1);

	reply.byteBuffer.writeString("US");
	reply.byteBuffer.writeString("'murrica");
	reply.byteBuffer.writeString("California");
	reply.byteBuffer.writeString("Los Angeles");
	reply.byteBuffer.writeFloat(0.00f);
	reply.byteBuffer.writeFloat(0.00f);

	reply.send(true);
}

void dw_messaging_send_global_im(bdByteBuffer& data)
{
	NPID onlineID;
	char* msg; int msglen;

	data.readUInt64(&onlineID);
	data.readBlob(&msg, &msglen);

	NP_SendMessage(onlineID, (uint8_t*)msg, msglen);

	Trace("dwtitleutils", "sent instant message to %llx", onlineID);
}

void dw_handle_tutils_message(int type, const char* buf, int len)
{
	bdByteBuffer data((char*)buf, len);
	char subtype = 0;
	data.readByte(&subtype);

	// bdTitleUtilities
	if (type == 12)
	{
		switch (subtype)
		{
			case 6:
				dw_tutils_get_server_time(data);
				break;
			default:
				Trace("dwtitleutils", "call %i", subtype);
				break;
		}
	}
	// bdDML
	else if (type == 27)
	{
		switch (subtype)
		{
			case 2:
				dw_dml_get_user_data(data);
				break;
		}
	}
	// bdMessaging
	else if (type == 6)
	{
		switch (subtype)
		{
			case 14:
				dw_messaging_send_global_im(data);
				break;
		}
	}
}