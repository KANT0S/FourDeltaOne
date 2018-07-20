using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml.Linq;

using Moserware.Skills;

namespace NPx
{
    public class Skills
    {
        private Thread _thread;
        private static ManualResetEvent _event;
        private static Queue<UpdateRequest> _requestQueue;

        private class UpdateRequest
        {
            public string Data {get;set;}
        }

        public Skills()
        {
            _requestQueue = new Queue<UpdateRequest>();
            _event = new ManualResetEvent(false);
        }

        public void Start()
        {
            Log.Info("Starting Skills");

            _thread = new Thread(Run);
            _thread.Start();
        }

        private void Handle(UpdateRequest request)
        {
            var matchString = request.Data;
            var document = new XDocument(matchString);

            if (document.Descendants("teams").Count() > 0)
            {
                HandleTeamBased(document);
            }
            else
            {
                //HandleDM(document);
            }
        }

        private void HandleTeamBased(XDocument document)
        {
            var alliesScore = int.Parse(document.Descendants("allies").First().Value);
            var axisScore = int.Parse(document.Descendants("axis").First().Value);

            var allies = new Team();
            var axis = new Team();

            var players = from player in document.Descendants("player")
                          select new
                          {
                              NPID = (int)(long.Parse(player.Descendants("xuid").First().Value, System.Globalization.NumberStyles.AllowHexSpecifier) & 0xFFFFFFFF),
                              Team = (player.Descendants("team").First().Value == "allies") ? allies : axis
                          };

            var playerIDs = (from player in players
                             select player.NPID).ToArray();

            var ranks = GetRanks(playerIDs);

            foreach (var player in players)
            {

            }
        }

        private Dictionary<int, Rating> GetRanks(int[] players)
        {
            // TODO: implement stuff
            return null;
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

                try
                {
                    while (_requestQueue.Count > 0)
                    {
                        UpdateRequest request;

                        lock (_requestQueue)
                        {
                            request = _requestQueue.Dequeue();
                        }

                        Handle(request);
                    }
                }
                catch (Exception e)
                {
                    Log.Error("Exception: " + e.ToString());
                }
            }
        }

        public static void Handle(string matchScores)
        {
            lock (_requestQueue)
            {
                _requestQueue.Enqueue(new UpdateRequest() { Data = matchScores });
            }
        }
    }
}
