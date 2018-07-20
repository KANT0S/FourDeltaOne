using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DWServer
{
    class DWMessaging
    {
        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            if (type == 6)
            {

                var packet = DWRouter.GetMessage(data);
                var call = packet.ByteBuffer.ReadByte();

                try
                {
                    switch (call)
                    {
                        case 1:
                            break;
                        case 14:
                            SendGlobalInstantMessage(data, packet);
                            break;
                        default:
                            Log.Debug("unknown packet "+call+" in bdMessaging");
                            //DWRouter.Unknown(data, packet);
                            break;
                    }
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }
            }
        }

        private static void SendGlobalInstantMessage(MessageData mdata, DWMessage packet)
        {
            var bdOnlineID = packet.ByteBuffer.ReadUInt64();
            var data = packet.ByteBuffer.ReadBlob();

            // route the message to the target user
            if (DWRouter.Connections.ContainsKey(bdOnlineID))
            {
                try
                {
                    //var ourID = (from conn in DWRouter.Connections
                    //             where conn.Value == mdata.Get<string>("cid")
                    //             select conn.Key).FirstOrDefault();
                    var ourID = DWRouter.GetIDForData(mdata);

                    var cid = DWRouter.Connections[bdOnlineID];
                    var treply = packet.MakeReply(2, false, cid); // 2 is 'push message'
                    treply.ByteBuffer.Write((uint)21);
                    treply.ByteBuffer.Write(ourID);
                    treply.ByteBuffer.Write("someone");
                    treply.ByteBuffer.WriteBlob(data);
                    treply.Send(true);
                    Log.Verbose("sent an instant message to " + bdOnlineID.ToString("X16") + " from " + ourID.ToString("X16"));
                }
                catch
                {
                    var reply2 = packet.MakeReply(1, false);
                    reply2.ByteBuffer.Write(0x8000000000000001);
                    reply2.ByteBuffer.Write((uint)0);
                    reply2.ByteBuffer.Write((byte)14);
                    reply2.ByteBuffer.Write((uint)0);
                    reply2.ByteBuffer.Write((uint)0);
                    reply2.Send(true);
                    return;
                }
                
            }

            // note: we shouldn't force success, but there's no clear error list yet
            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)14);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }
    }
}
