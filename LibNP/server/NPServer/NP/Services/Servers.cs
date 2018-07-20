using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NPx
{
    #region SessionInfo
    public partial class SessionInfo : global::ProtoBuf.IExtensible
    {
        public DateTime lastTouched { get; set; }
    }
    #endregion

    #region RPCServersCreateSessionMessage
    public partial class RPCServersCreateSessionMessage : NPRPCMessage<ServersCreateSessionMessage>
    {
        private static Random random = new Random();

        public override void Process(NPHandler client)
        {
            NPRPCResponse<ServersCreateSessionResultMessage> reply;

            if (!client.Authenticated)
            {
                reply = MakeResponse<ServersCreateSessionResultMessage>(client);
                reply.Message.result = 1;
                reply.Message.sessionid = 0;
                reply.Send();
                return;
            }

            var info = Message.info;
            info.address = BitConverter.ToUInt32(client.Address.Address.GetAddressBytes(), 0);
            info.port = client.Address.Port;
            info.npid = (ulong)client.NPID;
            info.lastTouched = DateTime.UtcNow;

            var bytes = new byte[8];
            random.NextBytes(bytes);

            var id = BitConverter.ToUInt64(bytes, 0);

            lock (Servers.Sessions)
            {
                var oldSessions = (from session in Servers.Sessions
                                   where session.Value.npid == (ulong)client.NPID
                                   select session.Key).ToList();

                foreach (var key in oldSessions)
                {
                    Servers.Sessions.Remove(key);
                }

                Servers.Sessions[id] = info;
            }

            reply = MakeResponse<ServersCreateSessionResultMessage>(client);
            reply.Message.result = 0;
            reply.Message.sessionid = id;
            reply.Send();
        }
    }
    #endregion

    #region RPCServersGetSessionsMessage
    public partial class RPCServersGetSessionsMessage : NPRPCMessage<ServersGetSessionsMessage>
    {
        public override void Process(NPHandler client)
        {
            IEnumerable<SessionInfo> sessionInfos;

            lock (Servers.Sessions)
            {
                // might be quite slow
                sessionInfos = Servers.Sessions.Values.ToList();
            }

            var reply = MakeResponse<ServersGetSessionsResultMessage>(client);
            reply.Message.servers.AddRange(sessionInfos);
            reply.Send();
        }
    }
    #endregion

    #region RPCServersUpdateSessionMessage
    public partial class RPCServersUpdateSessionMessage : NPRPCMessage<ServersUpdateSessionMessage>
    {
        public override void Process(NPHandler client)
        {
            if (!client.Authenticated)
            {
                ReplyWithError(client, 1);
                return;
            }

            var id = Message.sessionid;

            if (!Servers.Sessions.ContainsKey(id))
            {
                ReplyWithError(client, 2);
                return;
            }

            var sessionInfo = Servers.Sessions[id];

            if (sessionInfo.npid != (ulong)client.NPID)
            {
                ReplyWithError(client, 1);
                return;
            }

            sessionInfo.hostname = Message.hostname;
            sessionInfo.players = Message.players;
            sessionInfo.maxplayers = Message.maxplayers;
            sessionInfo.mapname = Message.mapname;
            sessionInfo.lastTouched = DateTime.UtcNow;

            ReplyWithError(client, 0);
        }

        private void ReplyWithError(NPHandler client, int error)
        {
            var reply = MakeResponse<ServersUpdateSessionResultMessage>(client);
            reply.Message.result = error;
            reply.Send();
        }
    }
    #endregion

    #region RPCServersDeleteSessionMessage
    public partial class RPCServersDeleteSessionMessage : NPRPCMessage<ServersDeleteSessionMessage>
    {
        public override void Process(NPHandler client)
        {
            if (!client.Authenticated)
            {
                ReplyWithError(client, 1);
                return;
            }

            var id = Message.sessionid;

            if (!Servers.Sessions.ContainsKey(id))
            {
                ReplyWithError(client, 2);
                return;
            }

            var sessionInfo = Servers.Sessions[id];

            if (sessionInfo.npid != (ulong)client.NPID)
            {
                ReplyWithError(client, 1);
                return;
            }

            lock (Servers.Sessions)
            {
                Servers.Sessions.Remove(id);
            }

            ReplyWithError(client, 0);
        }

        private void ReplyWithError(NPHandler client, int error)
        {
            var reply = MakeResponse<ServersUpdateSessionResultMessage>(client);
            reply.Message.result = error;
            reply.Send();
        }
    }
    #endregion

    #region Servers
    public static class Servers
    {
        public static Dictionary<ulong, SessionInfo> Sessions { get; set; }

        static Servers()
        {
            Sessions = new Dictionary<ulong, SessionInfo>();
        }

        public static void CleanSessions()
        {
            lock (Sessions)
            {
                var oldSessions = (from session in Sessions
                                   where session.Value.lastTouched < (DateTime.UtcNow - TimeSpan.FromSeconds(600))
                                   select session.Key).ToList();

                foreach (var session in oldSessions)
                {
                    Sessions.Remove(session);
                }
            }

            TaskScheduler.EnqueueTask(Servers.CleanSessions, 300);
        }

        public static void RemoveBy(long npid)
        {
            var oldSessions = (from session in Servers.Sessions
                               where session.Value.npid == (ulong)npid
                               select session.Key).ToList();

            foreach (var key in oldSessions)
            {
                lock (Sessions)
                {
                    Servers.Sessions.Remove(key);
                }
            }
        }
    }
    #endregion
}
