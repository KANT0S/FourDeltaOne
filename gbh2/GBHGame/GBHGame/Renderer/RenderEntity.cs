using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public class RenderEntity
    {
        public Vector3 Position { get; set; }
        public float Heading { get; set; }

        // stuff for sprite entities
        public Material Sprite { get; set; }
        public Vector2 Size { get; set; }

        public void Render(GraphicsDevice device, Effect effect, Camera camera)
        {
            // generate a world matrix
            var worldMatrix = Matrix.CreateRotationZ(MathHelper.ToRadians(Heading)) * Matrix.CreateTranslation(Position);

            // generate offset coordinates
            var x1 = -(Size.X / 2);
            var y1 = (Size.Y / 2);
            var x2 = (Size.X / 2);
            var y2 = -(Size.Y / 2);

            // FIXME: allocating may be bad
            var vertices = new VertexPositionNormalTexture[4];
            var indices = new short[6] { 0, 1, 2, 2, 3, 0 };

            vertices[0] = new VertexPositionNormalTexture(new Vector3(x1, y1, 0f), Vector3.Backward, Sprite.TransformUV(new Vector2(0, 0)));
            vertices[1] = new VertexPositionNormalTexture(new Vector3(x2, y1, 0f), Vector3.Backward, Sprite.TransformUV(new Vector2(1, 0)));
            vertices[2] = new VertexPositionNormalTexture(new Vector3(x2, y2, 0f), Vector3.Backward, Sprite.TransformUV(new Vector2(1, 1)));
            vertices[3] = new VertexPositionNormalTexture(new Vector3(x1, y2, 0f), Vector3.Backward, Sprite.TransformUV(new Vector2(0, 1)));

            effect.Parameters["World"].SetValue(worldMatrix);
            effect.Parameters["ColorMap"].SetValue(Sprite.DiffuseImage.Texture);
            effect.Techniques[0].Passes[0].Apply();

            device.DrawUserIndexedPrimitives(PrimitiveType.TriangleList, vertices, 0, 4, indices, 0, 2);
        }
    }
}
