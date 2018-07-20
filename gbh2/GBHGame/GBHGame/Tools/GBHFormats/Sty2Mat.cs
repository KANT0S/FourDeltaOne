using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;

namespace GBH
{
    public static class Sty2Mat
    {
        public static void ToolMain(string[] args)
        {
            var filename = args[1];

            var outMaterialFile = "Data/Styles/" + filename + ".material";
            var outBitmap = "Data/Styles/" + filename + ".png";

            Log.Initialize(LogLevel.All);
            Log.AddListener(new ConsoleLogListener());

            ConVar.Initialize();
            FileSystem.Initialize();
            StyleManager.Load("Styles/" + filename + ".sty");

            // save the texture
            Vector2 uv;
            var bitmap = StyleManager.GetBitmap();
            bitmap.Save(outBitmap, System.Drawing.Imaging.ImageFormat.Png);

            // create a material file
            var writer = new StreamWriter(outMaterialFile);

            for (int i = 0; i < 992; i++)
            {
                StyleManager.GetTileTextureBordered(i, out uv);

                writer.WriteLine(string.Format("gbh/{0}/{1}", filename, i));
                writer.WriteLine("{");
                writer.WriteLine(string.Format("\ttexture Styles/{0}.png", filename));
                writer.WriteLine(string.Format("\tuv {0} {1} {2} {3}", uv.X, uv.Y, uv.X + 0.03125f, uv.Y + (0.03125f / 2)));
                writer.WriteLine("}");
                writer.WriteLine();
            }

            writer.Close();
        }
    }
}
