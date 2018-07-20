using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

using Jitter.Collision;
using Jitter.Collision.Shapes;
using Jitter.Dynamics;
using Jitter.LinearMath;

namespace GBH
{
    public static class CellManager
    {
        private const int CELL_SIZE = 16;
        private const int CELL_DISTANCE = 20;
        private static MapCell[,] _cells;
        private static List<MapCell> _activated;

        public static IEnumerable<MapCell> ActivatedCells
        {
            get
            {
                return _activated.AsReadOnly();
            }
        }

        public static void InitializeFromMap()
        {
            int size = 256 / CELL_SIZE;
            _cells = new MapCell[size, size];

            _activated = new List<MapCell>();

            for (int x = 0; x < size; x++)
            {
                for (int y = 0; y < size; y++)
                {
                    _cells[x, y] = new MapCell(x, y, CELL_SIZE);
                }
            }
        }

        public static void Recenter(IEnumerable<Vector3> centerPoints)
        {
            // exit if no map loaded
            if (_activated == null)
            {
                return;
            }

            // unload far cells
            var toDelete = new List<MapCell>();
            
            foreach (var cell in _activated)
            {
                bool inRange = false;

                foreach (var point in centerPoints)
                {
                    if (point.Distance2D(cell.Center) < CELL_DISTANCE)
                    {
                        inRange = true;
                    }
                }

                if (!inRange)
                {
                    toDelete.Add(cell);
                }
            }

            foreach (var cell in toDelete)
            {
                cell.Unload();
                _activated.Remove(cell);
            }

            // load near cells
            int size = 256 / CELL_SIZE;

            for (int x = 0; x < size; x++)
            {
                for (int y = 0; y < size; y++)
                {
                    var cell = _cells[x, y];

                    if (!cell.Loaded)
                    {
                        bool inRange = false;

                        foreach (var point in centerPoints)
                        {
                            if (point.Distance2D(cell.Center) < CELL_DISTANCE)
                            {
                                inRange = true;
                            }
                        }

                        if (inRange)
                        {
                            cell.Load();
                            _activated.Add(cell);
                        }
                    }
                }
            }
        }
    }

    public class MapCell
    {
        public int X { get; set; }
        public int Y { get; set; }

        public bool Loaded { get; private set; }

        public Vector2 Center
        {
            get
            {
                return new Vector2(X + (_size / 2.0f), -Y - (_size / 2.0f));
            }
        }

        public BoundingBox Bounds { get; private set; }

        private Dictionary<string, RenderBank<VertexPositionNormalTexture>> _banks;

        public Dictionary<string, RenderBank<VertexPositionNormalTexture>> Banks
        {
            get
            {
                return _banks;
            }
        }

        private int _size;

        private TriangleMeshShape _collisionMesh;
        private RigidBody _collisionBody;

        public MapCell(int x, int y, int size)
        {
            X = x * size;
            Y = y * size;
            _size = size;

            Bounds = new BoundingBox(new Vector3(X, -Y - size, 0), new Vector3(X + size, -Y, 8));

            Loaded = false;
        }

        public void Load()
        {
            var startTime = Game.Time;

            _banks = MakeBanks(false);

            if (Renderer.Device != null)
            {
                foreach (var bank in _banks)
                {
                    bank.Value.UploadToGPU(Renderer.Device);
                }
            }

            // build collision mesh
            var colBanks = MakeBanks(true);
            var colPositions = new List<JVector>();
            var colTris = new List<TriangleVertexIndices>();

            foreach (var bank in colBanks.Values)
            {
                foreach (var vertex in bank.vertices)
                {
                    colPositions.Add(vertex.Position.ToJVector());
                }

                for (int i = 0; i < bank.numIndex; i += 3)
                {
                    colTris.Add(new TriangleVertexIndices(bank.indices[i], bank.indices[i + 1], bank.indices[i + 2]));
                }
            }

            var octree = new Octree(colPositions, colTris);

            _collisionMesh = new TriangleMeshShape(octree);
            _collisionBody = new RigidBody(_collisionMesh);
            _collisionBody.IsStatic = true;

            Server.PhysicsWorld.AddBody(_collisionBody);

            Loaded = true;

            var time = Game.Time - startTime;
            Log.Write(LogLevel.Debug, "load cell took {0}msec", time);
        }

        public void Unload()
        {
            foreach (var bank in _banks)
            {
                bank.Value.Dispose();
            }

            _banks = null;

            Server.PhysicsWorld.RemoveBody(_collisionBody);

            _collisionBody = null;
            _collisionMesh = null;

            Loaded = false;
        }

        private bool _isCol = false;
        private BlockInfo _block;
        private Dictionary<string, RenderBank<VertexPositionNormalTexture>> _workingBanks;

        private Dictionary<string, RenderBank<VertexPositionNormalTexture>> MakeBanks(bool isCol)
        {
            var banks = new Dictionary<string, RenderBank<VertexPositionNormalTexture>>();
            _workingBanks = banks;
            _isCol = isCol;

            for (int z = (isCol ? 1 : 0); z < 7; z++)
            {
                for (int x = X; x < (X + _size); x++)
                {
                    for (int y = Y; y < (Y + _size); y++)
                    {
                        BlockInfo block = MapManager.GetBlock(x, y, z);
                        BlockGeometry geometry = MapGeometry.GetBlock(block.SlopeType.Slope);

                        Vector3 offsetVector = new Vector3(x, -y, z);

                        if (geometry != null)
                        {
                            _block = block;
                            DrawSide(geometry.Sides[MapGeometry.SIDE_TOP], block.Top, offsetVector, false);
                            DrawSide(geometry.Sides[MapGeometry.SIDE_BOTTOM], block.Bottom, offsetVector, false);
                            DrawSide(geometry.Sides[MapGeometry.SIDE_LEFT], block.Left, offsetVector, false);
                            DrawSide(geometry.Sides[MapGeometry.SIDE_RIGHT], block.Right, offsetVector, false);
                            DrawSide(geometry.Sides[MapGeometry.SIDE_LID], block.Lid, offsetVector, true);
                        }
                    }
                }
            }

            // remove reference
            _workingBanks = null;

            return banks;
        }

        private RenderBank<VertexPositionNormalTexture> GetBank(string imageName)
        {
            if (!_workingBanks.ContainsKey(imageName))
            {
                _workingBanks[imageName] = new RenderBank<VertexPositionNormalTexture>();
            }

            return _workingBanks[imageName];
        }

        private void DrawSide(BlockSide side, SideTile part, Vector3 offset, bool isLid)
        {
            if (part.Sprite > 0)
            {
                if (_isCol && (!part.Wall && !isLid))
                {
                    if (_block.SlopeType.Slope != MapGeometry.SLOPE_DIAGONAL_DOWNLEFT && _block.SlopeType.Slope != MapGeometry.SLOPE_DIAGONAL_DOWNRIGHT &&
                        _block.SlopeType.Slope != MapGeometry.SLOPE_DIAGONAL_UPLEFT && _block.SlopeType.Slope != MapGeometry.SLOPE_DIAGONAL_UPRIGHT)
                    {
                        return;
                    }
                }

                BlockSide modifiedSide = ModifySide(side, part);

                ushort sprite = part.Sprite;

                string materialName = string.Format("gbh/bil/{0}", sprite);
                Material material = MaterialManager.FindMaterial(materialName);

                var bank = GetBank(material.BaseName);
                bank.ReserveSpace(side.Vertices.Length, (side.Vertices.Length == 4) ? 6 : 4);

                int vertIndex = bank.numVertex;

                bank.vertices[bank.numVertex++] = GetVertex(modifiedSide, 0, offset, part, material);
                bank.vertices[bank.numVertex++] = GetVertex(modifiedSide, 1, offset, part, material);
                bank.vertices[bank.numVertex++] = GetVertex(modifiedSide, 2, offset, part, material);

                bank.indices[bank.numIndex++] = (short)(vertIndex + 0);
                bank.indices[bank.numIndex++] = (short)(vertIndex + 1);
                bank.indices[bank.numIndex++] = (short)(vertIndex + 2);

                if (side.Vertices.Length == 4)
                {
                    bank.vertices[bank.numVertex++] = GetVertex(modifiedSide, 3, offset, part, material);

                    bank.indices[bank.numIndex++] = (short)(vertIndex + 2);
                    bank.indices[bank.numIndex++] = (short)(vertIndex + 3);
                    bank.indices[bank.numIndex++] = (short)(vertIndex + 0);
                }
            }
        }

        private static int[][] _rotations = new int[][]
        {
            // rotation
            new int[] { 0, 1, 2, 1, 2, 3, 0, 3 }, // 0
            new int[] { 0, 3, 0, 1, 2, 1, 2, 3 }, // 90
            new int[] { 2, 3, 0, 3, 0, 1, 2, 1 }, // 180
            new int[] { 2, 1, 2, 3, 0, 3, 0, 1 }, // 270

            // flipped rotation
            new int[] { 2, 1, 0, 1, 0, 3, 2, 3 }, // 0
            new int[] { 0, 1, 0, 3, 2, 3, 2, 1 }, // 90
            new int[] { 0, 3, 2, 3, 2, 1, 0, 1 }, // 180
            new int[] { 2, 3, 2, 1, 0, 1, 0, 3 }, // 270
        };

        private BlockSide ModifySide(BlockSide side, SideTile part)
        {
            BlockSide modifiedSide = new BlockSide();
            modifiedSide.Vertices = side.Vertices;
            modifiedSide.Normal = side.Normal;
            modifiedSide.UV = new Vector2[4];
            modifiedSide.UV[0] = side.UV[0];
            modifiedSide.UV[1] = side.UV[1];
            modifiedSide.UV[2] = side.UV[2];
            modifiedSide.UV[3] = side.UV[3];

            int rotation = part.Rotation;

            if (part.Flip)
            {
                rotation += 4;
            }

            float[] uvStuff = new float[4]
            {
                0.0f,
                0.0f,
                1.0f,
                1.0f
            };

            modifiedSide.UV = new Vector2[4];
            modifiedSide.UV[0] = new Vector2(uvStuff[_rotations[rotation][0]], uvStuff[_rotations[rotation][1]]);
            modifiedSide.UV[1] = new Vector2(uvStuff[_rotations[rotation][2]], uvStuff[_rotations[rotation][3]]);
            modifiedSide.UV[2] = new Vector2(uvStuff[_rotations[rotation][4]], uvStuff[_rotations[rotation][5]]);
            modifiedSide.UV[3] = new Vector2(uvStuff[_rotations[rotation][6]], uvStuff[_rotations[rotation][7]]);

            return modifiedSide;
        }

        private VertexPositionNormalTexture GetVertex(BlockSide side, int vertex, Vector3 offset, SideTile part, Material material)
        {
            return new VertexPositionNormalTexture(side.Vertices[vertex] + offset, side.Normal, material.TransformUV(side.UV[vertex]));
        }
    }
}
