using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DWServer
{
    public static class DWTickets
    {
        public static DWNPTicket ParseNPTicket(byte[] data)
        {
            return new DWNPTicket(data);
        }

        public static byte[] BuildGameTicket(byte[] cryptoKey, uint gameID, string nickname, byte licenseType)
        {
            if (cryptoKey.Length != 24)
            {
                throw new ArgumentException();
            }

            var stream = new MemoryStream();
            var writer = new BinaryWriter(stream);

            writer.Write(new byte[] { 0xDE, 0xAD, 0xBD, 0xEF });
            writer.Write((byte)licenseType);
            //writer.Write((byte)0xA);
            writer.Write(gameID); // game ID
            writer.Write(new byte[16] { 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA });
            writer.Write(0x1100001DEADC0DE);
            writer.Write(Encoding.ASCII.GetBytes(/*nickname*/"valuableAsset3".PadRight(64, '\0')));
            writer.Write(cryptoKey);
            writer.Write(new byte[7] { 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA });

            var retval = stream.ToArray();
            writer.Close();

            return retval;
        }

        public static byte[] BuildLSGTicket(byte[] cryptoKey, ulong npid, int userid, string nickname)
        {
            // TODO: encrypt this ticket
            var bytes = new byte[128];
            Array.Copy(cryptoKey, bytes, 24);
            Array.Copy(BitConverter.GetBytes(npid), 0, bytes, 24, 8);
            Array.Copy(BitConverter.GetBytes(userid), 0, bytes, 24 + 8, 4);

            var nicknameBytes = Encoding.UTF8.GetBytes(nickname);
            Array.Copy(nicknameBytes, 0, bytes, 24 + 8 + 4, nicknameBytes.Length);
            return bytes;
        }

        public static byte[] GetKeyFromLSGTicket(byte[] lsgTicket)
        {
            var bytes = new byte[24];
            Array.Copy(lsgTicket, bytes, 24);
            return bytes;
        }

        public static ulong GetIDFromLSGTicket(byte[] lsgTicket)
        {
            return BitConverter.ToUInt64(lsgTicket, 24);
        }

        public static int GetUserFromLSGTicket(byte[] lsgTicket)
        {
            return BitConverter.ToInt32(lsgTicket, 24 + 8);
        }

        public static string GetNameFromLSGTicket(byte[] lsgTicket)
        {
            return Encoding.UTF8.GetString(lsgTicket, 24 + 8 + 4, 64).TrimEnd('\0');
        }
    }

    public class DWNPTicket
    {
        public ulong SteamID { get; set; }
        public byte[] EncryptionKey { get; set; }
        public string NickName { get; set; }
        public string SessionID { get; set; }

        public DWNPTicket(byte[] data)
        {
            SteamID = BitConverter.ToUInt64(data, (32 + 24 + 64));

            EncryptionKey = new byte[24];
            Array.Copy(data, 32, EncryptionKey, 0, 24);

            SessionID = Encoding.ASCII.GetString(data, 0, 32).Trim('\0');
            NickName = Encoding.ASCII.GetString(data, 32 + 24, 64);
        }
    }
}
