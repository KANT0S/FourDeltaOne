using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Microsoft.Xna.Framework;
using Jitter.LinearMath;

namespace GBH
{
    public static class Extensions
    {
        public static bool EndOfStream(this Stream stream)
        {
            if (stream.Position >= stream.Length)
            {
                return true;
            }

            return false;
        }

        public static float Distance2D(this Vector3 vector, Vector2 vector2)
        {
            var vector3 = new Vector3(vector2, vector.Z);
            return (vector3 - vector).Length();
        }

        public static JVector ToJVector(this Vector3 vector)
        {
            return new JVector(vector.X, vector.Y, vector.Z);
        }

        public static Vector3 ToVector3(this JVector vector)
        {
            return new Vector3(vector.X, vector.Y, vector.Z);
        }
    }
}
