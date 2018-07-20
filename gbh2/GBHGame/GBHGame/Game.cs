using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using System.IO;

namespace GBH
{
    public static class Game
    {
        private static ConVar com_maxFPS;
        private static ConVar timescale;
        private static ConVar sv_running;
        private static ConVar cl_running;
        private static ConVar sv_paused;
        private static ConVar cl_paused;
        private static ConVar net_showpackets;
        public static ConVar mapname;
        public static ConVar nickname;

        public static void Initialize()
        {
            // set the current culture to the invariant culture
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;

            // initialize the logging service
            Log.Initialize(LogLevel.All);
            Log.AddListener(new ConsoleLogListener());
            Log.AddListener(new GameLogListener());
            Log.AddListener(new FileLogListener("GBHGame.log", false));

            Log.Write(LogLevel.Info, "GBH2 v0 initializing");
            Log.Write(LogLevel.Critical, "jeremych is an egghead and a nabsalad");

            ConVar.Initialize();
            FileSystem.Initialize();
            Win32System.Initialize();
            MapGeometry.Initialize();
            //StyleManager.Load("Styles/bil.sty");
            //MapManager.Load("Maps/MP1-comp.gmp");
            Camera.Initialize();
            NetManager.Initialize(29960);
            Client.Initialize();
            Server.Initialize();

            GameWindow.Initialize();
            
            com_maxFPS = ConVar.Register("com_maxFPS", 0, "Maximum framerate for the game loop.", ConVarFlags.Archived);
            timescale = ConVar.Register("timescale", 1.0f, "Scale time by this amount", ConVarFlags.Cheat);
            sv_running = ConVar.Register("sv_running", true, "Is the server running?", ConVarFlags.ReadOnly);
            cl_running = ConVar.Register("cl_running", false, "Is the client running?", ConVarFlags.ReadOnly);
            sv_paused = ConVar.Register("sv_paused", false, "Is the server paused?", ConVarFlags.ReadOnly);
            cl_paused = ConVar.Register("cl_paused", false, "Is the client paused?", ConVarFlags.ReadOnly);
            net_showpackets = ConVar.Register("net_showpackets", false, "Show network packets.", ConVarFlags.None);
            mapname = ConVar.Register("mapname", "", "Current mapname", ConVarFlags.ReadOnly);
            nickname = ConVar.Register("nickname", Environment.GetEnvironmentVariable("username"), "Your nickname", ConVarFlags.Archived);

            Renderer.Initialize();
            MaterialManager.ReadMaterialFile("base.material");
            MaterialManager.ReadMaterialFile("Styles/bil.material");
            ConsoleRenderer.Initialize();
            //StyleManager.CreateTextures(Renderer.Device);
            MapRenderer.Initialize(Renderer.Device);
            DeferredRenderer.Initialize(Renderer.Device);
            Renderer2D.Initialize(Renderer.Device);
            
            // jeez, people these days just need to get a *proper* nickname
            if (ConVar.GetValue<string>("nicknamee") == Environment.GetEnvironmentVariable("username"))
            {
                Log.Write(LogLevel.Info, "It looks it's your first time running GBH2. Please type 'nickname <WANTED NICKNAME>' to set your nickname.");
            }

            var path = Directory.GetCurrentDirectory() + "\\config.ini";
            IniFile ini = new IniFile(path);
            var value = ini.IniReadValue("CONFIG", "nickname");
            if (value != null)
            {
                ConVar.SetValue<string>("nicknamee", value);
            }
        }

        private static uint _lastTime;
        private static uint _frameTime;
        public static uint FrameMsec { get; private set; }
        public static float DeltaTime { get; private set; }

        public static void Process()
        {
            // limit FPS and handle events
            int minMsec = (com_maxFPS.GetValue<int>() > 0) ? (1000 / com_maxFPS.GetValue<int>()) : 1;
            uint msec = 0;

            do
            {
                _frameTime = EventSystem.HandleEvents();

                msec = _frameTime - _lastTime;
            } while (msec < minMsec);

            // handle time scaling
            var scale = timescale.GetValue<float>();
            msec = (uint)(msec * scale);

            if (msec < 1)
            {
                msec = 1;
            }
            else if (msec > 5000)
            {
                msec = 5000;
            }

            if (msec > 500)
            {
                Log.Write(LogLevel.Info, "Hitch warning: {0} msec frame time", msec);
            }

            DeltaTime = msec / 1000f;
            FrameMsec = msec;

            _lastTime = _frameTime;

            // process the command buffer
            Command.ExecuteBuffer();

            // handle network stuff
            NetManager.Process();

            // process game stuff
            Server.Process();

            // process client
            Client.Process();

            // more stuff (needs to be moved)
            Camera.Process();

            if (!sv_running.GetValue<bool>())
            {
                CellManager.Recenter(new[] { Camera.MainCamera.Position });
            }

            Renderer2D.InitPerFrame();
            FontManager.SetColor(Color.White);

            // camera moving
            DebugCamera.Process();

            ConsoleRenderer.Process();

            // render stuff
            if (Renderer.MakeDeviceAvailable())
            {
                Renderer.Clear();

                if (Client.State == Client.ClientState.Ingame)
                {
                    DeferredRenderer.Render3DStuff(Renderer.Device);
                }

                Renderer2D.Render(Renderer.Device);
                Renderer.Device.Present();
            }
        }

        #region game time
        private static long _timeBase = 0;

        public static uint Time
        {
            get
            {
                if (_timeBase == 0)
                {
                    _timeBase = Win32System.GetTime();
                }

                return (uint)(Win32System.GetTime() - _timeBase);
            }
        }
        #endregion
    }
}
