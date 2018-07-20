using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GBH
{
    [Flags]
    public enum LogLevel
    {
        None = 0,
        Trace = 1,
        Debug = 2,
        Info = 4,
        Warning = 8,
        Error = 16,
        Critical = 32,
        All = 63
    }
}
