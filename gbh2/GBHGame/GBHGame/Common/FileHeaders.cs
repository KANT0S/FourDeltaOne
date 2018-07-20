using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace GBH
{
    public class GBMPHeader
    {
        public uint Magic { get; set; }
        public ushort Version { get; set; }

        public void Read(BinaryReader reader)
        {
            Magic = reader.ReadUInt32();
            Version = reader.ReadUInt16();
        }
    }

    public class GBMPChunk
    {
        public uint Type { get; set; }
        public uint Size { get; set; }

        public void Read(BinaryReader reader)
        {
            Type = reader.ReadUInt32();
            Size = reader.ReadUInt32();
        }
    }
}
