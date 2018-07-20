using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;

namespace GBH
{
    public static class DebugCamera
    {
        private static bool _leftKey;
        private static bool _rightKey;
        private static bool _upKey;
        private static bool _downKey;
        private static bool _inKey;
        private static bool _outKey;

        public static void Process()
        {
            Vector3 delta = new Vector3();

            if (_rightKey)
            {
                delta += new Vector3(3.0f * Game.DeltaTime, 0, 0);
            }

            if (_leftKey)
            {
                delta += new Vector3(-3.0f * Game.DeltaTime, 0, 0);
            }
            if (_upKey)
            {
                delta += new Vector3(0, 3.0f * Game.DeltaTime, 0);
            }

            if (_downKey)
            {
                delta += new Vector3(0, -3.0f * Game.DeltaTime, 0);
            }

            if (_inKey)
            {
                delta += new Vector3(0, 0, -3.0f * Game.DeltaTime);
            }

            if (_outKey)
            {
                delta += new Vector3(0, 0, 3.0f * Game.DeltaTime);
            }

            Camera.MainCamera.Position += delta;

            if (Camera.MainCamera.Position.X > 255)
            {
                Camera.MainCamera.Position = new Vector3(4.0f, 40.0f, 12.0f);
            }

            FontManager.SetColor(Color.White);
            FontManager.SetFont(FontStyle.Console);
            FontManager.PrintString(new Vector2(5f, 5f), string.Format("Position: ({0}, {1})", Camera.MainCamera.Position.X, Camera.MainCamera.Position.Y));
            FontManager.PrintString(new Vector2(5f, 720f - 12f - 5f), string.Format("Client state: {0}", Client.State));
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

            if (key == Keys.PageUp)
            {
                _inKey = down;
            }

            if (key == Keys.PageDown)
            {
                _outKey = down;
            }
        }
    }
}
