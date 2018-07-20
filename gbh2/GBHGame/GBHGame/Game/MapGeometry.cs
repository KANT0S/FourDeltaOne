using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;

namespace GBH
{
    public class BlockSide
    {
        public Vector3[] Vertices { get; set; }
        public Vector2[] UV { get; set; }
        public Vector3 Normal { get; set; }

        public BlockSide()
        {
            Vertices = new Vector3[4];
            UV = new Vector2[4];
        }
    }

    public class BlockGeometry
    {
        public BlockSide[] Sides { get; set; }

        public BlockGeometry()
        {
            Sides = new BlockSide[5];

            for (int i = 0; i < 5; i++)
            {
                Sides[i] = new BlockSide();
            }
        }
    }

    public static class MapGeometry
    {
        private static BlockGeometry[] Blocks { get; set; }

        public const int SLOPE_NONE = 0;
        public const int SLOPE_26_UP_LOW = 1;
        public const int SLOPE_26_UP_HIGH = 2;
        public const int SLOPE_26_DOWN_LOW = 3;
        public const int SLOPE_26_DOWN_HIGH = 4;
        public const int SLOPE_26_LEFT_LOW = 5;
        public const int SLOPE_26_LEFT_HIGH = 6;
        public const int SLOPE_26_RIGHT_LOW = 7;
        public const int SLOPE_26_RIGHT_HIGH = 8;
        public const int SLOPE_7_UP = 9;
        public const int SLOPE_7_DOWN = 17;
        public const int SLOPE_7_LEFT = 25;
        public const int SLOPE_7_RIGHT = 33;
        public const int SLOPE_45_UP = 41;
        public const int SLOPE_45_DOWN = 42;
        public const int SLOPE_45_LEFT = 43;
        public const int SLOPE_45_RIGHT = 44;
        public const int SLOPE_DIAGONAL_UPLEFT = 45;
        public const int SLOPE_DIAGONAL_UPRIGHT = 46;
        public const int SLOPE_DIAGONAL_DOWNLEFT = 47;
        public const int SLOPE_DIAGONAL_DOWNRIGHT = 48;
        public const int SLOPE_DIAGONAL_SLOPE_UPLEFT = 49;
        public const int SLOPE_DIAGONAL_SLOPE_UPRIGHT = 50;
        public const int SLOPE_DIAGONAL_SLOPE_DOWNLEFT = 51;
        public const int SLOPE_DIAGONAL_SLOPE_DOWNRIGHT = 52;
        public const int SLOPE_PARTIAL_LEFT = 53;
        public const int SLOPE_PARTIAL_RIGHT = 54;
        public const int SLOPE_PARTIAL_TOP = 55;
        public const int SLOPE_PARTIAL_BOTTOM = 56;
        public const int SLOPE_PARTIAL_TOPLEFT = 57;
        public const int SLOPE_PARTIAL_TOPRIGHT = 58;
        public const int SLOPE_PARTIAL_BOTTOMLEFT = 59;
        public const int SLOPE_PARTIAL_BOTTOMRIGHT = 60;
        public const int SLOPE_PARTIAL_CENTER = 61;
        public const int SLOPE_BOTTOM_IS_A_SLOPE = 63;

        public const int SIDE_LID = 0;
        public const int SIDE_TOP = 1;
        public const int SIDE_BOTTOM = 2;
        public const int SIDE_LEFT = 3;
        public const int SIDE_RIGHT = 4;

        public static void Initialize()
        {
            Blocks = new BlockGeometry[64];

            Blocks[SLOPE_NONE] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 1.0f),
                             new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f));

            Blocks[SLOPE_26_UP_LOW] = DoBlock(new Vector3(0.0f, 0.0f, 0.5f), new Vector3(1.0f, 0.0f, 0.5f),
                                              new Vector3(0.0f, 1.0f, 0.0f), new Vector3(1.0f, 1.0f, 0.0f));
            Blocks[SLOPE_26_UP_HIGH] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 1.0f),
                                               new Vector3(0.0f, 1.0f, 0.5f), new Vector3(1.0f, 1.0f, 0.5f));

            Blocks[SLOPE_26_DOWN_LOW] = DoBlock(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(1.0f, 0.0f, 0.0f),
                                                new Vector3(0.0f, 1.0f, 0.5f), new Vector3(1.0f, 1.0f, 0.5f));
            Blocks[SLOPE_26_DOWN_HIGH] = DoBlock(new Vector3(0.0f, 0.0f, 0.5f), new Vector3(1.0f, 0.0f, 0.5f),
                                                 new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f));

            Blocks[SLOPE_26_RIGHT_LOW] = DoBlock(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(1.0f, 0.0f, 0.5f),
                                                 new Vector3(0.0f, 1.0f, 0.0f), new Vector3(1.0f, 1.0f, 0.5f));
            Blocks[SLOPE_26_RIGHT_HIGH] = DoBlock(new Vector3(0.0f, 0.0f, 0.5f), new Vector3(1.0f, 0.0f, 1.0f),
                                                  new Vector3(0.0f, 1.0f, 0.5f), new Vector3(1.0f, 1.0f, 1.0f));

            Blocks[SLOPE_26_LEFT_LOW] = DoBlock(new Vector3(0.0f, 0.0f, 0.5f), new Vector3(1.0f, 0.0f, 0.0f),
                                                new Vector3(0.0f, 1.0f, 0.5f), new Vector3(1.0f, 1.0f, 0.0f));
            Blocks[SLOPE_26_LEFT_HIGH] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 0.5f),
                                                 new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 0.5f));

            for (int part = 0; part < 8; part++)
            {
                Blocks[SLOPE_7_UP + part] = DoBlock(new Vector3(0.0f, 0.0f, 0.125f * (part + 1)), new Vector3(1.0f, 0.0f, 0.125f * (part + 1)),
                                                    new Vector3(0.0f, 1.0f, 0.125f * (part)), new Vector3(1.0f, 1.0f, 0.125f * (part)));

                Blocks[SLOPE_7_DOWN + part] = DoBlock(new Vector3(0.0f, 0.0f, 0.125f * (part)), new Vector3(1.0f, 0.0f, 0.125f * (part)),
                                                      new Vector3(0.0f, 1.0f, 0.125f * (part + 1)), new Vector3(1.0f, 1.0f, 0.125f * (part + 1)));

                Blocks[SLOPE_7_LEFT + part] = DoBlock(new Vector3(0.0f, 0.0f, 0.125f * (part + 1)), new Vector3(1.0f, 0.0f, 0.125f * (part)),
                                                      new Vector3(0.0f, 1.0f, 0.125f * (part + 1)), new Vector3(1.0f, 1.0f, 0.125f * (part)));

                Blocks[SLOPE_7_RIGHT + part] = DoBlock(new Vector3(0.0f, 0.0f, 0.125f * (part)), new Vector3(1.0f, 0.0f, 0.125f * (part + 1)),
                                                       new Vector3(0.0f, 1.0f, 0.125f * (part)), new Vector3(1.0f, 1.0f, 0.125f * (part + 1)));
            }

            Blocks[SLOPE_45_UP] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 1.0f),
                                          new Vector3(0.0f, 1.0f, 0.0f), new Vector3(1.0f, 1.0f, 0.0f));

            Blocks[SLOPE_45_DOWN] = DoBlock(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(1.0f, 0.0f, 0.0f),
                                            new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f));

            Blocks[SLOPE_45_LEFT] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 0.0f),
                                            new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 0.0f));

            Blocks[SLOPE_45_RIGHT] = DoBlock(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(1.0f, 0.0f, 1.0f),
                                             new Vector3(0.0f, 1.0f, 0.0f), new Vector3(1.0f, 1.0f, 1.0f));

            Blocks[SLOPE_DIAGONAL_UPLEFT] = DoBlock(new Vector3(1.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 1.0f),
                                                    new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f));

            Blocks[SLOPE_DIAGONAL_UPRIGHT] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 1.0f),
                                                     new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f));

            // fix for normal calculation
            Blocks[SLOPE_DIAGONAL_UPRIGHT].Sides[SIDE_LID].Normal = new Vector3(0.0f, 0.0f, 1.0f);

            Blocks[SLOPE_DIAGONAL_DOWNLEFT] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 1.0f),
                                                      new Vector3(1.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f));

            Blocks[SLOPE_DIAGONAL_DOWNRIGHT] = DoBlock(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(1.0f, 0.0f, 1.0f),
                                                       new Vector3(0.0f, 1.0f, 1.0f), new Vector3(0.0f, 1.0f, 1.0f));

            Blocks[SLOPE_DIAGONAL_SLOPE_UPLEFT] = DoBlock(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(1.0f, 0.0f, 1.0f),
                                                          new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f));

            #region blah
            /*
            Blocks[SLOPE_DIAGONAL_DOWNRIGHT] = new BlockGeometry()
            {
                Sides = new BlockSide[]
                {
                    // LID
                    new BlockSide()
                    {
                        Vertices = new Vector3[]
                        {
                            new Vector3(0.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 1.0f),
                            new Vector3(0.0f, -1.0f, 1.0f)
                        },
                        UV = new Vector2[]
                        {
                            new Vector2(0.0f, 0.0f),
                            new Vector2(1.0f, 0.0f),
                            new Vector2(1.0f, 1.0f),
                            new Vector2(0.0f, 1.0f)
                        }
                    },
                    // TOP
                    new BlockSide()
                    {
                        Vertices = new Vector3[]
                        {
                            new Vector3(0.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 0.0f),
                            new Vector3(0.0f, 0.0f, 0.0f)
                        },
                        UV = new Vector2[]
                        {
                            new Vector2(0.0f, 0.0f),
                            new Vector2(1.0f, 0.0f),
                            new Vector2(1.0f, 1.0f),
                            new Vector2(0.0f, 1.0f)
                        }
                    },
                    // BOTTOM
                    new BlockSide()
                    {
                        Vertices = new Vector3[]
                        {
                            new Vector3(0.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 0.0f),
                            new Vector3(0.0f, 0.0f, 0.0f)
                        },
                        UV = new Vector2[]
                        {
                            new Vector2(0.0f, 0.0f),
                            new Vector2(1.0f, 0.0f),
                            new Vector2(1.0f, 1.0f),
                            new Vector2(0.0f, 1.0f)
                        }
                    },
                    // LEFT
                    new BlockSide()
                    {
                        Vertices = new Vector3[]
                        {
                            new Vector3(0.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 0.0f),
                            new Vector3(0.0f, 0.0f, 0.0f)
                        },
                        UV = new Vector2[]
                        {
                            new Vector2(0.0f, 0.0f),
                            new Vector2(1.0f, 0.0f),
                            new Vector2(1.0f, 1.0f),
                            new Vector2(0.0f, 1.0f)
                        }
                    },
                    // RIGHT
                    new BlockSide()
                    {
                        Vertices = new Vector3[]
                        {
                            new Vector3(0.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 1.0f),
                            new Vector3(1.0f, 0.0f, 0.0f),
                            new Vector3(0.0f, 0.0f, 0.0f)
                        },
                        UV = new Vector2[]
                        {
                            new Vector2(0.0f, 0.0f),
                            new Vector2(1.0f, 0.0f),
                            new Vector2(1.0f, 1.0f),
                            new Vector2(0.0f, 1.0f)
                        }
                    },
                }
            };
            */
            #endregion

            Blocks[SLOPE_BOTTOM_IS_A_SLOPE] = Blocks[SLOPE_NONE];
        }

        public static BlockGeometry GetBlock(int slopeType)
        {
            return Blocks[slopeType];
        }

        private static BlockGeometry DoBlock(Vector3 topLeft, Vector3 topRight, Vector3 bottomLeft, Vector3 bottomRight)
        {
            BlockGeometry block = new BlockGeometry();

            // lid
            block.Sides[SIDE_LID].Vertices[0] = new Vector3(topLeft.X, -topLeft.Y, topLeft.Z);
            block.Sides[SIDE_LID].Vertices[1] = new Vector3(topRight.X, -topRight.Y, topRight.Z);
            block.Sides[SIDE_LID].Vertices[2] = new Vector3(bottomRight.X, -bottomRight.Y, bottomRight.Z);
            block.Sides[SIDE_LID].Vertices[3] = new Vector3(bottomLeft.X, -bottomLeft.Y, bottomLeft.Z);

            block.Sides[SIDE_LID].UV[0] = new Vector2(topLeft.X, topLeft.Y);
            block.Sides[SIDE_LID].UV[1] = new Vector2(topRight.X, topRight.Y);
            block.Sides[SIDE_LID].UV[2] = new Vector2(bottomRight.X, bottomRight.Y);
            block.Sides[SIDE_LID].UV[3] = new Vector2(bottomLeft.X, bottomLeft.Y);

            block.Sides[SIDE_LID].Normal = CalculateNormal(block.Sides[SIDE_LID], true);

            // top
            block.Sides[SIDE_TOP].Vertices[0] = new Vector3(topRight.X, -topRight.Y, topRight.Z);
            block.Sides[SIDE_TOP].Vertices[1] = new Vector3(topLeft.X, -topLeft.Y, topLeft.Z);
            block.Sides[SIDE_TOP].Vertices[2] = new Vector3(topLeft.X, -topLeft.Y, 0.0f);
            block.Sides[SIDE_TOP].Vertices[3] = new Vector3(topRight.X, -topRight.Y, 0.0f);

            block.Sides[SIDE_TOP].UV[0] = new Vector2(topRight.X, 0.0f);
            block.Sides[SIDE_TOP].UV[1] = new Vector2(topLeft.X, 0.0f);
            block.Sides[SIDE_TOP].UV[2] = new Vector2(topLeft.X, topLeft.Z);
            block.Sides[SIDE_TOP].UV[3] = new Vector2(topRight.X, topLeft.Z);

            block.Sides[SIDE_TOP].Normal = CalculateNormal(block.Sides[SIDE_TOP], true);

            // bottom
            block.Sides[SIDE_BOTTOM].Vertices[0] = new Vector3(bottomLeft.X, -bottomLeft.Y, bottomLeft.Z);
            block.Sides[SIDE_BOTTOM].Vertices[1] = new Vector3(bottomRight.X, -bottomRight.Y, bottomRight.Z);
            block.Sides[SIDE_BOTTOM].Vertices[2] = new Vector3(bottomRight.X, -bottomRight.Y, 0.0f);
            block.Sides[SIDE_BOTTOM].Vertices[3] = new Vector3(bottomLeft.X, -bottomLeft.Y, 0.0f);

            block.Sides[SIDE_BOTTOM].UV[0] = new Vector2(bottomLeft.X, 0.0f);
            block.Sides[SIDE_BOTTOM].UV[1] = new Vector2(bottomRight.X, 0.0f);
            block.Sides[SIDE_BOTTOM].UV[2] = new Vector2(bottomRight.X, bottomRight.Z);
            block.Sides[SIDE_BOTTOM].UV[3] = new Vector2(bottomLeft.X, bottomRight.Z);

            block.Sides[SIDE_BOTTOM].Normal = CalculateNormal(block.Sides[SIDE_BOTTOM], true);

            // left
            block.Sides[SIDE_LEFT].Vertices[0] = new Vector3(topLeft.X, -topLeft.Y, topLeft.Z);
            block.Sides[SIDE_LEFT].Vertices[1] = new Vector3(bottomLeft.X, -bottomLeft.Y, bottomLeft.Z);
            block.Sides[SIDE_LEFT].Vertices[2] = new Vector3(bottomLeft.X, -bottomLeft.Y, 0.0f);
            block.Sides[SIDE_LEFT].Vertices[3] = new Vector3(topLeft.X, -topLeft.Y, 0.0f);

            block.Sides[SIDE_LEFT].UV[0] = new Vector2(topLeft.Y, 0.0f);
            block.Sides[SIDE_LEFT].UV[1] = new Vector2(bottomLeft.Y, 0.0f);
            block.Sides[SIDE_LEFT].UV[2] = new Vector2(bottomLeft.Y, topLeft.Z);
            block.Sides[SIDE_LEFT].UV[3] = new Vector2(topLeft.Y, topLeft.Z);

            block.Sides[SIDE_LEFT].Normal = CalculateNormal(block.Sides[SIDE_LEFT], true);

            // right
            block.Sides[SIDE_RIGHT].Vertices[0] = new Vector3(bottomRight.X, -bottomRight.Y, bottomRight.Z);
            block.Sides[SIDE_RIGHT].Vertices[1] = new Vector3(topRight.X, -topRight.Y, topRight.Z);
            block.Sides[SIDE_RIGHT].Vertices[2] = new Vector3(topRight.X, -topRight.Y, 0.0f);
            block.Sides[SIDE_RIGHT].Vertices[3] = new Vector3(bottomRight.X, -bottomRight.Y, 0.0f);

            block.Sides[SIDE_RIGHT].UV[0] = new Vector2(bottomRight.Y, topRight.Z);
            block.Sides[SIDE_RIGHT].UV[1] = new Vector2(topRight.Y, topRight.Z);
            block.Sides[SIDE_RIGHT].UV[2] = new Vector2(topRight.Y, 0.0f);
            block.Sides[SIDE_RIGHT].UV[3] = new Vector2(bottomRight.Y, 0.0f);

            block.Sides[SIDE_RIGHT].Normal = CalculateNormal(block.Sides[SIDE_RIGHT], true);

            return block;
        }

        private static Vector3 CalculateNormal(BlockSide side, bool reverse)
        {
            Vector3 u = side.Vertices[1] - side.Vertices[0];
            Vector3 v = side.Vertices[2] - side.Vertices[0];

            return (reverse) ? -Vector3.Cross(u, v) : Vector3.Cross(u, v);
        }
    }
}
