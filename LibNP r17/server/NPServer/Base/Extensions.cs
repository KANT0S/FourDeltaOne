using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace NPx
{
    public static class Extensions
    {
        public static bool IsAtEOF(this Stream stream)
        {
            return (stream.Position >= stream.Length);
        }

        public static bool CanReadBytes(this Stream stream, int bytes)
        {
            return ((stream.Position + bytes) <= stream.Length);
        }

        public static string ClearColors(this string name)
        {
            return Regex.Replace(name, "\\^([0-9])", "");
        }

        public static string Truncate(this string str, int characters)
        {
            if (str.Length > characters)
            {
                return str.Substring(0, characters);
            }

            return str;
        }

        public static string ReadNullString(this BinaryReader reader)
        {
            string retval = "";
            char[] buffer = new char[1];
            buffer[0] = '\xFF';

            while (true)
            {
                var length = reader.Read(buffer, 0, 1);

                if (length == 0 || buffer[0] == 0)
                {
                    break;
                }

                retval += buffer[0];
                buffer[0] = '\0';
            }

            return retval;
        }

        public static string ReadFixedString(this BinaryReader reader, int length)
        {
            byte[] buffer = new byte[length];
            reader.Read(buffer, 0, length);

            string retval = Encoding.ASCII.GetString(buffer);
            return retval.Trim('\0');
        }

        public static uint ToUnixTime(this DateTime time)
        {
            var origin = new DateTime(1970, 1, 1, 0, 0, 0, 0);
            var diff = time - origin;
            return (uint)Math.Floor(diff.TotalSeconds);
        }
    }
}
