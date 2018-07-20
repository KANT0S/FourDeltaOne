using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Linq;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public enum FontStyle
    {
        Console,
    }

    public class RenderBank<T> where T : struct, IVertexType
    {
        public RenderBank()
        {
            vertices = new T[4];
            indices = new short[6];
        }

        public T[] vertices;
        public short[] indices;
        public int numVertex;
        public int numIndex;

        public void InitPerFrame()
        {
            numVertex = 0;
            numIndex = 0;
        }

        public void ReserveSpace(int nvertices, int nindices)
        {
            if ((numVertex + nvertices) >= vertices.Length)
            {
                Array.Resize(ref vertices, (numVertex + nvertices));
            }

            if ((numIndex + nindices) >= indices.Length)
            {
                Array.Resize(ref indices, (numIndex + nindices));
            }
        }

        public IndexBuffer indexBuffer;
        public VertexBuffer vertexBuffer;

        public void UploadToGPU(GraphicsDevice device)
        {
            if (vertices.Length == 0 || indices.Length == 0)
            {
                return;
            }

            indexBuffer = new IndexBuffer(device, typeof(short), indices.Length, BufferUsage.WriteOnly);
            indexBuffer.SetData(indices);

            vertexBuffer = new VertexBuffer(device, ((IVertexType)vertices[0]).VertexDeclaration, vertices.Length, BufferUsage.WriteOnly);
            vertexBuffer.SetData(vertices);
        }

        public void Dispose()
        {
            if (indexBuffer != null)
            {
                indexBuffer.Dispose();
            }

            if (vertexBuffer != null)
            {
                vertexBuffer.Dispose();
            }
        }
    }

    public static class FontManager
    {
        private static Color _color;
        private static Font _font;

        private static Font _consoleFont;

        private static BasicEffect _fontEffect;

        private static GraphicsDevice _device;

        private static Dictionary<Texture2D, RenderBank<VertexPositionColorTexture>> _banks;

        public static void Initialize(GraphicsDevice device)
        {
            _banks = new Dictionary<Texture2D, RenderBank<VertexPositionColorTexture>>();

            _consoleFont = new ConsoleFont();
            _consoleFont.Load(device);

            _font = _consoleFont;
            _color = Color.White;

            _device = device;

            _fontEffect = new BasicEffect(device);
        }

        public static void InitPerFrame()
        {
            foreach (var bank in _banks)
            {
                bank.Value.InitPerFrame();
            }
        }

        private static RenderBank<VertexPositionColorTexture> GetBank(Font font)
        {
            var texture = font.Texture;

            if (!_banks.ContainsKey(texture))
            {
                _banks.Add(texture, new RenderBank<VertexPositionColorTexture>());
            }

            var bank = _banks[texture];
            return bank;
        }

        public static int MeasureString(string text)
        {
            var width = 0;

            for (int i = 0; i < text.Length; i++)
            {
                width += _font.GetWidth(text[i]);
            }

            return width;
        }

        public static void PrintString(Vector2 position, string text)
        {
            for (int i = 0; i < text.Length; i++)
            {
                char character = text[i];

                // handle colors
                if (character == '^' && i < (text.Length - 1))
                {
                    if (text[i + 1] >= '0' && text[i + 1] <= '9')
                    {
                        i++;

                        int color = text[i] - '0';

                        if (color < _colors.Length)
                        {
                            SetColor(_colors[color]);
                            continue;
                        }
                    }
                }

                // print the character
                PrintChar(position, character);

                // update the offset
                position.X += _font.GetWidth(character);
            }
        }

        private static void PrintChar(Vector2 position, char character)
        {
            var bank = GetBank(_font);
            bank.ReserveSpace(4, 6);

            var rectangle = _font.GetCoordinates(character);

            // don't know why I need to +1 the second coordinates
            var u1 = rectangle.X / (float)_font.Texture.Width;
            var v1 = rectangle.Y / (float)_font.Texture.Height;
            var u2 = rectangle.Right / (float)_font.Texture.Width;
            var v2 = rectangle.Bottom / (float)_font.Texture.Height;

            var x1 = position.X;
            var y1 = position.Y;
            var x2 = x1 + rectangle.Width;
            var y2 = y1 + rectangle.Height;

            x1 = (x1 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;
            x2 = (x2 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;

            y1 = (y1 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;
            y2 = (y2 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;

            int vertex = bank.numVertex;
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x1, y1, 0), _color, new Vector2(u1, v1));
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x2, y1, 0), _color, new Vector2(u2, v1));
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x2, y2, 0), _color, new Vector2(u2, v2));
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x1, y2, 0), _color, new Vector2(u1, v2));

            bank.indices[bank.numIndex++] = (short)(vertex + 0);
            bank.indices[bank.numIndex++] = (short)(vertex + 1);
            bank.indices[bank.numIndex++] = (short)(vertex + 2);
            bank.indices[bank.numIndex++] = (short)(vertex + 2);
            bank.indices[bank.numIndex++] = (short)(vertex + 3);
            bank.indices[bank.numIndex++] = (short)(vertex + 0);
        }

        public static void Render(GraphicsDevice device)
        {
            var state = device.DepthStencilState;
            device.DepthStencilState = DepthStencilState.None;

            var blendState = device.BlendState;
            device.BlendState = BlendState.AlphaBlend;

            _fontEffect.TextureEnabled = true;
            _fontEffect.VertexColorEnabled = true;

            foreach (var bank in _banks)
            {
                _fontEffect.Texture = bank.Key;
                _fontEffect.CurrentTechnique.Passes[0].Apply();

                device.SamplerStates[0] = SamplerState.PointClamp;

                if (bank.Value.numIndex == 0)
                {
                    continue;
                }

                device.DrawUserIndexedPrimitives(PrimitiveType.TriangleList, bank.Value.vertices, 0, bank.Value.numVertex, bank.Value.indices, 0, bank.Value.numIndex / 3);
            }

            device.BlendState = blendState;
            device.DepthStencilState = state;
        }

        private static Color[] _colors = new[]
        {
            Color.Black,
            Color.Red,
            Color.Green,
            Color.Yellow,
            Color.Blue,
            Color.LightBlue,
            Color.Purple,
            Color.White
        };
        
        public static void SetColor(Color color)
        {
            _color = color;
        }

        public static void SetFont(FontStyle font)
        {
            switch (font)
            {
                case FontStyle.Console:
                    _font = _consoleFont;
                    break;
            }
        }
    }

    public abstract class Font
    {
        public abstract string Name { get; }
        public abstract int LineHeight { get; }
        protected abstract string Filename { get; }

        public Texture2D Texture { get; set; }

        public void Load(GraphicsDevice device)
        {
            var stream = FileSystem.OpenCopy("Fonts/" + Filename);

            Texture = Texture2D.FromStream(device, stream);
        }

        public abstract Rectangle GetCoordinates(char character);
        public abstract int GetWidth(char character);
    }

    /*
    public class Font
    {
        public string Name { get; set; }
        public int LineHeight { get; set; }

        private Dictionary<char, FontCharacter> _characters;
        private FontPage[] _pages;

        public Font(string name)
        {
            _characters = new Dictionary<char, FontCharacter>();

            Load(name);
        }

        private void Load(string name)
        {
            Name = name;

            var fileStream = FileSystem.OpenRead("Fonts/" + name);
            var document = XDocument.Load(fileStream);

            LineHeight = int.Parse(document.Descendants("common").First().Attribute("base").Value);

            var pages = int.Parse(document.Descendants("common").First().Attribute("pages").Value);
            _pages = new FontPage[pages];

            foreach (var page in document.Descendants("page"))
            {
                var pageData = new FontPage(page);
                _pages[pageData.ID] = pageData;
            }

            foreach (var character in document.Descendants("char"))
            {
                var charData = new FontCharacter(character, this);
                _characters[charData.Character] = charData;
            }
        }

        public void Initialize(GraphicsDevice device)
        {
            foreach (var page in _pages)
            {
                page.Initialize(device);
            }
        }

        public FontPage GetPage(int page)
        {
            return _pages[page];
        }

        public FontCharacter GetCharacter(char character)
        {
            return (_characters.ContainsKey(character)) ? _characters[character] : _characters['?'];
        }

        public int MeasureString(string text)
        {
            int width = 0;

            foreach (var character in text)
            {
                width += GetCharacter(character).Width;
            }

            return width;
        }
    }

    public class FontCharacter
    {
        public char Character { get; set; }
        public int OffsetX { get; set; }
        public int OffsetY { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public Rectangle Rectangle { get; set; }
        public FontPage Page { get; set; }

        public FontCharacter(XElement element, Font font)
        {
            Character = char.ConvertFromUtf32(int.Parse(element.Attribute("id").Value))[0];
            OffsetX = int.Parse(element.Attribute("xoffset").Value);
            OffsetY = int.Parse(element.Attribute("yoffset").Value);
            Width = int.Parse(element.Attribute("xadvance").Value);
            Height = font.LineHeight;
            Rectangle = new Rectangle(int.Parse(element.Attribute("x").Value), int.Parse(element.Attribute("y").Value), int.Parse(element.Attribute("width").Value), int.Parse(element.Attribute("height").Value));
            Page = font.GetPage(int.Parse(element.Attribute("page").Value));
        }
    }

    public class FontPage
    {
        public int ID { get; set; }
        public string Filename { get; set; }
        public Texture2D Texture { get; set; }

        public FontPage(XElement element)
        {
            ID = int.Parse(element.Attribute("id").Value);
            Filename = element.Attribute("file").Value;
        }

        public void Initialize(GraphicsDevice device)
        {
            var fileStream = FileSystem.OpenRead("Fonts/" + Filename);
            var memoryStream = new MemoryStream();
            fileStream.CopyTo(memoryStream);
            fileStream.Close();

            Texture = Texture2D.FromStream(device, memoryStream);
        }
    }
    */
}
