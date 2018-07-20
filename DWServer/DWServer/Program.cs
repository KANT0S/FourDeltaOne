using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading;

namespace DWServer
{
    class Program
    {
        public static TitleID Game { get; set; }

        static void Main(string[] args)
        {
#if !DEBUG
            Log.Initialize("DWServer.log", LogLevel.Data | LogLevel.Info | LogLevel.Error, true);
#else
            Log.Initialize("DWServer.log", LogLevel.Data | LogLevel.Info | LogLevel.Error | LogLevel.Debug | LogLevel.Verbose, true);
#endif
            //Log.Info("Starting DW server");
            Log.Info(string.Format("Starting DW server ({0})", (args.Length == 1) ? args[0] : "t5"));

            var game = (args.Length == 1) ? args[0] : "t5";
            
            if (game == "t5")
            {
                Game = TitleID.T5;
            }
            else if (game == "iw5")
            {
                Game = TitleID.IW5;
            }

            /*
            TCPHandler ipServer = new TCPHandler(3074);
            ipServer.Start();
            TCPHandler ipServer3 = new TCPHandler(3077);
            ipServer3.Start();
             */
            Database.Initialize();
            DWRouter.OnStart();
            //TCPHandler.OnStart();

            var handler = new TCPHandler();
            handler.Start((game == "t5") ? 3074 : 3078);

            Heatmap.StartThread();

            HttpHandler.Start();
            /*MessageData server2 = new MessageData("none");
            server2.Arguments["name"] = "demonware2";
            server2.Arguments["port"] = (int)3077;
            TCPHandler.Net_TcpListen("none", server2);*/

            new Thread(() =>
            {
                while (true)
                {
                    //Thread.Sleep(30000); // from alterOps DWServer
                    Thread.Sleep(3000); // from internLabs DWServer

                    try
                    {
                        DWMatch.CleanSessions();
                    }
                    catch (Exception e)
                    {
                        Log.Error(e.ToString());
                    }
                }
            }).Start();

            new Thread(() =>
            {
                while (true)
                {
                    Thread.Sleep(30000);

                    try
                    {
                        //DWGroups.updateCache();
                    }
                    catch (Exception e)
                    {
                        Log.Error(e.ToString());
                    }
                }
            }).Start();

            while (true)
            {
                try
                {
                    Log.WriteAway();
                }
                catch (Exception e) { Log.Error(e.ToString()); }

                Thread.Sleep(1000);
            }
        }
    }
}
