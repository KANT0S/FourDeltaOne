using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MongoDB.Bson;
using MongoDB.Driver;
using MongoDB.Driver.Builders;
using MongoDB.Bson.Serialization.Attributes;

namespace DWServer
{
    public class DWProfiles
    {
        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            var packet = DWRouter.GetMessage(data);
            var call = packet.ByteBuffer.ReadByte();

            switch (call)
            {
                case 1:
                    GetPublicInfos(data, packet);
                    break;
                case 3:
                    SetPublicInfo(data, packet);
                    break;
            }
        }

        public class PublicProfile
        {
            [BsonId]
            public ObjectId id;

            public int user_id;
            public int profile_int;
            public byte[] profile_blob;
            public int blobsize;
        }

        private static void GetPublicInfos(MessageData data, DWMessage packet)
        {
            var entityIDs = new List<BsonInt32>();
            while (packet.ByteBuffer.PeekByte() == 10)
            {
                entityIDs.Add((int)(packet.ByteBuffer.ReadUInt64() & 0xFFFFFFFF));
            }

            var profileInfos = new List<PublicProfileInfo>();

            
            var query = Query.In("user_id", entityIDs);
            var profiles = Database.APublicProfile.Find(query);

            if (profiles.Count() > 0)
            {
                foreach (var profile in profiles)
                {
                    profileInfos.Add(new PublicProfileInfo()
                    {
                        UserID = (ulong)(0x110000100000000 | (uint)profile.user_id),
                        UnknownInt = profile.profile_int,
                        ProfileData = profile.profile_blob
                    });
                }
            }

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)8);
            reply.ByteBuffer.Write((uint)profileInfos.Count());
            reply.ByteBuffer.Write((uint)profileInfos.Count());

            foreach (var info in profileInfos)
            {
                info.Serialize(reply);
            }

            reply.Send(true);
        }

        private static void SetPublicInfo(MessageData data, DWMessage packet)
        {
            var profileInfo = new PublicProfileInfo();
            profileInfo.Deserialize(packet);

            /*ulong user = 0;
            lock (DWRouter.Connections)
            {
                user = (from conn in DWRouter.Connections
                        where conn.Value == data.Get<string>("cid")
                        select conn.Key).FirstOrDefault();
            }*/
            ulong user = DWRouter.GetIDForData(data);

            var existing = Database.APublicProfile.Find(Query.EQ("user_id", (int)(user & 0xFFFFFFFF)));
            var item = new PublicProfile();

            if (existing.Count() > 0)
            {
                item = existing.First();
            }

            item.user_id = (int)(user & 0xFFFFFFFF);
            item.profile_int = profileInfo.UnknownInt;
            item.profile_blob = profileInfo.ProfileData;
            item.blobsize = profileInfo.ProfileData.Length;

            Database.APublicProfile.Save(item);


            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)8);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        public class PublicProfileInfo
        {
            public ulong UserID { get; set; }
            public int UnknownInt { get; set; }
            public byte[] ProfileData { get; set; }

            public void Deserialize(DWMessage packet)
            {
                if (Program.Game == TitleID.T5)
                {
                    UnknownInt = packet.ByteBuffer.ReadInt32();
                }

                ProfileData = packet.ByteBuffer.ReadBlob();
            }

            public void Serialize(DWMessage packet)
            {
                packet.ByteBuffer.Write(UserID);

                if (Program.Game == TitleID.T5)
                {
                    packet.ByteBuffer.Write(UnknownInt);
                }

                packet.ByteBuffer.WriteBlob(ProfileData);
            }
        }
    }
}
