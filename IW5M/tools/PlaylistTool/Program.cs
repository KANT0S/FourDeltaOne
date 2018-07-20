using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Ionic.Zlib;

namespace PlaylistTool
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("Usage: PlaylistTool.exe [build|extract]");
                Console.WriteLine("Make sure playlists.aggr/playlists.info/stuff.info are in the current directory.");
                return;
            }

            var mode = args[0];

            if (mode == "build")
            {
                Build();
            }
            else if (mode == "extract")
            {
                Extract();
            }
            else
            {
                Console.WriteLine("Invalid mode: " + mode);
            }
        }

        private static void Build()
        {
            var playlists = ZlibStream.CompressBuffer(File.ReadAllBytes("playlists.info"));
            var stuff = ZlibStream.CompressBuffer(File.ReadAllBytes("stuff.info"));

            var stream = File.Open("playlists.aggr", FileMode.Create, FileAccess.Write);
            var writer = new BinaryWriter(stream);

            writer.Write((ushort)1);
            writer.Write((ushort)(12 + 0));
            writer.Write((ushort)playlists.Length);

            writer.Write((ushort)1);
            writer.Write((ushort)(12 + playlists.Length));
            writer.Write((ushort)stuff.Length);

            writer.Write(playlists);
            writer.Write(stuff);

            writer.Close();
        }

        private static void Extract()
        {
            var stream = File.OpenRead("playlists.aggr");
            var reader = new BinaryReader(stream);

            var streams = new List<byte[]>();

            while (reader.ReadUInt16() == 1)
            {
                var offset = reader.ReadUInt16();
                var length = reader.ReadUInt16();

                var oldPos = stream.Position;
                stream.Position = offset;

                var bytes = reader.ReadBytes(length);
                streams.Add(ZlibStream.UncompressBuffer(bytes));

                stream.Position = oldPos;
            }

            File.WriteAllBytes("playlists.info", streams[0]);
            File.WriteAllBytes("stuff.info", streams[1]);
        }
    }
}
