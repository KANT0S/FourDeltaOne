using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;
using MongoDB.Driver;
using MongoDB.Driver.Builders;

namespace DWServer
{
    public class DWEventLog
    {
        public class BinaryEvent
        {
            [BsonId]
            public ObjectId id;
            public byte[] data;
            public int type;
        }

        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            var packet = DWRouter.GetMessage(data);
            var call = packet.ByteBuffer.ReadByte();

            switch (call)
            {
                case 2:
                    LogBinaryEvent(data, packet);
                    break;
            }
        }

        // not native name
        private static void LogBinaryEvent(MessageData mdata, DWMessage packet)
        {
            var data = packet.ByteBuffer.ReadBlob();
            var type = packet.ByteBuffer.ReadUInt32();

            var e = new BinaryEvent()
            {
                data = data,
                type = (int)type
            };

            Database.ABlobEvents.Insert(e);

            Log.Debug("Inserted a binary event of type " + type);

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)8);
            reply.ByteBuffer.Write(0);
            reply.ByteBuffer.Write(0);
        }
    }
}
