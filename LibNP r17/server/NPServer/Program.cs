using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading;

using DreamSeat;

namespace NPx
{
    public class Program
    {
        public static XNP Database { get; private set; }
        public static bool IW5Available { get; private set; }

        static void Main(string[] args)
        {
#if !DEBUG
            Log.Initialize("NPx.log", LogLevel.Data | LogLevel.Info | LogLevel.Error, true);
#else
            Log.Initialize("NPx.log", LogLevel.All & ~LogLevel.Debug, true);
#endif
            Log.Info("NPServer starting...");

            // connect to the database
            Database = XNP.Create();

            if (Database == null)
            {
                return;
            }

            // initialize CouchDB client
            CDatabase.Start();

            // start the NP socket
            NPSocket streamServer = new NPSocket();
            streamServer.Start();
            
            // start the NP auth API
            NPAuthAPI authAPI = new NPAuthAPI();
            authAPI.Start();

            // start the Steam friend linker
            //SteamFriendsLinker steamLinker = new SteamFriendsLinker();
            //steamLinker.Start();

            // start the profile data handler
            ProfileData profileData = new ProfileData();
            profileData.Start();

            // start the matchdata parser
            MatchData matchData = new MatchData();
            matchData.Start();

            // start the matchdata converter
            //MatchDataConverter matchDataConverter = new MatchDataConverter();
            //matchDataConverter.Start();

            // start aux threads
            NPHandler.Start();
            TaskScheduler.Start();
            HttpHandler.Start();

            StatFileHandler.Register();

            TaskScheduler.EnqueueTask(Servers.CleanSessions, 300);

            // main idle loop
            while (true)
            {
                try
                {
                    Log.WriteAway();
                }
                catch (Exception e) { Log.Error(e.ToString()); }

                if (File.Exists("data/iw5m"))
                {
                    IW5Available = true;
                }

                Thread.Sleep(5000);
            }
        }
    }
}
