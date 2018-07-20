using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

namespace NPx
{
    public static class CIHandler
    {
        class CIReportState
        {
            public NPHandler Client { get; set; }
            public int Reason { get; set; }
        }

        public static void Handle(NPHandler client, string data)
        {
            var reason = int.Parse(data.Split(' ')[1]);

            if (reason == 0 || reason == 50001)
            {
                client.LastCI = DateTime.UtcNow;
                return;
            }

            ThreadPool.QueueUserWorkItem(stateo =>
            {
                var state = (CIReportState)stateo;

                if (client.Unclean)
                {
                    return;
                }

                // first, ban the player globally
                try
                {
                    using (var wc = new WebClient())
                    {
                        var url = string.Format("http://auth.iw4.prod.fourdeltaone.net/log.php?s={0}&r={1}", state.Client.SessionToken, state.Reason);
                        Log.Debug(url);
                        Log.Info(wc.DownloadString(url));
                    }
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }

                // then try to send a message to the server they're on
                Thread.Sleep(500);

                var serverID = state.Client.CurrentServer;
                var npID = state.Client.NPID;

                try
                {
                    var remote = NPSocket.GetClient(serverID);
                    var message = new NPRPCResponse<AuthenticateKickUserMessage>(remote);
                    message.Message.npid = (ulong)npID;
                    message.Message.reason = 1;
                    message.Message.reasonString = string.Format("Cheat detected (#{0})", state.Reason);
                    message.Send();
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }

                state.Client.Unclean = true;
            }, new CIReportState()
            {
                Client = client,
                Reason = reason
            });
        }
    }
}
