using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

using NPx;

namespace NPx
{
    public partial class RPCAuthenticateValidateTicketMessage : NPRPCMessage<AuthenticateValidateTicketMessage>
    {
        public override void Process(NPHandler client)
        {
            var ipNum = (uint)IPAddress.NetworkToHostOrder(Message.clientIP);
            var ip = new IPAddress(BitConverter.GetBytes(ipNum));
            var ticket = ReadTicket(Message.ticket);
            var npid = Message.npid;

            if (npid == 0)
            {
                npid = ticket.clientID;
            }

            var valid = false;
            var groupID = 0;

            if (ticket.version == 1)
            {
                if (client.NPID == (long)ticket.serverID)
                {
                    if (npid == ticket.clientID)
                    {
                        var remoteClient = NPSocket.GetClient((long)ticket.clientID);
                        remoteClient.CurrentServer = client.NPID;

                        if ((DateTime.UtcNow - remoteClient.LastCI).TotalSeconds > 30)
                        {
                            return;
                        }

                        if (remoteClient != null && !remoteClient.Unclean)
                        {
                            Log.Debug("Ticket auth: remote address " + remoteClient.Address.Address.ToString());
                            Log.Debug("Ticket auth: message address " + ip.ToString());

                            if (ipNum == 0 || remoteClient.Address.Address.Equals(ip))
                            {
                                valid = true;

                                groupID = remoteClient.GroupID;

                                Log.Info("Successfully authenticated a ticket for client " + remoteClient.NPID.ToString("x16"));
                            }
                            else
                            {
                                Log.Info("Ticket auth: IP address didn't match.");
                            }
                        }
                        else
                        {
                            Log.Info("Ticket auth: no such client");
                        }
                    }
                    else
                    {
                        Log.Info("Ticket auth: NPID didn't match.");
                    }
                }
                else
                {
                    Log.Info("Ticket auth: server NPID didn't match.");
                }
            }
            else
            {
                Log.Info("Ticket auth: version didn't match.");
            }

            var reply = MakeResponse<AuthenticateValidateTicketResultMessage>(client);
            reply.Message.result = (valid) ? 0 : 1;
            reply.Message.groupID = groupID;
            reply.Message.npid = npid;
            reply.Send();
        }

        private class NPTicket
        {
            public int version;
            public ulong clientID;
            public ulong serverID;
            public uint time;
        }

        private NPTicket ReadTicket(byte[] bytes)
        {
            var ticket = new NPTicket();
            ticket.version = BitConverter.ToInt32(bytes, 0);

            if (ticket.version == 1)
            {
                ticket.clientID = BitConverter.ToUInt64(bytes, 4);
                ticket.serverID = BitConverter.ToUInt64(bytes, 12);
                ticket.time = BitConverter.ToUInt32(bytes, 20);
            }

            return ticket;
        }
    }

    public partial class RPCAuthenticateWithTokenMessage : NPRPCMessage<AuthenticateWithTokenMessage>
    {
        public override void Process(NPHandler client)
        {
            Log.Data("Client authenticating with token");

            NPAuthAPI.RequestAuthForToken(this, client);
        }
    }

    public partial class RPCAuthenticateWithKeyMessage : NPRPCMessage<AuthenticateWithKeyMessage>
    {
        private static int _anonymousNPID = 0;

        public override void Process(NPHandler client)
        {
            Log.Data("Client authenticating with key " + Message.licenseKey);

            var thread = new Thread(this.HandleAuthentication);
            thread.Start(client);
        }

        private void HandleAuthentication(object cliento)
        {
            var client = (NPHandler)cliento;

            int result = 1;
            long npid = 0;
            bool valid = false;

            if (Message.licenseKey != "")
            {
                var database = XNP.Create();
                var keys = from key in database.LicenseKeys
                           where key.Key == Message.licenseKey
                           select key;

                valid = (keys.Count() == 1);
                result = (keys.Count() == 1) ? 0 : 1;
                npid = 0;

                if (valid)
                {
                    npid = 0x120000100000000 | keys.First().ID;

                    NPHandler existingClient = NPSocket.GetClient(npid);

                    if (existingClient != null)
                    {
                        existingClient.CloseConnection(true);
                    }
                }

                database.Dispose();
            }
            else
            {
                valid = true;
                result = 0;
                npid = 0x130000100000000 | (uint)Interlocked.Increment(ref _anonymousNPID);
            }

            var reply = MakeResponse<AuthenticateResultMessage>(client);
            reply.Message.result = result;

            if (valid)
            {
                reply.Message.npid = (ulong)npid;
                reply.Message.sessionToken = new byte[16] { 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
            }
            else
            {
                reply.Message.npid = (ulong)0;
                reply.Message.sessionToken = new byte[16] { 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
            }

            reply.Send();

            client.Authenticated = valid;
            client.NPID = npid;
            NPSocket.SetClient(npid, client);
        }
    }

    public partial class RPCAuthenticateWithDetailsMessage : NPRPCMessage<AuthenticateWithDetailsMessage>
    {
        public override void Process(NPHandler client)
        {
            Log.Data("Client authenticating with username " + Message.username);

            var thread = new Thread(this.HandleAuthentication);
            thread.Start(client);
        }

        private void HandleAuthentication(object cliento)
        {
            var client = (NPHandler)cliento;

            using (var wc = new WebClient())
            {
                var authString = string.Format("{0}&&{1}", Message.username, Message.password);

                // TODO: properly implement all of this (error checking, ...)
                var resultString = wc.UploadString("http://auth.iw4.prod.fourdeltaone.net/remauth.php", authString);
                var result = resultString.Split('#');
                var success = false;
                var npid = 0L;

                Log.Data(resultString);

                if (result[0] == "ok")
                {
                    success = true;
                    npid = (0x110000100000000 | uint.Parse(result[2]));
                }

                var reply = MakeResponse<AuthenticateResultMessage>(client);
                reply.Message.result = (success) ? 0 : 1;
                reply.Message.npid = (ulong)npid;
                reply.Message.sessionToken = new byte[16] { 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

                if (resultString.Contains("banned from this board"))
                {
                    reply.Message.result = 3;
                }

                client.Authenticated = success;
                client.NPID = npid;
                NPSocket.SetClient(npid, client);

                reply.Send();
            }
        }
    }
}
