using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace NPx
{
    public class ProfileData
    {
        private StructuredData _structuredData;

        private Thread _thread;
        private static ManualResetEvent _event;
        private static Queue<UpdateRequest> _requestQueue;

        private class UpdateRequest
        {
            public long UserID { get; set; }
        }

        public ProfileData()
        {
            _requestQueue = new Queue<UpdateRequest>();
            _event = new ManualResetEvent(false);
            _structuredData = new StructuredData("data/playerdata.xml");
        }

        public void Start()
        {
            Log.Info("Starting ProfileData");

            _thread = new Thread(Run);
            _thread.Start();

            if (!File.Exists("data/migration_profiledata_001_done"))
            {
                var thread = new Thread(Migrate);
                thread.Start();
            }
        }

        private void Migrate()
        {
            // wait for the database to be connected
            Thread.Sleep(5000);

            // get unique user IDs that are 'important' to fetch
            var database = XNP.Create();
            var query = from association in database.ExternalPlatforms
                        select association.UserID;

            foreach (var userID in query)
            {
                Handle(0x110000100000000 | (uint)userID);
            }

            File.WriteAllText("data/migration_profiledata_001_done", "yep");
        }

        private void Handle(UpdateRequest request)
        {
            Log.Info("Handling profile update request for " + request.UserID.ToString("X16"));

            var filename = StorageUtils.GetFilename("iw4.stat", request.UserID);

            if (!File.Exists(filename))
            {
                return;
            }

            _structuredData.IW5 = true;
            _structuredData.SetData(File.ReadAllBytes(filename));
            
            var prestige = _structuredData.Get("prestige").Get<int>();
            var experience = _structuredData.Get("experience").Get<int>();
            var cardicon = _structuredData.Get("cardIcon").Get<string>();
            var cardtitle = _structuredData.Get("cardTitle").Get<string>();

            var shortUserID = (int)(request.UserID & 0xFFFFFFFF);
            var profileData = new IW4Profiles();
            var isNew = true;

            var query = from profile in Database.IW4Profiles
                        where profile.UserID == shortUserID
                        select profile;

            if (query.Count() > 0)
            {
                profileData = query.First();
                isNew = false;
            }

            profileData.UserID = shortUserID;
            profileData.Prestige = (sbyte)prestige;
            profileData.Experience = experience;
            profileData.CardIcon = cardicon;
            profileData.CardTitle = cardtitle;

            if (isNew)
            {
                Database.IW4Profiles.InsertOnSubmit(profileData);
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

        public static void Handle(long userID)
        {
            lock (_requestQueue)
            {
                _requestQueue.Enqueue(new UpdateRequest() { UserID = userID });
            }

            _event.Set();

            //Log.Debug("Adding an event.");
        }
    }
}
