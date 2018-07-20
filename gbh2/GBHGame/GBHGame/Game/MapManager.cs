using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;

namespace GBH
{
    public struct SlopeType
    {
        private byte _value;

        public SlopeType(byte value) { _value = value; }

        public byte Ground
        {
            get
            {
                return (byte)(_value & 0x3);
            }
        }

        public byte Slope
        {
            get
            {
                return (byte)((_value >> 2) & 0x3F);
            }
        }

        public ushort Value
        {
            get
            {
                return _value;
            }
        }
    }

    public struct SideTile
    {
        private ushort _value;

        public SideTile(ushort value) { _value = value; }

        public ushort Sprite
        {
            get
            {
                return (ushort)(_value & 0x3FF);
            }
        }

        public bool Wall
        {
            get
            {
                return ((_value >> 10) & 0x1) == 1;
            }
        }

        public bool BulletWall
        {
            get
            {
                return ((_value >> 11) & 0x1) == 1;
            }
        }

        public bool Flat
        {
            get
            {
                return ((_value >> 12) & 0x1) == 1;
            }
        }

        public bool Flip
        {
            get
            {
                return ((_value >> 13) & 0x1) == 1;
            }
        }

        public int Rotation
        {
            get
            {
                return (_value >> 14) & 0x3;
            }
        }

        public ushort Value
        {
            get
            {
                return _value;
            }
        }
    }

    public class BlockInfo
    {
        public SideTile Left { get; set; }
        public SideTile Right { get; set; }
        public SideTile Top { get; set; }
        public SideTile Bottom { get; set; }
        public SideTile Lid { get; set; }

        public byte Arrows { get; set; }

        public SlopeType SlopeType { get; set; }

        public void Read(BinaryReader reader)
        {
            Left = new SideTile(reader.ReadUInt16());
            Right = new SideTile(reader.ReadUInt16());
            Top = new SideTile(reader.ReadUInt16());
            Bottom = new SideTile(reader.ReadUInt16());
            Lid = new SideTile(reader.ReadUInt16());

            Arrows = reader.ReadByte();

            SlopeType = new SlopeType(reader.ReadByte());
        }
    }

    public class CompressedMap
    {
        public uint[] Base { get; set; }
        public uint ColumnWords { get; set; }
        public uint[] Column { get; set; }
        public uint NumBlocks { get; set; }
        public BlockInfo[] Block { get; set; }

        public CompressedMap()
        {
            Base = new uint[256 * 256];
        }

        public void Read(BinaryReader reader)
        {
            // read the base/columnwords fields
            for (int i = 0; i < (256 * 256); i++)
            {
                Base[i] = reader.ReadUInt32();
            }

            ColumnWords = reader.ReadUInt32();

            // read columns
            Column = new uint[ColumnWords];

            for (int i = 0; i < ColumnWords; i++)
            {
                Column[i] = reader.ReadUInt32();
            }

            // read blocks
            NumBlocks = reader.ReadUInt32();
            Block = new BlockInfo[NumBlocks];
            
            for (int i = 0; i < NumBlocks; i++)
            {
                Block[i] = new BlockInfo();
                Block[i].Read(reader);
            }
        }
    }

    public class MapLight
    {
        public Color Color { get; set; }
        public Vector3 Position { get; set; }
        public float Radius { get; set; }
        public byte Intensity { get; set; }
        public byte Shape { get; set; }
        public byte OnTime { get; set; }
        public byte OffTime { get; set; }

        public void Read(BinaryReader reader)
        {
            uint argb = reader.ReadUInt32();
            Color = new Color((int)((argb & 0xFF0000) >> 16), (int)((argb & 0xFF00) >> 8), (int)(argb & 0xFF));

            Position = new Vector3(ReadFix16(reader), ReadFix16(reader), ReadFix16(reader));
            Radius = ReadFix16(reader);

            Intensity = reader.ReadByte();
            Shape = reader.ReadByte();
            OnTime = reader.ReadByte();
            OffTime = reader.ReadByte();
        }

        private float ReadFix16(BinaryReader reader)
        {
            ushort value = reader.ReadUInt16();
            float retval = (float)((value & 0x7F80) >> 7);

            retval += (value & 0x7F) / 128.0f;

            if ((value & 0x8000) != 0)
            {
                retval = -retval;
            }

            return retval;
        }
    }

    public static class MapManager
    {
        private static CompressedMap _compressedMap;
        private static MapLight[] _lights;
        private static string _currentMap;

        public static MapLight[] Lights
        {
            get
            {
                return _lights;
            }
        }

        public static BlockInfo GetBlock(int x, int y, int z)
        {
            if (y > 255 || y < 0 || x > 255 || x < 0)
            {
                return _compressedMap.Block[0];
            }

            uint column = _compressedMap.Base[(256 * y) + x];
            uint columnInfo = _compressedMap.Column[column];

            byte height = (byte)(columnInfo & 0xFF);
            byte offset = (byte)((columnInfo & 0xFF00) >> 8);

            int i = (z - offset);

            if (i < 0 || z >= height)
            {
                return _compressedMap.Block[0];
            }

            return _compressedMap.Block[_compressedMap.Column[column + (i + 1)]];
        }

        public static void Load(string filename)
        {
            if (filename.ToLowerInvariant() == _currentMap)
            {
                return;
            }

            _currentMap = filename.ToLowerInvariant();

            ConVar.SetValue("mapname", Path.GetFileNameWithoutExtension(filename));

            Stream mapFile = FileSystem.OpenCopy(filename);
            BinaryReader reader = new BinaryReader(mapFile);

            GBMPHeader header = new GBMPHeader();
            header.Read(reader);

            if (header.Magic != 0x504D4247 || header.Version != 500)
            {
                throw new InvalidOperationException("this isn't a GBMP v.500");
            }

            while (!mapFile.EndOfStream())
            {
                GBMPChunk chunk = new GBMPChunk();
                chunk.Read(reader);

                if (chunk.Type == 0x50414D44) // DMAP
                {
                    LoadCompressedMap(mapFile);
                }
                else if (chunk.Type == 0x5448474C) // LGHT
                {
                    LoadLights(mapFile, (chunk.Size / 16));
                }
                else
                {
                    mapFile.Position += chunk.Size;
                }
            }

            mapFile.Close();

            CellManager.InitializeFromMap();
        }

        private static void LoadCompressedMap(Stream mapFile)
        {
            _compressedMap = new CompressedMap();
            _compressedMap.Read(new BinaryReader(mapFile));
        }

        private static void LoadLights(Stream mapFile, uint numLights)
        {
            BinaryReader reader = new BinaryReader(mapFile);

            _lights = new MapLight[numLights];

            for (uint i = 0; i < numLights; i++)
            {
                _lights[i] = new MapLight();
                _lights[i].Read(reader);
            }
        }
    }
}
