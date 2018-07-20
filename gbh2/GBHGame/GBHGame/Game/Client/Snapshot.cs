using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace GBH
{
    public class Snapshot
    {
        public uint Sequence { get; set; }
        public uint ServerTime { get; set; }
        public uint ClientTime { get; set; }
        public Entity[] Entities { get; set; }

        public Snapshot()
        {
            Entities = new Entity[4096];
        }

        public PlayerEntity LocalPlayer
        {
            get
            {
                // FIXME: replace with actual ID stuff
                return (PlayerEntity)Entities[Client.ClientNum];
            }
            set
            {
                Entities[Client.ClientNum] = value;
            }
        }

        public static Snapshot InterpolateBetween(Snapshot left, Snapshot right, uint time)
        {
            if (left == null)
            {
                return right;
            }

            Snapshot retval = new Snapshot();
            retval.Sequence = right.Sequence;
            retval.ServerTime = time;

            Array.Copy(right.Entities, retval.Entities, retval.Entities.Length);

            float factor = MathHelper.Clamp((float)(time - left.ClientTime) / (float)(right.ClientTime - left.ClientTime), 0f, 1f);

            for (int i = 0; i < left.Entities.Length; i++)
            {
                // FIXME: might be bad?
                if (left.Entities[i] == null || right.Entities[i] == null)
                {
                    continue;
                }

                retval.Entities[i] = retval.Entities[i].Clone();
                retval.Entities[i].InterpolateFrom(left.Entities[i], factor);
            }

            return retval;
        }
    }
}
