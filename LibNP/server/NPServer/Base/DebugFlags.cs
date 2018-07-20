using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NPx
{
    public static class DebugFlags
    {
        private static Dictionary<string, bool> _flags = new Dictionary<string, bool>();

        public static bool Get(string flag)
        {
            return _flags.TryGetValue(flag, false);
        }

        public static bool Toggle(string flag)
        {
            _flags[flag] = !Get(flag);

            return _flags[flag];
        }
    }
}
