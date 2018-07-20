using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GBH
{
    public class NetChannel
    {
        public NetChannelType ChannelType { get; private set; }
        public NetAddress Address { get; private set; }
        public uint SequenceIn { get; private set; }
        public uint SequenceOut { get; private set; }

        public NetChannel(NetChannelType type, NetAddress address)
        {
            ChannelType = type;
            Address = address;
        }

        public void SendMessage(BitStream stream)
        {
            SendMessage(stream.Bytes);
        }

        public void SendMessage(byte[] message)
        {
            // message header
            var outMessage = new BitStream();
            outMessage.WriteUInt32(SequenceOut);

            SequenceOut++;

            // write the message
            outMessage.WriteBytes(message);

            NetManager.SendPacket(ChannelType, Address, outMessage.Bytes);

            // log the packet
            if (ConVar.GetValue<bool>("net_showpackets"))
            {
                Log.Write(LogLevel.Debug, "sending {0}b, seq={1}", outMessage.Bytes.Length, SequenceOut);
            }
        }

        public bool ProcessPacket(byte[] packet)
        {
            var message = new BitStream(packet);
            var sequence = message.ReadUInt32();

            if (sequence < SequenceIn)
            {
                Log.Write(LogLevel.Debug, "out of order packet ({0}, {1})", sequence, SequenceIn);
                return false;
            }

            if (sequence > (SequenceIn + 1))
            {
                Log.Write(LogLevel.Debug, "dropped packet ({0}, {1})", sequence, SequenceIn);
            }

            SequenceIn = sequence;

            return true;
        }
    }

    public enum NetChannelType
    {
        ClientToServer,
        ServerToClient
    }
}
