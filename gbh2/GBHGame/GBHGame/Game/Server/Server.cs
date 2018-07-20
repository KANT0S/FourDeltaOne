using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Jitter;
using Jitter.Collision;

namespace GBH
{
    public static class Server
    {
        private static World _physicsWorld;

        public static World PhysicsWorld
        {
            get
            {
                return _physicsWorld;
            }
        }

        private static uint _residualTime;
        private static uint _serverTime;

        // an identifier that changes on every map change
        private static int _stateID;

        private static ServerClient[] _clients;
        private static Entity[] _entities;

        public const byte SnapshotNumber = 1;
        public const byte ReliableCommandNumber = 2;
        public const byte InitStateNumber = 3;

        private static Random _rng;

        public static uint Time
        {
            get
            {
                return _serverTime;
            }
        }

        public static Entity[] Entities
        {
            get
            {
                return _entities;
            }
        }

        public static int EntitySpawnKey { get; set; }

        /// <summary>
        /// Global server initialization code
        /// </summary>
        public static void Initialize()
        {
            // temp stuff to keep in sync with client using Game.Time
            //_serverTime = Game.Time;

            ConVar.Register("sv_gravity", 0.5f, "Gravitational value", ConVarFlags.None);

            _clients = new ServerClient[24];

            _rng = new Random();
            
            // testing stuff: add local client
            //_clients[0] = new ServerClient();
            //_clients[0].Channel = new NetChannel(NetChannelType.ServerToClient, NetAddress.Loopback);
            //_clients[0].Name = "me";
        }

        private static string _mapName;

        /// <summary>
        /// 'status' command handler
        /// </summary>
        public static void Status_f()
        {
            if (!ConVar.GetValue<bool>("sv_running"))
            {
                Log.Write(LogLevel.Error, "Server is not running.");
            }
            else
            {
                Log.Write(LogLevel.Info, "map: {0}", _mapName);
                Log.Write(LogLevel.Info, "num name               state address              ");
                Log.Write(LogLevel.Info, "--- ------------------ ----- ---------------------");
                foreach (ServerClient client in _clients)
                {
                    if (client != null)
                    {
                        var num = client.ClientNum.ToString().PadLeft(3);
                        var name = client.Name.ToString().PadRight(18);
                        var state = "     ";

                        switch (client.State)
                        {
                            case ServerClientState.Connected:
                            case ServerClientState.Initialized:
                                state = "CNCT ";
                                break;
                            case ServerClientState.Zombie:
                                state = "ZMBI ";
                                break;
                        }

                        var ip = client.ExternalIP.ToString().PadRight(21);

                        Log.Write(LogLevel.Info, "{0} {1} {2} {3}", num, name, state, ip);
                    }
                }
            }
        }

        /// <summary>
        /// 'kick' command handler
        /// </summary>
        /// <param name="name">player to kick</param>
        public static void Kick_f(string name)
        {
            if (!ConVar.GetValue<bool>("sv_running"))
            {
                Log.Write(LogLevel.Error, "Server is not running.");
                return;
            }

            foreach (ServerClient thisClient in _clients)
            {
                if (thisClient != null)
                {
                    if (thisClient.Name.ToLower() == name.ToLower())
                    {
                        DropClient(thisClient, "Player kicked");
                    }
                }
            }
        }

        /// <summary>
        /// kill client command
        /// </summary>
        /// <param name="name">player to kill</param>
        // Welcome to operation KillClient. In this operation you were given the choice to kill a client. You choose to so now we are at this function
        // Now, first things first we need to check if your the host and not just some random clinet trying to troll!
        // Right, so now we've past that we need to fetch our client, once we have him we'll just make him respawn.
        // once respawned we now increase his death count by one! Yes one, not two, not three - one!
        // now, i come to think about it this just seems pointles.. But oh well, what can you do - I don't like backspace or ctrl+z, so meh.
        public static void KillClient(string name)
        {
            // check that it's the server that used the command
            if (!ConVar.GetValue<bool>("sv_running"))
            {
                Log.Write(LogLevel.Error, "Server is not running.");
                return;
            }

            foreach (ServerClient client in _clients)
            {
                if (client != null)
                {
                    if (client.Name.ToLower() == name.ToLower())
                    {
                        // now to actually kill the client..
                        SendReliableCommand(null, "print \"{0} was killed...\"", client.Name);
                        // spawn him
                        client.Entity.Spawn();
                        // add up his death count
                        client.Deaths = 123;
                    }
                }
            }
        }

        /// <summary>
        /// Initializes the server after a map change.
        /// </summary>
        public static void InitializeMap(string mapName)
        {
            _mapName = mapName;
            _stateID = _rng.Next();

            CreatePhysicsWorld();

            // add entities
            _entities = new Entity[4096];

            // set current clients to be inactive
            foreach (var client in _clients)
            {
                if (client != null && client.State != ServerClientState.Zombie)
                {
                    client.State = ServerClientState.Connected;
                }
            }
        }

        // temp function!
        public static void CreatePhysicsWorld()
        {
            // initialize physics world
            var collisionSystem = new CollisionSystemSAP();
            _physicsWorld = new World(collisionSystem);
        }

        /// <summary>
        /// Per-frame server processing function
        /// </summary>
        public static void Process()
        {
            if (!ConVar.GetValue<bool>("sv_running"))
            {
                return;
            }

            CellManager.Recenter(from client in _clients
                                 where client != null && client.Entity != null
                                 select client.Entity.Position);

            _residualTime += Game.FrameMsec;

            // 20 FPS = 50msec intervals
            while (_residualTime > 50)
            {
                _residualTime -= 50;
                _serverTime += 50;

                ProcessGame();
            }
        }

        /// <summary>
        /// Per-server-frame processing function.
        /// </summary>
        private static void ProcessGame()
        {
            //Log.Write(LogLevel.Info, "We are your genetic destiny.");
            // send snapshots to clients
            DropDrownedClients();
            SendSnapshots();
            CleanZombies();
            CheckForTimeouts();
        }

        private static void DropDrownedClients()
        {
            foreach (var client in _clients)
            {
                if (client == null)
                {
                    continue;
                }

                if (client.State != ServerClientState.Active)
                {
                    continue;
                }

                if (client.Entity.Position.Z < 0f)
                {
                    SendReliableCommand(null, "print \"{0} drowned...\"", client.Name);
                    //client.Entity.Die();
                    client.Entity.Spawn();
                }
            }
        }

        private static void CleanZombies()
        {
            for (int i = 0; i < _clients.Length; i++)
            {
                if (_clients[i] != null && _clients[i].State == ServerClientState.Zombie)
                {
                    if ((_serverTime - _clients[i].ZombifiedAt) > 1000)
                    {
                        _clients[i] = null;

                        Log.Write(LogLevel.Debug, "Reaped client {0}.", i);
                    }
                }
            }
        }

        private static void CheckForTimeouts()
        {
            for (int i = 0; i < _clients.Length; i++)
            {
                if (_clients[i] != null && _clients[i].State != ServerClientState.Zombie)
                {
                    // TODO: make into a convar
                    if ((_serverTime - _clients[i].LastMessageReceivedAt) > 30000)
                    {
                        DropClient(_clients[i], "Connection timed out");
                    }
                }
            }
        }

        public static void DropClient(ServerClient client, string reason)
        {
            client.SendReliableCommand(string.Format("disconnect \"{0}\"", reason), true);
            SendSnapshot(client);

            Log.Write(LogLevel.Info, "Client {0} disconnected (name {1}, reason {2})", client.ClientNum, client.Name, reason);
            SendReliableCommand(null, "print \"{0} disconnected ({1})\"", client.Name, reason);
            SendSnapshot(client);

            client.DestroyEntity();

            client.ZombifiedAt = _serverTime;
            client.State = ServerClientState.Zombie;
        }

        public static void SendReliableCommand(ServerClient client, string command, params string[] args)
        {
            command = string.Format(command, args);

            if (client != null)
            {
                client.SendReliableCommand(command);
            }
            else
            {
                foreach (ServerClient thisClient in _clients)
                {
                    if (thisClient != null)
                    {
                        thisClient.SendReliableCommand(command);
                    }
                }
            }
        }

        private static void SendSnapshots()
        {
            foreach (var client in _clients)
            {
                if (client == null)
                {
                    continue;
                }

                if (client.State == ServerClientState.Active)
                {
                    SendSnapshot(client);
                }
            }
        }

        private static void SendSnapshot(ServerClient client)
        {
            // send entity states
            var message = new BitStream();

            // reliable messages
            for (uint i = client.ReliableAcknowledged + 1; i <= client.ReliableSequence; i++)
            {
                // header byte
                message.WriteByte(Server.ReliableCommandNumber);

                // command number
                message.WriteUInt32(i);

                // command
                message.WriteString(client.ReliableCommands[i & (client.ReliableCommands.Length - 1)]);
            }

            message.WriteByte(Server.SnapshotNumber);
            message.WriteUInt32(_serverTime);
            message.WriteUInt32(client.LastReceivedReliableCommand);

            var useDelta = true;

            if ((client.Channel.SequenceOut - client.LastAcknowledgedMessage) >= 28)
            {
                useDelta = false;
                Log.Write(LogLevel.Warning, "client {0} lagged behind more than [max stored old packets]", client.Name);
            }

            for (int i = 0; i < Entities.Length; i++)
            {
                var entity = Entities[i];

                if (entity == null)
                {
                    continue;
                }

                // write the entity number
                message.WriteInt32(i, 12);

                var entityBase = (useDelta) ? ((client.EntityBases[i] != null) ? client.EntityBases[i].Get() : null) : null;

                var deltaMessage = new DeltaBitStream(entityBase, message);

                // write the spawn key
                deltaMessage.WriteInt32(entity.SpawnKey, 20);

                // write the type code
                deltaMessage.WriteInt32(entity.TypeCode, 4);

                // write the actual data
                entity.Serialize(deltaMessage);

                // set the new base
                if (client.EntityBases[i] == null)
                {
                    client.EntityBases[i] = new ServerClientEntityBase(client);
                }

                client.EntityBases[i].Set(deltaMessage.NewBase);
            }

            message.WriteInt32(4095, 12);
            message.WriteByte(0xFF);

            // send the message
            client.Channel.SendMessage(message);
        }

        private static void SendInitializationState(ServerClient client)
        {
            // send enough state for the client to initialize the game
            var message = new BitStream();
            message.WriteByte(Server.InitStateNumber);

            // current state id
            message.WriteInt32(_stateID);

            // map name
            message.WriteString(_mapName);

            // and send the message
            message.WriteByte(0xFF);
            client.Channel.SendMessage(message);

            client.LastInitStateMessage = client.Channel.SequenceOut;
        }

        public static void ProcessPacket(NetAddress from, byte[] packet)
        {
            if (packet.Length < 4)
            {
                return;
            }

            var message = new BitStream(packet);
            var sequenceNum = message.ReadUInt32();

            if (sequenceNum == 0xFFFFFFFF)
            {
                ProcessOutOfBand(from, packet);
                return;
            }

            for (int i = 0; i < _clients.Length; i++)
            {
                if (_clients[i] != null)
                {
                    if (_clients[i].Channel.Address == from)
                    {
                        if (_clients[i].Channel.ProcessPacket(packet))
                        {
                            ProcessClientMessage(_clients[i], message);
                        }
                    }
                }
            }
        }

        private static void ProcessClientMessage(ServerClient client, BitStream message)
        {
            client.LastMessageReceivedAt = _serverTime;
            client.LastAcknowledgedMessage = message.ReadUInt32();
            client.ReliableAcknowledged = message.ReadUInt32();

            // check if this command is from an older initialization state
            var stateID = message.ReadInt32();
            if (stateID != _stateID)
            {
                if (client.LastAcknowledgedMessage >= client.LastInitStateMessage)
                {
                    SendInitializationState(client);
                }

                return;
            }

            byte command = 0;

            try
            {
                do
                {
                    command = message.ReadByte();

                    switch (command)
                    {
                        case UserCommand.CommandType:
                            ProcessClientUserCommand(client, message);
                            break;
                        case 2: // TODO: magic number fix
                            ProcessClientReliableCommand(client, message);
                            break;
                    }
                } while (command != 0xFF);
            }
            catch (Exception e)
            {
                Log.Write(LogLevel.Warning, "Error while processing client message: " + e.ToString());
                DropClient(client, "Error while processing client message.");
            }
        }

        private static void ProcessClientReliableCommand(ServerClient client, BitStream message)
        {
            uint commandSequence = message.ReadUInt32();
            string command = message.ReadString();

            if (commandSequence <= client.LastExecutedReliableCommand)
            {
                return;
            }

            string[] args = Command.Tokenize(command);

            switch (args[0])
            {
                case "say":
                    if (args.Length < 2)
                    {
                        break;
                    }

                    string msg = string.Format("{0}: {1}", client.Name, string.Join(" ", args, 1, args.Length - 1));
                    Log.Write(LogLevel.Info, msg);
                    SendReliableCommand(null, "print \"" + msg + "\"");
                    break;
                case "disconnect":
                    DropClient(client, "Disconnected from server");
                    break;
            }

            client.LastReceivedReliableCommand = commandSequence;
            client.LastExecutedReliableCommand = commandSequence;
        }

        private static void ProcessClientUserCommand(ServerClient client, BitStream message)
        {
            var numCommands = message.ReadByte();
            var commands = new UserCommand[numCommands];
            UserCommand? lastCommand = null;

            for (int i = 0; i < numCommands; i++)
            {
                commands[i] = new UserCommand();
                commands[i].Deserialize(message, lastCommand);

                lastCommand = commands[i];
            }

            if (client.State != ServerClientState.Active && client.State != ServerClientState.Zombie)
            {
                client.InitializeEntity();
            }

            foreach (var command in commands)
            {
                client.ProcessCommand(command);
            }
        }

        private static Dictionary<NetAddress, int> _challenges = new Dictionary<NetAddress, int>();

        private static void ProcessOutOfBand(NetAddress from, byte[] packet)
        {
            var message = Encoding.UTF8.GetString(packet, 4, packet.Length - 4);
            var args = Command.Tokenize(message);

            if (args.Length == 0)
            {
                return;
            }

            switch (args[0])
            {
                case "getchallenge":
                    var rnd = new Random();
                    _challenges[from] = (int)(rnd.Next() ^ _serverTime);

                    NetManager.SendOOBPacket(NetChannelType.ServerToClient, from, "challengeResponse {0}", _challenges[from]);
                    break;
                case "connect":
                    if (args.Length != 3)
                    {
                        return;
                    }

                    int challenge = 0;
                    int.TryParse(args[1], out challenge);

                    if (!_challenges.ContainsKey(from) || _challenges[from] != challenge)
                    {
                        return;
                    }

                    _challenges.Remove(from);

                    int i;

                    for (i = 0; i < _clients.Length; i++)
                    {
                        if (_clients[i] == null)
                        {
                            _clients[i] = new ServerClient();
                            _clients[i].Channel = new NetChannel(NetChannelType.ServerToClient, from);
                            _clients[i].Name = args[2];
                            _clients[i].ClientNum = i;
                            _clients[i].ExternalIP = from;
                            _clients[i].LastMessageReceivedAt = _serverTime;
                            break;
                        }
                    }

                    SendReliableCommand(null, "print \"{0} joined\"", args[2]);
                    Log.Write(LogLevel.Info, "Client {0} connected (name {1}, from {2})", i, args[2], from);

                    NetManager.SendOOBPacket(NetChannelType.ServerToClient, from, "connectResponse {0} {1}", i, _mapName);
                    break;
            }
        }
    }
}
