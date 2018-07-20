using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NPx
{
    public partial class RPCMessagingSendDataMessage : NPRPCMessage<MessagingSendDataMessage>
    {
        public override void Process(NPHandler client)
        {
            if (!client.Authenticated)
            {
                return;
            }

            var npidTo = (long)Message.npid;
            var clientTo = NPSocket.GetClient(npidTo);

            var response = new NPRPCResponse<MessagingSendDataMessage>(clientTo);
            response.Message.npid = (ulong)client.NPID;
            response.Message.data = Message.data;
            response.Send();

            Log.Info("sent instant message from " + client.NPID.ToString("X16") + " to " + npidTo.ToString("X16"));
        }
    }
}
