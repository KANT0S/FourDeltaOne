using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;

namespace GBH
{
    public class ConsoleLogListener : ILogListener
    {
        public void LogMessage(string source, string message, LogLevel level)
        {
            var levelS = level.ToString().ToUpper();
            var sourceS = (source == string.Empty) ? string.Empty : ("[" + source + "]");

            var text = string.Format("[{0}] {1} - {3}", (Game.Time / 1000.0).ToString("0.000"), sourceS, levelS, message);
            Console.WriteLine(text);
        }

        public bool WantsFilteredMessages { get { return true; } }
    }
}
