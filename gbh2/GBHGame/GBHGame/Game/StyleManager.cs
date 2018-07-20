using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using System.Drawing;
using System.Runtime.InteropServices;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public class StyleManager
    {
        private static ushort[] _paletteIndex;
        private static uint[] _palette;
        private static byte[] _tile;

        //private static Texture2D[] _tileTextures;
        private static Texture2D _tileTexture;

        public static void Load(string filename)
        {
            Stream styleFile = FileSystem.OpenCopy(filename);
            BinaryReader reader = new BinaryReader(styleFile);

            GBMPHeader header = new GBMPHeader();
            header.Read(reader);

            if (header.Magic != 0x54534247 || header.Version != 700)
            {
                throw new InvalidOperationException("this isn't a GBST v.700");
            }

            while (!styleFile.EndOfStream())
            {
                GBMPChunk chunk = new GBMPChunk();
                chunk.Read(reader);

                Log.Write(LogLevel.Debug, "found a chunk of type " + Encoding.ASCII.GetString(BitConverter.GetBytes(chunk.Type)));

                var oldPosition = styleFile.Position;

                if (chunk.Type == 0x584C4150) // PALX
                {
                    LoadPaletteIndex(styleFile);
                }
                else if (chunk.Type == 0x4C415050) // PPAL
                {
                    LoadPalettes(styleFile, chunk.Size);
                }
                else if (chunk.Type == 0x454C4954) // TILE
                {
                    LoadTiles(styleFile);
                }
                
                styleFile.Position = oldPosition + chunk.Size;
            }

            styleFile.Close();
        }

        private static void LoadPaletteIndex(Stream styleFile)
        {
            BinaryReader reader = new BinaryReader(styleFile);
            _paletteIndex = new ushort[16384];

            for (int i = 0; i < _paletteIndex.Length; i++)
            {
                _paletteIndex[i] = reader.ReadUInt16();
            }
        }

        private static void LoadPalettes(Stream styleFile, uint length)
        {
            BinaryReader reader = new BinaryReader(styleFile);
            _palette = new uint[length / 4];

            for (int i = 0; i < _palette.Length; i++)
            {
                _palette[i] = reader.ReadUInt32();
            }
        }

        private static void LoadTiles(Stream styleFile)
        {
            BinaryReader reader = new BinaryReader(styleFile);
            _tile = reader.ReadBytes(64 * 64 * 992);
        }

        public static void CreateTextures(GraphicsDevice device)
        {
            CreateTileTextures(device);

            _tile = null;
        }

        public static Bitmap GetBitmap()
        {
            uint[] colors = new uint[2048 * 4096];

            for (int i = 0; i < 992; i++)
            {
                AddToTileTextureBordered(colors, i);
            }

            // swap the image channels
            for (int i = 0; i < colors.Length; i++)
            {
                colors[i] = (colors[i] & 0x000000ff) << 16 | (colors[i] & 0x0000FF00) | (colors[i] & 0x00FF0000) >> 16 | (colors[i] & 0xFF000000);
            }  
            
            GCHandle handle = GCHandle.Alloc(colors, GCHandleType.Pinned);

            return new Bitmap(2048, 4096, (2048 * 4), System.Drawing.Imaging.PixelFormat.Format32bppArgb, handle.AddrOfPinnedObject());
        }

        private static void CreateTileTextures(GraphicsDevice device)
        {
            uint[] colors = new uint[2048 * 2048];

            for (int i = 0; i < 992; i++)
            {
                //_tileTextures[i] = CreateTileTexture(device, i);
                AddToTileTexture(colors, i);
            }

            _tileTexture = new Texture2D(device, 2048, 2048, false, SurfaceFormat.Color);
            _tileTexture.SetData(colors);
            //_tileTexture.SaveAsPng(File.OpenWrite(@"blah.png"), 2048, 2048);
        }

        private static void AddToTileTexture(uint[] colors, int tileIndex)
        {
            int startX = (tileIndex % 32) * 64;
            int startY = (tileIndex / 32) * 64;

            int vpal = _paletteIndex[tileIndex];

            for (int x = 0; x < 64; x++)
            {
                for (int y = 0; y < 64; y++)
                {
                    int tileColor = _tile[(y + (tileIndex / 4) * 64) * 256 + (x + (tileIndex % 4) * 64)];
                    int paletteId = (vpal / 64) * 256 * 64 + (vpal % 64) + tileColor * 64;
                    uint finalColor = (uint)((_palette[paletteId] & 0xFFFFFF) | (0xFF << 24));

                    byte r = (byte)(finalColor & 0xFF);
                    byte g = (byte)((finalColor & 0xFF00) >> 8);
                    byte b = (byte)((finalColor & 0xFF0000) >> 16);
                    byte a = (byte)((finalColor & 0xFF000000) >> 24);

                    finalColor = (uint)((r << 16) | (g << 8) | (b) | (a << 24));

                    // transparent cases
                    if (tileColor == 0)
                    {
                        finalColor = 0;
                    }

                    colors[((startY + y) * 2048) + (startX + x)] = finalColor;
                }
            }
        }

        private static void AddToTileTextureBordered(uint[] colors, int tileIndex)
        {
            int startX = 1 + (tileIndex % 31) * 64 + ((tileIndex % 31) * 2);
            int startY = 1 + (tileIndex / 31) * 64 + ((tileIndex / 31) * 2);

            int vpal = _paletteIndex[tileIndex];

            for (int x = 0; x < 64; x++)
            {
                for (int y = 0; y < 64; y++)
                {
                    int tileColor = _tile[(y + (tileIndex / 4) * 64) * 256 + (x + (tileIndex % 4) * 64)];
                    int paletteId = (vpal / 64) * 256 * 64 + (vpal % 64) + tileColor * 64;
                    uint finalColor = (uint)((_palette[paletteId] & 0xFFFFFF) | (0xFF << 24));

                    byte r = (byte)(finalColor & 0xFF);
                    byte g = (byte)((finalColor & 0xFF00) >> 8);
                    byte b = (byte)((finalColor & 0xFF0000) >> 16);
                    byte a = (byte)((finalColor & 0xFF000000) >> 24);

                    finalColor = (uint)((r << 16) | (g << 8) | (b) | (a << 24));

                    // transparent cases
                    if (tileColor == 0)
                    {
                        finalColor = 0;
                    }

                    colors[((startY + y) * 2048) + (startX + x)] = finalColor;
                }
            }

            for (int x = 0; x < 64; x++)
            {
                colors[((startY + 64) * 2048) + (startX + x)] = colors[((startY + 63) * 2048) + (startX + x)];
                colors[((startY - 1) * 2048) + (startX + x)] = colors[((startY) * 2048) + (startX + x)];
            }

            for (int y = -1; y < 65; y++)
            {
                colors[((startY + y) * 2048) + (startX - 1)] = colors[((startY + y) * 2048) + (startX)];
                colors[((startY + y) * 2048) + (startX + 64)] = colors[((startY + y) * 2048) + (startX + 63)];
            }
        }

        /*
        private static Texture2D CreateTileTexture(GraphicsDevice device, int tileIndex)
        {
            Texture2D texture = new Texture2D(device, 64, 64, false, SurfaceFormat.Color);
            uint[] colors = new uint[64 * 64];

            int vpal = _paletteIndex[tileIndex];

            for (int x = 0; x < 64; x++)
            {
                for (int y = 0; y < 64; y++)
                {
                    int tileColor = _tile[(y + (tileIndex / 4) * 64) * 256 + (x + (tileIndex % 4) * 64)];
                    int paletteId = (vpal / 64) * 256 * 64 + (vpal % 64) + tileColor * 64;
                    uint finalColor = (uint)((_palette[paletteId] & 0xFFFFFF) | (0xFF << 24));

                    byte r = (byte)(finalColor & 0xFF);
                    byte g = (byte)((finalColor & 0xFF00) >> 8);
                    byte b = (byte)((finalColor & 0xFF0000) >> 16);
                    byte a = (byte)((finalColor & 0xFF000000) >> 24);

                    finalColor = (uint)((r << 16) | (g << 8) | (b) | (a << 24));

                    // transparent cases
                    if (tileColor == 0)
                    {
                        finalColor = 0;
                    }

                    colors[(y * 64) + x] = finalColor;
                }
            }

            texture.SetData(colors);

            return texture;
        }

        public static Texture2D GetTileTexture(int tileIndex)
        {
            return _tileTextures[tileIndex];
        }
        */

        public static Texture2D GetTileTexture(int tileIndex, out Vector2 uv)
        {
            uv = new Vector2((tileIndex % 32) * 0.03125f, (tileIndex / 32) * 0.03125f);

            return _tileTexture;
        }

        public static Texture2D GetTileTextureBordered(int tileIndex, out Vector2 uv)
        {
            int startX = 1 + (tileIndex % 31) * 64 + ((tileIndex % 31) * 2);
            int startY = 1 + (tileIndex / 31) * 64 + ((tileIndex / 31) * 2);

            uv = new Vector2(startX * (1f / 2048f), startY * (1f / 4096f));

            return _tileTexture;
        }
    }
}
