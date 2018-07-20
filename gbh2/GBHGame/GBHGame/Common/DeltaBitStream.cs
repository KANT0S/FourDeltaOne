using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GBH
{
    public class DeltaBitStream
    {
        private BitStream _bitStream;
        private BitStream _baseStream;
        private BitStream _newBaseStream;

        public BitStream NewBase
        {
            get
            {
                return _newBaseStream;
            }
        }

        public DeltaBitStream(BitStream old)
        {
            _baseStream = (old != null) ? new BitStream(old.Bytes) : null;
            _newBaseStream = new BitStream();
            _bitStream = new BitStream();
        }

        public DeltaBitStream(BitStream old, BitStream message)
        {
            _baseStream = (old != null) ? new BitStream(old.Bytes) : null;
            _newBaseStream = new BitStream();
            _bitStream = message;
        }

        public DeltaBitStream(BitStream old, byte[] data)
        {
            _baseStream = (old != null) ? new BitStream(old.Bytes) : null;
            _newBaseStream = new BitStream();
            _bitStream = new BitStream(data);
        }

        #region writers
        public void WriteBool(bool value)
        {
            _baseStream.ReadBool();
            _newBaseStream.WriteBool(value);
            _bitStream.WriteBool(value);
        }

        public void WriteByte(byte value)
        {
            _newBaseStream.WriteByte(value);

            if (_baseStream != null && value == _baseStream.ReadByte())
            {
                _bitStream.WriteBool(false);
            }
            else
            {
                _bitStream.WriteBool(true);
                _bitStream.WriteByte(value);
            }
        }

        public void WriteInt16(short value)
        {
            _newBaseStream.WriteInt16(value);

            if (_baseStream != null && value == _baseStream.ReadInt16())
            {
                _bitStream.WriteBool(false);
            }
            else
            {
                _bitStream.WriteBool(true);
                _bitStream.WriteInt16(value);
            }
        }

        public void WriteInt32(int value)
        {
            _newBaseStream.WriteInt32(value);

            if (_baseStream != null && value == _baseStream.ReadInt32())
            {
                _bitStream.WriteBool(false);
            }
            else
            {
                _bitStream.WriteBool(true);
                _bitStream.WriteInt32(value);
            }
        }

        public void WriteUInt32(uint value)
        {
            _newBaseStream.WriteUInt32(value);

            if (_baseStream != null && value == _baseStream.ReadUInt32())
            {
                _bitStream.WriteBool(false);
            }
            else
            {
                _bitStream.WriteBool(true);
                _bitStream.WriteUInt32(value);
            }
        }

        public void WriteInt32(int value, int bits)
        {
            _newBaseStream.WriteInt32(value, bits);

            if (_baseStream != null && value == _baseStream.ReadInt32(bits))
            {
                _bitStream.WriteBool(false);
            }
            else
            {
                _bitStream.WriteBool(true);
                _bitStream.WriteInt32(value, bits);
            }
        }

        public void WriteUInt32(uint value, int bits)
        {
            _newBaseStream.WriteUInt32(value, bits);

            if (_baseStream != null && value == _baseStream.ReadUInt32(bits))
            {
                _bitStream.WriteBool(false);
            }
            else
            {
                _bitStream.WriteBool(true);
                _bitStream.WriteUInt32(value, bits);
            }
        }

        public void WriteSingle(float value)
        {
            _newBaseStream.WriteSingle(value);

            if (_baseStream != null && value == _baseStream.ReadSingle())
            {
                _bitStream.WriteBool(false);
            }
            else
            {
                _bitStream.WriteBool(true);
                _bitStream.WriteSingle(value);
            }
        }
        #endregion

        #region readers
        public bool ReadBool()
        {
            bool value = _bitStream.ReadBool();
            _baseStream.ReadBool();
            _newBaseStream.WriteBool(value);
            return value;
        }

        public byte ReadByte()
        {
            byte value = (_baseStream == null) ? (byte)0 : _baseStream.ReadByte();

            if (_bitStream.ReadBool())
            {
                value = _bitStream.ReadByte();
            }

            _newBaseStream.WriteByte(value);

            return value;
        }

        public short ReadInt16()
        {
            short value = (_baseStream == null) ? (short)0 : _baseStream.ReadInt16();

            if (_bitStream.ReadBool())
            {
                value = _bitStream.ReadInt16();
            }

            _newBaseStream.WriteInt16(value);

            return value;
        }

        public int ReadInt32()
        {
            int value = (_baseStream == null) ? 0 : _baseStream.ReadInt32();

            if (_bitStream.ReadBool())
            {
                value = _bitStream.ReadInt32();
            }

            _newBaseStream.WriteInt32(value);

            return value;
        }

        public int ReadInt32(int bits)
        {
            int value = (_baseStream == null) ? 0 : _baseStream.ReadInt32(bits);

            if (_bitStream.ReadBool())
            {
                value = _bitStream.ReadInt32(bits);
            }

            _newBaseStream.WriteInt32(value, bits);

            return value;
        }

        public uint ReadUInt32()
        {
            uint value = (_baseStream == null) ? 0 : _baseStream.ReadUInt32();

            if (_bitStream.ReadBool())
            {
                value = _bitStream.ReadUInt32();
            }

            _newBaseStream.WriteUInt32(value);

            return value;
        }

        public uint ReadUInt32(int bits)
        {
            uint value = (_baseStream == null) ? 0 : _baseStream.ReadUInt32(bits);

            if (_bitStream.ReadBool())
            {
                value = _bitStream.ReadUInt32(bits);
            }

            _newBaseStream.WriteUInt32(value, bits);

            return value;
        }

        public float ReadSingle()
        {
            float value = (_baseStream == null) ? 0 : _baseStream.ReadSingle();

            if (_bitStream.ReadBool())
            {
                value = _bitStream.ReadSingle();
            }
            
            _newBaseStream.WriteSingle(value);

            return value;
        }
        #endregion
    }
}
