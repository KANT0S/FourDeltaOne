using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using System.Net;
using System.IO;

namespace DWServer
{
    public class DWRouter
    {
        public static void OnStart()
        {
            Connections = new Dictionary<ulong, string>();
            ConnectionsReverse = new Dictionary<string, ulong>();
            CIDToUser = new Dictionary<string, int>();
            CIDToName = new Dictionary<string, string>();
            CIDToTitle = new Dictionary<string, TitleID>();
        }

        private class PerfState
        {
            public int Min { get; set; }
            public int Max { get; set; }

            public void Add(int time)
            {
                if (time < Min || Min == 0)
                {
                    Min = time;
                }

                if (time > Max)
                {
                    Max = time;
                }
            }
        }

        private static Dictionary<int, PerfState> _states = new Dictionary<int, PerfState>();

        private static PerfState GetPerfState(int type)
        {
            if (!_states.ContainsKey(type))
            {
                _states.Add(type, new PerfState());
            }

            return _states[type];
        }

        private int _bytesRead = 0;
        private int _totalBytes = 0;
        private MemoryStream _messageBuffer;
        private byte _messageType = 0;
        private bool _encrypted = false;
        private uint _iv = 0;

        public void handlePacket(MessageData data)
        {
            var buffer = data.Get<byte[]>("data");
            var source = data.Get<EndPoint>("source");

            var stream = new MemoryStream(buffer);
            var reader = new BinaryReader(stream);

            var time = data.Get<DateTime>("time");
            var delay = (DateTime.Now - time).TotalMilliseconds;
            var delay2 = DateTime.Now;

            try
            {
                while (stream.Position < stream.Length)
                {
                    var origin = 0;
                    var len = 0;

                    if (_bytesRead == 0)
                    {
                        _totalBytes = 0;
                        try
                        {
                            _totalBytes = reader.ReadInt32();
                        }
                        catch
                        {
                            _totalBytes = 0;
                        }

                        //Log.Info("GOT A PACKET OF SIZE " + _totalBytes.ToString("X"));

                        if (_totalBytes == 0xC8)
                        {
                            _totalBytes = 0;
                            break;
                        }

                        if (_totalBytes > (256 * 1024))
                        {
                            Log.Error("LENGTH IS XBOX");
                            break;
                        }

                        if (_totalBytes == 0 || _totalBytes < 0)
                        {
                            //TCPHandler.sendPacket(message, client);
                            MessageData packet = new MessageData("none");
                            packet["data"] = new byte[4];
                            packet["cid"] = data.Get<string>("cid");
                            TCPHandler.Net_TcpSend(packet);
                            continue;
                        }

                        _messageBuffer = new MemoryStream();

                        origin += 4;
                        len -= 4;
                    }

                    len += Math.Min((buffer.Length - (int)stream.Position), (_totalBytes - _bytesRead));

                    var newBytes = reader.ReadBytes(len);
                    _messageBuffer.Write(newBytes, 0, len);
                    _bytesRead += len;

                    if (_bytesRead > _totalBytes)
                    {
                        //Debugger.Break();
                    }

                    if (_bytesRead >= _totalBytes)
                    {
                        _bytesRead = 0;
                        _messageBuffer.Position = 0;

                        var breader = new BinaryReader(_messageBuffer);

                        _totalBytes--;
                        var pdtype = breader.ReadByte();
                        var encrypted = (pdtype == 1); // can be 0, 1 and 0xFF
                        var ptype = 0xFF;
                        byte[] pdata = null;

                        if (pdtype == 0xFF)
                        {
                            // TODO: handle this one
                            Log.Debug("Got a 0xFF pdtype!");
                            //return;
                            continue;
                        }
                        var initTime = DateTime.Now;

                        if (!encrypted)
                        {
                            ptype = breader.ReadByte();
                            _totalBytes -= 1;

                            pdata = breader.ReadBytes(_totalBytes);
                        }
                        else
                        {
                            try
                            {
                                var key = GetGlobalKey(data);
                                var iv = DWCrypto.CalculateInitialVector(breader.ReadUInt32());
                                var edata = breader.ReadBytes(_totalBytes - 4);
                                var ddata = DWCrypto.Decrypt(iv, key, edata);

                                var dstream = new MemoryStream(ddata);
                                var dreader = new BinaryReader(dstream);

                                // TODO: find out why this hash isn't just truncated SHA-1
                                var hash = dreader.ReadUInt32();

                                ptype = dreader.ReadByte();
                                pdata = dreader.ReadBytes((int)(dstream.Length - 5));

                                dreader.Close();
                                dstream.Close();

                            }
                            catch { }

                        }
                        var initTime2 = DateTime.Now;
                        var delay3 = (DateTime.Now - delay2).TotalMilliseconds;
                        //Log.Debug("Received a " + _totalBytes + " byte packet (type " + ptype + ") from DemonWare - delay " + delay + "ms. delay after reading " + delay3 + "ms.");
                        /*
                        var message = Messages.NewMessage("dw.packet-received");
                        message["data"] = pdata;
                        message["type"] = ptype;
                        message["crypt"] = encrypted;
                        message["source"] = source;
                        message["cid"] = data["cid"];
                        message["delay"] = delay;
                        message["time"] = DateTime.Now;
                        message.SendNow();
                        */
                        MessageData message = new MessageData("none");
                        message["data"] = pdata;
                        message["type"] = ptype;
                        message["crypt"] = encrypted;
                        message["source"] = source;
                        message["cid"] = data["cid"];
                        message["ci"] = data["ci"];
                        message["delay"] = delay;
                        message["time"] = DateTime.Now;
                        redirectPacket(message);

                        /*var msec = (int)((DateTime.Now - initTime).TotalMilliseconds);
                        var state = GetPerfState(ptype);
                        state.Add(msec);

                        msec = (int)((DateTime.Now - initTime2).TotalMilliseconds);
                        var state2 = GetPerfState(ptype + 1000);
                        state2.Add(msec);

                        Log.Debug("service " + ptype + " min " + state.Min + "/" + state2.Min + " max " + state.Max + "/" + state2.Max);*/
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());

                // disconnect to prevent further pollution of system
                TCPHandler.ForceDisconnect(data);
            }

            reader.Close();
        }

        private void redirectPacket(MessageData data)
        {
            data.Arguments["handled"] = false;

            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");
            bool routed = false;
            try
            {
                if (!crypt && (type == 28 || type == 12 || type == 26))
                {
                    DWAuther.DW_PacketReceived(data);
                    routed = true;
                    //Log.Debug("got auth packet");
                }
                if (crypt && type == 28)
                {
                    DWGroups.DW_PacketReceived(data);
                    routed = true;
                }
                if (type == 7)
                {
                    DWLobby.DW_PacketReceived(data);
                    routed = true;
                    //Log.Debug("got lobby packet");
                }
                if (type == 6)
                {
                    DWMessaging.DW_PacketReceived(data);
                    routed = true;
                    //Log.Debug("got service packet");
                }
                if (type == 21)
                {
                    DWMatch.DW_PacketReceived(data);
                    routed = true;
                    //Log.Debug("got match packet");
                }
                if ((type == 10) || (type == 18) || (type == 23) || (type == 8 || type == 50 || type == 4 || type == 58))
                {
                    DWStorage.DW_PacketReceived(data);
                    routed = true;
                    //Log.Debug("got storage packet");
                }
                if (type == 12 && crypt)
                {
                    DWTitles.DW_PacketReceived(data);
                    routed = true;
                    //Log.Debug("got Title packet");
                }
                if (type == 8)
                {
                    DWProfiles.DW_PacketReceived(data);
                    routed = true;
                }
                if (type == 27)
                {
                    DWDML.DW_PacketReceived(data);
                }
                if (type == 67)
                {
                    DWEventLog.DW_PacketReceived(data);
                }
            }
            catch (Exception e)
            {
                Log.Error("Exception: " + e.ToString());
            }
            /*if(routed == false)
            {
                Log.Debug("UNHANDLED PACKET: " + type);
            }*/

            try
            {
                if (crypt && !data.Get<bool>("handled"))
                {
                    DWRouter.Unknown(data, DWRouter.GetMessage(data));
                }
            }
            catch
            {
                Log.Error("UNKNOWN FAILED, THIS CONNECTION CAN BE CONSIDERED DEAD.");

                // disconnect to prevent pollution of system
                TCPHandler.ForceDisconnect(data);
            }

        }


        public static void Unknown(MessageData data, DWMessage packet)
        {
            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)2);
            reply.Send(true);
        }

        public static DWMessage GetMessage(MessageData data)
        {
            return new DWMessage() { Data = data, ByteBuffer = new DWByteBuffer(data.Get<byte[]>("data")), BitBuffer = new DWBitBuffer(data.Get<byte[]>("data")), Buffer = data.Get<byte[]>("data") };
        }

        public static void Net_TcpDisconnected(MessageData data)
        {
            var cid = data.Get<string>("cid");
            _keys.Remove(cid);
            CIDToUser.Remove(cid);
            CIDToName.Remove(cid);
            CIDToTitle.Remove(cid);

            if (ConnectionsReverse.ContainsKey(cid))
            {
                Connections.Remove(ConnectionsReverse[cid]);
            }

            ConnectionsReverse.Remove(cid);
        }

        private static Dictionary<string, byte[]> _keys = new Dictionary<string, byte[]>();

        public static byte[] GetGlobalKey(MessageData data)
        {
            var cid = data.Get<string>("cid");

            if (_keys.ContainsKey(cid))
            {
                return _keys[cid];
            }

            return new byte[24];
        }

        public static void SetGlobalKey(MessageData data, byte[] key)
        {
            if (key[0] == 0 && key[4] == 0 && key[8] == 0)
            {
                //Debugger.Break();
            }

            _keys[data.Get<string>("cid")] = key;
        }

        public static ulong GetIDForData(MessageData data)
        {
            var cid = data.Get<string>("cid");

            if (!ConnectionsReverse.ContainsKey(cid))
            {
                return 0;
            }

            return ConnectionsReverse[cid];
        }

        public static TitleID GetTitleIDForData(MessageData data)
        {
            var cid = data.Get<string>("cid");

            if (!CIDToTitle.ContainsKey(cid))
            {
                return TitleID.T5;
            }

            return CIDToTitle[cid];
        }

        public static Dictionary<ulong, string> Connections { get; set; }
        public static Dictionary<string, ulong> ConnectionsReverse { get; set; }
        public static Dictionary<string, int> CIDToUser { get; set; }
        public static Dictionary<string, string> CIDToName { get; set; }
        public static Dictionary<string, TitleID> CIDToTitle { get; set; }

    }

    public enum TitleID
    {
        T5 = 18301,
        IW5 = 18409
    }

    public class DWMessage
    {
        public MessageData Data { get; set; }
        public DWBitBuffer BitBuffer { get; set; }
        public DWByteBuffer ByteBuffer { get; set; }
        public byte[] Buffer { get; set; }

        public DWMessage MakeReply(byte type, bool isBit)
        {
            var buf = new byte[8192]; // this will actually be messed up due to Array.Resize later on
            Data.Arguments["type"] = (int)type;

            var message = new DWMessage();
            message.Data = Data;
            message.Buffer = buf;

            if (isBit)
            {
                message.BitBuffer = new DWBitBuffer(buf);
            }
            else
            {
                message.ByteBuffer = new DWByteBuffer(buf);
                message.ByteBuffer.Write(new byte[] { 0xef, 0xbe, 0xad, 0xde, type });
            }

            return message;
        }

        public DWMessage MakeReply(byte type, bool isBit, string cid)
        {
            var buf = new byte[0]; // this will actually be messed up due to Array.Resize later on

            var message = new DWMessage();
            message.Data = new MessageData("none");
            message.Data.Arguments["type"] = (int)type;
            message.Data.Arguments["cid"] = cid;
            message.Buffer = buf;

            if (isBit)
            {
                message.BitBuffer = new DWBitBuffer(buf);
            }
            else
            {
                message.ByteBuffer = new DWByteBuffer(buf);
                message.ByteBuffer.Write(new byte[] { 0xef, 0xbe, 0xad, 0xde, type });
            }

            return message;
        }

        public void Send(bool encrypted)
        {
            var buffer = (BitBuffer != null) ? BitBuffer.Bytes : ByteBuffer.Bytes;

            byte[] array = null;

            if (!encrypted)
            {
                array = new byte[buffer.Length + 6];
                Array.Copy(BitConverter.GetBytes(buffer.Length + 2), 0, array, 0, 4);
                array[4] = 0;
                array[5] = (byte)Data.Get<int>("type");
                Array.Copy(buffer, 0, array, 6, buffer.Length);
            }
            else
            {
                array = new byte[buffer.Length + 4 + 4 + 1];
                Array.Copy(BitConverter.GetBytes(buffer.Length + 5), 0, array, 0, 4);
                array[4] = 1;
                Array.Copy(BitConverter.GetBytes(0x13371337), 0, array, 5, 4);

                var iv = DWCrypto.CalculateInitialVector(0x13371337);
                var key = DWRouter.GetGlobalKey(Data);

                var crypted = DWCrypto.Encrypt(iv, key, buffer);
                Array.Resize(ref array, crypted.Length + 4 + 4 + 1);

                Array.Copy(BitConverter.GetBytes(array.Length - 4), 0, array, 0, 4);
                Array.Copy(crypted, 0, array, 9, crypted.Length);
            }

            //Log.Debug("sending a reply");
            Data.Arguments["handled"] = true;

            MessageData message = new MessageData("none");
            message["data"] = array;
            message["cid"] = Data["cid"];
            TCPHandler.Net_TcpSend(message);
        }
        //public DWByteBuffer ByteBuffer { get; set; }
    }
}
