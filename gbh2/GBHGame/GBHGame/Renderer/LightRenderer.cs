using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    internal struct PointLightVertex : IVertexType
    {
        public Vector3 Position;
        public Color Color;
        public Vector3 Center;
        public Vector2 ExtraData;

        public VertexDeclaration VertexDeclaration
        {
            get
            {
                return new VertexDeclaration(
                            new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position, 0),
                            new VertexElement(12, VertexElementFormat.Color, VertexElementUsage.Color, 0),
                            new VertexElement(16, VertexElementFormat.Vector3, VertexElementUsage.Normal, 0),
                            new VertexElement(28, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0)
                       );
            }
        }
    }

    public static class LightRenderer
    {
        private static Effect _pointEffect;
        private static Effect _dirAmbEffect;
        private static Vector2 _halfPixel;
        private static ConVar r_sunIntensity;

        public static void Initialize(GraphicsDevice device)
        {
            _pointEffect = EffectManager.Load("PointLight", device);
            _dirAmbEffect = EffectManager.Load("Ambient", device);

            _halfPixel = new Vector2(0.5f / (float)device.PresentationParameters.BackBufferWidth, 0.5f / (float)device.PresentationParameters.BackBufferHeight);

            r_sunIntensity = ConVar.Register("r_sunIntensity", 1.6f, "Sun intensity, duh.", ConVarFlags.Archived);
        }

        public static void RenderDirectionalLight(GraphicsDevice device, RenderTarget2D normalRT, RenderTarget2D depthRT, RenderTarget2D shadowRT)
        {
            _dirAmbEffect.Parameters["HalfPixel"].SetValue(_halfPixel);
            _dirAmbEffect.Parameters["NormalMap"].SetValue(normalRT);
            //_dirAmbEffect.Parameters["DepthMap"].SetValue(depthRT);
            _dirAmbEffect.Parameters["ShadowMap"].SetValue(shadowRT);
            _dirAmbEffect.Parameters["InvertViewProjection"].SetValue(Matrix.Invert(Camera.MainCamera.ViewProjectionMatrix));
            _dirAmbEffect.Parameters["g_directional"].SetValue(r_sunIntensity.GetValue<float>());

            _dirAmbEffect.CurrentTechnique.Passes[0].Apply();
            DeferredRenderer.RenderFullScreenQuad(device);
        }

        public static void RenderLights(GraphicsDevice device, RenderTarget2D depthRT)
        {
            // get a series of cubes
            MapLight[] lights = GetMapLights().ToArray();

            if (lights.Length == 0)
            {
                return;
            }

            // create the vertex/index arrays
            PointLightVertex[] vertices = new PointLightVertex[lights.Length * 8];
            short[] indices = new short[lights.Length * 36];

            int vertexI = 0, indexI = 0;

            foreach (MapLight light in lights)
            {
                Vector3 lightPosition = new Vector3(light.Position.X, -light.Position.Y, light.Position.Z);

                Vector3[] cuboid_planes = new Vector3[]
                {
                    Vector3.UnitX * -light.Radius, Vector3.UnitX * light.Radius,
                    Vector3.UnitY * -light.Radius, Vector3.UnitY * light.Radius,
                    Vector3.UnitZ * -light.Radius, Vector3.UnitZ * light.Radius,
                };

                Vector3[] thisVertices = new Vector3[]
                {
                    lightPosition + cuboid_planes[0] + cuboid_planes[2] + cuboid_planes[4],
                    lightPosition + cuboid_planes[0] + cuboid_planes[2] + cuboid_planes[5],
                    lightPosition + cuboid_planes[0] + cuboid_planes[3] + cuboid_planes[4],
                    lightPosition + cuboid_planes[0] + cuboid_planes[3] + cuboid_planes[5],
                    lightPosition + cuboid_planes[1] + cuboid_planes[2] + cuboid_planes[4],
                    lightPosition + cuboid_planes[1] + cuboid_planes[2] + cuboid_planes[5],
                    lightPosition + cuboid_planes[1] + cuboid_planes[3] + cuboid_planes[4],
                    lightPosition + cuboid_planes[1] + cuboid_planes[3] + cuboid_planes[5],
                };

                int vertexOffset = vertexI;

                for (int i = 0; i < 8; i++)
                {
                    vertices[vertexI++] = new PointLightVertex() { Center = lightPosition, Position = thisVertices[i], Color = light.Color, ExtraData = new Vector2(light.Radius, light.Intensity / 255.0f) };
                }

                for (int i = 0; i < 36; i++)
                {
                    indices[indexI++] = (short)(vertexOffset + _pointIndices[i]);
                }
            }

            // draw the vertices using the effect we care about
            Camera.Apply(_pointEffect);
            _pointEffect.Parameters["HalfPixel"].SetValue(_halfPixel);
            _pointEffect.Parameters["DepthMap"].SetValue(depthRT);
            _pointEffect.Parameters["InvertViewProjection"].SetValue(Matrix.Invert(Camera.MainCamera.ViewProjectionMatrix));

            BlendState oldState = device.BlendState;
            device.BlendState = BlendState.Additive;

            DepthStencilState oldDepth = device.DepthStencilState;
            device.DepthStencilState = DepthStencilState.None;

            _pointEffect.Techniques[0].Passes[0].Apply();
            device.DrawUserIndexedPrimitives(PrimitiveType.TriangleList, vertices, 0, vertices.Length, indices, 0, indices.Length / 3);

            device.BlendState = BlendState.Opaque;
            device.DepthStencilState = DepthStencilState.Default;
        }

        private static short[] _pointIndices = new short[36]
        {
            2, 6, 4, 2, 4, 0,
            7, 3, 1, 7, 1, 5,
            3, 2, 0, 3, 0, 1,
            6, 7, 5, 6, 5, 4,
            0, 4, 5, 0, 5, 1,
            3, 7, 6, 3, 6, 2
        };

        private static IEnumerable<MapLight> GetMapLights()
        {
            // TODO: put this in some helper file?
            Vector3[] corners = Camera.MainCamera.BoundingFrustum.GetCorners();

            int x1 = (int)Math.Floor(corners[4].X);
            int x2 = (int)Math.Ceiling(corners[5].X);
            int y1 = (int)Math.Floor(-corners[4].Y);
            int y2 = (int)Math.Ceiling(-corners[7].Y);

            x1 -= 5;
            x2 += 5;
            y1 -= 5;
            y2 += 5;

            var lights = from light in MapManager.Lights
                         where light.Position.X > x1 && light.Position.X < x2 && light.Position.Y > y1 && light.Position.Y < y2
                         select light;

            return lights;
        }
    }
}
