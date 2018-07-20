using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;

namespace GBH
{
    public class ServerClient
    {
        public string Name { get; set; }
        public int ClientNum { get; set; }
        public ServerClientState State { get; set; }
        public NetChannel Channel { get; set; }
        public PlayerEntity Entity { get; set; }
        public uint LastAcknowledgedMessage { get; set; }
        public uint LastInitStateMessage { get; set; }
        public uint LastReceivedReliableCommand { get; set; }
        public uint LastExecutedReliableCommand { get; set; }
        public ServerClientEntityBase[] EntityBases { get; set; }
        public uint LastMessageReceivedAt { get; set; }
        public NetAddress ExternalIP { get; set; }
        public int Deaths { get; set; }

        public uint ReliableSequence { get; set; }
        public uint ReliableAcknowledged { get; set; }
        public string[] ReliableCommands { get; set; }

        public uint ZombifiedAt { get; set; }

        public ServerClient()
        {
            ReliableCommands = new string[64];
        }

        public void InitializeEntity()
        {
            EntityBases = new ServerClientEntityBase[Server.Entities.Length];

            Entity = new PlayerEntity();
            Server.Entities[ClientNum] = Entity;

            Entity.SpawnKey = Server.EntitySpawnKey++;
            Entity.Spawn();

            State = ServerClientState.Active;
        }

        public void DestroyEntity()
        {
            Server.Entities[ClientNum] = null;
        }

        public void ProcessCommand(UserCommand command)
        {
            Entity.Command = command;
            Entity.Think();
        }

        public void SendReliableCommand(string command)
        {
            SendReliableCommand(command, false);
        }

        public void SendReliableCommand(string command, bool isDisconnectCommand)
        {
            if (State != ServerClientState.Active)
            {
                return;
            }

            uint unacknowledged = ReliableSequence - ReliableAcknowledged;

            if (unacknowledged > ReliableCommands.Length && !isDisconnectCommand)
            {
                Server.DropClient(this, "server command overflow");
            }

            ReliableSequence++;
            ReliableCommands[ReliableSequence & (ReliableCommands.Length - 1)] = command;
        }
    }

    public enum ServerClientState
    {
        Connected,
        Initialized,
        Active,
        Zombie
    }

    public class ServerClientEntityBase
    {
        private ServerClient _client;
        private BitStream[] _bases;

        internal ServerClientEntityBase(ServerClient client)
        {
            _client = client;
            _bases = new BitStream[32];
        }

        public BitStream Get()
        {
            // FIXME: might go bad if we're more than 32 packets behind?
            return _bases[_client.LastAcknowledgedMessage % _bases.Length];
        }

        public void Set(BitStream newBase)
        {
            _bases[_client.Channel.SequenceOut % _bases.Length] = newBase;
        }
    }
}
