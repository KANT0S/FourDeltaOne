using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace GBH
{
    public static class Command
    {
        public static void ExecuteNow(string command)
        {
            string[] args = Tokenize(command);

            if (args.Length == 0)
            {
                return;
            }

            ConVar.HandleCommand(args);

            // some quick commands
            if (args[0] == "a")
            {
                args = new[] { "map", "mp1-comp" };
            }
            else if (args[0] == "b")
            {
                args = new[] { "connect", "192.168.178.83:29960" };
            }

            // quick hack to allow a connect command
            if (args[0] == "connect")
            {
                Client.Connect_f(args);
            }
            else if (args[0] == "map")
            {
                if (args.Length != 2)
                {
                    Log.Write(LogLevel.Error, "Please enter the map name you want.");
                    return;
                }
                var mapName = args[1];

                MapManager.Load(string.Format("Maps/{0}.gmp", mapName));
                Server.InitializeMap(mapName);

                Client.Connect_f(new[] { "connect", "localhost" });
            }
            else if (args[0] == "say")
            {
                Client.SendReliableCommand(command);
            }

            if (args[0] == "quit")
            {
                Log.Write(LogLevel.Info, "Client shutting down..");
                Environment.Exit(1);
            }

            // status command, mmk?
            if (args[0] == "status")
            {
                Server.Status_f();
            }

            if (args[0] == "kick")
            {
                if (args.Length != 2)
                {
                    Log.Write(LogLevel.Error, "Please enter the nickname of the user you want to kick.");
                    return;
                }

                Server.Kick_f(args[1]);
            }

            if (args[0] == "clear")
            {
                Client.ClearConsole();
            }

            // isn't this an odd place to do it
            if (args[0] == "nickname")
            {
                // TODO: Introduce a config saving system based on the quake one.
                if (args[1].Length > 18)
                {
                    Log.Write(LogLevel.Error, "Your nickname is to long.");
                    return;
                }
                ConVar.SetValue<string>("nicknamee", args[1]);
                var path = Directory.GetCurrentDirectory() + "\\config.ini";
                IniFile ini = new IniFile(path);
                ini.IniWriteValue("CONFIG", "nickname", args[1]);
            }

            if (args[0] == "kill")
            {
                if (args.Length != 2)
                {
                    Log.Write(LogLevel.Error, "You didn't specify the user to 'kill'.");
                    return;
                }
                Server.KillClient(args[1]);
                /*SendReliableCommand(null, "print \"{0} drowned...\"", client.Name);
                //client.Entity.Die();
                client.Entity.Spawn();*/
            }
        }

        public static string[] Tokenize(string text)
        {
            int i = 0;
            int j = 0;
            string[] args = new string[0];

            while (true)
            {
                // skip whitespace and comments and such
                while (true)
                {
                    // skip whitespace and control characters
                    while (i < text.Length && text[i] <= ' ')
                    {
                        i++;
                    }

                    if (i >= text.Length)
                    {
                        return args;
                    }

                    // hopefully this will fix some errors
                    if (i == 0)
                    {
                        break;
                    }

                    // skip comments
                    if (text[i] == '/' && text[i + 1] == '/')
                    {
                        return args;
                    }

                    // /* comments
                    if (text[i] == '/' && text[i + 1] == '*')
                    {
                        while (i < (text.Length - 1) && (text[i] != '*' || text[i + 1] != '/'))
                        {
                            i++;
                        }

                        if (i >= text.Length)
                        {
                            return args;
                        }

                        i += 2;
                    }
                    else
                    {
                        break;
                    }
                }

                Array.Resize(ref args, args.Length + 1);

                StringBuilder arg = new StringBuilder();

                // quoted strings
                if (text[i] == '"')
                {
                    bool inEscape = false;

                    while (true)
                    {
                        i++;

                        if (i >= text.Length)
                        {
                            break;
                        }

                        if (text[i] == '"' && !inEscape)
                        {
                            break;
                        }

                        if (text[i] == '\\')
                        {
                            inEscape = true;
                        }
                        else
                        {
                            arg.Append(text[i]);
                            inEscape = false;
                        }
                    }

                    i++;

                    args[j] = arg.ToString();
                    j++;

                    if (i >= text.Length)
                    {
                        return args;
                    }

                    continue;
                }

                // non-quoted strings
                while (i < text.Length && text[i] > ' ')
                {
                    if (text[i] == '"')
                    {
                        break;
                    }

                    if (i < (text.Length - 1))
                    {
                        if (text[i] == '/' && text[i + 1] == '/')
                        {
                            break;
                        }

                        if (text[i] == '/' && text[i + 1] == '*')
                        {
                            break;
                        }
                    }

                    arg.Append(text[i]);

                    i++;
                }

                args[j] = arg.ToString();
                j++;

                if (i >= text.Length)
                {
                    return args;
                }
            }
        }

        private static string _commandBuffer = "";

        public static void AddToBuffer(string text)
        {
            _commandBuffer += text;
        }

        public static void ExecuteBuffer()
        {
            while (_commandBuffer.Length > 0)
            {
                // parse the command up to a newline/semicolon
                int i;
                bool inQuote = false;

                for (i = 0; i < _commandBuffer.Length; i++)
                {
                    if (_commandBuffer[i] == '"')
                    {
                        inQuote = !inQuote;
                    }

                    if (!inQuote && _commandBuffer[i] == ';')
                    {
                        break;
                    }

                    if (_commandBuffer[i] == '\r' || _commandBuffer[i] == '\n')
                    {
                        break;
                    }
                }

                string command = _commandBuffer.Substring(0, i);
                _commandBuffer = _commandBuffer.Substring(i + 1);
                
                // and execute the command
                ExecuteNow(command);
            }
        }
    }
}
