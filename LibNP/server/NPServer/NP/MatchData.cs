using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml.Linq;
using System.Globalization;

namespace NPx
{
    public class MatchData
    {
        private Thread _thread;
        private static ManualResetEvent _event;
        private static Queue<UpdateRequest> _requestQueue;

        private class UpdateRequest
        {
            public string Data { get; set; }
        }

        public MatchData()
        {
            _requestQueue = new Queue<UpdateRequest>();
            _event = new ManualResetEvent(false);
        }

        public void Start()
        {
            Log.Info("Starting MatchData");

            _thread = new Thread(Run);
            _thread.Start();
        }

        private void Handle(UpdateRequest request)
        {
            Log.Debug("Whopla!");

            var matchString = request.Data;
            var document = XDocument.Parse(matchString.Split(new char[] { '\n' }, 2)[1]);

            var map = document.Root.Attribute("map").Value;
            var gameType = document.Root.Attribute("gametype").Value;
            var time = document.Root.Attribute("date").Value;

            var affectedIDs = from gameEvent in document.Descendants()
                              where gameEvent.Attribute("from") != null && !gameEvent.Attribute("from").Value.StartsWith("bot")
                              select (int)(long.Parse(gameEvent.Attribute("from").Value, NumberStyles.AllowHexSpecifier) & 0xFFFFFFFF);

            affectedIDs = affectedIDs.Concat(from gameEvent in document.Descendants()
                                             where gameEvent.Attribute("to") != null && !gameEvent.Attribute("to").Value.StartsWith("bot")
                                             select (int)(long.Parse(gameEvent.Attribute("to").Value, NumberStyles.AllowHexSpecifier) & 0xFFFFFFFF));

            affectedIDs = affectedIDs.Distinct();

            var uniqueID = Guid.NewGuid().ToString().ToLower();

            var filename = string.Format("data/matches/{0}/{1}/{2}.xml", gameType, map, uniqueID);
            Directory.CreateDirectory(Path.GetDirectoryName(filename));
            File.WriteAllText(filename, document.ToString());

            var match = new Matches();
            match.MatchGuiD = uniqueID;
            match.MatchGameType = gameType;
            match.MatchFilename = filename;
            match.MatchMap = map;
            match.MatchTime = int.Parse(time);

            Database.Matches.InsertOnSubmit(match);
            Database.SubmitChanges();

            foreach (var affectedID in affectedIDs)
            {
                var matchUser = new MatchUsers();
                matchUser.MatchID = uniqueID;
                matchUser.MatchUser = affectedID;

                Database.MatchUsers.InsertOnSubmit(matchUser);
            }

            Database.SubmitChanges();
        }

        private XNP Database { get; set; }

        private void Run()
        {
            Log.Info("Connecting to the database...");

            Database = XNP.Create();

            if (Database == null)
            {
                return;
            }

            while (true)
            {
                _event.WaitOne();
                _event.Reset();

                while (_requestQueue.Count > 0)
                {
                    UpdateRequest request;

                    lock (_requestQueue)
                    {
                        request = _requestQueue.Dequeue();
                    }

                    try
                    {
                        Handle(request);
                    }
                    catch (Exception e)
                    {
                        Log.Error("Exception: " + e.ToString());
                    }
                }
            }
        }

        public static void Handle(string matchScores)
        {
            lock (_requestQueue)
            {
                _requestQueue.Enqueue(new UpdateRequest() { Data = matchScores });
                _event.Set();

                Log.Debug("Adding an event.");
            }
        }
    }

}
