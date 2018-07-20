using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Ionic.Zlib;

namespace DWServer.WAD
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
            //Console.WriteLine("Adding file: {0}", FileLocation);
            this.Content = File.ReadAllBytes(FileLocation);
            this.FileName = Path.GetFileName(FileLocation);
            this.Compressed = ZlibStream.CompressBuffer(Content);
        }

        public TWADFile(string FileName, UInt32 CompressedLength, UInt32 Length, UInt32 Offset)
        {
            //Console.WriteLine("Found file: {0}", FileName);
            this.FileName = FileName;
            this.CompressedLength = CompressedLength;
            this.Length = Length;
            this.Offset = Offset;
        }

        public TWADFile(byte[] Content, string FileName)
        {
            this.Content = Content;
            this.FileName = FileName;
            this.Compressed = ZlibStream.CompressBuffer(this.Content);
        }

        public byte[] GetData()
        {
            this.Content = ZlibStream.UncompressBuffer(this.Compressed);
            return this.Content;
        }

        public void ReCompress()
        {
            this.Compressed = ZlibStream.CompressBuffer(this.Content);
        }

        public byte[] Content { get; set; }
        public string FileName { get; set; }
        public byte[] Compressed { get; set; }

        public UInt32 CompressedLength { get; set; }
        public UInt32 Length { get; set; }
        public UInt32 Offset { get; set; }
    }


    class Manager
    {
        public static void ParseTWAD(byte[] FileContent)
        {
            MemoryStream TWADMStream = new MemoryStream(FileContent);
            BinaryReader TWADReader = new BinaryReader(TWADMStream);

            // Is our file a TWAD?
            UInt32 Magic = TWADReader.ReadUInt32(true);

            if (Magic != 0xAB773354)
            {
                // Nope.avi
                //Console.WriteLine("ERROR: Specified file is not a valid TWAD file");
                return;
            }

            // Create a new directory to put extracted files in
            Directory.CreateDirectory("extracted");

            // Read the timestamp of this TWADs' generation
            UInt32 TimeStamp = TWADReader.ReadUInt32();
            //Console.WriteLine("TWAD was created on {0}", (new DateTime(1970, 1, 1, 0, 0, 0)).AddSeconds(TimeStamp));

            // How many files are in here?
            UInt32 FileCount = TWADReader.ReadUInt32();
            //Console.WriteLine("TWAD contains {0} files", FileCount);

            TWADReader.ReadUInt32();

            List<TWADFile> TWADFiles = new List<TWADFile>();

            for (int i = 0; i < FileCount; i++)
            {
                // Get info for each file individually
                string FileName = TWADReader.ReadString(32);
                UInt32 CompressedLength = TWADReader.ReadUInt32(true);
                UInt32 Length = TWADReader.ReadUInt32(true);
                UInt32 Offset = TWADReader.ReadUInt32(true);
                TWADFiles.Add(new TWADFile(FileName, CompressedLength, Length, Offset));
            }

            foreach (TWADFile FileEntry in TWADFiles)
            {
                // Process file entries and extract them to the 'extracted' directory
                TWADMStream.Position = FileEntry.Offset;
                FileEntry.Compressed = TWADReader.ReadBytes((Int32)FileEntry.CompressedLength);
                File.WriteAllBytes("extracted\\" + FileEntry.FileName, FileEntry.GetData());
            }

            // Close the Streams
            TWADReader.Close();
            TWADMStream.Close();
        }



        public static void AutoGenerateTWADs(List<TWADFile> TWADFiles)
        {
            // Automatically generate TWADs for all available languages

            // Generate the default 'english' TWAD
            GenerateTWAD(TWADFiles);

            // Available languages - default 'english' should be first
            string[] Languages = new string[] { "english", "french", "german", "italian", "japanese", "polish", "russian", "spanish" };

            for (int i = 1; i < Languages.Length; i++)
            {
                // Create a new TWADFile list
                List<TWADFile> TWADFilesLoc = new List<TWADFile>();

                // Add already existing files while changing their data to match target language
                foreach (TWADFile TFile in TWADFiles)
                    TWADFilesLoc.Add(new TWADFile(Encoding.ASCII.GetBytes(Encoding.ASCII.GetString(TFile.Content).Replace(Languages[0], Languages[i])), TFile.FileName));

                // Generate the TWAD file
                GenerateTWAD(TWADFilesLoc, String.Format("online_tu12_mp_{0}.wad", Languages[i]));
            }
        }

        public static TWADFile GetFileFromWAD(string FileName, string Language = "english")
        {
            MemoryStream TWADMStream = new MemoryStream(File.ReadAllBytes("data/pub/online_tu12_mp_" + Language + ".wad"));
            BinaryReader TWADReader = new BinaryReader(TWADMStream);

            TWADReader.ReadUInt32(true);
            TWADReader.ReadUInt32();
            UInt32 FileCount = TWADReader.ReadUInt32();
            TWADReader.ReadUInt32();

            List<TWADFile> TWADFiles = new List<TWADFile>();

            for (int i = 0; i < FileCount; i++)
            {
                string NewFileName = TWADReader.ReadString(32);
                UInt32 CompressedLength = TWADReader.ReadUInt32(true);
                UInt32 Length = TWADReader.ReadUInt32(true);
                UInt32 Offset = TWADReader.ReadUInt32(true);
                TWADFiles.Add(new TWADFile(NewFileName, CompressedLength, Length, Offset));
            }

            foreach (TWADFile FileEntry in TWADFiles)
            {
                if (FileEntry.FileName == FileName)
                {
                    TWADMStream.Position = FileEntry.Offset;
                    FileEntry.Compressed = TWADReader.ReadBytes((Int32)FileEntry.CompressedLength);
                    FileEntry.Content = FileEntry.GetData();
                    TWADReader.Close();
                    TWADMStream.Close();
                    return FileEntry;
                }
            }

            TWADReader.Close();
            TWADMStream.Close();

            return new TWADFile("error", 0, 0, 0);
        }

        public static bool AddFileToWAD(TWADFile InFile, string Language = "english")
        {
            try
            {
                MemoryStream TWADMStream = new MemoryStream(File.ReadAllBytes("data/pub/online_tu12_mp_" + Language + ".wad"));
                BinaryReader TWADReader = new BinaryReader(TWADMStream);

                TWADReader.ReadUInt32(true);
                TWADReader.ReadUInt32();
                UInt32 FileCount = TWADReader.ReadUInt32();
                TWADReader.ReadUInt32();

                List<TWADFile> TWADFiles = new List<TWADFile>();

                for (int i = 0; i < FileCount; i++)
                {
                    string NewFileName = TWADReader.ReadString(32);
                    UInt32 CompressedLength = TWADReader.ReadUInt32(true);
                    UInt32 Length = TWADReader.ReadUInt32(true);
                    UInt32 Offset = TWADReader.ReadUInt32(true);
                    TWADFiles.Add(new TWADFile(NewFileName, CompressedLength, Length, Offset));
                }

                bool FoundFile = false;

                foreach (TWADFile FileEntry in TWADFiles)
                {
                    if (FileEntry.FileName == InFile.FileName)
                    {
                        FileEntry.Content = InFile.Content;
                        FileEntry.Compressed = InFile.Compressed;
                        FoundFile = true;
                    }
                    else
                    {
                        TWADMStream.Position = FileEntry.Offset;
                        FileEntry.Compressed = TWADReader.ReadBytes((Int32)FileEntry.CompressedLength);
                        FileEntry.Content = FileEntry.GetData();
                    }
                }

                if (!FoundFile)
                    TWADFiles.Add(InFile);

                TWADReader.Close();
                TWADMStream.Close();

                // Recompress the WAD

                TWADMStream = new MemoryStream();
                BinaryWriter TWADWriter = new BinaryWriter(TWADMStream);

                TWADWriter.Write((UInt32)0xAB773354, true);

                TimeSpan ts = (DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0));
                TWADWriter.Write((UInt32)ts.TotalSeconds);
                TWADWriter.Write((UInt32)TWADFiles.Count);

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
                File.WriteAllBytes("data/pub/online_tu12_mp_" + Language + ".wad", TWADMStream.ToArray());

                // Close the streams
                TWADWriter.Close();
                TWADMStream.Close();

                return true;
            }
            catch
            {
                return false;
            }
        }

        public static void GenerateTWAD(List<TWADFile> TWADFiles, string FileName = "online_tu12_mp_english.wad")
        {
            //Console.WriteLine("Creating new TWAD file: {0}", FileName);

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
