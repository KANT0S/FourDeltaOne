using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public static class MapRenderer
    {
        private static AlphaTestEffect _renderEffect;
        private static BasicEffect _wireframeEffect;

        private static RasterizerState _normalState;
        private static RasterizerState _wireframeState;

        private static SamplerState _samplerState;

        private static VertexPositionColorTexture[] _vertices;

        public static void Initialize(GraphicsDevice device)
        {
            device.BlendState = BlendState.Opaque;
            device.DepthStencilState = DepthStencilState.Default;

            _normalState = new RasterizerState();
            _normalState.CullMode = CullMode.None;

            _samplerState = new SamplerState();
            _samplerState.Filter = TextureFilter.Point;

            device.SamplerStates[0] = _samplerState;

            device.RasterizerState = _normalState;

            _wireframeState = new RasterizerState();
            _wireframeState.FillMode = FillMode.WireFrame;
            _wireframeState.DepthBias = -0.000001f;
            _wireframeState.CullMode = CullMode.None;

            _renderEffect = new AlphaTestEffect(device);
            _renderEffect.VertexColorEnabled = true;
            //_renderEffect.TextureEnabled = true;
            _renderEffect.ReferenceAlpha = 200;
            _renderEffect.AlphaFunction = CompareFunction.GreaterEqual;
            //_renderEffect.Texture = StyleManager.GetTileTexture(1);

            _wireframeEffect = new BasicEffect(device);
            //_wireframeEffect.DiffuseColor = Color.White.ToVector3();

            // 20 blocks X, 20 blocks Y, 7 blocks Z, 5 sides per block, 6 verts per side
            _vertices = new VertexPositionColorTexture[20 * 20 * 7 * 5 * 6];
        }

        public static void Render(GraphicsDevice device)
        {
            RenderEffect(device, _renderEffect);

            device.RasterizerState = _wireframeState;
            //RenderEffect(device, _wireframeEffect);
            device.RasterizerState = _normalState;
        }

        private static void RenderEffect(GraphicsDevice device, Effect effect)
        {
            device.BlendState = BlendState.Opaque;
            device.DepthStencilState = DepthStencilState.Default;

            Camera.Apply((IEffectMatrices)effect);

            foreach (EffectPass pass in effect.CurrentTechnique.Passes)
            {
                pass.Apply();

                //RenderPass(device);
            }
        }

        private static uint _batches = 0;

        public static void RenderPass(GraphicsDevice device, Effect effect, Camera camera)
        {
            // set up the effect
            device.DepthStencilState = DepthStencilState.Default;
            device.RasterizerState = _normalState;

            effect.Parameters["World"].SetValue(Matrix.Identity);

            foreach (var cell in CellManager.ActivatedCells)
            {
                if (camera.BoundingFrustum.Intersects(cell.Bounds))
                {
                    foreach (var bankStuff in cell.Banks)
                    {
                        //var image = ImageManager.FindImage(bankStuff.Key);
                        var material = MaterialManager.FindMaterial(bankStuff.Key);
                        var bank = bankStuff.Value;

                        effect.Parameters["ColorMap"].SetValue(material.DiffuseImage.Texture);
                        effect.Techniques[0].Passes[0].Apply();

                        if (bank.vertexBuffer != null)
                        {
                            device.Indices = bank.indexBuffer;
                            device.SetVertexBuffer(bank.vertexBuffer);
                            device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, bank.vertices.Length, 0, bank.indices.Length / 3);
                        }
                        else
                        {
                            if (bank.vertices.Length >= 3)
                            {
                                device.DrawUserIndexedPrimitives(PrimitiveType.TriangleList, bank.vertices, 0, bank.vertices.Length, bank.indices, 0, bank.indices.Length / 3);
                            }
                        }
                    }
                }
            }
        }
    }
}
