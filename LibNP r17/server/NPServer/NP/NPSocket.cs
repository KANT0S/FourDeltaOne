using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace NPx
{
    internal class NPServerClient
    {
        public byte[] receiveBuffer = new byte[1024];
        public byte[] sendBuffer = new byte[1024];
        public SocketAsyncEventArgs readEventArgs = new SocketAsyncEventArgs();
        public SocketAsyncEventArgs writeEventArgs = new SocketAsyncEventArgs();
        public Socket socket;
        public NPHandler handler;
    }

    public class NPSocket
    {
        private SocketAsyncEventArgs _acceptEventArgs;
        private Socket _socket;
        private List<NPServerClient> _clients;
        private Dictionary<long, NPHandler> _clientsByID;
        private static NPSocket _instance;

        public NPSocket()
        {
            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _acceptEventArgs = new SocketAsyncEventArgs();

            _clients = new List<NPServerClient>();
            _clientsByID = new Dictionary<long, NPHandler>();
            _instance = this;
        }

        public void Start()
        {
            Log.Info("Starting StreamServer");

            _acceptEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(_acceptEventArgs_Completed);
            _socket.Bind(new IPEndPoint(IPAddress.Any, 3025));
            _socket.Listen(1);

            /*TaskScheduler.EnqueueTask(() =>
            {
                var clients = _clients.ToArray();

                foreach (var client in clients)
                {
                    
                }
            }, 60);*/

            DoAccept(_acceptEventArgs);
        }

        public static int NumClients
        {
            get
            {
                return _instance._clients.Count;
            }
        }

        private static NPHandler[] _handlers = new NPHandler[16384];

        public static IEnumerable<NPHandler> GetClients()
        {
            if (_handlers.Length < _instance._clients.Count)
            {
                var difference = _instance._clients.Count - _handlers.Length;
                var newAddon = (int)(Math.Ceiling(difference / 2048.0) * 2048);

                _handlers = new NPHandler[_handlers.Length + newAddon];
            }

            lock (_instance._clients)
            {
                int i = 0;

                foreach (var client in _instance._clients)
                {
                    _handlers[i] = client.handler;
                    i++;
                }
            }

            return _handlers;
        }

        public static NPHandler GetClient(long npid)
        {
            /*lock (_instance._clients)
            {
                return (from client in _instance._clients
                        where client.handler.NPID == npid
                        select client.handler).FirstOrDefault();
            }*/

            if (_instance._clientsByID.ContainsKey(npid))
            {
                return _instance._clientsByID[npid];
            }

            return null;
        }

        public static void SetClient(long npid, NPHandler handler)
        {
            _instance._clientsByID[npid] = handler;
        }

        public static void Broadcast(string message)
        {
            _instance.DoBroadcast(message);
        }

        private void DoBroadcast(string message)
        {
            var messageBuffer = Encoding.ASCII.GetBytes(message);

            foreach (var client in _clients)
            {
                try
                {
                    Array.Copy(messageBuffer, client.sendBuffer, messageBuffer.Length);
                    client.writeEventArgs.SetBuffer(0, messageBuffer.Length);
                    client.socket.SendAsync(client.writeEventArgs);
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }
            }
        }

        private void DoAccept(SocketAsyncEventArgs args)
        {
            args.AcceptSocket = null;

            if (!_socket.AcceptAsync(args))
            {
                _acceptEventArgs_Completed(this, args);
            }
        }

        void _acceptEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            try
            {
                if (e.SocketError == SocketError.Success)
                {
                    var clientSocket = e.AcceptSocket;
                    var client = new NPServerClient();
                    client.readEventArgs.AcceptSocket = clientSocket;
                    client.readEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(client_Completed);
                    client.readEventArgs.UserToken = client;
                    client.readEventArgs.SetBuffer(client.receiveBuffer, 0, client.receiveBuffer.Length);
                    client.writeEventArgs.AcceptSocket = clientSocket;
                    client.writeEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(client_Completed);
                    client.writeEventArgs.UserToken = client;
                    client.writeEventArgs.SetBuffer(client.sendBuffer, 0, client.sendBuffer.Length);
                    client.socket = clientSocket;
                    client.handler = new NPHandler(client);

                    lock (_clients)
                    {
                        _clients.Add(client);
                    }

                    //DoReceive(client.readEventArgs);
                    if (!client.socket.ReceiveAsync(client.readEventArgs))
                    {
                        HandleReceive(client.readEventArgs);
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
            }

            DoAccept(e);
        }

        /*void DoReceive(SocketAsyncEventArgs args)
        {
            var client = (NPServerClient)args.UserToken;

            if (!client.socket.ReceiveAsync(args))
            {
                DoReceive(args);
            }
        }*/

        void HandleReceive(SocketAsyncEventArgs e)
        {
            try
            {
                var client = (NPServerClient)e.UserToken;

                do
                {
                    if (e.SocketError != SocketError.Success || e.BytesTransferred == 0)
                    {
                        CloseSocket(e);
                        return;
                    }

                    client.handler.HandlePacket(client.receiveBuffer, e.BytesTransferred);
                }
                while (!client.socket.ReceiveAsync(e));
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
                CloseSocket(e);
            }
        }

        internal static void Close(NPServerClient client)
        {
            _instance.CloseSocket(client.readEventArgs);
        }

        void CloseSocket(SocketAsyncEventArgs e)
        {
            try
            {
                var client = (NPServerClient)e.UserToken;

                Servers.RemoveBy(client.handler.NPID);

                lock (_clients)
                {
                    _clients.Remove(client);

                    if (_clientsByID.ContainsKey(client.handler.NPID))
                    {
                        _clientsByID.Remove(client.handler.NPID);
                    }
                }

                try
                {
                    client.socket.Shutdown(SocketShutdown.Both);
                }
                catch { }

                client.socket.Close();
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
            }
        }

        void client_Completed(object sender, SocketAsyncEventArgs e)
        {
            try
            {
                switch (e.LastOperation)
                {
                    case SocketAsyncOperation.Receive:
                        HandleReceive(e);
                        break;

                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
            }
        }
    }
}
