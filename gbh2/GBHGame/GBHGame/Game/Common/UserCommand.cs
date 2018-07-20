using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GBH
{
    [Flags]
    public enum ClientButtons
    {
        Forward = 1,
        Backward = 2,
        RotateLeft = 4,
        RotateRight = 8
    }

    public struct UserCommand
    {
        public const byte CommandType = 1;

        // time the command was generated, based on server time
        public uint ServerTime { get; set; }

        // buttons being pressed
        public ClientButtons Buttons { get; set; }

        // serialization functions
        public void Serialize(BitStream message, UserCommand? old)
        {
            if (old == null)
            {
                message.WriteUInt32(ServerTime);
                message.WriteInt16((short)Buttons);
            }
            else
            {
                var oldCommand = old.Value;

                if ((ServerTime - oldCommand.ServerTime) < 255)
                {
                    message.WriteBool(true);
                    message.WriteByte((byte)(ServerTime - oldCommand.ServerTime));
                }
                else
                {
                    message.WriteBool(false);
                    message.WriteUInt32(ServerTime);
                }

                message.WriteDeltaInt16((short)Buttons, (short)oldCommand.Buttons);
            }
        }

        public void Deserialize(BitStream message, UserCommand? old)
        {
            if (old == null)
            {
                ServerTime = message.ReadUInt32();
                Buttons = (ClientButtons)message.ReadInt16();
            }
            else
            {
                var oldCommand = old.Value;
                
                // read server time
                if (message.ReadBool())
                {
                    // is delta time
                    ServerTime = oldCommand.ServerTime + message.ReadByte();
                }
                else
                {
                    // is absolute time
                    ServerTime = message.ReadUInt32();
                }

                // read buttons
                Buttons = (ClientButtons)message.ReadDeltaInt16((short)oldCommand.Buttons);
            }
        }

        public bool TestButton(ClientButtons button)
        {
            return (Buttons & button) == button;
        }
    }
}
