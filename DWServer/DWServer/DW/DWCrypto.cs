using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using System.Security.Cryptography;

namespace DWServer
{
    public static class DWCrypto
    {
        public static byte[] CalculateInitialVector(uint initialValue)
        {
            var thash = new TigerHash();
            var hash = thash.ComputeHash(BitConverter.GetBytes(initialValue));
            thash.Clear();

            return hash;
        }

        public static byte[] GenerateRandom(int length)
        {
            var array = new byte[length];

            var rng = RandomNumberGenerator.Create();
            rng.GetNonZeroBytes(array);

            return array;
        }

        public static byte[] Decrypt(byte[] iv, byte[] key, byte[] data)
        {
            if (key.Length != 24 || iv.Length != 24)
            {
                throw new ArgumentException();
            }

            var des = TripleDES.Create();
            des.Padding = PaddingMode.None;

            var inStream = new MemoryStream(data);
            var retval = new byte[data.Length];
            try
            {
                var cStream = new CryptoStream(inStream, des.CreateDecryptor(key, iv), CryptoStreamMode.Read);

                
                cStream.Read(retval, 0, retval.Length);

                cStream.Close();
            }
            catch { }
            inStream.Close();
            des.Clear();

            return retval;
        }

        public static byte[] Encrypt(byte[] iv, byte[] key, byte[] data)
        {
            if (key.Length != 24 || iv.Length != 24)
            {
                throw new ArgumentException();
            }

            var des = TripleDES.Create();
            des.Padding = PaddingMode.Zeros;

            var outStream = new MemoryStream();
            var cStream = new CryptoStream(outStream, des.CreateEncryptor(key, iv), CryptoStreamMode.Write);

            cStream.Write(data, 0, data.Length);
            cStream.FlushFinalBlock();
            var retval = outStream.ToArray();

            cStream.Close();
            outStream.Close();
            des.Clear();

            return retval;
        }

        //public static uint CalculatePacketHash(byte[] )
    }
}
