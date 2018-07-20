using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;
using MongoDB.Driver;
using MongoDB.Driver.Builders;

using Ionic.Zlib;
using System.IO;
using System.Globalization;
using System.Threading;

namespace UpdateMatchData
{
    class Program
    {
        public static MongoServer Server { get; set; }
        public static MongoDatabase ADatabase { get; set; }
        private static StructuredData Data { get; set; }

        static void Main(string[] args)
        {
            // connect to the mongo instance
            Server = MongoServer.Create("mongodb://localhost");

            var credentials = new MongoCredentials("alterops", "tsd8hdASD");
            ADatabase = Server.GetDatabase("alterops", credentials);

            // fetch all type 1 binary events (1 = public match)
            var collection = ADatabase.GetCollection<BinaryEvent>("blobEvents");
            var query = Query.EQ("type", 1);
            var events = collection.Find(query);

            Data = new StructuredData("matchdata.def");

            foreach (var bevent in events)
            {
                ProcessEvent(bevent.data);

                //collection.Remove(Query.EQ("_id", bevent.id));
            }
        }

        static void ProcessEvent(byte[] blob)
        {
            var decompressedBlob = ZlibStream.UncompressBuffer(blob);
            Data.SetData(decompressedBlob);

            var affectedIDs = new List<int>();

            for (int i = 0; i < 30; i++)
            {
                var xuidTag = string.Format("players.{0}.xuid", i);
                var xuid = Data.Get(xuidTag).Get<string>();

                if (xuid == "")
                {
                    break;
                }

                var userID = 0;
                int.TryParse(xuid.Substring(8), NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture.NumberFormat, out userID);

                if (userID == 0)
                {
                    continue;
                }

                affectedIDs.Add(userID);
            }

            var uniqueID = Guid.NewGuid();
            var matchData = new MatchData()
            {
                matchID = uniqueID,
                matchGameType = Data.Get("gametype").Get<string>(),
                matchMap = Data.Get("map").Get<string>(),
                matchTime = new DateTime(1970, 1, 1, 0, 0, 0).AddSeconds(Data.Get("dateTime").Get<int>()),
                matchBlob = blob
            };

            var matches = ADatabase.GetCollection<MatchData>("matchesIW5");
            matches.Insert(matchData);

            var matchUsers = new List<MatchUser>();
            foreach (var user in affectedIDs)
            {
                matchUsers.Add(new MatchUser()
                {
                    userID = user,
                    matchID = uniqueID
                });
            }

            var users = ADatabase.GetCollection<MatchUser>("matchUsersIW5");
            users.InsertBatch(matchUsers);

            Console.WriteLine("wrote match " + uniqueID);

            Thread.Sleep(50);
        }
    }

    public class MatchData
    {
        [BsonId]
        public Guid matchID;
        public string matchGameType;
        public string matchMap;
        public DateTime matchTime;
        public byte[] matchBlob;
    }

    public class MatchUser
    {
        [BsonId]
        public ObjectId id;
        public Guid matchID;
        public int userID;
    }

    public class BinaryEvent
    {
        [BsonId]
        public ObjectId id;
        public byte[] data;
        public int type;
    }
}
