using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GBH
{
    public class BitStream
    {
        private byte[] _bytes;
        private int _curBit;
        private int _maxBit;

        public byte[] Bytes
        {
            get
            {
                return _bytes;
            }
        }

        public BitStream()
        {
            _bytes = new byte[0];
        }

        public BitStream(byte[] data)
        {
            _bytes = data;
            _maxBit = data.Length * 8;
        }

        #region readers
        public bool ReadBool()
        {
            return (ReadBits(1)[0] == 1);
        }

        public byte ReadByte()
        {
            return ReadBits(8)[0];
        }

        public short ReadInt16()
        {
            return BitConverter.ToInt16(ReadBits(16), 0);
        }

        public int ReadInt32()
        {
            return ReadInt32(32);
        }

        public int ReadInt32(int bits)
        {
            return BitConverter.ToInt32(ReadBits(bits), 0);
        }

        public uint ReadUInt32()
        {
            return ReadUInt32(32);
        }

        public uint ReadUInt32(int bits)
        {
            return BitConverter.ToUInt32(ReadBits(bits), 0);
        }

        public float ReadSingle()
        {
            return BitConverter.ToSingle(ReadBits(32), 0);
        }

        public string ReadString()
        {
            short length = ReadInt16();
            byte[] bytes = ReadBits(length * 8);

            return Encoding.UTF8.GetString(bytes).TrimEnd('\0'); // trim off any \0 characters that might be due to the array being padded to 4 length
        }
        #endregion

        #region delta readers
        public byte ReadDeltaByte(byte old)
        {
            if (ReadBool())
            {
                return ReadByte();
            }

            return old;
        }

        public short ReadDeltaInt16(short old)
        {
            if (ReadBool())
            {
                return ReadInt16();
            }

            return old;
        }

        public int ReadDeltaInt32(int old)
        {
            if (ReadBool())
            {
                return ReadInt32();
            }

            return old;
        }

        public uint ReadDeltaUInt32(uint old)
        {
            if (ReadBool())
            {
                return ReadUInt32();
            }

            return old;
        }

        public int ReadDeltaInt32(int old, int bits)
        {
            if (ReadBool())
            {
                return ReadInt32(bits);
            }

            return old;
        }

        public uint ReadDeltaUInt32(uint old, int bits)
        {
            if (ReadBool())
            {
                return ReadUInt32(bits);
            }

            return old;
        }

        public float ReadDeltaSingle(float old)
        {
            if (ReadBool())
            {
                return ReadSingle();
            }

            return old;
        }
        #endregion

        #region writers
        public bool WriteBytes(byte[] value)
        {
            return Write(value.Length * 8, value);
        }

        public bool WriteBool(bool value)
        {
            return Write(1, new byte[] { (byte)((value) ? 1 : 0) });
        }

        public bool WriteByte(byte value)
        {
            return Write(8, new byte[] { value });
        }

        public bool WriteInt16(short value)
        {
            return Write(16, BitConverter.GetBytes(value));
        }

        public bool WriteInt32(int value)
        {
            return WriteInt32(value, 32);
        }

        public bool WriteInt32(int value, int bits)
        {
            return Write(bits, BitConverter.GetBytes(value));
        }

        public bool WriteUInt32(uint value)
        {
            return WriteUInt32(value, 32);
        }

        public bool WriteUInt32(uint value, int bits)
        {
            return Write(bits, BitConverter.GetBytes(value));
        }

        public bool WriteSingle(float value)
        {
            return Write(32, BitConverter.GetBytes(value));
        }

        public bool WriteString(string value)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(value);
            WriteInt16((short)bytes.Length);
            return WriteBytes(bytes);
        }
        #endregion

        #region delta writers
        public void WriteDeltaByte(byte value, byte old)
        {
            WriteBool(value != old);

            if (value != old)
            {
                WriteByte(value);
            }
        }

        public void WriteDeltaInt16(short value, short old)
        {
            WriteBool(value != old);

            if (value != old)
            {
                WriteInt16(value);
            }
        }

        public void WriteDeltaInt32(int value, int old)
        {
            WriteBool(value != old);

            if (value != old)
            {
                WriteInt32(value);
            }
        }

        public void WriteDeltaInt32(int value, int old, int bits)
        {
            WriteBool(value != old);

            if (value != old)
            {
                WriteInt32(value, bits);
            }
        }

        public void WriteDeltaUInt32(uint value, uint old)
        {
            WriteBool(value != old);

            if (value != old)
            {
                WriteUInt32(value);
            }
        }

        public void WriteDeltaUInt32(uint value, uint old, int bits)
        {
            WriteBool(value != old);

            if (value != old)
            {
                WriteUInt32(value, bits);
            }
        }

        public void WriteDeltaSingle(float value, float old)
        {
            WriteBool(value != old);

            if (value != old)
            {
                WriteSingle(value);
            }
        }
        #endregion

        #region read/write implementations
        public byte[] ReadBits(int bits)
        {
            //return ReadBytesInternal(ByteCount(bits));

            if (bits == 0)
            {
                throw new InvalidOperationException("Attempted to read from an empty buffer.");
            }

            if ((_curBit + bits) > _maxBit)
            {
                throw new InvalidOperationException("Attempted to read beyond end of stream.");
            }

            var curByte = _curBit >> 3;
            var curOut = 0;

            var byteCount = ByteCount(bits);

            var output = new byte[Math.Max(byteCount, 4)];

            while (bits > 0)
            {
                var minBit = (bits < 8) ? bits : 8;

                var thisByte = (int)_bytes[curByte];
                curByte++;

                var remain = _curBit & 7;

                if ((minBit + remain) <= 8)
                {
                    output[curOut] = (byte)((0xFF >> (8 - minBit)) & (thisByte >> remain));
                }
                else
                {
                    output[curOut] = (byte)((0xFF >> (8 - minBit)) & (_bytes[curByte] << (8 - remain)) | (thisByte >> remain));
                }

                curOut++;
                _curBit += minBit;
                bits -= minBit;
            }

            return output;
        }

        public bool Write(int bits, byte[] data)
        {
            //return WriteBytesInternal(ByteCount(bits), data);

            if (bits == 0)
            {
                return false;
            }

            if ((data.Length * 8) < bits)
            {
                return false;
            }

            if ((_bytes.Length * 8) < (_curBit + bits))
            {
                var length = ByteCount(_curBit + bits);
                Array.Resize(ref _bytes, length);
            }

            // this code is weird
            var bit = bits;

            while (bit > 0)
            {
                var bitPos = _curBit & 7;
                var remBit = 8 - bitPos;
                var thisWrite = (bit < remBit) ? bit : remBit;

                var mask = (byte)((0xFF >> remBit) | (0xFF << (bitPos + thisWrite)));
                var bytePos = _curBit >> 3;

                var tempByte = (byte)(mask & _bytes[bytePos]);
                var thisBit = (byte)((bits - bit) & 7);
                var thisByte = (bits - bit) >> 3;

                var thisData = data[thisByte];

                var nextByte = (((bits - 1) >> 3) > thisByte) ? data[thisByte + 1] : 0;

                thisData = (byte)((nextByte << (8 - thisBit)) | (thisData >> thisBit));

                var outByte = (byte)(~mask & (thisData << bitPos) | tempByte);
                _bytes[bytePos] = outByte;

                _curBit += thisWrite;
                bit -= thisWrite;

                if (_maxBit < _curBit)
                {
                    _maxBit = _curBit;
                }
            }

            return true;
        }

        private int ByteCount(int bits)
        {
            return (bits / 8) + (((bits % 8) == 0) ? 0 : 1);
        }

        public byte[] ReadBytesInternal(int bytes)
        {
            if ((_curBit + bytes) > _maxBit)
            {
                throw new InvalidOperationException("Attempted to read beyond end of stream.");
            }

            var retval = new byte[Math.Max(bytes, 4)];
            Array.Copy(_bytes, _curBit, retval, 0, bytes);

            _curBit += bytes;

            return retval;
        }

        public bool WriteBytesInternal(int bytes, byte[] data)
        {
            if ((_curBit + bytes) > _bytes.Length)
            {
                Array.Resize(ref _bytes, (_curBit + data.Length));
            }

            Array.Copy(data, 0, _bytes, _curBit, bytes);
            _curBit += bytes;

            return true;
        }
        #endregion
    }
}
