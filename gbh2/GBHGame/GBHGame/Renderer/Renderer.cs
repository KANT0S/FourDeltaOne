using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public static class DeferredRenderer
    {
        private static RenderTarget2D _colorRT;
        private static RenderTarget2D _depthRT;
        private static RenderTarget2D _normalRT;
        private static RenderTarget2D _lightRT;
        private static RenderTarget2D _shadowRT;
        private static RenderTarget2D _hdrRT;
        private static RenderTarget2D _hdrBloomRT;

        public static RenderTarget2D DepthRT
        {
            get
            {
                return _depthRT;
            }
        }

        private static Effect _baseEffect;
        private static Effect _compositeEffect;
        private static Effect _bloomEffect;
        private static Effect _lightnessEffect;

        // lightness RTs
        private static RenderTarget2D _avgLightRT;
        private static RenderTarget2D[] _lightDownsampleRTs;

        private static ConVar r_hdr_enable;

        public static void Initialize(GraphicsDevice device)
        {
            // register convar
            r_hdr_enable = ConVar.Register("r_hdr_enable", true, "Enable high dynamic range rendering.", ConVarFlags.Archived);

            // create render targets
            int width = device.PresentationParameters.BackBufferWidth;
            int height = device.PresentationParameters.BackBufferHeight;

            _colorRT = new RenderTarget2D(device, width, height, false, SurfaceFormat.Color, DepthFormat.Depth16);
            _depthRT = new RenderTarget2D(device, width, height, false, SurfaceFormat.Single, DepthFormat.Depth16);
            _normalRT = new RenderTarget2D(device, width, height, false, SurfaceFormat.Color, DepthFormat.Depth16);
            _lightRT = new RenderTarget2D(device, width, height, false, (r_hdr_enable.GetValue<bool>()) ? SurfaceFormat.Rgba1010102 : SurfaceFormat.Color, DepthFormat.Depth16);

            if (r_hdr_enable.GetValue<bool>())
            {
                _hdrRT = new RenderTarget2D(device, width, height, false, SurfaceFormat.Rgba1010102, DepthFormat.None);
                _hdrBloomRT = new RenderTarget2D(device, width, height, false, SurfaceFormat.Rgba1010102, DepthFormat.None);

                int sampleEntries = 1;
                int startSize = Math.Min(width / 16, height / 16);

                int size = 16;
                for (size = 16; size < startSize; size *= 4)
                {
                    sampleEntries++;
                }

                _lightDownsampleRTs = new RenderTarget2D[sampleEntries];

                size /= 4;

                for (int i = 0; i < sampleEntries; i++)
                {
                    _lightDownsampleRTs[i] = new RenderTarget2D(device, size, size, false, SurfaceFormat.Single, DepthFormat.None);
                    size /= 4;
                }

                _avgLightRT = new RenderTarget2D(device, 1, 1, false, SurfaceFormat.Single, DepthFormat.None);
            }

            // load shaders
            _baseEffect = EffectManager.Load("BaseDraw", device);
            _compositeEffect = EffectManager.Load("ScreenComposite", device);
            _bloomEffect = EffectManager.Load("Bloom", device);
            _lightnessEffect = EffectManager.Load("Lightness", device);

            // light renderer
            LightRenderer.Initialize(device);

            // shadow renderer
            ShadowRenderer.Initialize(device);
        }

        private static void ClearRenderTarget(GraphicsDevice device, RenderTarget2D rt, Color color)
        {
            device.SetRenderTarget(rt);
            device.Clear(color);            
            device.SetRenderTarget(null);
        }

        public static void Render3DStuff(GraphicsDevice device)
        {
            // clear render targets
            ClearRenderTarget(device, _colorRT, Color.Black);
            ClearRenderTarget(device, _depthRT, Color.White);
            ClearRenderTarget(device, _normalRT, Color.Gray);

            // set render targets
            device.SetRenderTargets(_colorRT, _depthRT, _normalRT);

            // set up the shader
            Camera.Apply(_baseEffect);

            // render the scene
            RenderScene(device, _baseEffect, Camera.MainCamera);

            // set render target to the back buffer
            device.SetRenderTargets(null);

            // render shadow stuff
            _shadowRT = ShadowRenderer.RenderShadows(device);
            
            // set render target to the light buffer
            device.SetRenderTarget(_lightRT);

            // clear the RT
            device.Clear(Color.Transparent);

            // render ambient/directional light
            LightRenderer.RenderDirectionalLight(device, _normalRT, _depthRT, _shadowRT);

            // render lights
            //LightRenderer.RenderLights(device, _depthRT);

            //return;

            // again go back to the back buffer
            device.SetRenderTarget(_hdrRT); // will be null if not enabled

            // draw the screen composite
            DrawScreenComposite(device);

            if (_hdrRT != null)
            {
                //DrawAvgLightness(device);

                device.SetRenderTarget(_hdrBloomRT);
                DrawHDRBloom(device, true);
                device.SetRenderTarget(null);
                DrawHDRBloom(device, false);
            }
        }

        public static void RenderScene(GraphicsDevice device, Effect effect, Camera camera)
        {
            // draw the map
            MapRenderer.RenderPass(device, effect, camera);

            // draw entities
            Client.RenderEntities(device, effect, camera);
        }

        private static Vector2 _halfPixel;

        private static void DrawScreenComposite(GraphicsDevice device)
        {
            _halfPixel = new Vector2(0.5f / (float)device.PresentationParameters.BackBufferWidth, 0.5f / (float)device.PresentationParameters.BackBufferHeight);

            _compositeEffect.Parameters["ColorMap"].SetValue(_colorRT);
            _compositeEffect.Parameters["LightMap"].SetValue(_lightRT);
            //_compositeEffect.Parameters["ShadowOcclusionMap"].SetValue(_shadowRT);
            _compositeEffect.Parameters["HalfPixel"].SetValue(_halfPixel);
            _compositeEffect.Techniques[0].Passes[0].Apply();

            RenderFullScreenQuad(device);
        }

        private static void DrawAvgLightness(GraphicsDevice device)
        {
            // render initial lightness values
            device.SetRenderTarget(_lightDownsampleRTs[0]);
            _lightnessEffect.CurrentTechnique = _lightnessEffect.Techniques["CalculateLightness"];
            _lightnessEffect.Parameters["frameTex"].SetValue(_hdrRT);
            _lightnessEffect.Parameters["sourceSize"].SetValue(new Vector2(_hdrRT.Width, _hdrRT.Height));
            _lightnessEffect.CurrentTechnique.Passes[0].Apply();

            RenderFullScreenQuad(device);

            int i;

            for (i = 1; i < _lightDownsampleRTs.Length; i++)
            {
                device.SetRenderTarget(_lightDownsampleRTs[i]);
                _lightnessEffect.CurrentTechnique = _lightnessEffect.Techniques["DownsampleLightness"];
                _lightnessEffect.Parameters["frameTex"].SetValue(_lightDownsampleRTs[i - 1]);
                _lightnessEffect.Parameters["sourceSize"].SetValue(new Vector2(_lightDownsampleRTs[i - 1].Width, _lightDownsampleRTs[i - 1].Height));
                _lightnessEffect.CurrentTechnique.Passes[0].Apply();

                RenderFullScreenQuad(device);
            }

            device.SetRenderTarget(_avgLightRT);
            _lightnessEffect.CurrentTechnique = _lightnessEffect.Techniques["DownsampleLightnessFinalize"];
            _lightnessEffect.Parameters["frameTex"].SetValue(_lightDownsampleRTs[i - 1]);
            _lightnessEffect.Parameters["sourceSize"].SetValue(new Vector2(_lightDownsampleRTs[i - 1].Width, _lightDownsampleRTs[i - 1].Height));
            _lightnessEffect.CurrentTechnique.Passes[0].Apply();

            RenderFullScreenQuad(device);
        }

        private static void DrawHDRBloom(GraphicsDevice device, bool horz)
        {
            var width = _hdrRT.Width;
            var height = _hdrRT.Height;

            //_bloomEffect.Parameters["texture_size"].SetValue(new Vector4(width, height, 1.0f / width, 1.0f / height));
            _bloomEffect.Parameters["frameTex"].SetValue(_hdrRT);
            //_bloomEffect.Parameters["avgLightTex"].SetValue(_avgLightRT);

            if (!horz)
            {
                //_bloomEffect.Parameters["bloomTex"].SetValue(_hdrBloomRT);
            }

            _bloomEffect.CurrentTechnique = _bloomEffect.Techniques[(horz) ? 0 : 1];
            _bloomEffect.CurrentTechnique.Passes[0].Apply();

            RenderFullScreenQuad(device);
        }

        public static void RenderFullScreenQuad(GraphicsDevice device)
        {
            VertexPositionTexture3D[] vertices = new VertexPositionTexture3D[]
                {
                    new VertexPositionTexture3D(
                        new Vector3(1, -1, 0),
                        new Vector3(1, 1, 1)
                    ),
                    new VertexPositionTexture3D(
                        new Vector3(-1, -1, 0),
                        new Vector3(0, 1, 3)
                    ),
                    new VertexPositionTexture3D(
                        new Vector3(-1, 1, 0),
                        new Vector3(0, 0, 0)
                    ),
                    new VertexPositionTexture3D(
                        new Vector3(1, 1, 0),
                        new Vector3(1, 0, 1)
                    )
                };

            short[] indices = new short[] { 0, 1, 2, 2, 3, 0 };

            device.DrawUserIndexedPrimitives(PrimitiveType.TriangleList, vertices, 0, 4, indices, 0, 2);
        }
    }

    internal struct VertexPositionTexture3D : IVertexType
    {
        public Vector3 Position;
        public Vector3 Texture;

        public VertexPositionTexture3D(Vector3 position, Vector3 texture)
        {
            Position = position;
            Texture = texture;
        }

        public VertexDeclaration VertexDeclaration
        {
            get
            {
                return new VertexDeclaration(
                            new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position, 0),
                            new VertexElement(12, VertexElementFormat.Vector3, VertexElementUsage.TextureCoordinate, 0)
                       );
            }
        }
    }
}
