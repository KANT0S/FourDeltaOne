using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Text.RegularExpressions;

namespace DWServer
{
    class DWMatch
    {
        public static void Net_TcpDisconnected(MessageData data)
        {
            try
            {
                var toRemove = new List<ulong>();

                lock (_sessions)
                {
                    foreach (var session in _sessions)
                    {
                        if (session.Value.CID == data.Get<string>("cid"))
                        {
                            toRemove.Add(session.Key);
                        }
                    }

                    foreach (var id in toRemove)
                    {
                        _sessions.Remove(id);
                    }
                }
            }
            catch { }
        }

        public static void CleanSessions()
        {
            IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> sessions = null;
            lock (_sessions)
            {
                sessions = from session in _sessions
                           where session.Value.TitleID == TitleID.T5 && (((MatchMakingInfoT5)session.Value).LicenseType == 2 || ((MatchMakingInfoT5)session.Value).LicenseType == 4)
                           select session;

                sessions = sessions.ToList();
            }
            var socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            socket.ReceiveTimeout = 2000;

            var packet = Encoding.ASCII.GetBytes("?????123456 dvardump");
            packet[0] = 0xFF; packet[1] = 0xFF; packet[2] = 0xFF; packet[3] = 0xFF; packet[4] = 0x00;

            var response = new byte[1536];

            foreach (var session in sessions)
            {
                var hostAvailable = true;

                socket.SendTo(packet, session.Value.EndPoint);

                try
                {
                    socket.Receive(response);
                }
                catch (SocketException)
                {
                    hostAvailable = false;
                }

                _sessions[session.Key].Reachable = hostAvailable;
            }

            socket.Close();
        }

        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            if (type == 21)
            {

                var packet = DWRouter.GetMessage(data);
                var call = packet.ByteBuffer.ReadByte();

                try
                {
                    switch (call)
                    {
                        case 1:
                            CreateSession(data, packet);
                            break;
                        case 2:
                            UpdateSession(data, packet);
                            break;
                        case 3:
                            DeleteSession(data, packet);
                            break;
                        case 4:
                            FindSessionFromID(data, packet);
                            break;
                        case 5:
                            FindSessions(data, packet);
                            break;
                        case 8:
                            InviteToSession(data, packet);
                            break;
                        case 9:
                            SubmitPerformance(data, packet);
                            break;
                        case 10:
                            GetPerformanceValues(data, packet);
                            break;
                        case 12:
                            UpdateSessionPlayers(data, packet);
                            break;
                        case 13:
                            FindSessionsPaged(data, packet);
                            break;
                        case 14:
                            FindSessionsByEntityIDs(data, packet);
                            break;
                        default:
                            Log.Debug("unknown packet "+call+" in bdMatchMaking");
                            //DWRouter.Unknown(data, packet);
                            break;
                    }
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }
            }
        }

        private static void SubmitPerformance(MessageData mdata, DWMessage packet)
        {
            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)21);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        private static void GetPerformanceValues(MessageData mdata, DWMessage packet)
        {
            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)21);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        private static void UnknownHandler(MessageData mdata, DWMessage packet)
        {
            string ukblob2 = "";
            {
                for (int i = 0; i < packet.Buffer.Length; i++)
                {
                    ukblob2 = ukblob2 + packet.Buffer[i].ToString("X2");
                }
            }
            Log.Debug(ukblob2);
            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)1);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        private static void FindSessionFromID(MessageData mdata, DWMessage packet)
        {
            var sidBlob = packet.ByteBuffer.ReadBlob();
            var sid = BitConverter.ToUInt64(sidBlob, 0);
            IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> matchingSessions = null;

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)13);

            lock (_sessions)
            {
                matchingSessions = (from session in _sessions
                                    where session.Key == sid
                                    select session).Take(1);
                reply.ByteBuffer.Write((uint)matchingSessions.Count());
                reply.ByteBuffer.Write((uint)matchingSessions.Count());
                foreach (var session in matchingSessions)
                {
                    session.Value.Serialize(reply);
                }

            }
            reply.Send(true);

            Console.WriteLine("get id returned {0} sessions", matchingSessions.Count());
        }

        private static void FindZombieSessions(MessageData mdata, DWMessage packet)
        {
            var a = packet.ByteBuffer.ReadInt32(); // gametype
            var b = packet.ByteBuffer.ReadInt32(); // netcode version
            packet.ByteBuffer.ReadDataType(0x14);
            packet.ByteBuffer.ReadDataType(0x14);
            packet.ByteBuffer.ReadDataType(0x14);
            packet.ByteBuffer.ReadDataType(0x14);
            var g = packet.ByteBuffer.ReadInt32(); // playlist

            IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> matchingSessions = new Dictionary<ulong, bdMatchMakingInfo>();

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)13);

            lock (_sessions)
            {
                matchingSessions = (from session in _sessions
                                    where session.Value.TitleID == TitleID.T5 && ((MatchMakingInfoT5)session.Value).NetcodeVersion == b && ((MatchMakingInfoT5)session.Value).GameType == a && ((MatchMakingInfoT5)session.Value).Playlist == g
                                    select session);
                reply.ByteBuffer.Write((uint)matchingSessions.Count());
                reply.ByteBuffer.Write((uint)matchingSessions.Count());
                foreach (var session in matchingSessions)
                {
                    session.Value.Serialize(reply);
                }

            }
            reply.Send(true);

            Console.WriteLine("get zm g {0} a {1} returned {2} sessions", g, a, matchingSessions.Count());
        }

        private static void FindSessions(MessageData mdata, DWMessage packet)
        {
            var queryType = packet.ByteBuffer.ReadUInt32(); // query type
            var vb = packet.ByteBuffer.ReadUInt32(); // seems to be 0
            var count = packet.ByteBuffer.ReadUInt32();

            var titleID = DWRouter.GetTitleIDForData(mdata);

            if (titleID == TitleID.T5)
            {
                if (queryType == 2)
                {
                    FindZombieSessions(mdata, packet);
                }
                else if (queryType == 4)
                {
                    // technically a UInt64, but we don't seem to like bit shifting :)
                    packet.ByteBuffer.ReadDataType(10);
                    var portbytes = packet.ByteBuffer.Read(2);
                    var ipbytes = packet.ByteBuffer.Read(4);
                    var port = BitConverter.ToUInt16(portbytes, 0);
                    var address = BitConverter.ToUInt32(ipbytes, 0);
                    packet.ByteBuffer.Read(2);

                    FindSessionByIP(mdata, packet, address, port);
                }
            }
            else if (titleID == TitleID.IW5)
            {
                IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> matchingSessions = null;
                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000001);
                reply.ByteBuffer.Write((uint)0);
                reply.ByteBuffer.Write((byte)13);

                Func<MatchMakingInfoIW5, bool> condition = null;

                /*switch (queryType)
                {
                    case 1:

                        break;
                }*/
                var a = packet.ByteBuffer.ReadInt32();
                var e = packet.ByteBuffer.ReadInt32();

                condition = info =>
                    {
                        return info.Playlist == e;
                    };

                var rand = new Random();

                lock (_sessions)
                {
                    matchingSessions = (from session in _sessions
                                        where (session.Value.TitleID == TitleID.IW5 &&
                                               condition((MatchMakingInfoIW5)session.Value))
                                        orderby rand.Next()
                                        select session).Take((int)count);
                    reply.ByteBuffer.Write((uint)matchingSessions.Count());
                    reply.ByteBuffer.Write((uint)matchingSessions.Count());
                    foreach (var session in matchingSessions)
                    {
                        session.Value.Serialize(reply);
                    }

                }

                reply.Send(true);
            }
        }

        private static void FindSessionByIP(MessageData mdata, DWMessage packet, uint address, ushort port)
        {
            IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> matchingSessions = null;

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)13);

            lock (_sessions)
            {
                matchingSessions = (from session in _sessions
                                    where (session.Value.TitleID == TitleID.T5 && session.Value.IPAddress == address && session.Value.Port == port) || (mdata.Get<IPEndPoint>("source").Address.Address == session.Value.IPAddress && session.Value.InternalIPAddress == address && session.Value.InternalPort == port)
                                    select session).Take(1);
                reply.ByteBuffer.Write((uint)matchingSessions.Count());
                reply.ByteBuffer.Write((uint)matchingSessions.Count());
                foreach (var session in matchingSessions)
                {
                    session.Value.Serialize(reply);
                }

            }
            reply.Send(true);

            Console.WriteLine("get ip {0} returned {1} sessions", address, matchingSessions.Count());
        }

        private static void InviteToSession(MessageData mdata, DWMessage packet)
        {
            var sidBlob = packet.ByteBuffer.ReadBlob();
            var sid = BitConverter.ToUInt64(sidBlob, 0);

            var attachment = packet.ByteBuffer.ReadBlob();

            var onlineIDs = new List<ulong>();
            while (packet.ByteBuffer.PeekByte() == 10)
            {
                onlineIDs.Add(packet.ByteBuffer.ReadUInt64());
            }

            foreach (var onlineID in onlineIDs)
            {
                var ourID = DWRouter.GetIDForData(mdata);
                var ourName = DWRouter.CIDToName[mdata.Get<string>("cid")];
                var theirID = DWRouter.Connections[onlineID];
                var treply = packet.MakeReply(2, false, theirID); // 2 is 'push message'
                treply.ByteBuffer.Write((uint)6);
                treply.ByteBuffer.Write(ourID);
                treply.ByteBuffer.Write(ourName);
                treply.ByteBuffer.WriteBlob(sidBlob);
                treply.ByteBuffer.WriteBlob(attachment);
                treply.Send(true);

                Log.Debug("sent an invite to " + onlineID.ToString("X16") + " from " + ourID.ToString("X16"));
            }

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)21);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        private static void FindSessionsPaged(MessageData mdata, DWMessage packet)
        {
            try
            {
                // TODO: paging; do this by connection
                // return perPage results first, then on next requests either return more starting from the last result
                // or return 0 results. the game should always query until equal/0 is returned?

                var a = packet.ByteBuffer.ReadUInt32();  // always 6
                var b = packet.ByteBuffer.ReadBool();    // new paging token?
                var c = packet.ByteBuffer.ReadBlob();    // paging token ID, or something useless like typename
                var d = packet.ByteBuffer.ReadUInt32();  // results per page

                var ca = packet.ByteBuffer.ReadUInt32(); // netcode version
                var cb = packet.ByteBuffer.ReadUInt32(); // -1
                var cc = packet.ByteBuffer.ReadUInt32(); // server type
                var cd = packet.ByteBuffer.ReadUInt32(); // stat DDL version
                var ce = packet.ByteBuffer.ReadUInt32(); // unknown, should be -1 actually
                var cf = packet.ByteBuffer.ReadUInt32(); // -1 
                var cg = packet.ByteBuffer.ReadUInt32(); // country ID

                IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> matchingSessions = null;
                var wager = 0;
                if (cc == 2)
                {
                    wager = 2;
                }
                else if (cc == 1)
                {
                    wager = 1;
                }
                else
                {
                    wager = 1;
                }
                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000001);
                reply.ByteBuffer.Write((uint)0);
                reply.ByteBuffer.Write((byte)13);
                lock (_sessions)
                {
                   
                    matchingSessions = (from session in _sessions
                                        where (session.Value.TitleID == TitleID.T5 && 
                                               ((MatchMakingInfoT5)session.Value).LicenseType != 5) &&
                                               ((MatchMakingInfoT5)session.Value).StatDDL == cd &&
                                               ((MatchMakingInfoT5)session.Value).NetcodeVersion == ca &&
                                               ((MatchMakingInfoT5)session.Value).Wager == wager &&
                                               ((MatchMakingInfoT5)session.Value).Reachable == true
                                            select session).Take((int)d);
                    reply.ByteBuffer.Write((uint)matchingSessions.Count());
                    reply.ByteBuffer.Write((uint)matchingSessions.Count());
                    foreach (var session in matchingSessions)
                    {
                        session.Value.Serialize(reply);
                    }
                    Console.WriteLine("rp {0} ptb {1} t {2} returned {3} sessions", d, b, cc, matchingSessions.Count());
                }

                reply.Send(true);
            }
            catch (Exception e)
            {
                Log.Error(e.ToString());
            }
        }

        private static void FindSessionsByEntityIDs(MessageData mdata, DWMessage packet)
        {
            var entityIDs = new List<ulong>();
            while (packet.ByteBuffer.PeekByte() == 10)
            {
                entityIDs.Add(packet.ByteBuffer.ReadUInt64());
            }

            DWMessage reply;
            IEnumerable<KeyValuePair<ulong, bdMatchMakingInfo>> matchingSessions = new Dictionary<ulong, bdMatchMakingInfo>();
            reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)14);
            lock (_sessions)
            {
                matchingSessions = (from session in _sessions
                                    where (
                                        session.Value.TitleID == TitleID.T5 && 
                                        entityIDs.Contains(((MatchMakingInfoT5)session.Value).OnlineID)
                                    )
                                    select session);

                reply.ByteBuffer.Write((uint)matchingSessions.Count());
                reply.ByteBuffer.Write((uint)matchingSessions.Count());

                foreach (var session in matchingSessions)
                {
                    session.Value.Serialize(reply);
                }
            }
            reply.Send(true);

            Console.WriteLine("returned {0} sessions", matchingSessions.Count());
        }

        private static void CreateSession(MessageData mdata, DWMessage packet)
        {
            Net_TcpDisconnected(mdata);
            var seed = 0;
            string IP = mdata.Get<string>("cid");
            string[] IPParts = IP.Split(':');
            seed = BitConverter.ToInt32(IPAddress.Parse(IPParts[0]).GetAddressBytes(), 0);
            TimeSpan ts = (DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0));

            seed += (int)ts.TotalSeconds;

            var rnd = new Random(seed);
            var sid = new byte[8];
            rnd.NextBytes(sid);
            
            var titleID = DWRouter.GetTitleIDForData(mdata);
            bdMatchMakingInfo info = null;

            if (titleID == TitleID.T5)
            {
                info = new MatchMakingInfoT5();
            }
            else if (titleID == TitleID.IW5)
            {
                info = new MatchMakingInfoIW5();
            }

            info.Deserialize(packet);
            info.SessionID = sid;
            info.CID = mdata.Get<string>("cid");
            info.User = (DWRouter.CIDToUser.ContainsKey(info.CID)) ? DWRouter.CIDToUser[info.CID] : 0;
            info.Reachable = true;

            lock (_sessions)
            {
                _sessions.Add(BitConverter.ToUInt64(sid, 0), info);
            }

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)1);
            reply.ByteBuffer.Write((uint)1);
            reply.ByteBuffer.Write((uint)1);
            reply.ByteBuffer.WriteBlob(info.SessionID);
            reply.Send(true);

            if (titleID == TitleID.T5)
            {
                var tInfo = (MatchMakingInfoT5)info;
                Log.Debug(tInfo.OnlineID.ToString("X16") + " registered session (map " + tInfo.MapName + ", gametype " + tInfo.GameTypeName + ")");
            }
            else if (titleID == TitleID.IW5)
            {
                Log.Debug("Registered session.");
            }
        }
        
        private static void UpdateSession(MessageData mdata, DWMessage packet)
        {
            var sidBlob = packet.ByteBuffer.ReadBlob();
            var sid = BitConverter.ToUInt64(sidBlob, 0);

            var titleID = DWRouter.GetTitleIDForData(mdata);
            bdMatchMakingInfo info = null;

            if (titleID == TitleID.T5)
            {
                info = new MatchMakingInfoT5();
            }
            else if (titleID == TitleID.IW5)
            {
                info = new MatchMakingInfoIW5();
            }

            info.Deserialize(packet);
            info.SessionID = sidBlob;
            info.CID = mdata.Get<string>("cid");
            info.User = (DWRouter.CIDToUser.ContainsKey(info.CID)) ? DWRouter.CIDToUser[info.CID] : 0;
            info.Reachable = true;

            _sessions[sid] = info;

            if (titleID == TitleID.T5)
            {
                var tInfo = (MatchMakingInfoT5)info;
                Log.Debug(tInfo.OnlineID.ToString("X16") + " updated session (map " + tInfo.MapName + ", gametype " + tInfo.GameTypeName + ")");
            }
            else if (titleID == TitleID.IW5)
            {
                Log.Debug("Updated session.");
            }

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)1);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        private static void DeleteSession(MessageData mdata, DWMessage packet)
        {
            var sidBlob = packet.ByteBuffer.ReadBlob();
            var sid = BitConverter.ToUInt64(sidBlob, 0);

            lock (_sessions)
            {
                var matchingSessions = (from session in _sessions
                                        where session.Value.CID == mdata.Get<string>("cid")
                                        select session);

                var toRemove = new List<ulong>();

                foreach (var session in matchingSessions)
                {
                    toRemove.Add(session.Key);
                }

                foreach (var key in toRemove)
                {
                    _sessions.Remove(key);
                }
            }

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)1);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        private static void UpdateSessionPlayers(MessageData mdata, DWMessage packet)
        {
            try
            {
                var sidBlob = packet.ByteBuffer.ReadBlob();
                var players = packet.ByteBuffer.ReadUInt32();
                var sid = BitConverter.ToUInt64(sidBlob, 0);
                var success = false;

                if (_sessions.ContainsKey(sid))
                {
                    bdMatchMakingInfo info;

                    lock (_sessions)
                    {
                        info = _sessions[sid];
                        info.Deserialize(packet);
                        info.Players = players;
                        _sessions[sid] = info;
                    }

                    success = true;

                    if (info.TitleID == TitleID.T5)
                    {
                        var tInfo = (MatchMakingInfoT5)info;
                        Log.Debug("Updated session (map " + tInfo.MapName + ", gametype " + tInfo.GameTypeName + ")");
                    }
                    else if (info.TitleID == TitleID.IW5)
                    {
                        Log.Debug("Updated session");
                    }
                }

                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000001);

                if (success)
                {
                    reply.ByteBuffer.Write((uint)0);
                    reply.ByteBuffer.Write((byte)12);
                    reply.ByteBuffer.Write((uint)1);
                    reply.ByteBuffer.Write((uint)1);
                    reply.ByteBuffer.WriteBlob(sidBlob);
                }
                else
                {
                    reply.ByteBuffer.Write((uint)2);
                }

                reply.Send(true);
            }
            catch { }

        }

        public static Dictionary<ulong, bdMatchMakingInfo> Sessions
        {
            get { return _sessions; }
        }

        private static Dictionary<ulong, bdMatchMakingInfo> _sessions = new Dictionary<ulong, bdMatchMakingInfo>();
    }

    public class MatchMakingInfoIW5 : bdMatchMakingInfo
    {
        public int Unknown1 { get; set; }
        public int Playlist { get; set; }
        public int PlaylistVersion { get; set; }
        public int Unknown4 { get; set; }
        public int Unknown5 { get; set; }
        public int Unknown6 { get; set; }
        public int Unknown7 { get; set; }
        public int Unknown8 { get; set; }
        public int Unknown9 { get; set; }
        public int UnknownA { get; set; }
        public int UnknownB { get; set; }
        public byte[] UnknownBlob { get; set; }
        public int UnknownC { get; set; }
        public uint UnknownUInt1 { get; set; }
        public uint UnknownUInt2 { get; set; }
        public float UnknownFloat1 { get; set; }
        public float UnknownFloat2 { get; set; }
        public int UnknownD { get; set; }
        public int UnknownE { get; set; }
        public int UnknownF { get; set; }
        public int UnknownG { get; set; }
        public int UnknownH { get; set; }
        public int UnknownI { get; set; }
        public int UnknownJ { get; set; }
        public int UnknownK { get; set; }
        public int UnknownL { get; set; }
        public int UnknownM { get; set; }
        public int UnknownN { get; set; }
        public int UnknownO { get; set; }
        public int UnknownP { get; set; }
        public int UnknownQ { get; set; }
        public int UnknownR { get; set; }
        public int UnknownS { get; set; }
        public ulong UnknownUInt64 { get; set; }

        public override void Serialize(DWMessage packet)
        {
            base.Serialize(packet);

            packet.ByteBuffer.Write(Unknown1);
            packet.ByteBuffer.Write(Playlist);
            packet.ByteBuffer.Write(PlaylistVersion);
            packet.ByteBuffer.Write(Unknown4);
            packet.ByteBuffer.Write(Unknown5);
            packet.ByteBuffer.Write(Unknown6);
            packet.ByteBuffer.Write(Unknown7);
            packet.ByteBuffer.Write(Unknown8);
            packet.ByteBuffer.Write(Unknown9);
            packet.ByteBuffer.Write(UnknownA);
            packet.ByteBuffer.Write(UnknownB);
            packet.ByteBuffer.WriteBlob(UnknownBlob);
            packet.ByteBuffer.Write(UnknownC);
            packet.ByteBuffer.Write(UnknownUInt1);
            packet.ByteBuffer.Write(UnknownUInt2);
            packet.ByteBuffer.Write(UnknownFloat1);
            packet.ByteBuffer.Write(UnknownFloat2);
            packet.ByteBuffer.Write(UnknownD);
            packet.ByteBuffer.Write(UnknownE);
            packet.ByteBuffer.Write(UnknownF);
            packet.ByteBuffer.Write(UnknownG);
            packet.ByteBuffer.Write(UnknownH);
            packet.ByteBuffer.Write(UnknownI);
            packet.ByteBuffer.Write(UnknownJ);
            packet.ByteBuffer.Write(UnknownK);
            packet.ByteBuffer.Write(UnknownL);
            packet.ByteBuffer.Write(UnknownM);
            packet.ByteBuffer.Write(UnknownN);
            packet.ByteBuffer.Write(UnknownO);
            packet.ByteBuffer.Write(UnknownP);
            packet.ByteBuffer.Write(UnknownQ);
            packet.ByteBuffer.Write(UnknownR);
            packet.ByteBuffer.Write(UnknownS);
            packet.ByteBuffer.Write(UnknownUInt64);
        }

        public override void Deserialize(DWMessage packet)
        {
            base.Deserialize(packet);

            Unknown1 = packet.ByteBuffer.ReadInt32();
            Playlist = packet.ByteBuffer.ReadInt32();
            PlaylistVersion = packet.ByteBuffer.ReadInt32();
            Unknown4 = packet.ByteBuffer.ReadInt32();
            Unknown5 = packet.ByteBuffer.ReadInt32();
            Unknown6 = packet.ByteBuffer.ReadInt32();
            Unknown7 = packet.ByteBuffer.ReadInt32();
            Unknown8 = packet.ByteBuffer.ReadInt32();
            Unknown9 = packet.ByteBuffer.ReadInt32();
            UnknownA = packet.ByteBuffer.ReadInt32();
            UnknownB = packet.ByteBuffer.ReadInt32();
            UnknownBlob = packet.ByteBuffer.ReadBlob();
            UnknownC = packet.ByteBuffer.ReadInt32();
            UnknownUInt1 = packet.ByteBuffer.ReadUInt32();
            UnknownUInt2 = packet.ByteBuffer.ReadUInt32();
            UnknownFloat1 = packet.ByteBuffer.ReadFloat();
            UnknownFloat2 = packet.ByteBuffer.ReadFloat();
            UnknownD = packet.ByteBuffer.ReadInt32();
            UnknownE = packet.ByteBuffer.ReadInt32();
            UnknownF = packet.ByteBuffer.ReadInt32();
            UnknownG = packet.ByteBuffer.ReadInt32();
            UnknownH = packet.ByteBuffer.ReadInt32();
            UnknownI = packet.ByteBuffer.ReadInt32();
            UnknownJ = packet.ByteBuffer.ReadInt32();
            UnknownK = packet.ByteBuffer.ReadInt32();
            UnknownL = packet.ByteBuffer.ReadInt32();
            UnknownM = packet.ByteBuffer.ReadInt32();
            UnknownN = packet.ByteBuffer.ReadInt32();
            UnknownO = packet.ByteBuffer.ReadInt32();
            UnknownP = packet.ByteBuffer.ReadInt32();
            UnknownQ = packet.ByteBuffer.ReadInt32();
            UnknownR = packet.ByteBuffer.ReadInt32();
            UnknownS = packet.ByteBuffer.ReadInt32();
            UnknownUInt64 = packet.ByteBuffer.ReadUInt64();
        }
    }

    public class MatchMakingInfoT5 : bdMatchMakingInfo
    {
        public int NetcodeVersion { get; set; }
        public byte[] UnknownBlob2 { get; set; }
        public string HostName { get; set; }
        public ulong UnknownLong1 { get; set; }
        public string MapName { get; set; }
        public int GameType { get; set; }
        public int Playlist { get; set; }
        public int Unknown6 { get; set; }
        public int Unknown7 { get; set; }
        public int Unknown8 { get; set; }
        public uint LicenseType { get; set; }
        public int UnknownA { get; set; }
        public int UnknownB { get; set; }
        public int MaxPlayers { get; set; }
        public int UnknownD { get; set; }
        public ulong OnlineID { get; set; }
        public int Wager { get; set; }
        public int UnknownF { get; set; }
        public string UnknownString3 { get; set; }
        public ulong UnknownLong3 { get; set; }
        public uint StatDDL { get; set; } // used for CACValidate

        public string GameTypeName
        {
            get
            {
                var retval = "";

                for (int i = 0; i <= 5; i++)
                {
                    var c = (GameType >> (i * 5)) & 31;

                    if (c != 0)
                    {
                        retval += (char)(c + 96);
                    }
                }

                return DescribeGameType(retval);
            }
        }

        public static string DetermineMapName(string map)
        {
            switch (map)
            {
                // standard mp maps
                case "mp_array":
                    return "Array";
                case "mp_cairo":
                    return "Havana";
                case "mp_cosmodrome":
                    return "Launch";
                case "mp_cracked":
                    return "Cracked";
                case "mp_crisis":
                    return "Crisis";
                case "mp_duga":
                    return "Grid";
                case "mp_firingrange":
                    return "Firing Range";
                case "mp_hanoi":
                    return "Hanoi";
                case "mp_havoc":
                    return "Jungle";
                case "mp_mountain":
                    return "Summit";
                case "mp_nuked":
                    return "Nuketown";
                case "mp_radiation":
                    return "Radiation";
                case "mp_russianbase":
                    return "WMD";
                case "mp_villa":
                    return "Villa";

                case "mp_berlinwall2":
                    return "Berlin Wall";
                case "mp_discovery":
                    return "Discovery";
                case "mp_kowloon":
                    return "Kowloon";
                case "mp_stadium":
                    return "Stadium";
                case "mp_hotel":
                    return "Hotel";
                case "mp_zoo":
                    return "Zoo";
                case "mp_stockpile":
                    return "Stockpile";
                case "mp_area51":
                    return "Hangar 18";
                case "mp_drivein":
                    return "Drive-in";
                case "mp_silo":
                    return "Silo";
                case "mp_golfcourse":
                    return "Hazard";
                case "mp_gridlock":
                    return "Convoy";
                case "mp_outskirts":
                    return "Stockpile";
                default:
                    return map;

            }
        }

        public static string DescribeGameType(string type)
        {
            switch (type)
            {
                case "tdm":
                    return "Team Deathmatch";
                case "dm":
                    return "Free-for-all";
                case "dom":
                    return "Domination";
                case "sab":
                    return "Sabotage";
                case "sd":
                    return "Search & Destroy";
                case "arena":
                    return "Arena";
                case "dem":
                    return "Demolition";
                case "ctf":
                    return "Capture the Flag";
                case "koth":
                    return "Headquaters";
                // wager gametypes
                case "hlnd":
                    return "Sticks and Stones";
                case "gun":
                    return "Gun Game";
                case "shrp":
                    return "Sharpshooter";
                case "oic":
                    return "One in the Chamber";
                default:
                    return type;
            }
        }

        public override void Deserialize(DWMessage packet)
        {
            base.Deserialize(packet);

            NetcodeVersion = packet.ByteBuffer.ReadInt32();
            UnknownBlob2 = packet.ByteBuffer.ReadBlob();
            HostName = packet.ByteBuffer.ReadString();
            UnknownLong1 = packet.ByteBuffer.ReadUInt64();
            MapName = DetermineMapName(packet.ByteBuffer.ReadString());
            GameType = packet.ByteBuffer.ReadInt32();
            Playlist = packet.ByteBuffer.ReadInt32();
            Unknown6 = packet.ByteBuffer.ReadInt32();
            Unknown7 = packet.ByteBuffer.ReadInt32();
            Unknown8 = packet.ByteBuffer.ReadInt32();
            LicenseType = packet.ByteBuffer.ReadUInt32();
            UnknownA = packet.ByteBuffer.ReadInt32();
            UnknownB = packet.ByteBuffer.ReadInt32();
            MaxPlayers = packet.ByteBuffer.ReadInt32();
            UnknownD = packet.ByteBuffer.ReadInt32();
            OnlineID = packet.ByteBuffer.ReadUInt64();
            Wager = packet.ByteBuffer.ReadInt32();
            UnknownF = packet.ByteBuffer.ReadInt32();
            UnknownString3 = packet.ByteBuffer.ReadString();
            UnknownLong3 = packet.ByteBuffer.ReadUInt64();
            StatDDL = packet.ByteBuffer.ReadUInt32();
        }

        public override void Serialize(DWMessage packet)
        {
            base.Serialize(packet);

            packet.ByteBuffer.Write(NetcodeVersion);
            packet.ByteBuffer.WriteBlob(UnknownBlob2);
            packet.ByteBuffer.Write(HostName);
            packet.ByteBuffer.Write(UnknownLong1);
            packet.ByteBuffer.Write(MapName);
            packet.ByteBuffer.Write(GameType);
            packet.ByteBuffer.Write(Playlist);
            packet.ByteBuffer.Write(Unknown6);
            packet.ByteBuffer.Write(Unknown7);
            packet.ByteBuffer.Write(Unknown8);
            packet.ByteBuffer.Write(LicenseType);
            packet.ByteBuffer.Write(UnknownA);
            packet.ByteBuffer.Write(UnknownB);
            packet.ByteBuffer.Write(MaxPlayers);
            packet.ByteBuffer.Write(UnknownD);
            packet.ByteBuffer.Write(OnlineID);
            packet.ByteBuffer.Write(Wager);
            packet.ByteBuffer.Write(UnknownF);
            packet.ByteBuffer.Write(UnknownString3);
            packet.ByteBuffer.Write(UnknownLong3);
            packet.ByteBuffer.Write(StatDDL);
        }
    }

    public class bdMatchMakingInfo
    {
        public bool Reachable { get; set; }
        public UInt32 IPAddress { get; set; }
        public UInt16 Port { get; set; }
        public UInt32 InternalIPAddress { get; set; }
        public UInt16 InternalPort { get; set; }
        public byte[] Address { get; set; }
        public byte[] SessionID { get; set; }
        public uint Unknown1 { get; set; }
        public uint Unknown2 { get; set; }
        public uint Players { get; set; } // uncertain, but not set on create, only in deserialize on client, and set from base proto. UPDATE (27/07/2012): Yep, this value isn't correct.
        public string CID { get; set; }
        public int User { get; set; }
        public TitleID TitleID
        {
            get
            {
                if (this is MatchMakingInfoT5)
                {
                    return TitleID.T5;
                }
                else if (this is MatchMakingInfoIW5)
                {
                    return TitleID.IW5;
                }

                return TitleID.T5;
            }
        }

        public IPEndPoint EndPoint
        {
            get
            {
                return new IPEndPoint(new IPAddress(BitConverter.GetBytes(IPAddress)), Port);
            }
        }

        public string ConnectionValue
        {
            get
            {
                return new IPAddress(BitConverter.GetBytes(IPAddress)).ToString() + ":" + Port;
            }
        }

        public string someIP
        {
            get
            {
                return new IPAddress(BitConverter.GetBytes(IPAddress)).ToString();
            }
        }

        public string CountryCode
        {
            get
            {
                return new WebClient().DownloadString("http://website.com/getlocfromip.php?ip=" + someIP);
            }
        }

        public virtual void Serialize(DWMessage packet)
        {
            packet.ByteBuffer.WriteBlob(Address);
            packet.ByteBuffer.WriteBlob(SessionID);
            packet.ByteBuffer.Write(Unknown1);
            packet.ByteBuffer.Write(Unknown2);
            packet.ByteBuffer.Write(Players);
        }

        public virtual void Deserialize(DWMessage packet)
        {
            Address = packet.ByteBuffer.ReadBlob();
            //UnknownBlob = packet.ByteBuffer.ReadBlob();
            Unknown1 = packet.ByteBuffer.ReadUInt32();
            Unknown2 = packet.ByteBuffer.ReadUInt32();
            //Unknown3 = packet.ByteBuffer.ReadUInt32();

            byte[] addrbytes = new byte[4];
            byte[] portbytes = new byte[2];
            Array.ConstrainedCopy(Address, 0, addrbytes, 0, 4);
            Array.ConstrainedCopy(Address, 4, portbytes, 0, 2);
            InternalIPAddress = BitConverter.ToUInt32(addrbytes, 0);
            InternalPort = BitConverter.ToUInt16(portbytes, 0);
            
            addrbytes = new byte[4];
            portbytes = new byte[2];
            Array.ConstrainedCopy(Address, 18, addrbytes, 0, 4);
            Array.ConstrainedCopy(Address, 22, portbytes, 0, 2);
            IPAddress = BitConverter.ToUInt32(addrbytes, 0);
            Port = BitConverter.ToUInt16(portbytes, 0);

            if (addrbytes[0] == 0)
            {
                IPAddress = InternalIPAddress;
                Port = InternalPort;
            }
            
        }
    }
}
