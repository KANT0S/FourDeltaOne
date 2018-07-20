using System;

namespace GBH
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            if (args.Length >= 1)
            {
                if (args[0] == "exportMap")
                {
                    ExportMap.ExportMain(args);
                    return;
                }

                if (args[0] == "exportStyle")
                {
                    ExportStyle.ExportMain(args);
                    return;
                }

                if (args[0] == "sty2mat")
                {
                    Sty2Mat.ToolMain(args);
                    return;
                }
            }

            var outStream = new BitStream();
            outStream.WriteBool(true);
            outStream.WriteByte(125);
            outStream.WriteInt16(32766);
            outStream.WriteInt32(300000);
            outStream.WriteInt32(300000, 20);
            outStream.WriteSingle(0.1f);

            var inStream = new BitStream(outStream.Bytes);
            Console.WriteLine(inStream.ReadBool());
            Console.WriteLine(inStream.ReadByte());
            Console.WriteLine(inStream.ReadInt16());
            Console.WriteLine(inStream.ReadInt32());
            Console.WriteLine(inStream.ReadInt32(20));
            Console.WriteLine(inStream.ReadSingle());

            Game.Initialize();

            while (true)
            {
                Game.Process();
            }
        }
    }
}

