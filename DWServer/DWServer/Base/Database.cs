using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading;

using MongoDB.Bson;
using MongoDB.Driver;

namespace DWServer
{
    class Database
    {
        public static string connectionString =
                "mongodb://5.231.40.23";
              //"mongodb://localhost";

        public static MongoServer Server { get; set; }
        public static MongoDatabase ADatabase { get; set; }
        public static MongoCollection<GroupUser> AGroupUser { get; set; }
        public static MongoCollection<GroupUserCache> AGroupUserCache { get; set; }
        public static MongoCollection<DWProfiles.PublicProfile> APublicProfile { get; set; }
        public static MongoCollection<DWStorage.File> AFiles { get; set; }
        public static MongoCollection<DWEventLog.BinaryEvent> ABlobEvents { get; set; }
        public static MongoCollection<DWAuther.ServerKey> AServerKeys { get; set; }

        public static void Initialize()
        {
            Server = MongoServer.Create(connectionString);

            var credentials = new MongoCredentials("DWServer", "uMuYd18Gf");
            ADatabase = Server.GetDatabase("DWServer", credentials);

            if (Program.Game == TitleID.T5)
            {
                // clear data and load collections
                AGroupUser = ADatabase.GetCollection<GroupUser>("groupUsersT5");
                AGroupUserCache = ADatabase.GetCollection<GroupUserCache>("groupUsersCacheT5");
                APublicProfile = ADatabase.GetCollection<DWProfiles.PublicProfile>("publicProfilesT5");
            }
            else if (Program.Game == TitleID.IW5)
            {
                // clear data and load collections
                AGroupUser = ADatabase.GetCollection<GroupUser>("groupUsersIW5");
                AGroupUserCache = ADatabase.GetCollection<GroupUserCache>("groupUsersCacheIW5");
                APublicProfile = ADatabase.GetCollection<DWProfiles.PublicProfile>("publicProfilesIW5");
            }

            AGroupUser.RemoveAll();
            AGroupUser.EnsureIndex("groupID");
            AGroupUser.EnsureIndex("userID");

            AGroupUserCache.RemoveAll();
            AGroupUserCache.EnsureIndex("groupID");

            APublicProfile.EnsureIndex("user_id");

            AFiles = ADatabase.GetCollection<DWStorage.File>("files");
            AFiles.EnsureIndex("filename");

            ABlobEvents = ADatabase.GetCollection<DWEventLog.BinaryEvent>("blobEvents");

            AServerKeys = ADatabase.GetCollection<DWAuther.ServerKey>("serverKeysIW5");
            AServerKeys.EnsureIndex("keyHash");
        }
    }
}
