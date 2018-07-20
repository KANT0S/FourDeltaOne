using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace GBH
{
    public static class Win32System
    {
        public static void Initialize()
        {
            TimeCaps caps = new TimeCaps();
            timeGetDevCaps(ref caps, 8);
            timeBeginPeriod(caps.wPeriodMin);            
        }

        public static uint GetTime()
        {
            return timeGetTime();
        }

        [DllImport("winmm.dll", SetLastError = true)]
        static extern UInt32 timeGetDevCaps(ref TimeCaps timeCaps,
                                            UInt32 sizeTimeCaps);

        [StructLayout(LayoutKind.Sequential)]
        public struct TimeCaps
        {
            public UInt32 wPeriodMin;
            public UInt32 wPeriodMax;
        };

        [DllImport("winmm.dll")]
        static extern uint timeBeginPeriod(uint uMilliseconds);

        [DllImport("winmm.dll")]
        static extern uint timeGetTime();

    }
}
