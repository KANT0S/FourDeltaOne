using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

using MongoDB.Bson;
using MongoDB.Driver;
using MongoDB.Driver.Builders;
using MongoDB.Bson.Serialization.Attributes;

namespace DWServer
{
    class DWAuther
    {
        private static Dictionary<ulong, string> _onlineIDToName = new Dictionary<ulong, string>();

        public static string GetNameForID(ulong id)
        {
            if (!_onlineIDToName.ContainsKey(id))
            {
                return id.ToString("X16");
            }

            return _onlineIDToName[id];
        }


        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            if (!crypt)
            {
                if (type == 28)
                {
                    HandleClientAuth(data);
                }
                else if (type == 12)
                {
                    HandleServerAuth(data);
                }
                else if (type == 26)
                {
                    HandleIW5ServerAuth(data, type);
                }
            }
        }

        private class ClientAuthstate
        {
            public DWMessage Packet { get; set; }
            public byte[] Ticket { get; set; }
            public uint Random { get; set; }
            public uint GameID { get; set; }
            public bool OddBool { get; set; }
            public string Source { get; set; }
        }

        private static void HandleClientAuth(MessageData data)
        {
            var packet = DWRouter.GetMessage(data);

            bool unknownBool;
            uint randomNumber;
            uint gameID;
            uint ticketLength;
            byte[] ticket;

            packet.BitBuffer.UseDataTypes = false;
            packet.BitBuffer.ReadBoolean(out unknownBool);
            packet.BitBuffer.UseDataTypes = true;

            packet.BitBuffer.ReadUInt32(out randomNumber);
            packet.BitBuffer.ReadUInt32(out gameID);
            packet.BitBuffer.ReadUInt32(out ticketLength);

            ticket = new byte[ticketLength];
            packet.BitBuffer.ReadBytes((int)ticketLength, out ticket);

            var cid = data.Get<string>("cid");

            // and the connection
            //var ci = TCPHandler.Connections.Find(c => c.ConnectionID == cid);

            var SourceSocket = cid.Split(':');
            // create state
            var state = new ClientAuthstate()
            {
                GameID = gameID,
                Ticket = ticket,
                Packet = packet,
                Random = randomNumber,
                OddBool = unknownBool,
                Source = SourceSocket[0]
            };

            // start thread
            var thread = new Thread(DoClientAuth);
            thread.Start(state);
        }

        private static void DoClientAuth(object stateo)
        {
            var state = (ClientAuthstate)stateo;

            // obtain key auth
            try
            {
                var npticket = DWTickets.ParseNPTicket(state.Ticket);
                var url = "http://website.com/check_session.php?sid=" + npticket.SessionID;
                var wc = new WebClient();
                var webString = wc.DownloadString(url);
                var authData = webString.Split(' ');

                if (authData[0] == "1")
                {
                    var userID = int.Parse(authData[1]);

                    if ((uint)(npticket.SteamID & 0xFFFFFFFF) == userID)
                    {
                        var ivBase = BitConverter.ToUInt32(DWCrypto.GenerateRandom(4), 0);
                        var iv = DWCrypto.CalculateInitialVector(ivBase);
                        var key = npticket.EncryptionKey;

                        var globalKey = DWCrypto.GenerateRandom(24);

                        var gameTicket = DWTickets.BuildGameTicket(globalKey, state.GameID, npticket.NickName, 0);
                        var lsgTicket = DWTickets.BuildLSGTicket(globalKey, npticket.SteamID, userID, npticket.NickName);

                        var encryptedGameTicket = DWCrypto.Encrypt(iv, key, gameTicket);

                        var reply = state.Packet.MakeReply(29, true);
                        reply.BitBuffer.UseDataTypes = false;
                        reply.BitBuffer.WriteBoolean(false);
                        reply.BitBuffer.WriteUInt32(700);
                        reply.BitBuffer.WriteUInt32(ivBase);
                        reply.BitBuffer.WriteBytes(encryptedGameTicket);
                        reply.BitBuffer.WriteBytes(lsgTicket);
                        reply.Send(false);

                        Log.Debug("user " + userID + " authenticated client: " + state.Source + "");
                        return;
                    }
                }
                wc.Dispose();
            }
            catch (Exception e)
            {
                Log.Debug("Exception: "+e.ToString());
            }
        }

        private class ServerAuthstate
        {
            public DWMessage Packet { get; set; }
            public ulong KeyData { get; set; }
            public uint Random { get; set; }
            public uint GameID { get; set; }
            public bool OddBool { get; set; }
        }

        private static void HandleServerAuth(MessageData data)
        {
            var packet = DWRouter.GetMessage(data);

            bool unknownBool;
            uint randomNumber;
            uint gameID;
            byte[] keyDataBuf = new byte[8];

            packet.BitBuffer.UseDataTypes = false;
            packet.BitBuffer.ReadBoolean(out unknownBool);
            packet.BitBuffer.UseDataTypes = true;

            packet.BitBuffer.ReadUInt32(out randomNumber);
            packet.BitBuffer.ReadUInt32(out gameID);
            packet.BitBuffer.Read(64, keyDataBuf);

            // create state
            var state = new ServerAuthstate()
            {
                GameID = gameID,
                KeyData = BitConverter.ToUInt64(keyDataBuf, 0),
                Packet = packet,
                Random = randomNumber,
                OddBool = unknownBool
            };

            // start thread
            if (Program.Game == TitleID.T5)
            {
                var thread = new Thread(DoServerAuth);
                thread.Start(state);
            }
            else if (Program.Game == TitleID.IW5)
            {
                var thread = new Thread(DoIW5ServerAuth);
                thread.Start(state);
            }
        }

        private static void DoServerAuth(object stateo)
        {
            var state = (ServerAuthstate)stateo;

            // obtain key auth
            try
            {
                var url = "http://website.com/check_key.php?key=" + state.KeyData.ToString("x16");
                var wc = new WebClient();
                var resultData = wc.DownloadString(url);
                var authData = resultData.Split(' ');

                if (authData[0] == "1")
                {
                    var licenseType = int.Parse(authData[1]);
                    var userID = int.Parse(authData[3]);

                    var key = Extensions.SwapBytes64(Extensions.ParseHexString(authData[2]));

                    // generate iv
                    var ivBase = BitConverter.ToUInt32(DWCrypto.GenerateRandom(4), 0);
                    var iv = DWCrypto.CalculateInitialVector(ivBase);

                    // blah
                    var globalKey = DWCrypto.GenerateRandom(24);

                    var gameTicket = DWTickets.BuildGameTicket(globalKey, state.GameID, "", (byte)licenseType); // 4: official
                    var lsgTicket = DWTickets.BuildLSGTicket(globalKey, state.KeyData, userID, "");

                    var encryptedGameTicket = DWCrypto.Encrypt(iv, key, gameTicket);

                    var reply = state.Packet.MakeReply(13, true);
                    reply.BitBuffer.UseDataTypes = false;
                    reply.BitBuffer.WriteBoolean(false);
                    reply.BitBuffer.WriteUInt32(700);
                    reply.BitBuffer.WriteUInt32(ivBase);
                    reply.BitBuffer.WriteBytes(encryptedGameTicket);
                    reply.BitBuffer.WriteBytes(lsgTicket);
                    reply.Send(false);

                    Log.Debug("user "+userID+" authenticated server");
                    return;
                }
            }
            catch (Exception e)
            {
                Log.Debug("Exception: "+e.ToString());
            }
        }

        private class IW5ServerAuthstate
        {
            public DWMessage Packet { get; set; }
            public ulong KeyData { get; set; }
            public uint Random { get; set; }
            public uint GameID { get; set; }
            public bool OddBool { get; set; }
        }

        private static void HandleIW5ServerAuth(MessageData data, int type)
        {
            var packet = DWRouter.GetMessage(data);

            bool unknownBool;
            uint randomNumber;
            uint gameID;
            byte[] rsaKeyBuf = new byte[1120 / 8];

            packet.BitBuffer.UseDataTypes = false;
            packet.BitBuffer.ReadBoolean(out unknownBool);
            packet.BitBuffer.UseDataTypes = true;

            packet.BitBuffer.ReadUInt32(out randomNumber);
            packet.BitBuffer.ReadUInt32(out gameID);

            // create state
            var state = new IW5ServerAuthstate()
            {
                GameID = gameID,
                Packet = packet,
                Random = randomNumber,
                OddBool = unknownBool
            };

            if (type == 26)
            {
                packet.BitBuffer.Read(1120, rsaKeyBuf);

                // start thread
                var thread = new Thread(CreateIW5ServerKey);
                thread.Start(state);
            }
        }

        public class ServerKey
        {
            [BsonId]
            public ObjectId id;

            public long keyHash;
            public string key;
            public int unkInt;
        }

        private static void DoIW5ServerAuth(object stateo)
        {
            var state = (ServerAuthstate)stateo;

            // obtain key auth
            try
            {
                var query = Query.EQ("keyHash", (long)state.KeyData);
                var result = Database.AServerKeys.Find(query);

                if (result.Count() > 0)
                {
                    var keyEntry = result.First();
                    var keyString = keyEntry.key;

                    var thash = new TigerHash();
                    var key = thash.ComputeHash(Encoding.ASCII.GetBytes(keyString));

                    // generate iv
                    var ivBase = BitConverter.ToUInt32(DWCrypto.GenerateRandom(4), 0);
                    var iv = DWCrypto.CalculateInitialVector(ivBase);

                    // blah
                    var globalKey = DWCrypto.GenerateRandom(24);

                    var gameTicket = DWTickets.BuildGameTicket(globalKey, state.GameID, "", 0); // 4: official
                    var lsgTicket = DWTickets.BuildLSGTicket(globalKey, state.KeyData, 1, "");

                    var encryptedGameTicket = DWCrypto.Encrypt(iv, key, gameTicket);

                    var reply = state.Packet.MakeReply(13, true);
                    reply.BitBuffer.UseDataTypes = false;
                    reply.BitBuffer.WriteBoolean(false);
                    reply.BitBuffer.WriteUInt32(700);
                    reply.BitBuffer.WriteUInt32(ivBase);
                    reply.BitBuffer.WriteBytes(encryptedGameTicket);
                    reply.BitBuffer.WriteBytes(lsgTicket);
                    reply.Send(false);
                    return;
                }
            }
            catch (Exception e)
            {
                Log.Debug("Exception: " + e.ToString());
            }
        }

        private static void CreateIW5ServerKey(object stateo)
        {
            var state = (IW5ServerAuthstate)stateo;

            // obtain key auth
            try
            {
                Log.Debug("got a request for a new IW5 dedi key; seems fun to me");

                // actually the advanced RSA stuff should be used but that might be incompatible with the .NET implementation of RSA
                var passGen = new PasswordGenerator();
                passGen.Maximum = 20;
                passGen.Minimum = 20;
                var key = passGen.Generate();

                key = string.Format("X{0}-{1}-{2}-{3}-{4}", key.Substring(1, 3), key.Substring(4, 4), key.Substring(8, 4), key.Substring(12, 4), key.Substring(16, 4));

                var thash = new TigerHash();
                var hash = thash.ComputeHash(Encoding.ASCII.GetBytes(key));
                var keyHash = BitConverter.ToInt64(hash, 0);

                var keyEntry = new ServerKey();
                keyEntry.key = key;
                keyEntry.keyHash = keyHash;
                keyEntry.unkInt = new Random().Next();
                Database.AServerKeys.Save(keyEntry);

                var keyStuff = new byte[86];
                Array.Copy(Encoding.ASCII.GetBytes(key), keyStuff, key.Length);

                var obfuscationKey = "43FCB2ACF2D72593DD7CD1C69E0F03C07229F4C83166F7B05BA0C5FE3AA3A2D93EK2495783KDKN92939DK";
                var i = 0;

                foreach (var character in obfuscationKey)
                {
                    keyStuff[i] ^= (byte)character;
                    i++;
                }

                // generate iv
                var ivBase = BitConverter.ToUInt32(DWCrypto.GenerateRandom(4), 0);
                var iv = DWCrypto.CalculateInitialVector(ivBase);

                // blah
                var globalKey = DWCrypto.GenerateRandom(24);

                var gameTicket = DWTickets.BuildGameTicket(globalKey, state.GameID, "", (byte)0); // 4: official
                var lsgTicket = DWTickets.BuildLSGTicket(globalKey, (ulong)keyHash, 1, "");

                var encryptedGameTicket = DWCrypto.Encrypt(iv, hash, gameTicket);

                var reply = state.Packet.MakeReply(25, true);
                reply.BitBuffer.UseDataTypes = false;
                reply.BitBuffer.WriteBoolean(false);
                reply.BitBuffer.WriteUInt32(700);
                reply.BitBuffer.WriteUInt32(ivBase);
                reply.BitBuffer.WriteBytes(encryptedGameTicket);
                reply.BitBuffer.WriteBytes(lsgTicket);
                reply.BitBuffer.WriteBytes(keyStuff);
                reply.BitBuffer.WriteInt32(keyEntry.unkInt);

                reply.Send(false);
            }
            catch (Exception e)
            {
                Log.Debug("Exception: " + e.ToString());
            }
        }
    }
}
