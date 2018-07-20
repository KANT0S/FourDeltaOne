using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace GBH
{
    public static class ExportMap
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
            
            mapWriter.Close();

            // export lights
            mapFile = File.Open(string.Format("{0}/lights.lua", outDir), FileMode.Create, FileAccess.Write);
            mapWriter = new StreamWriter(mapFile);

            mapWriter.WriteLine("local l");
            mapWriter.WriteLine("gbh_lights = {}");

            foreach (var light in MapManager.Lights)
            {
                mapWriter.WriteLine(string.Format("l = light_from_table({{ pos = vector3({0}, {1}, {2}), diff = vector3({3}, {4}, {5}), range = {6}, coronaColour = V_ZERO }})", light.Position.X * 4.5f, light.Position.Y * -4.5f, light.Position.Z * 4.5f, light.Color.R / 255.0f, light.Color.G / 255.0f, light.Color.B / 255.0f, light.Radius * 4.5f));
                mapWriter.WriteLine("l.enabled = true");
                mapWriter.WriteLine("table.insert(gbh_lights, l)");
                mapWriter.WriteLine();
            }

            mapWriter.Close();
        }
    }
}
