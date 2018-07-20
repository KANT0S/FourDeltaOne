using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;

using Jitter;
using Jitter.Dynamics;
using Jitter.LinearMath;

namespace GBH
{
    public static class PlayerMovement
    {
        private static PlayerEntity _entity;
        private static UserCommand _command;
        private static uint _msec;
        private static float _dT;
        private static World _pw;

        private static void Rotate()
        {
            if (_command.TestButton(ClientButtons.RotateRight))
            {
                _entity.Rotation += new Vector3(0f, 0f, -90f * _dT);

                if (_entity.Rotation.Z < 0f)
                {
                    _entity.Rotation += new Vector3(0f, 0f, 360f);
                }
            }

            if (_command.TestButton(ClientButtons.RotateLeft))
            {
                _entity.Rotation += new Vector3(0f, 0f, 90f * _dT);

                if (_entity.Rotation.Z >= 360f)
                {
                    _entity.Rotation -= new Vector3(0f, 0f, 360f);
                }
            }
        }

        private static void Walk()
        {
            Vector3 direction = Vector3.Zero;

            // possibly change this to be more shared (only do forward/backward difference)
            var heading = 0f;

            if (_command.TestButton(ClientButtons.Forward))
            {
                heading = MathHelper.ToRadians(_entity.Rotation.Z);
                direction = new Vector3((float)Math.Sin(heading) * -1f, (float)Math.Cos(heading) * 1f, 0f);
            }
            if (_command.TestButton(ClientButtons.Backward))
            {
                heading = MathHelper.ToRadians(_entity.Rotation.Z + 180f);
                direction = new Vector3((float)Math.Sin(heading) * -1f, (float)Math.Cos(heading) * 1f, 0f);
            }

            /*if (direction == Vector3.Zero)
            {
                return;
            }*/

            float slopeZ = 1.0f * _dT + 0.25f;

            var velocity = direction * _dT;
            velocity.Z = _entity.VelocityZ;
            velocity.Z -= ConVar.GetValue<float>("sv_gravity") * _dT;

            // perform collision tests
            JVector normal; float fraction; RigidBody body; JVector testDirection;

            if (direction != Vector3.Zero)
            {
                // X axis
                testDirection = new JVector(velocity.X, 0f, 0f);
                testDirection += new JVector(0.25f * direction.X, 0f, 0f);
                _pw.CollisionSystem.Raycast((_entity.Position + new Vector3(0f, 0f, slopeZ)).ToJVector(), testDirection, null, out body, out normal, out fraction);

                if (fraction <= 1.0f)
                {
                    // hit a wall
                    //                 float pedBorder = -0.25f;
                    //                 if (testDirection.X < 0f)
                    //                 {
                    //                     pedBorder = -pedBorder;
                    //                 }

                    velocity.X = 0.0f;
                    //                _entity.Position = new Vector3(_entity.Position.X + (testDirection * fraction).X + pedBorder, _entity.Position.Y, _entity.Position.Z);
                }

                // Y axis
                testDirection = new JVector(0f, velocity.Y, 0f);
                testDirection += new JVector(0f, 0.25f * direction.Y, 0f);
                _pw.CollisionSystem.Raycast((_entity.Position + new Vector3(0f, 0f, slopeZ)).ToJVector(), testDirection, null, out body, out normal, out fraction);

                if (fraction <= 1.0f)
                {
                    // hit a wall
                    velocity.Y = 0.0f;
                }
            }

            // Z axis
            Vector3 basePosition = (_entity.Position + new Vector3(0f, 0f, slopeZ));
            testDirection = new JVector(0f, 0f, velocity.Z - slopeZ);
            _pw.CollisionSystem.Raycast((basePosition).ToJVector(), testDirection, null, out body, out normal, out fraction);

            if (fraction <= 1.0f)
            {
                // hit the ground
                velocity.Z = 0.0f;
                _entity.Position = new Vector3(basePosition.X, basePosition.Y, basePosition.Z + (testDirection * fraction).Z);
            }

            _entity.Position += velocity;
            _entity.VelocityZ = velocity.Z;
        }

        public static void ProcessSingle()
        {
            _msec = _command.ServerTime - _entity.LastCommandTime;
            _entity.LastCommandTime = _command.ServerTime;

            _dT = _msec / 1000f;

            Rotate();
            Walk();            
        }

        public static void Process(PlayerEntity entity, UserCommand command, World pw)
        {
            uint finalTime = command.ServerTime;

            if (finalTime < entity.LastCommandTime)
            {
                return;
            }

            if (finalTime > (entity.LastCommandTime + 1000))
            {
                entity.LastCommandTime = finalTime - 1000;
            }

            while (entity.LastCommandTime != finalTime)
            {
                uint msec = finalTime - entity.LastCommandTime;

                if (msec > 66)
                {
                    msec = 66;
                }

                _pw = pw;
                _entity = entity;
                _command = command;

                _command.ServerTime = entity.LastCommandTime + msec;
                ProcessSingle();
            }
        }
    }
}
