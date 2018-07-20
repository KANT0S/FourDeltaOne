using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace DWServer
{
    internal class TCPClient
    {
        public byte[] receiveBuffer = new byte[1024];
        public byte[] sendBuffer = new byte[1024];
        public SocketAsyncEventArgs readEventArgs = new SocketAsyncEventArgs();
        public SocketAsyncEventArgs writeEventArgs = new SocketAsyncEventArgs();
        public Socket socket;
        public DWRouter router;

        public string ConnectionID
        {

            get
            {
                try
                {
                    var source = (IPEndPoint)socket.RemoteEndPoint;

                    return string.Format("{0}:{1}", source.Address.ToString(), source.Port);
                }
                catch { }
                return "::";
            }
        }
    }

    public class TCPHandler
    {
        private SocketAsyncEventArgs _acceptEventArgs;
        private Socket _socket;
        private Dictionary<string, TCPClient> _clients;
        private static TCPHandler _instance;

        public TCPHandler()
        {
            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _acceptEventArgs = new SocketAsyncEventArgs();

            _clients = new Dictionary<string, TCPClient>();
            _instance = this;
        }

        public void Start(int port)
        {
            Log.Info("Starting StreamServer");

            _acceptEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(_acceptEventArgs_Completed);
            _socket.Bind(new IPEndPoint(IPAddress.Any, port));
            _socket.Listen(1);

            DoAccept(_acceptEventArgs);
        }

        public static void Net_TcpSend(MessageData data)
        {
            // TODO: optimize this
            try
            {
                var buffer = data.Get<byte[]>("data");

                // get the cid
                var cid = data.Get<string>("cid");
                var ci = _instance._clients[cid];
                ci.socket.SendBufferSize = 8192;
                // and BeginSend it
                ci.socket.ReceiveTimeout = 10000;
                ci.socket.SendTimeout = 10000;
                ci.socket.BeginSend(buffer, 0, buffer.Length, SocketFlags.None, new AsyncCallback(_instance.Socket_Send), ci);
            }
            catch { }
        }

        private void Socket_Send(IAsyncResult ar)
        {
            // get ci and end
            var ci = (TCPClient)ar.AsyncState;

            try
            {
                ci.socket.EndSend(ar);
            }
            catch (Exception)
            {
                _instance.CloseSocket(ci.readEventArgs);
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
                    var client = new TCPClient();
                    client.readEventArgs.AcceptSocket = clientSocket;
                    client.readEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(client_Completed);
                    client.readEventArgs.UserToken = client;
                    client.readEventArgs.SetBuffer(client.receiveBuffer, 0, client.receiveBuffer.Length);
                    client.writeEventArgs.AcceptSocket = clientSocket;
                    client.writeEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(client_Completed);
                    client.writeEventArgs.UserToken = client;
                    client.writeEventArgs.SetBuffer(client.sendBuffer, 0, client.sendBuffer.Length);
                    client.socket = clientSocket;
                    client.router = new DWRouter();

                    lock (_clients)
                    {
                        _clients.Add(client.ConnectionID, client);
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
            var client = (TCPClient)args.UserToken;

            while (!client.socket.ReceiveAsync(args))
            {
                Log.Debug("Handling stuff - " + client.ConnectionID);
                HandleReceive(args);
            }
        }*/

        public static void ForceDisconnect(MessageData message)
        {
            _instance.CloseSocket(message.Get<TCPClient>("ci").readEventArgs);
        }

        void HandleReceive(SocketAsyncEventArgs e)
        {
            try
            {
                var client = (TCPClient)e.UserToken;

                do
                {
                    if (e.SocketError != SocketError.Success || e.BytesTransferred == 0)
                    {
                        CloseSocket(e);
                        return;
                    }

                    var messageData = new byte[e.BytesTransferred];
                    Array.Copy(client.receiveBuffer, messageData, e.BytesTransferred);

                    try
                    {
                        MessageData packet = new MessageData("none");
                        packet["data"] = messageData;
                        packet["socket"] = client.socket;
                        packet["cid"] = client.ConnectionID;
                        packet["source"] = client.socket.RemoteEndPoint;
                        packet["ci"] = client;
                        packet["time"] = DateTime.Now;
                        client.router.handlePacket(packet);
                    }
                    catch (Exception ex)
                    {
                        Log.Error(ex.ToString());
                    }
                }
                while (!client.socket.ReceiveAsync(e));
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
                CloseSocket(e);
            }
        }

        internal static void Close(TCPClient client)
        {
            _instance.CloseSocket(client.readEventArgs);
        }

        void CloseSocket(SocketAsyncEventArgs e)
        {
            try
            {
                var client = (TCPClient)e.UserToken;
                lock (_clients)
                {
                    _clients.Remove(client.ConnectionID);
                }

                MessageData message = new MessageData("none");
                message["source"] = client.socket.RemoteEndPoint;
                message["cid"] = client.ConnectionID;

                try
                {
                    DWMatch.Net_TcpDisconnected(message);
                    DWGroups.Net_TcpDisconnected(message);
                    DWRouter.Net_TcpDisconnected(message);
                }
                catch (Exception ex)
                {
                    Log.Error(ex.ToString());
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
