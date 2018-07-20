using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DWServer
{
    public class DWByteBuffer
    {
        private byte[] _bytes;
        private int _curByte;

        public int RemainingBytes
        {
            get
            {
                return _bytes.Length - _curByte;
            }
        }

        public byte[] Bytes
        {
            get
            {
                return _bytes;
            }
        }

        public bool UseDataTypes
        {
            get;
            set;
        }

        public bool DataTypePackingEnabled
        {
            get;
            set;
        }

        public DWByteBuffer(byte[] data)
        {
            UseDataTypes = true;

            _bytes = data;
            _curByte = 0;
        }

        public void Write(byte[] data)
        {
            try
            {
                if ((_curByte + data.Length) > _bytes.Length)
                {
                    Array.Resize(ref _bytes, (_curByte + data.Length));
                }

                Array.Copy(data, 0, _bytes, _curByte, data.Length);
                _curByte += data.Length;
            }
            catch { }
        }

        public void InitializeReply(byte type)
        {
            Write(new byte[] { 0xef, 0xbe, 0xad, 0xde, type });
        }

        public void WriteDataType(byte dataType)
        {
            if (!UseDataTypes) return;

            Write(new[] { dataType });
        }

        public void Write(uint data)
        {
            WriteDataType(8); // shouldn't this be an enum?
            Write(BitConverter.GetBytes(data));
        }

        public void Write(int data)
        {
            WriteDataType(7);
            Write(BitConverter.GetBytes(data));
        }

        public void Write(long data)
        {
            WriteDataType(0x9);
            Write(BitConverter.GetBytes(data));
        }

        public void Write(ulong data)
        {
            WriteDataType(0xA);
            Write(BitConverter.GetBytes(data));
        }

        public void Write(byte data)
        {
            WriteDataType(3);
            Write(new[] { data });
        }

        public void Write(float data)
        {
            WriteDataType(13);
            Write(BitConverter.GetBytes(data));
        }

        public void Write(string data)
        {
            WriteDataType(16);
            Write(Encoding.ASCII.GetBytes(data));
            Write(new byte[] { 0 });
        }

        public void WriteBlob(byte[] data)
        {
            WriteDataType(0x13);
            Write((uint)data.Length);
            Write(data);
        }

        public byte[] Read(int length)
        {
            var data = new byte[length];
            Array.Copy(_bytes, _curByte, data, 0, data.Length);
            _curByte += data.Length;
            return data;
        }

        public byte PeekByte()
        {
            return _bytes[_curByte];
        }

        public void ReadDataType(byte dataType)
        {
            ReadDataType(dataType, true);
        }

        private byte _lastDataType = 0;

        public bool ReadDataType(byte dataType, bool throwEx)
        {
            if (!UseDataTypes) return true;

            if (DataTypePackingEnabled && _lastDataType == dataType) return true;

            var type = Read(1)[0];
            if (type != dataType)
            {
                if (throwEx)
                {
                    throw new InvalidOperationException(string.Format("data type {0} does not match {1}", type, dataType));
                }
                else
                {
                    return false;
                }
            }

            _lastDataType = type;

            return true;
        }

        public byte[] ReadBlob()
        {
            ReadDataType(0x13);
            var size = ReadUInt32();

            return Read((int)size);
        }

        public byte ReadByte()
        {
            ReadDataType(3);

            return Read(1)[0];
        }

        public bool ReadBool()
        {
            ReadDataType(1);

            return (Read(1)[0] != 0);
        }

        public int ReadInt32()
        {
            ReadDataType(7);

            return BitConverter.ToInt32(Read(4), 0);
        }

        public uint ReadUInt32()
        {
            if (!ReadDataType(8, false))
            {
                return 0xFFFFFFFF;
            }

            return BitConverter.ToUInt32(Read(4), 0);
        }

        public ulong ReadUInt64()
        {
            ReadDataType(10);

            return BitConverter.ToUInt64(Read(8), 0);
        }

        public float ReadFloat()
        {
            ReadDataType(13);

            return BitConverter.ToSingle(Read(4), 0);
        }

        public string ReadString()
        {
            ReadDataType(16);

            string retval = "";
            byte cur = 0;
            while ((cur = Read(1)[0]) != 0x00)
            {
                retval += (char)cur;
            }

            return retval;
        }
    }
}
