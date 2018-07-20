using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DWServer
{
    class DWTitles
    {
        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            if (type == 12 && crypt)
            {
                var packet = DWRouter.GetMessage(data);
                var call = packet.ByteBuffer.ReadByte();

                switch (call)
                {
                    case 6:
                        GetServerTime(data, packet);
                        break;
                    default:
                        //DWRouter.Unknown(data, packet);
                        Log.Info("unknown packet " + call + " in bdTitleUtilities");
                        break;
                }
            }
        }

        private static void GetServerTime(MessageData data, DWMessage packet)
        {
            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000000);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)6);
            reply.ByteBuffer.Write((uint)1);
            reply.ByteBuffer.Write((uint)1);
            reply.ByteBuffer.Write(DateTime.UtcNow.ToUnixTime());
            reply.Send(true);
        }
    }
}
