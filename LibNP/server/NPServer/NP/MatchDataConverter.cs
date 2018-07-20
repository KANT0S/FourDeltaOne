using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;
using MongoDB.Driver;
using MongoDB.Driver.Builders;

using Ionic.Zlib;

namespace NPx
{
    public class MatchDataConverter
    {
        private Thread _thread;
        public static MongoServer Server { get; set; }
        public static MongoDatabase ADatabase { get; set; }
        private static StructuredData Data { get; set; }

        public void Start()
        {
            try
            {
                Server = MongoServer.Create("mongodb://localhost");

                var credentials = new MongoCredentials("alterops", "tsd8hdASD");
                ADatabase = Server.GetDatabase("alterops", credentials);

                Data = new StructuredData("data/matchdata_iw5.xml");
                Data.IW5 = true;

                _thread = new Thread(Run);
                _thread.Start();
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
            }
        }

        private void Run()
        {
            Database = XNP.Create();

            if (Database == null)
            {
                return;
            }

            while (true)
            {
                try
                {
                    // fetch all type 1 binary events (1 = public match)
                    var collection = ADatabase.GetCollection<BinaryEvent>("blobEvents");
                    var query = Query.EQ("type", 1);
                    var events = collection.Find(query);

                    foreach (var bevent in events)
                    {
                        ProcessEvent(bevent.data);

                        collection.Remove(Query.EQ("_id", bevent.id));
                        break;
                    }
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }

                Thread.Sleep(5 * 60);
            }
        }

        private XNP Database { get; set; }

        private void ProcessEvent(byte[] blob)
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

            var map = Data.Get("map").Get<string>();
            var gameType = Data.Get("gametype").Get<string>();

            var uniqueID = Guid.NewGuid().ToString().ToLower();

            var filename = string.Format("data/matches/iw5/{0}/{1}/{2}.match", gameType, map, uniqueID);
            Directory.CreateDirectory(Path.GetDirectoryName(filename));
            File.WriteAllBytes(filename, blob);

            var match = new Matches();
            match.MatchGuiD = uniqueID;
            match.MatchGameType = gameType;
            match.MatchFilename = filename;
            match.MatchMap = map;
            match.MatchTime = Data.Get("dateTime").Get<int>();

            Database.Matches.InsertOnSubmit(match);
            Database.SubmitChanges();

            foreach (var affectedID in affectedIDs)
            {
                var matchUser = new MatchUsers();
                matchUser.MatchID = uniqueID;
                matchUser.MatchUser = affectedID;

                Database.MatchUsers.InsertOnSubmit(matchUser);
            }
        }
    }

    public class BinaryEvent
    {
        [BsonId]
        public ObjectId id;
        public byte[] data;
        public int type;
    }
}
