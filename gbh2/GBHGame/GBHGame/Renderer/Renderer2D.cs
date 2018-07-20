using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public static class Renderer2D
    {
        private static GraphicsDevice _device;

        private static RenderBank<VertexPositionColorTexture> _rectangles;
        private static RenderBank<VertexPositionColorTexture> _rectanglesFilled;

        private static BasicEffect _rectangleEffect;

        public static void Initialize(GraphicsDevice device)
        {
            _device = device;

            _rectangles = new RenderBank<VertexPositionColorTexture>();
            _rectanglesFilled = new RenderBank<VertexPositionColorTexture>();

            _rectangleEffect = new BasicEffect(_device);

            FontManager.Initialize(device);
        }

        public static void InitPerFrame()
        {
            _rectangles.InitPerFrame();
            _rectanglesFilled.InitPerFrame();

            FontManager.InitPerFrame();
        }

        public static void FillRectangle(Color color, int x, int y, int w, int h)
        {
            var bank = _rectanglesFilled;

            var x1 = (float)x;
            var x2 = (float)x + w;

            var y1 = (float)y;
            var y2 = (float)y + h;

            x1 = (x1 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;
            x2 = (x2 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;

            y1 = (y1 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;
            y2 = (y2 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;

            bank.ReserveSpace(4, 6);

            int vertex = bank.numVertex;
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x1, y1, 0), color, Vector2.Zero);
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x2, y1, 0), color, Vector2.Zero);
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x2, y2, 0), color, Vector2.Zero);
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x1, y2, 0), color, Vector2.Zero);

            bank.indices[bank.numIndex++] = (short)(vertex + 0);
            bank.indices[bank.numIndex++] = (short)(vertex + 1);
            bank.indices[bank.numIndex++] = (short)(vertex + 2);
            bank.indices[bank.numIndex++] = (short)(vertex + 2);
            bank.indices[bank.numIndex++] = (short)(vertex + 3);
            bank.indices[bank.numIndex++] = (short)(vertex + 0);
        }

        public static void DrawRectangle(Color color, int x, int y, int w, int h)
        {
            var bank = _rectangles;

            var x1 = (float)x;
            var x2 = (float)x + w;

            var y1 = (float)y;
            var y2 = (float)y + h;

            x1 = (x1 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;
            x2 = (x2 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;

            y1 = (y1 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;
            y2 = (y2 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;

            bank.ReserveSpace(4, 8);

            int vertex = bank.numVertex;
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x1, y1, 0), color, Vector2.Zero);
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x2, y1, 0), color, Vector2.Zero);
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x2, y2, 0), color, Vector2.Zero);
            bank.vertices[bank.numVertex++] = new VertexPositionColorTexture(new Vector3(x1, y2, 0), color, Vector2.Zero);

            bank.indices[bank.numIndex++] = (short)(vertex + 0);
            bank.indices[bank.numIndex++] = (short)(vertex + 1);
            bank.indices[bank.numIndex++] = (short)(vertex + 1);
            bank.indices[bank.numIndex++] = (short)(vertex + 2);
            bank.indices[bank.numIndex++] = (short)(vertex + 2);
            bank.indices[bank.numIndex++] = (short)(vertex + 3);
            bank.indices[bank.numIndex++] = (short)(vertex + 3);
            bank.indices[bank.numIndex++] = (short)(vertex + 0);
        }

        public static void Render(GraphicsDevice device)
        {
            var state = device.DepthStencilState;
            device.DepthStencilState = DepthStencilState.None;

            var blendState = device.BlendState;
            device.BlendState = BlendState.AlphaBlend;

            var banks = new[] { _rectanglesFilled, _rectangles };

            _rectangleEffect.TextureEnabled = false;
            _rectangleEffect.VertexColorEnabled = true;

            var lines = false;

            foreach (var bank in banks)
            {
                _rectangleEffect.CurrentTechnique.Passes[0].Apply();

                device.SamplerStates[0] = SamplerState.PointClamp;

                if (bank.numIndex == 0)
                {
                    continue;
                }

                device.DrawUserIndexedPrimitives((lines) ? PrimitiveType.LineList : PrimitiveType.TriangleList, bank.vertices, 0, bank.numVertex, bank.indices, 0, bank.numIndex / ((lines) ? 2 : 3));

                lines = true; // draw lines for the second run
            }

            device.BlendState = blendState ?? BlendState.Opaque;
            device.DepthStencilState = state ?? DepthStencilState.Default;

            FontManager.Render(device);
        }

        public static void DrawTexturedQuad(GraphicsDevice device, Rectangle position, Texture2D texture)
        {
            var x1 = (float)position.Left;
            var x2 = (float)position.Right;

            var y1 = (float)position.Top;
            var y2 = (float)position.Bottom;

            x1 = (x1 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;
            x2 = (x2 / (_device.PresentationParameters.BackBufferWidth) * 2f) - 1;

            y1 = (y1 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;
            y2 = (y2 / (_device.PresentationParameters.BackBufferHeight) * -2f) + 1;

            VertexPositionColorTexture[] vertices = new VertexPositionColorTexture[]
                {
                    new VertexPositionColorTexture(
                        new Vector3(x1, y1, 0),
                        Color.White,
                        new Vector2(0, 0)
                    ),
                    new VertexPositionColorTexture(
                        new Vector3(x2, y1, 0),
                        Color.White,
                        new Vector2(1, 0)
                    ),
                    new VertexPositionColorTexture(
                        new Vector3(x2, y2, 0),
                        Color.White,
                        new Vector2(1, 1)
                    ),
                    new VertexPositionColorTexture(
                        new Vector3(x1, y2, 0),
                        Color.White,
                        new Vector2(0, 1)
                    )
                };

            short[] indices = new short[] { 0, 1, 2, 2, 3, 0 };

            var state = device.DepthStencilState;
            device.DepthStencilState = DepthStencilState.None;

            var blendState = device.BlendState;
            device.BlendState = BlendState.AlphaBlend;

            _rectangleEffect.TextureEnabled = true;
            _rectangleEffect.VertexColorEnabled = false;
            _rectangleEffect.Texture = texture;
            _rectangleEffect.CurrentTechnique.Passes[0].Apply();

            device.DrawUserIndexedPrimitives(PrimitiveType.TriangleList, vertices, 0, 4, indices, 0, 2);

            device.BlendState = blendState ?? BlendState.Opaque;
            device.DepthStencilState = state ?? DepthStencilState.Default;
        }
    }
}
