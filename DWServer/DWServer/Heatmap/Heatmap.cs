using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.NetworkInformation;
using System.Threading;
using Ionic.Zlib;

namespace DWServer
{
    public class Heatmap
    {
        public static void StartThread()
        {
            Thread HeatmapThread = new Thread(Generate);
            HeatmapThread.Start();
        }

        struct Coordinates
        {
            public Coordinates(double X, double Y) : this()
            {
                this.x = (int)X;
                this.y = (int)Y;
            }

            public int x { get; set; }
            public int y { get; set; }
        }

        private static List<string> GetPlayers()
        {
            IPGlobalProperties ipProperties = IPGlobalProperties.GetIPGlobalProperties();

            IPEndPoint[] endPoints = ipProperties.GetActiveTcpListeners();
            TcpConnectionInformation[] tcpConnections = ipProperties.GetActiveTcpConnections();

            List<string> Players = new List<string>();

            foreach (TcpConnectionInformation info in tcpConnections)
                if (info.LocalEndPoint.Port == 3078)
                    Players.Add(info.RemoteEndPoint.Address.ToString());

            return Players;
        }

        private static Coordinates GetRelativeCoords(double lat, double lon, int width, int height)
        {
            double x = ((lon + 180) * ((double)width / 360));
            double y = (((lat * -1) + 90) * ((double)height / 180));
            return new Coordinates(x, y);
        }

        private static List<Coordinates> GetPlayerCoordinates()
        {
            List<string> PlayerAddresses = GetPlayers();
            List<Coordinates> PlayerCoordinates = new List<Coordinates>();

            foreach (string Address in PlayerAddresses)
            {
                Location PlayerLocation = DWDML._geoIP.getLocation(Address);

                if (PlayerLocation != null)
                {
                    PlayerCoordinates.Add(GetRelativeCoords(PlayerLocation.latitude, PlayerLocation.longitude, 256, 256));
                }
            }

            return PlayerCoordinates;
        }

        private static void Generate()
        {
            while (true)
            {
                try
                {
                    Log.Debug("Generating Heatmap");

                    List<Coordinates> PlayerCoordinates = GetPlayerCoordinates();

                    MemoryStream mStream = new MemoryStream();
                    BinaryWriter Writer = new BinaryWriter(mStream);

                    for (int i = 255; i >= 0; i--)
                    {
                        for (int j = 0; j < 256; j++)
                        {
                            if (PlayerCoordinates.Contains(new Coordinates(j, i)))
                                Writer.Write((byte)0x01);
                            else
                                Writer.Write((byte)0x00);
                        }
                    }

                    byte[] Compressed = ZlibStream.CompressBuffer(mStream.ToArray());

                    FileStream HeatmapFile = File.Create("data/pub/heatmap.raw", Compressed.Length);
                    HeatmapFile.Write(Compressed, 0, Compressed.Length);
                    HeatmapFile.Close();

                    DWStorage.FlushPublisherFiles();
                }
                catch (Exception ex)
                {
                    Log.Error(String.Format("Heatmap Generation Failed: {0}", ex));
                }

                Thread.Sleep(300000);
            }
        }
    }
}
