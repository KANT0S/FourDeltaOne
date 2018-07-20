using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public class EffectManager
    {
        public static Effect Load(string filename, GraphicsDevice device)
        {
            var file = FileSystem.OpenRead(string.Format("Effects/{0}_d3d", filename));
            var buffer = new byte[file.Length];
            file.Read(buffer, 0, buffer.Length);
            file.Close();

            var effect = new Effect(device, buffer);

            return effect;
        }
    }
}
