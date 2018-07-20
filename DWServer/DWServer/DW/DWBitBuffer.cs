using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using BKSystem.IO;

namespace DWServer
{
    public class DWBitBuffer
    {
        //private BitStream _stream;
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

        public DWBitBuffer(byte[] data)
        {
            UseDataTypes = true;

            _bytes = data;
            _maxBit = _bytes.Length * 8;

            //var memoryData = new MemoryStream(data);
            //_stream = new BitStream(memoryData);
            //memoryData.Close();

            //_stream.Position = 0;
        }

        public bool ReadBytes(int bytes, out byte[] output)
        {
            output = new byte[bytes];

            //var read = _stream.Read(output, 0, bytes);
            var read = Read(bytes * 8, output);

            if (read)
            {
                return false;
            }

            return true;
        }

        public bool ReadBoolean(out bool output)
        {
            output = false;

            if (ReadDataType(1))
            {
                //var read = _stream.Read(out output);
                var outbyte = new byte[1];
                var read = Read(1, outbyte);

                if (read)
                {
                    output = (outbyte[0] != 0);
                    return true;
                }
            }

            return false;
        }

        public bool ReadUInt32(out uint output)
        {
            output = 0;

            if (ReadDataType(8))
            {
                var outbyte = new byte[4];
                var read = Read(32, outbyte);//_stream.Read(out output);

                if (read)
                {
                    output = BitConverter.ToUInt32(outbyte, 0);
                    return true;
                }
            }

            return false;
        }

        public bool ReadDataType(byte expected)
        {
            if (!UseDataTypes) return true;

            byte[] actual = new byte[1];
            //var read = _stream.Read(out actual, 0, 5);
            var read = Read(5, actual);

            if (read)
            {
                return (actual[0] == expected);
            }

            return false;
        }

        public bool WriteBoolean(bool data)
        {
            if (WriteDataType(1))
            {
                return Write(1, new byte[] { (byte)(data ? 1 : 0) });
            }

            return false;
        }

        public bool WriteBytes(byte[] data)
        {
            return Write(data.Length * 8, data);
        }

        public bool WriteInt32(int value)
        {
            if (WriteDataType(7))
            {
                return Write(32, BitConverter.GetBytes(value));
            }

            return false;
        }

        public bool WriteUInt32(uint value)
        {
            if (WriteDataType(8))
            {
                return Write(32, BitConverter.GetBytes(value));
            }

            return false;
        }

        public bool WriteDataType(byte dataType)
        {
            if (!UseDataTypes) return true;

            return Write(5, new[] { dataType });
        }

        /*public bool Read(int bits, byte[] output)
        {
            if ((_curBit + bits) > _maxBit)
            {
                return false;
            }

            if (output.Length < (bits >> 3))
            {
                return false;
            }

            // this is a pretty slow method, any improvements would be appreciated... I'm bad at algorithms
            int bitsRead = 0;
            while (bitsRead < bits)
            {
                var thisBit = (_bytes[_curBit >> 3] & (1 << (_curBit & 7))) >> (_curBit & 7);
            }

            return true;
        }*/

        public bool Read(int bits, byte[] output)
        {
            if (bits == 0)
            {
                return false;
            }

            if ((_curBit + bits) > _maxBit)
            {
                return false;
            }

            var curByte = _curBit >> 3;
            var curOut = 0;

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

            return true;
        }

        public bool Write(int bits, byte[] data)
        {
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
                //Array.Resize(ref _bytes, _bytes.Length + ((_curBit + bits) / 8) + 1);
                var length = (int)(Math.Ceiling((_curBit + bits) / 8.0));
                Array.Resize(ref _bytes, length);
            }

            // actually copied from disassembly, blah
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

        /*
        public bool Write(int bits, byte[] data)
        {
            if (bits == 0)
            {
                return false;
            }

            if ((data.Length * 8) < bits)
            {
                return false;
            }

            var inByte = 0;
            var outByte = (_curBit >> 3);

            while (bits > 0)
            {
                var size = (bits < 8) ? bits : 8;
                var bitPos = (_curBit & 7);

                if ((bitPos + size) <= 8)
                {
                    //_bytes[outByte] = (byte)((_bytes[outByte] & (0xFF >> (8 - size))) | data[inByte]);
                    var tempByte = (byte)(_bytes[outByte] & (0xFF >> (size)));
                    tempByte |= (byte)(data[inByte] << (bitPos));
                    _bytes[outByte] = tempByte;

                    outByte++;
                }
                else
                {
                    
                }

                inByte++;
            }

            return true;
        }
        */

        public bool UseDataTypes
        {
            get;
            set;
        }
    }
}
