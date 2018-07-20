using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NPx
{
    public class NPRPCResponse<T> where T : global::ProtoBuf.IExtensible
    {
        public T Message { get; set; }
        public int ID { get; set; }
        private INPRPCMessage SourceMessage { get; set; }
        private NPHandler SourceClient { get; set; }

        public NPRPCResponse(NPHandler client)
        {
            SourceClient = client;
            Message = Activator.CreateInstance<T>();
        }

        public NPRPCResponse(INPRPCMessage message, NPHandler client)
            : this(client)
        {
            SourceMessage = message;
            ID = SourceMessage.ID;
        }

        public void Send()
        {
            var message = (NPRPCMessage<T>)NPMessageFactory.CreateMessage(typeof(T));
            message.Message = Message;
            message.ID = ID;

            var buffer = message.Serialize();
            SourceClient.Send(buffer);
        }
    }
}
