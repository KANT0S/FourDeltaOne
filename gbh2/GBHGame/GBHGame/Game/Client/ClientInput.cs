using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework.Input;

namespace GBH
{
    public static class ClientInput
    {
        private const int UCMD_BACKUP = 64;
        private static int _commandNum;
        private static Queue<UserCommand> _userCommands = new Queue<UserCommand>();
        private static UserCommand[] _userCommandBackups = new UserCommand[UCMD_BACKUP];
        private static bool _leftKey;
        private static bool _rightKey;
        private static bool _upKey;
        private static bool _downKey;
        
        public static void CreateCommand()
        {
            var command = new UserCommand();
            command.ServerTime = (uint)(Game.Time + Client.TimeBase);

            if (_upKey)
            {
                command.Buttons |= ClientButtons.Forward;
            }

            if (_downKey)
            {
                command.Buttons |= ClientButtons.Backward;
            }

            if (_leftKey)
            {
                command.Buttons |= ClientButtons.RotateLeft;
            }

            if (_rightKey)
            {
                command.Buttons |= ClientButtons.RotateRight;
            }

            _userCommands.Enqueue(command);

            _commandNum++;
            _userCommandBackups[_commandNum & (UCMD_BACKUP - 1)] = command;
        }

        public static int CommandID
        {
            get
            {
                return _commandNum;
            }
        }

        public static UserCommand? GetCommand()
        {
            if (_userCommands.Count == 0)
            {
                return null;
            }

            return _userCommands.Dequeue();
        }

        public static UserCommand? GetCommand(int index)
        {
            if (index > _commandNum)
            {
                throw new ArgumentException("command index too high");
            }

            if (index <= (_commandNum - UCMD_BACKUP))
            {
                return null;
            }

            return _userCommandBackups[index & (UCMD_BACKUP - 1)];
        }

        public static int NumCommands
        {
            get
            {
                return _userCommands.Count;
            }
        }

        public static void HandleKey(Keys key, bool down)
        {
            if (key == Keys.Left)
            {
                _leftKey = down;
            }

            if (key == Keys.Right)
            {
                _rightKey = down;
            }

            if (key == Keys.Up)
            {
                _upKey = down;
            }

            if (key == Keys.Down)
            {
                _downKey = down;
            }
        }
    }
}
