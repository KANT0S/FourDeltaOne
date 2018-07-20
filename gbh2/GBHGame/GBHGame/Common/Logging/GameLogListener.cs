using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;

namespace GBH
{
    public class GameLogListener : ILogListener
    {
        public void LogMessage(string source, string message, LogLevel level)
        {
            ConsoleRenderer.Print(message);
        }

        public bool WantsFilteredMessages { get { return true; } }
    }
}
