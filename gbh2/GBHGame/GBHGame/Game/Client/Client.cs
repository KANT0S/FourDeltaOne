using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public static class Client
    {
        public static int TimeBase
        {
            get
            {
                return _serverTimeDelta;
            }
        }

        public static ClientState State { get; set; }

        public enum ClientState
        {
            Idle,
            Challenging,
            Connecting,
            Connected,
            Ingame
        }

        private static NetChannel _serverChannel;
        private static uint _clientTime;
        private static int _serverTimeDelta;
        private static int _clientNum = 0;
        //private static Entity[] _entities;
        private static uint _lastMessageReceivedAt;
        private static int _svStateID;

        private static Snapshot[] _snapHistory;
        private static Snapshot _lastSnap;
        private static Snapshot _curSnap;
        private static Snapshot _snap;

        private static ConVar cl_maxpackets;
        private static ConVar cl_snapDelay;

        public static void Initialize()
        {
            cl_maxpackets = ConVar.Register("cl_maxpackets", 33, "", ConVarFlags.Archived);

            // FIXME: interpolation only works at server frame time
            cl_snapDelay = ConVar.Register("cl_snapDelay", 50, "Amount of time to delay the rendering by", ConVarFlags.Archived);

            _serverChannel = new NetChannel(NetChannelType.ClientToServer, NetAddress.Loopback);

            //_entities = new Entity[4096];
            InitializeConnection();

            State = ClientState.Idle;
        }

        private static void InitializeConnection()
        {
            _entityBases = new BitStream[4096];
            _snapHistory = new Snapshot[32];

            _reliableSequence = 0;
            _reliableAcknowledged = 0;
            _reliableCommands = new string[64];
        }

        public static void Process()
        {
            // update server time
            _clientTime += Game.FrameMsec;

            if (State != ClientState.Idle)
            {
                // create user command
                ClientInput.CreateCommand();

                // check for timeout
                CheckForTimeout();

                // check if a packet needs sending
                CheckCommandPacket();
                CheckConnectPacket();

                if (State == ClientState.Ingame)
                {
                    // set the rendering snapshot
                    SetRenderSnapshot();

                    // predict the local player's state
                    PredictPlayer();

                    // set the camera position
                    if (_snap != null)
                    {
                        if (_snap.LocalPlayer != null)
                        {
                            //var rotZ = MathHelper.ToRadians(_snap.LocalPlayer.Rotation.Z);

                            Camera.MainCamera.Position = new Vector3(_snap.LocalPlayer.Position.X, _snap.LocalPlayer.Position.Y, 10f + MathHelper.Max(_snap.LocalPlayer.Position.Z, 0f));
                            //Camera.MainCamera.Position += (new Vector3((float)Math.Sin(-rotZ), (float)Math.Cos(-rotZ), 0f) * -5f);
                            //Camera.MainCamera.ViewMatrix = Matrix.CreateLookAt(Camera.MainCamera.Position, new Vector3(_snap.LocalPlayer.Position.X, _snap.LocalPlayer.Position.Y, _snap.LocalPlayer.Position.Z), new Vector3(0f, 0f, 1f));
                        }
                    }
                }
            }
        }

        public static void Shutdown()
        {
            if (State == ClientState.Ingame)
            {
                SendReliableCommand("disconnect");
                SendCommandPacket();
                SendReliableCommand("disconnect");
                SendCommandPacket();
            }
        }

        private static void CheckForTimeout()
        {
            if (State == ClientState.Idle)
            {
                return;
            }

            if ((_clientTime - _lastMessageReceivedAt) > 30000)
            {
                Log.Write(LogLevel.Warning, "Server connection timed out.");
                State = ClientState.Idle;
            }
        }

        //static Vector3 _predictedErrorPos;
        //static uint _predictedErrorPosTime;
        static uint _oldTime;

        private static void PredictPlayer()
        {
            if (_snap == null || _snap.LocalPlayer == null)
            {
                return;
            }

            PlayerEntity originalPlayer = (PlayerEntity)_snap.LocalPlayer.Clone();

            var predictSnap = _curSnap;

            if (predictSnap == null || predictSnap.LocalPlayer == null)
            {
                predictSnap = _lastSnap;

                if (predictSnap == null || predictSnap.LocalPlayer == null)
                {
                    return;
                }
            }

            _snap.LocalPlayer = (PlayerEntity)predictSnap.LocalPlayer.Clone();

            int currentCommand = ClientInput.CommandID;

            for (int commandNum = (currentCommand - 63); commandNum < currentCommand; commandNum++)
            {
                UserCommand? command = ClientInput.GetCommand(commandNum);

                if (command == null)
                {
                    continue;
                }

                if (command.Value.ServerTime <= predictSnap.LocalPlayer.LastCommandTime)
                {
                    continue;
                }

                /*if (originalPlayer.LastCommandTime == _snap.LocalPlayer.LastCommandTime)
                {
                    if (originalPlayer.Position != _snap.LocalPlayer.Position)
                    {
                        Log.Write(LogLevel.Trace, "prediction error (position)");
                    }

                    if (originalPlayer.Rotation != _snap.LocalPlayer.Rotation)
                    {
                        Log.Write(LogLevel.Trace, "prediction error (rotation)");
                    }

                    Vector3 deltaPos = originalPlayer.Position - _snap.LocalPlayer.Position;
                    Vector3 deltaRot = originalPlayer.Rotation - _snap.LocalPlayer.Rotation;

                    if (deltaPos.LengthSquared() > 0.01f)
                    {
                        Log.Write(LogLevel.Trace, "prediction miss (position, len {0})", deltaPos.Length());

                        uint t = _clientTime - _predictedErrorPosTime;
                        float f = (100f - t) / 100f;

                        if (f < 0)
                        {
                            f = 0;
                        }

                        _predictedErrorPos *= f;
                        _predictedErrorPos += deltaPos;
                        _predictedErrorPosTime = _oldTime;
                    }
                }*/

                PlayerMovement.Process((PlayerEntity)_snap.LocalPlayer, command.Value, Server.PhysicsWorld);
            }

            /*
            // correct for errors accumulated above
            {
                uint t = _clientTime - _predictedErrorPosTime;
                float f = (100f - t) / 100f;

                if (f > 0 && f < 1)
                {
                    _snap.LocalPlayer.Position *= f;
                    _snap.LocalPlayer.Position += _predictedErrorPos;
                }
                else
                {
                    _predictedErrorPosTime = 0;
                }
            }
            */

            _oldTime = _clientTime;
        }

        private static void SetRenderSnapshot()
        {
            if (_snapHistory[0] == null)
            {
                return;
            }

            if (_curSnap != null)
            {
                _snap = Snapshot.InterpolateBetween(_lastSnap, _curSnap, (uint)(_clientTime - cl_snapDelay.GetValue<int>()));
            }
        }

        public static void RenderEntities(GraphicsDevice device, Effect effect, Camera camera)
        {
            if (_snap == null)
            {
                return;
            }

            for (int i = 0; i < _snap.Entities.Length; i++)
            {
                if (_snap.Entities[i] != null)
                {
                    var refEnt = _snap.Entities[i].GetRenderEntity();
                    refEnt.Render(device, effect, camera);
                }
            }
        }

        private static int _lastConnectPacketTime;
        private static NetAddress _currentServer;
        private static int _challenge;

        public static void Connect_f(string[] args)
        {
            if (ConVar.GetValue<string>("nicknamee") == ("")) // baddd
            {
                Log.Write(LogLevel.Info, "You haven't set a nickname, set one now:)");
                return;
            }
            if (args.Length != 2)
            {
                Log.Write(LogLevel.Info, "usage: connect [ip]");
                return;
            }

            if (args[1] != "localhost")
            {
                ConVar.SetValue<bool>("sv_running", false);
            }

            var server = NetAddress.Resolve(args[1]);

            if (_currentServer != server)
            {
                _currentServer = server;

                State = ClientState.Challenging;

                _lastMessageReceivedAt = _clientTime;
                _lastConnectPacketTime = -999999;
            }
        }

        private static void CheckConnectPacket()
        {
            switch (State)
            {
                case ClientState.Challenging:
                    if ((_clientTime - _lastConnectPacketTime) > 2000)
                    {
                        NetManager.SendOOBPacket(NetChannelType.ClientToServer, _currentServer, "getchallenge");
                        _lastConnectPacketTime = (int)_clientTime;
                    }
                    break;
                case ClientState.Connecting:
                    if ((_clientTime - _lastConnectPacketTime) > 2000)
                    {
                        // TODO: use a proper nickname here
                        // TODO: make nickname changes *dynamic* :o
                        var nicknameToUse = ConVar.GetValue<string>("nicknamee");
                        NetManager.SendOOBPacket(NetChannelType.ClientToServer, _currentServer, "connect {0} \"{1}\"", _challenge, nicknameToUse);
                        _lastConnectPacketTime = (int)_clientTime;
                    }
                    break;
            } 
        }

        private static uint _timeSinceLastPacket;
        private static uint _commandSequence;

        private static void CheckCommandPacket()
        {
            if (State != ClientState.Connected && State != ClientState.Ingame)
            {
                return;
            }

            _timeSinceLastPacket += Game.FrameMsec;

            if (_timeSinceLastPacket > (1000 / cl_maxpackets.GetValue<int>()))
            {
                SendCommandPacket();
                _timeSinceLastPacket = 0;
            }
        }

        private static string[] _reliableCommands;
        private static int _reliableAcknowledged;
        private static int _reliableSequence;
        private static uint _lastExecutedReliable;

        public static void SendReliableCommand(string command)
        {
            int unacknowledged = _reliableSequence - _reliableAcknowledged;

            if (unacknowledged > _reliableCommands.Length)
            {
                throw new InvalidOperationException("client command overflow");
            }

            _reliableSequence++;
            _reliableCommands[_reliableSequence & (_reliableCommands.Length - 1)] = command;
        }

        private static uint _lastReliableMessage;
        private static uint _lastServerMessage;

        private static void SendCommandPacket()
        {
            var message = new BitStream();
            UserCommand? command, lastCommand = null;

            // last received server message
            message.WriteUInt32(_lastServerMessage);

            // last received reliable message
            message.WriteUInt32(_lastReliableMessage);

            // current server state ID
            message.WriteInt32(_svStateID);

            // reliable messages
            for (int i = _reliableAcknowledged + 1; i <= _reliableSequence; i++)
            {
                // header byte
                message.WriteByte(2);

                // command number
                message.WriteInt32(i);

                // command
                message.WriteString(_reliableCommands[i & (_reliableCommands.Length - 1)]);
            }
            
            // header byte
            message.WriteByte(UserCommand.CommandType);

            // command count
            message.WriteByte((byte)ClientInput.NumCommands);

            while ((command = ClientInput.GetCommand()) != null)
            {
                // write a command
                command.Value.Serialize(message, lastCommand);

                // set the new command as base
                lastCommand = command;
            }

            // end of command
            message.WriteByte(0xFF);

            // now send the command to the server
            _serverChannel.SendMessage(message);
            
            _commandSequence++;
        }

        private static BitStream[] _entityBases;

        private static void ProcessSnapshot(BitStream message)
        {
            if (State == ClientState.Connected)
            {
                State = ClientState.Ingame;
            }

            var snapshot = new Snapshot();
            snapshot.Sequence = _serverChannel.SequenceIn;
            snapshot.ServerTime = message.ReadUInt32();

            _serverTimeDelta = (int)((int)snapshot.ServerTime - (int)_clientTime);

            _reliableAcknowledged = message.ReadInt32();

            int entityNumber = message.ReadInt32(12);

            while (entityNumber != 4095)
            {
                var entityBase = (_entityBases[entityNumber] != null) ? _entityBases[entityNumber] : null;
                var deltaMessage = new DeltaBitStream(entityBase, message);

                var spawnKey = deltaMessage.ReadInt32(20);
                var typeCode = deltaMessage.ReadInt32(4);

                snapshot.Entities[entityNumber] = Entity.Create(typeCode);
                snapshot.Entities[entityNumber].Deserialize(deltaMessage);

                _entityBases[entityNumber] = deltaMessage.NewBase;

                entityNumber = message.ReadInt32(12);
            }

            // push snapshot history array back by one
            for (int i = (_snapHistory.Length - 2); i >= 0; i--)
            {
                _snapHistory[i + 1] = _snapHistory[i];
            }

            // and set the first snapshot to the current one
            _snapHistory[0] = snapshot;

            // define current and previous snapshots
            int delayMsec = cl_snapDelay.GetValue<int>();
            int numSnaps = 0;

            for (int i = 0; i < _snapHistory.Length; i++)
            {
                if (_snapHistory[i] != null)
                {
                    if (_snapHistory[i].ServerTime < (_snapHistory[0].ServerTime - delayMsec))
                    {
                        numSnaps = i;
                        break;
                    }
                }
            }
                         
            int snapDelay = Math.Max(0, Math.Min(_snapHistory.Length - 2, numSnaps));

            _lastSnap = _snapHistory[snapDelay + 1];
            _curSnap = _snapHistory[snapDelay];

            if (_curSnap == null)
            {
                _curSnap = _snapHistory[0];
            }

            _curSnap.ClientTime = _clientTime;
        }

        private static void ProcessReliableCommand(BitStream message)
        {
            uint commandSequence = message.ReadUInt32();
            string command = message.ReadString();

            if (commandSequence <= _lastExecutedReliable)
            {
                return;
            }

            string[] args = Command.Tokenize(command);

            switch (args[0])
            {
                case "print":
                    if (args.Length < 2)
                    {
                        break;
                    }

                    Log.Write(LogLevel.Info, "{0}", string.Join(" ", args, 1, args.Length - 1));
                    break;
                case "disconnect":
                    State = ClientState.Idle;
                    Log.Write(LogLevel.Error, "Disconnected from server: {0}", (args.Length == 2) ? args[1] : "no message");
                    break;
            }

            _lastExecutedReliable = commandSequence;
            _lastReliableMessage = commandSequence;
        }

        private static void ProcessInitializationState(BitStream message)
        {
            _svStateID = message.ReadInt32();

            // load the server's current map
            var mapName = message.ReadString();

            MapManager.Load(string.Format("Maps/{0}.gmp", mapName));
        }

        private static void ProcessServerMessage(BitStream message)
        {
            _lastMessageReceivedAt = _clientTime;
            _lastServerMessage = message.ReadUInt32();

            byte command = 0;

#if !DEBUG
            try
            {
#endif
                do
                {
                    command = message.ReadByte();

                    switch (command)
                    {
                        case Server.SnapshotNumber:
                            ProcessSnapshot(message);
                            break;
                        case Server.ReliableCommandNumber:
                            ProcessReliableCommand(message);
                            break;
                        case Server.InitStateNumber:
                            ProcessInitializationState(message);
                            break;
                    }
                } while (command != 0xFF);
#if !DEBUG
            }
            catch (Exception e)
            {
                Log.Write(LogLevel.Warning, "Error while processing client message: " + e.ToString());
                // TODO: disconnect client
            }
#endif
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

            if (from != _serverChannel.Address)
            {
                return;
            }

            if (_serverChannel.ProcessPacket(packet))
            {
                message = new BitStream(packet); // reset the bitstream
                ProcessServerMessage(message);
            }
        }

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
                case "challengeResponse":
                    if (State != ClientState.Challenging)
                    {
                        return;
                    }

                    _lastConnectPacketTime = -999999;
                    _lastMessageReceivedAt = _clientTime;

                    _challenge = int.Parse(args[1]);
                    State = ClientState.Connecting;
                    break;
                case "connectResponse":
                    if (State != ClientState.Connecting)
                    {
                        return;
                    }

                    _clientNum = int.Parse(args[1]);

                    if (!ConVar.GetValue<bool>("sv_running"))
                    {
                        Server.CreatePhysicsWorld();
                        MapManager.Load(string.Format("Maps/{0}.gmp", args[2]));
                    }

                    _lastConnectPacketTime = -999999;
                    _lastMessageReceivedAt = _clientTime;

                    _serverChannel = new NetChannel(NetChannelType.ClientToServer, from);
                    InitializeConnection();

                    State = ClientState.Connected;
                    break;
            }
        }

        public static int ClientNum
        {
            get
            {
                return _clientNum;
            }
        }

        public static void ClearConsole()
        {
            // clearing the external and 'internal' console
            Log.Write(LogLevel.Info, "Clearing the console");
            Console.Clear();
            ConsoleRenderer._screenBuffer.Clear();
        }
    }
}
