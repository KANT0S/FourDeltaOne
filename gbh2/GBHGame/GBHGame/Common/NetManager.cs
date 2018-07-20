using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace GBH
{
    public static class NetManager
    {
        private struct NetPacket
        {
            public NetAddress adr;
            public byte[] data;
            public uint time;

            public NetPacket(NetAddress adr, byte[] data)
                : this()
            {
                this.adr = adr;
                this.data = data;
                this.time = Game.Time;
            }
        }

        private static Socket _socket;
        private static List<NetPacket> _receiveQueue;
        private static List<NetPacket> _sendQueue;

        private static ConVar net_forceLatency;

        public static void Initialize(int port)
        {
            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

            bool bound = false;

            while (!bound)
            {
                try
                {
                    _socket.Bind(new IPEndPoint(IPAddress.Any, port));
                    bound = true;
                }
                catch (SocketException e)
                {
                    if (e.SocketErrorCode == SocketError.AddressAlreadyInUse)
                    {
                        port++;
                    }
                    else
                    {
                        throw e;
                    }
                }
            }

            Log.Write(LogLevel.Info, "Listening on port {0}", port);

            _socket.Blocking = false;

            _receiveQueue = new List<NetPacket>();
            _sendQueue = new List<NetPacket>();

            net_forceLatency = ConVar.Register("net_forceLatency", 0, "Force network packets to be sent/arrive with latency.", ConVarFlags.None);
        }

        public static void Process()
        {
            byte[] buffer = new byte[16384];
            int bytes = 0;
            EndPoint remoteEP = new IPEndPoint(IPAddress.Any, 0);

            while (true)
            {
                try
                {
                    bytes = _socket.ReceiveFrom(buffer, buffer.Length, SocketFlags.None, ref remoteEP);
                }
                catch (SocketException e)
                {
                    if (e.SocketErrorCode != SocketError.WouldBlock && e.SocketErrorCode != SocketError.ConnectionReset)
                    {
                        Log.Write(LogLevel.Info, "socket error {0}", e.SocketErrorCode);
                    }
                    else
                    {
                        break;
                    }
                }

                if (bytes > 0)
                {
                    byte[] cutBuffer = new byte[bytes];
                    Array.Copy(buffer, cutBuffer, bytes);

                    NetAddress from = new NetAddress((IPEndPoint)remoteEP);

                    if (net_forceLatency.GetValue<int>() == 0)
                    {
                        ProcessPacket(from, cutBuffer);
                    }
                    else
                    {
                        _receiveQueue.Add(new NetPacket(from, cutBuffer));
                    }
                }
            }

            if (_sendQueue.Count > 0)
            {
                var toSend = _sendQueue.ToArray();
                var time = Game.Time;
                var latency = net_forceLatency.GetValue<int>();

                foreach (var item in toSend)
                {
                    if (time >= (item.time + latency))
                    {
                        SendPacketInternal(item.adr, item.data);
                        _sendQueue.Remove(item);
                    }
                }
            }

            if (_receiveQueue.Count > 0)
            {
                var toReceive = _receiveQueue.ToArray();
                var time = Game.Time;
                var latency = net_forceLatency.GetValue<int>();

                foreach (var item in toReceive)
                {
                    if (time >= (item.time + latency))
                    {
                        ProcessPacket(item.adr, item.data);
                        _receiveQueue.Remove(item);
                    }
                }
            }
        }

        private static void ProcessPacket(NetAddress from, byte[] buffer)
        {
            if (ConVar.GetValue<bool>("sv_running"))
            {
                Server.ProcessPacket(from, buffer);
            }
            else
            {
                Client.ProcessPacket(from, buffer);
            }
        }

        public static void SendOOBPacket(NetChannelType type, NetAddress address, string format, params object[] others)
        {
            string message = "????" + string.Format(format, others);
            byte[] data = Encoding.UTF8.GetBytes(message);

            data[0] = 0xFF; data[1] = 0xFF; data[2] = 0xFF; data[3] = 0xFF;

            SendPacket(type, address, data);
        }

        public static void SendPacket(NetChannelType type, NetAddress address, byte[] packet)
        {
            if (address.Type == AddressType.Loopback)
            {
                SendLoopbackPacket(type, packet);
            }
            else
            {
                if (net_forceLatency.GetValue<int>() == 0)
                {
                    SendPacketInternal(address, packet);
                }
                else
                {
                    _sendQueue.Add(new NetPacket(address, packet));
                }
            }
        }

        private static void SendPacketInternal(NetAddress address, byte[] packet)
        {
            _socket.SendTo(packet, address.EndPoint);
        }

        private static Queue<byte[]> _clientLoopQueue = new Queue<byte[]>();
        private static Queue<byte[]> _serverLoopQueue = new Queue<byte[]>();

        private static void SendLoopbackPacket(NetChannelType type, byte[] packet)
        {
            if (type == NetChannelType.ClientToServer)
            {
                _serverLoopQueue.Enqueue(packet);
            }
            else if (type == NetChannelType.ServerToClient)
            {
                _clientLoopQueue.Enqueue(packet);
            }
        }

        public static byte[] GetLoopbackPacket(NetChannelType type)
        {
            Queue<byte[]> queue = null;

            if (type == NetChannelType.ClientToServer)
            {
                queue = _serverLoopQueue;
            }
            else if (type == NetChannelType.ServerToClient)
            {
                queue = _clientLoopQueue;
            }

            if (queue != null)
            {
                if (queue.Count > 0)
                {
                    return queue.Dequeue();
                }
            }

            return null;
        }
    }

    public struct NetAddress
    {
        public AddressType Type { get; private set; }
        public IPEndPoint EndPoint { get; private set; }

        public static NetAddress Loopback
        {
            get
            {
                return new NetAddress() { Type = AddressType.Loopback };
            }
        }

        public static NetAddress Resolve(string name)
        {
            try
            {
                string[] namePort = name.Split(':');
                int port = (namePort.Length == 2) ? int.Parse(namePort[1]) : 29960;
                name = namePort[0];

                IPHostEntry entry = Dns.GetHostEntry(name);
                IPAddress ip = Array.Find(entry.AddressList, a => a.AddressFamily == AddressFamily.InterNetwork);

                if (ip.AddressFamily == AddressFamily.InterNetwork && ip.GetAddressBytes()[0] == 127)
                {
                    return Loopback;
                }

                return new NetAddress(new IPEndPoint(ip, port));
            }
            catch (Exception e)
            {
                Log.Write(LogLevel.Warning, e.Message);
            }

            return NetAddress.Loopback; // might be bad
        }

        public NetAddress(IPEndPoint endpoint)
            : this()
        {
            Type = AddressType.IPv4;
            EndPoint = endpoint;
        }

        public override string ToString()
        {
            if (Type == AddressType.IPv4)
            {
                return EndPoint.ToString();
            }
            else if (Type == AddressType.Loopback)
            {
                return "localhost";
            }
            else
            {
                return "";
            }
        }

        public static bool operator ==(NetAddress left, NetAddress right)
        {
            if (left.Type != right.Type)
            {
                return false;
            }

            if (left.EndPoint == null)
            {
                return right.EndPoint == null;
            }

            return left.EndPoint.Equals(right.EndPoint);
        }

        public static bool operator !=(NetAddress left, NetAddress right)
        {
            return !(left == right);
        }
    }

    public enum AddressType
    {
        Invalid,
        Loopback,
        IPv4
    }
}
