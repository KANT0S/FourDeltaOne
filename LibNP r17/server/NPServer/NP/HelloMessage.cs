using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using NPx;

namespace NPx
{
    public partial class RPCHelloMessage : NPRPCMessage<HelloMessage>
    {
        public override void Process(NPHandler client)
        {
            Log.Data(string.Format("Got {0} {1} {2} {3}", Message.name, Message.stuff, Message.number, Message.number2));

            var reply = MakeResponse<HelloMessage>(client);
            reply.Message = Message;
            reply.Message.name = "plaza";
            reply.Send();
        }
    }
}
