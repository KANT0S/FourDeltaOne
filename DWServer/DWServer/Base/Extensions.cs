using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DWServer
{
    public static class Extensions
    {
        public static uint ToUnixTime(this DateTime time)
        {
            TimeSpan ts = (time.ToUniversalTime() - new DateTime(1970, 1, 1, 0, 0, 0));
            return (uint)(ts.TotalSeconds);
        }

        public static string ToHexString(this byte[] buffer)
        {
            var sb = new StringBuilder();
            foreach (var b in buffer)
            {
                sb.Append(b.ToString("x2"));
            }

            return sb.ToString();
        }

        public static byte[] ParseHexString(string str)
        {
            if ((str.Length % 2) != 0)
            {
                throw new ArgumentException("string length is not a multiple of 2", "str");
            }

            var bytes = new byte[str.Length / 2];
            for (int i = 0; i < bytes.Length; i++)
            {
                bytes[i] = byte.Parse(str.Substring(i * 2, 2), System.Globalization.NumberStyles.AllowHexSpecifier);
            }

            return bytes;
        }

        public static byte[] SwapBytes64(byte[] bytes)
        {
            if ((bytes.Length % 8) != 0)
            {
                throw new ArgumentException("array length is not a multiple of 8", "bytes");
            }

            var retval = new byte[bytes.Length];
            var cycles = bytes.Length / 8;

            for (int i = 0; i < cycles; i++)
            {
                for (int j = 0, k = 7; j < 8; j++, k--)
                {
                    retval[(i * 8) + k] = bytes[(i * 8) + j];
                }
            }

            return retval;
        }

    }
}
