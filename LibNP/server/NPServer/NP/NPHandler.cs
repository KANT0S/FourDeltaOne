using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

using NPx;

namespace NPx
{
    public class NPHandler
    {
        // static stuff
        private static Thread _thread;

        public static void Start()
        {
            _thread = new Thread(new ThreadStart(Run));
            _thread.Start();
        }

        private static void Run()
        {
            while (true)
            {
                Thread.Sleep(1);

                var clients = NPSocket.GetClients();
                foreach (var client in clients)
                {
                    if (client == null)
                    {
                        continue;
                    }

                    client.Process();
                }
            }
        }

        public static int PacketQueueSize
        {
            get
            {
                return _packetQueueSize;
            }
        }

        private static int _packetQueueSize;

        // public state
        public bool Authenticated { get; set; }
        public long NPID { get; set; }
        public int GroupID { get; set; }
        public string SessionToken { get; set; }
        public long CurrentServer { get; set; }
        public bool Unclean { get; set; }
        public DateTime LastCI { get; set; }

        public IPEndPoint Address
        {
            get
            {
                return (IPEndPoint)_client.socket.RemoteEndPoint;
            }
        }

        // message queue
        private Queue<NPMessage> _messages;

        // NP socket state
        private NPServerClient _client;

        // message reading state
        private int _bytesRead = 0;
        private int _totalBytes = 0;
        private MemoryStream _messageBuffer;
        private int _messageType = 0;
        private int _messageID = 0;

        internal NPHandler(NPServerClient client)
        {
            _messages = new Queue<NPMessage>();
            _client = client;

            LastCI = DateTime.UtcNow;
        }

        public void Send(byte[] buffer)
        {
            try
            {
                //Array.Copy(buffer, _client.sendBuffer, buffer.Length);
                //_client.writeEventArgs.SetBuffer(0, buffer.Length);
                //_client.socket.SendAsync(_client.writeEventArgs);

                _client.socket.BeginSend(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None, Socket_Send, null);
            }
            catch (Exception e)
            {
                Log.Error(e.ToString());
                //NPSocket.Close(_client);
            }
        }

        private void Socket_Send(IAsyncResult async)
        {
            try
            {
                _client.socket.EndSend(async);
            }
            catch (Exception e)
            {
                Log.Error(e.ToString());
                //NPSocket.Close(_client);
            }
        }

        public void Process()
        {
            while (_messages.Count > 0)
            {
                Interlocked.Decrement(ref _packetQueueSize);

                var message = _messages.Dequeue();
                message.Process();                
            }
        }

        public void HandlePacket(byte[] buffer, int packetLength)
        {
            // TODO: make it capable of reading multiple messages per packet
            var newBuffer = new byte[packetLength];
            Array.Copy(buffer, newBuffer, packetLength);

            var stream = new MemoryStream(newBuffer);
            var reader = new BinaryReader(stream);

            var origin = 0;
            var len = newBuffer.Length;

            if (_bytesRead == 0)
            {
                var signature = reader.ReadUInt32();
                if (signature != 0xDEADC0DE)
                {
                    Log.Debug("Signature doesn't match.");
                    return;
                }

                var length = reader.ReadInt32();
                var mtype = reader.ReadInt32();
                var id = reader.ReadInt32();

                _totalBytes = length;
                _messageBuffer = new MemoryStream();
                _messageType = mtype;
                _messageID = id;

                origin = 16;
                len -= 16;
            }

            _messageBuffer.Write(newBuffer, origin, len);
            _bytesRead += len;

            if (_bytesRead >= _totalBytes)
            {
                _bytesRead = 0;
                _messageBuffer.Position = 0;

                var message = new NPMessage(this);
                message.Buffer = _messageBuffer;
                message.Type = _messageType;
                message.ID = _messageID;
                _messages.Enqueue(message);

                Interlocked.Increment(ref _packetQueueSize);
            }   
        }

        public void CloseConnection(bool closeApp)
        {
            if (closeApp)
            {
                var closeMessage = new NPRPCResponse<CloseAppMessage>(this);
                closeMessage.Message.reason = "Another client connected with the same ID.";
                closeMessage.Send();
            }

            //NPSocket.Close(_client);
        }
    }
}
