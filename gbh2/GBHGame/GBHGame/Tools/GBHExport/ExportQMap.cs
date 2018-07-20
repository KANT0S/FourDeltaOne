using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace GBH
{
    public static class ExportQMap
    {
        public static void ExportMain(string[] args)
        {
            var filename = args[1];

            var outDir = "Export/QMaps/" + filename + ".gbh2map";

            Log.Initialize(LogLevel.All);
            Log.AddListener(new ConsoleLogListener());

            ConVar.Initialize();
            FileSystem.Initialize();
            MapManager.Load("Maps/" + filename + ".gmp");

            // export cell files
            var cellWidth = 32;
            var cellHeight = 32;

            var blockScale = 4.5f;

            var mapFile = File.Open(string.Format("{0}/cells.lua", outDir), FileMode.Create, FileAccess.Write);
            var mapWriter = new StreamWriter(mapFile);

            for (int x = 0; x < (256 / cellWidth); x++)
            {
                for (int y = 0; y < (256 / cellHeight); y++)
                {
                    var cellName = string.Format("{0}_{1}_{2}", filename, x, y);
                    var cellFile = File.OpenWrite(string.Format("{0}/cells/{1}.cell", outDir, cellName));
                    var cellWriter = new BinaryWriter(cellFile);

                    for (int c = 0; c < 7; c++)
                    {
                        for (int b = (y * cellHeight); b < (y * cellHeight) + cellHeight; b++)
                        {
                            for (int a = (x * cellWidth); a < (x * cellWidth) + cellWidth; a++)
                            {
                                var block = MapManager.GetBlock(a, b, c);

                                cellWriter.Write(block.Left.Value);
                                cellWriter.Write(block.Right.Value);
                                cellWriter.Write(block.Top.Value);
                                cellWriter.Write(block.Bottom.Value);
                                cellWriter.Write(block.Lid.Value);

                                cellWriter.Write(block.SlopeType.Value);
                            }

                        }
                    }

                    cellFile.Close();

                    mapWriter.WriteLine(string.Format("class \"cells/{0}\" (GBHClass) {{ castShadows = false, renderingDistance = {1} }}", cellName, 300));
                    mapWriter.WriteLine(string.Format("object \"cells/{0}\" ({1}, {2}, {3}) {{}}", cellName, (x * cellWidth * blockScale), -(y * cellWidth * blockScale), 0));
                }
            }
        }
    }
}
