using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Xml.Linq;

using NPx;

namespace NPx
{
    public class SteamFriendsLinker
    {
        private Thread[] _threads;
        private Thread _thread2;
        private WebClient _client;
        private static ManualResetEvent _event;
        private static Queue<UpdateRequest> _requestQueue;

        private class UpdateRequest
        {
            public int UserID { get; set; }
            public long SteamID { get; set; }
        }

        public SteamFriendsLinker()
        {
            _client = new WebClient();
            _requestQueue = new Queue<UpdateRequest>();
            _event = new ManualResetEvent(false);
        }

        public void Start()
        {
            Log.Info("Starting SteamFriendsLinker");

            _threads = new Thread[4];

            for (int i = 0; i < 4; i++)
            {
                _threads[i] = new Thread(Run);
                _threads[i].Start();
            }

            _thread2 = new Thread(RunUpdater);
            _thread2.Start();
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

        private void RunUpdater()
        {
            var database = XNP.Create();

            if (database == null)
            {
                return;
            }
            
            while (true)
            {
                try
                {
                    /*var links = from link in database.ExternalPlatforms
                                select link;

                    foreach (var link in links)
                    {
                        UpdateForUser(link.UserID, link.PlatformID);
                    }*/
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }

                Thread.Sleep(6 * 60 * 60 * 1000); // 6 hours
            }
        }

        private void Handle(UpdateRequest request)
        {
            var data = _client.DownloadString(string.Format("http://steamcommunity.com/profiles/{0}/friends?xml=1", request.SteamID));
            var document = XDocument.Parse(data);

            if (document.Descendants("error").Count() > 0)
            {
                Log.Error("profile " + request.SteamID + ": " + document.Descendants("error").First().Value);
                return;
            }

            DeleteExternalFriendsForUser(request.UserID);

            var friends = (from friend in document.Descendants("friend")
                           select long.Parse(friend.Value)).ToArray();

            Log.Info(string.Format("Found {0} friends for user {1}.", friends.Count(), document.Descendants("steamID").First().Value));

            var people = from linkage in Database.ExternalPlatforms
                         where friends.Contains(linkage.PlatformID)
                         select linkage.UserID;

            foreach (var person in people)
            {
                Database.Friends.InsertOnSubmit(new Friends()
                {
                    UserID = request.UserID,
                    FriendID = person,
                    IsExternal = 1,
                    Friend = 1
                });
            }

            Database.SubmitChanges();
        }

        private void DeleteExternalFriendsForUser(int userID)
        {
            var externalFriends = from friend in Database.Friends
                                  where friend.IsExternal == 1 && friend.UserID == userID
                                  select friend;

            foreach (var friend in externalFriends)
            {
                Database.Friends.DeleteOnSubmit(friend);
            }
        }

        public static void UpdateForUser(int userID, long steamID)
        {
            return;

            lock (_requestQueue)
            {
                _requestQueue.Enqueue(new UpdateRequest()
                {
                    UserID = userID,
                    SteamID = steamID
                });
            }

            _event.Set();
        }
    }
}
