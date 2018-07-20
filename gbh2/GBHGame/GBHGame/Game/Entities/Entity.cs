using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;

namespace GBH
{
    public abstract class Entity
    {
        public Vector3 Position { get; set; }
        public Vector3 Rotation { get; set; }
        public int SpawnKey { get; set; }

        // server-side only
        public abstract void Initialize();
        public abstract void InitializeFromProperties(Dictionary<string, string> properties);
        public abstract void Spawn();
        public abstract void Think();

        // network handling
        public abstract int TypeCode { get; }

        public virtual void Deserialize(DeltaBitStream message)
        {
            Position = new Vector3(message.ReadSingle(), message.ReadSingle(), message.ReadSingle());
            Rotation = new Vector3(message.ReadInt16() / 10f, message.ReadInt16() / 10f, message.ReadInt16() / 10f);
        }

        public virtual void Serialize(DeltaBitStream message)
        {
            message.WriteSingle(Position.X);
            message.WriteSingle(Position.Y);
            message.WriteSingle(Position.Z);

            message.WriteInt16((short)(Rotation.X * 10));
            message.WriteInt16((short)(Rotation.Y * 10));
            message.WriteInt16((short)(Rotation.Z * 10));
        }

        // client-side only
        public abstract void ClientProcess();
        public abstract RenderEntity GetRenderEntity();

        public virtual void InterpolateFrom(Entity from, float factor)
        {
            if (Math.Abs(Rotation.Z - from.Rotation.Z) < 90f)
            {
                Rotation = (Rotation * factor) + (from.Rotation * (1f - factor));                
            }

            Position = (Position * factor) + (from.Position * (1f - factor));
        }

        // creationism
        public static Entity Create(int typeCode)
        {
            switch (typeCode)
            {
                case 0:
                    return new PlayerEntity();
                default:
                    return null;
            }
        }

        public abstract Entity Clone();
    }
}
