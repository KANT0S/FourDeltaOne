using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using ProtoBuf;

namespace NPx
{
    public interface INPRPCMessage
    {
        byte[] Serialize();
        void Deserialize(byte[] message);
        void Deserialize(Stream message);

        void Process(NPHandler client);

        int Type { get; }
        int ID { get; set; }
    }

    public abstract class NPRPCMessage<T> : INPRPCMessage where T : global::ProtoBuf.IExtensible
    {
        public abstract int Type
        {
            get;
        }

        public int ID
        {
            get;
            set;
        }

        public virtual void Process(NPHandler client) { }

        public T Message { get; set; }

        public byte[] Serialize()
        {
            var buffer = new MemoryStream();
            buffer.Write(new byte[16], 0, 16);
            Serializer.Serialize(buffer, Message);

            buffer.Position = 0;
            var writer = new BinaryWriter(buffer);
            writer.Write(0xDEADC0DE);
            writer.Write((int)buffer.Length - 16);
            writer.Write(Type);
            writer.Write(ID);

            return buffer.ToArray();
        }

        public void Deserialize(byte[] buffer)
        {
            Deserialize(new MemoryStream(buffer));
        }

        public void Deserialize(Stream message)
        {
            Message = Serializer.Deserialize<T>(message);
        }

        public NPRPCResponse<TResponse> MakeResponse<TResponse>(NPHandler client) where TResponse : global::ProtoBuf.IExtensible
        {
            return new NPRPCResponse<TResponse>(this, client);
        }
    }
}
