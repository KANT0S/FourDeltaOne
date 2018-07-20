using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace GBH
{
    public static class ExportStyle
    {
        public static void ExportMain(string[] args)
        {
            var filename = args[1];

            var outDir = "Export/Maps/" + filename + "/";
            Directory.CreateDirectory(outDir);
            Directory.CreateDirectory(outDir + "cells");

            Log.Initialize(LogLevel.All);
            Log.AddListener(new ConsoleLogListener());

            ConVar.Initialize();
            FileSystem.Initialize();
            StyleManager.Load("Styles/" + filename + ".sty");

            var bitmap = StyleManager.GetBitmap();
            bitmap.Save(string.Format("{0}/{1}.png", outDir, filename), System.Drawing.Imaging.ImageFormat.Png);
        }
    }
}
