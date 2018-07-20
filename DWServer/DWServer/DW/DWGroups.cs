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
    class GroupUser
    {
        [BsonId]
        public ObjectId id;
        public long userID;
        public int groupID;
    }
    class GroupUserCache
    {
        [BsonId]
        public ObjectId id;
        public int groupID;
        public int totalCount;
    }

    class DWGroups
    {
        public static void Net_TcpDisconnected(MessageData data)
        {
            /*ulong userID = 0;

            lock (DWRouter.Connections)
            {
                userID = (from conn in DWRouter.Connections
                          where conn.Value == data.Get<string>("cid")
                          select conn.Key).FirstOrDefault();
            }*/

            ulong userID = DWRouter.GetIDForData(data);;

            if (userID == 0)
            {
                return;
            }

            RemoveUserGroups(userID);
        }

        private static void RemoveUserGroups(ulong userID)
        {
            var query = Query.EQ("userID", (long)userID);

            Database.AGroupUser.Remove(query);
        }

        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            var packet = DWRouter.GetMessage(data);
            var call = packet.ByteBuffer.ReadByte();

            switch (call)
            {
                case 1:
                    SetGroups(data, packet);
                    break;
                case 4:
                    GetGroupCounts(data, packet);
                    break;
            }
        }

        private static void GetGroupCounts(MessageData data, DWMessage packet)
        {
            packet.ByteBuffer.DataTypePackingEnabled = true;
            var a = packet.ByteBuffer.ReadByte(); // 108
            var b = packet.ByteBuffer.ReadUInt32(); // count * 4
            var count = packet.ByteBuffer.ReadUInt32();

            var groups = new List<BsonValue>();

            for (int i = 0; i < count; i++)
            {
                groups.Add((int)packet.ByteBuffer.ReadUInt32());
            }
        
            var results = Database.AGroupUserCache.Find(Query.In("groupID", groups));

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)28);
            reply.ByteBuffer.Write((uint)results.Count());
            reply.ByteBuffer.Write((uint)results.Count());

            foreach (var result in results)
            {
                reply.ByteBuffer.Write((uint)result.groupID);
                reply.ByteBuffer.Write((uint)result.totalCount);
            }
            
            reply.Send(true);
        }

        private static void SetGroups(MessageData data, DWMessage packet)
        {
            ulong userID = DWRouter.GetIDForData(data);

            /*lock (DWRouter.Connections)
            {
                userID = (from conn in DWRouter.Connections
                          where conn.Value == data.Get<string>("cid")
                          select conn.Key).FirstOrDefault();
            }*/

            RemoveUserGroups(userID);

            packet.ByteBuffer.DataTypePackingEnabled = true;
            var a = packet.ByteBuffer.ReadByte(); // 108
            var b = packet.ByteBuffer.ReadUInt32(); // count * 4
            var count = packet.ByteBuffer.ReadUInt32();

            var groups = new List<GroupUser>();

            for (int i = 0; i < count; i++)
            {
                groups.Add(new GroupUser() { groupID = (int)packet.ByteBuffer.ReadUInt32(), userID = (long)userID });
            }

            Database.AGroupUser.InsertBatch(groups);

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)28);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        public static void updateCache()
        {
            int serverCount = 0;
            var results = Database.AGroupUser.Group(Query.Null, "groupID", new BsonDocument("count", 0), new BsonJavaScript("function(obj, prev) { prev.count++; }"), null);

            var groups = new List<GroupUserCache>();
            foreach (var result in results)
            {
                groups.Add(new GroupUserCache() { groupID = (int)result["groupID"].AsDouble, totalCount = (int)result["count"].AsDouble });
            }

            IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> sessions = new Dictionary<ulong, bdMatchMakingInfo>();
            lock (DWMatch.Sessions)
            {
                sessions = from session in DWMatch.Sessions
                           where (session.Value.TitleID == TitleID.T5 && (((MatchMakingInfoT5)session.Value).LicenseType == 2 || ((MatchMakingInfoT5)session.Value).LicenseType == 4))
                           select session;

                serverCount = sessions.Count();
            }

            groups.Add(new GroupUserCache() { groupID = 490, totalCount = serverCount });
            groups.Add(new GroupUserCache() { groupID = 491, totalCount = 0 });
            groups.Add(new GroupUserCache() { groupID = 492, totalCount = 0 });
            Database.AGroupUserCache.RemoveAll();
            Database.AGroupUserCache.EnsureIndex("groupID");
            Database.AGroupUserCache.InsertBatch(groups);
        }
    }
}
