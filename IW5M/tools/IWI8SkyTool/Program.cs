using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;

namespace IWI8SkyTool
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Help();
                return;
            }

            var action = args[0];

            switch (action)
            {
                case "build":
                    var filename = args[1];
                    Build(filename);                    

                    break;
                case "extract":
                    Extract(args[1]);
                    break;
            }
        }

        private static void Build(string filename)
        {
            filename = filename.Replace(Path.GetExtension(filename), "");

            var iwiFilename = filename + ".iwi";
            var outFilename = filename + "_new.iwi";

            if (!File.Exists(iwiFilename))
            {
                Console.WriteLine("Could not find " + iwiFilename);
                return;
            }

            var basename = Path.GetFileNameWithoutExtension(filename);
            var stream = File.OpenRead(iwiFilename);
            var reader = new BinaryReader(stream);

            var code = reader.ReadUInt32();

            if (code != 0x08695749)
            {
                Console.WriteLine("This is not an IWi8 file.");
                return;
            }

            reader.ReadUInt16();
            var type = reader.ReadByte();
            reader.ReadByte();

            if (type != 0x01)
            {
                Console.WriteLine("This is not a sky file.");
                return;
            }

            var compression = reader.ReadUInt16();

            if ((compression & 0xFF) != 0x0B)
            {
                Console.WriteLine("This tool supports only DXT1 textures.");
                return;
            }

            var width = reader.ReadUInt16();
            var height = reader.ReadUInt16();
            var depth = reader.ReadUInt16();

            var end = reader.ReadInt32();
            var start = reader.ReadInt32();
            start = (start == end) ? 32 : start;

            var size = (end - start) / 6;

            reader.BaseStream.Seek(0, SeekOrigin.Begin);

            var outFile = File.Open(outFilename, FileMode.Create, FileAccess.Write);
            var writer = new BinaryWriter(outFile);

            writer.Write(reader.ReadBytes(start));
            writer.Write(new byte[size * 6]);

            for (int i = 0; i < 6; i++)
            {
                var inFilename = basename + (i + 1).ToString() + ".png";
                writer.BaseStream.Seek(start + (size * i), SeekOrigin.Begin);

                if (!File.Exists(inFilename))
                {
                    writer.Close();
                    File.Delete(outFilename);
                    Console.WriteLine("Could not find " + inFilename);
                    return;
                }

                var newData = new byte[(int)width * (int)height * 4];
                var conData = new byte[(int)width * (int)height * 4];
                var bmp = new Bitmap(Image.FromFile(inFilename));
                var rect = new Rectangle(0, 0, (int)width, (int)height);
                var bmpdata = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

                System.Runtime.InteropServices.Marshal.Copy(bmpdata.Scan0, newData, 0, (int)width * (int)height * 4);

                bmp.UnlockBits(bmpdata);

                // swap argb to rgba (?)
                for (int j = 0; j < (width * height); j++)
                {
                    conData[(j * 4) + 3] = newData[(j * 4) + 3];
                    conData[(j * 4) + 2] = newData[(j * 4) + 0];
                    conData[(j * 4) + 1] = newData[(j * 4) + 1];
                    conData[(j * 4) + 0] = newData[(j * 4) + 2];
                }

                var data = DXTEncoder.EncodeDXT1(conData, width, height);

                writer.Write(data);
            }

            writer.Close();
        }

        private static void Extract(string filename)
        {
            if (!File.Exists(filename))
            {
                Console.WriteLine("The file does not exist.");
                return;
            }

            var basename = Path.GetFileNameWithoutExtension(filename);
            var stream = File.OpenRead(filename);
            var reader = new BinaryReader(stream);

            var code = reader.ReadUInt32();

            if (code != 0x08695749)
            {
                Console.WriteLine("This is not an IWi8 file.");
                return;
            }

            reader.ReadUInt16();
            var type = reader.ReadByte();
            reader.ReadByte();

            if (type != 0x01)
            {
                Console.WriteLine("This is not a sky file.");
                return;
            }

            var compression = reader.ReadUInt16();

            if ((compression & 0xFF) != 0x0B)
            {
                Console.WriteLine("This tool supports only DXT1 textures.");
                return;
            }

            var width = reader.ReadUInt16();
            var height = reader.ReadUInt16();
            var depth = reader.ReadUInt16();

            var end = reader.ReadInt32();
            var start = reader.ReadInt32();
            start = (start == end) ? 32 : start;

            var size = (end - start) / 6;

            for (int i = 0; i < 6; i++)
            {
                var outFilename = basename + (i + 1).ToString() + ".png";
                reader.BaseStream.Seek(start + (size * i), SeekOrigin.Begin);

                var data = reader.ReadBytes(size);
                var bitmap = GetBitmap(width, height, data);
                bitmap.Save(outFilename, System.Drawing.Imaging.ImageFormat.Png);
                bitmap.Dispose();
            }
        }

        private static Bitmap GetBitmap(int width, int height, byte[] data)
        {
            byte[] newData = data;

            newData = DXTDecoder.DecodeDXT1(data, width, height);

            var bmp = new Bitmap((int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            var rect = new Rectangle(0, 0, (int)width, (int)height);
            var bmpdata = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.WriteOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            System.Runtime.InteropServices.Marshal.Copy(newData, 0, bmpdata.Scan0, (int)width * (int)height * 4);

            bmp.UnlockBits(bmpdata);

            return bmp;
        }

        private static void Help()
        {
            Console.WriteLine("usage: {0} [build/extract] filename", Path.GetFileNameWithoutExtension(System.Reflection.Assembly.GetEntryAssembly().Location));
            Console.WriteLine(" - build: filename should be an existing .iwi file - output will be _new.iwi");
            Environment.Exit(1);
        }
    }
}
