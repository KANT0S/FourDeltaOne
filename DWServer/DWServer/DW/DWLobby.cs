using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DWServer
{
    class DWLobby
    {
        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");

            if (type == 7)
            {
                var packet = DWRouter.GetMessage(data);

                bool initialBool;
                uint gameID, randomNumber;
                byte[] ticket;

                packet.BitBuffer.UseDataTypes = false;
                packet.BitBuffer.ReadBoolean(out initialBool);
                packet.BitBuffer.UseDataTypes = true;

                packet.BitBuffer.ReadUInt32(out gameID);
                packet.BitBuffer.ReadUInt32(out randomNumber);

                packet.BitBuffer.ReadBytes(128, out ticket);

                // parse LSG ticket
                var key = DWTickets.GetKeyFromLSGTicket(ticket);
                DWRouter.SetGlobalKey(packet.Data, key);

                lock (DWRouter.Connections)
                {
                    var id = DWTickets.GetIDFromLSGTicket(ticket);
                    DWRouter.Connections[id] = packet.Data.Get<string>("cid");
                }

                lock (DWRouter.CIDToUser)
                {
                    var id = DWTickets.GetUserFromLSGTicket(ticket);
                    DWRouter.CIDToUser[packet.Data.Get<string>("cid")] = id;
                }

                lock (DWRouter.ConnectionsReverse)
                {
                    var id = DWTickets.GetIDFromLSGTicket(ticket);
                    DWRouter.ConnectionsReverse[packet.Data.Get<string>("cid")] = id;
                }

                lock (DWRouter.CIDToName)
                {
                    var name = DWTickets.GetNameFromLSGTicket(ticket);
                    DWRouter.CIDToName[packet.Data.Get<string>("cid")] = name;
                }

                lock (DWRouter.CIDToTitle)
                {
                    var title = (TitleID)gameID;
                    DWRouter.CIDToTitle[packet.Data.Get<string>("cid")] = title;
                }

                // now make a reply.
                /*
                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000000);
                reply.ByteBuffer.Write(0);
                reply.ByteBuffer.Write((byte)6);
                reply.ByteBuffer.Write(1);
                reply.ByteBuffer.Write(1);
                reply.ByteBuffer.Write(0x12345678);
                reply.Send(true);*/
            }
        }
    }
}
