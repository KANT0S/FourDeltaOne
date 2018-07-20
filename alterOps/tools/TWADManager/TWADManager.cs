using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Ionic.Zlib;

namespace TWADManager
{
    public static class Extensions
    {
        public static void Write(this BinaryWriter Writer, string value, int length)
        {
            Writer.Write(Encoding.ASCII.GetBytes(value));
            int RemainingBytes = length - value.Length;
            for (int i = 0; i < RemainingBytes; i++)
                Writer.Write((byte)0x00);
        }

        public static void Write(this BinaryWriter Writer, UInt32 value, bool bigendian = false)
        {
            byte[] Value = BitConverter.GetBytes(value);
            if (bigendian)
                Array.Reverse(Value);

            Writer.Write(Value);
        }

        public static UInt32 ReadUInt32(this BinaryReader Reader, bool bigendian = false)
        {
            byte[] Value = Reader.ReadBytes(4);
            if (bigendian)
                Array.Reverse(Value);
            return BitConverter.ToUInt32(Value, 0);
        }

        public static string ReadString(this BinaryReader Reader, int length)
        {
            byte[] Value = Reader.ReadBytes(length);

            return Encoding.ASCII.GetString(Value).Trim('\0');
        }
    }

    public class TWADFile
    {
        public TWADFile(string FileLocation)
        {
            Console.WriteLine("Adding file: {0}", FileLocation);
            this.Content = File.ReadAllBytes(FileLocation);
            this.FileName = Path.GetFileName(FileLocation);
            this.Compressed = ZlibStream.CompressBuffer(Content);
        }

        public TWADFile(string FileName, UInt32 CompressedLength, UInt32 Length, UInt32 Offset)
        {
            Console.WriteLine("Found file: {0}", FileName);
            this.FileName = FileName;
            this.CompressedLength = CompressedLength;
            this.Length = Length;
            this.Offset = Offset;
        }

        public byte[] GetData()
        {
            return ZlibStream.UncompressBuffer(this.Compressed);
        }

        public byte[] Content { get; set; }
        public string FileName { get; set; }
        public byte[] Compressed { get; set; }

        public UInt32 CompressedLength { get; set; }
        public UInt32 Length { get; set; }
        public UInt32 Offset { get; set; }
    }

    class TWADManager
    {
        static void Main(string[] args)
        {
            if (args.Length < 2 || (args[0] != "create" && args[0] != "extract"))
            {
                Console.WriteLine("Usage: TWADManager <create|extract> <directory|file> [filename]");
            }
            else
            {
                if (args[0] == "create")
                {
                    if (!Directory.Exists(args[1]))
                    {
                        Console.WriteLine("The specified directory could not be found");
                        return;
                    }

                    List<TWADFile> TWADFiles = new List<TWADFile>();

                    foreach (string FilePath in Directory.GetFiles(args[1]))
                        TWADFiles.Add(new TWADFile(FilePath));

                    if (args.Length > 2)
                        GenerateTWAD(TWADFiles, args[2]);
                    else
                        GenerateTWAD(TWADFiles);
                }
                else
                {
                    if (!File.Exists(args[1]))
                    {
                        Console.WriteLine("The specified file could not be found");
                        return;
                    }
                    byte[] FileContent = File.ReadAllBytes(args[1]);
                    ParseTWAD(FileContent);
                }
            }
        }

        public static void ParseTWAD(byte[] FileContent)
        {
            MemoryStream TWADMStream = new MemoryStream(FileContent);
            BinaryReader TWADReader = new BinaryReader(TWADMStream);

            UInt32 Magic = TWADReader.ReadUInt32(true);

            if (Magic != 0xAB773354)
            {
                Console.WriteLine("ERROR: Specified file is not a valid TWAD file");
                return;
            }

            Directory.CreateDirectory("extracted");

            UInt32 TimeStamp = TWADReader.ReadUInt32();
            Console.WriteLine("TWAD was created on {0}", (new DateTime(1970, 1, 1, 0, 0, 0)).AddSeconds(TimeStamp));

            UInt32 FileCount = TWADReader.ReadUInt32();
            Console.WriteLine("TWAD contains {0} files", FileCount);

            TWADReader.ReadUInt32();

            List<TWADFile> TWADFiles = new List<TWADFile>();

            for (int i = 0; i < FileCount; i++)
            {
                string FileName = TWADReader.ReadString(32);
                UInt32 CompressedLength = TWADReader.ReadUInt32(true);
                UInt32 Length = TWADReader.ReadUInt32(true);
                UInt32 Offset = TWADReader.ReadUInt32(true);
                TWADFiles.Add(new TWADFile(FileName, CompressedLength, Length, Offset));
            }

            foreach (TWADFile FileEntry in TWADFiles)
            {
                TWADMStream.Position = FileEntry.Offset;
                FileEntry.Compressed = TWADReader.ReadBytes((Int32)FileEntry.CompressedLength);
                File.WriteAllBytes("extracted\\" + FileEntry.FileName, FileEntry.GetData());
            }

            TWADReader.Close();
            TWADMStream.Close();
        }

        public static void GenerateTWAD(List<TWADFile> TWADFiles, string FileName = "online_tu12_mp_english.wad")
        {
            Console.WriteLine("Creating new TWAD file: {0}", FileName);

            MemoryStream TWADMStream = new MemoryStream();
            BinaryWriter TWADWriter = new BinaryWriter(TWADMStream);

            // TWAD Identifier
            TWADWriter.Write((UInt32)0xAB773354, true);

            // Timestamp
            TimeSpan ts = (DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0));
            TWADWriter.Write((UInt32)ts.TotalSeconds);

            // File count
            TWADWriter.Write((UInt32)TWADFiles.Count);

            // What is this?
            TWADWriter.Write((UInt32)0);

            int CurrentOffset = 16 + (44 * TWADFiles.Count);

            foreach (TWADFile WFile in TWADFiles)
            {
                // Construct our directory
                TWADWriter.Write(WFile.FileName, 32);
                TWADWriter.Write((UInt32)WFile.Compressed.Length, true);
                TWADWriter.Write((UInt32)WFile.Content.Length, true);
                TWADWriter.Write((UInt32)CurrentOffset, true);
                CurrentOffset += WFile.Compressed.Length;
            }

            // Add the file data
            foreach (TWADFile WFile in TWADFiles)
                TWADWriter.Write(WFile.Compressed);

            // Write our file
            File.WriteAllBytes(FileName, TWADMStream.ToArray());

            // Close the streams
            TWADWriter.Close();
            TWADMStream.Close();
        }
    }
}
