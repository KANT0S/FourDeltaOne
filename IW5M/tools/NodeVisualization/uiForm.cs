using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;

using Arrows;

namespace NodeVisualization
{
    public partial class uiForm : Form
    {
        ArrowRenderer r = new ArrowRenderer(10, (float)Math.PI / 6, true);
        Bitmap compass;
        Vector minimap1;
        Vector minimap2;
        float[] compassCorners;

        List<Node> nodes = new List<Node>();
        Node currentNode;

        public uiForm()
        {
            InitializeComponent();
            r.SetThetaInDegrees(90);

            compass = new Bitmap(@"R:\bigtemp\mw3i\compass_map_mp_dome.png");

            // when doing stuff here, note that coords are yxz.
            minimap1 = new Vector(-1072, 2240);
            minimap2 = new Vector(3008, -1840);

            //ConvertMinimap();
            compassCorners = new float[] { 3008, 2240, -1072, -1840 };

            panel2.Width = 1024;
            panel2.Height = 1024;
        }

        // wrong as dot product isn't like assuming north/west are both positive - usual is northeast positive
        private void ConvertMinimap()
        {
            var cornerdiff = minimap2 - minimap1;

            var north = new Vector((float)Math.Sin(0), (float)Math.Cos(0));
            var west = new Vector(north.Y, 0 - north.X);

            Vector northwest;
            Vector southeast;

            if (Vector.DotProduct(cornerdiff, west) > 0)
            {
                if (Vector.DotProduct(cornerdiff, north) > 0)
                {
                    northwest = minimap2;
                    southeast = minimap1;
                }
                else
                {
                    var side = north * Vector.DotProduct(cornerdiff, north);
                    northwest = minimap2 - side;
                    southeast = minimap1 + side;
                }
            }
            else
            {
                if (Vector.DotProduct(cornerdiff, north) > 0)
                {
                    var side = north * Vector.DotProduct(cornerdiff, north);
                    northwest = minimap1 + side;
                    southeast = minimap2 - side;                    
                }
                else
                {
                    northwest = minimap1;
                    southeast = minimap2;
                }
            }

            compassCorners = new float[] { northwest.X, northwest.Y, southeast.X, southeast.Y };
        }

        private void openButton_Click(object sender, EventArgs e)
        {
            nodes.Clear();

            openFileDialog1.ShowDialog();

            var name = openFileDialog1.FileName;
            var reader = File.OpenText(name);

            var width = compassCorners[2] - compassCorners[0];
            var height = compassCorners[3] - compassCorners[1];

            var offx = compassCorners[0];
            var offy = compassCorners[1];

            while (!reader.EndOfStream)
            {
                var line = reader.ReadLine();

                if (line.StartsWith("node "))
                {
                    var match = Regex.Match(line, "node ([0-9]+): ([0-9\\.\\-]+) ([0-9\\.\\-]+) ([0-9\\.\\-]+)");

                    var x = float.Parse(match.Groups[3].Value, CultureInfo.InvariantCulture);
                    var y = float.Parse(match.Groups[2].Value, CultureInfo.InvariantCulture);
                    var z = float.Parse(match.Groups[4].Value, CultureInfo.InvariantCulture);;

                    var mx = (x - offx);
                    var my = (y - offy);

                    mx = (mx / width) * (compass.Width * 2);
                    my = (my / height) * (compass.Height * 2);

                    currentNode = new Node();
                    currentNode.id = int.Parse(match.Groups[1].Value);
                    currentNode.origin = new Vector(y, x, z);
                    currentNode.mapOrigin = new Vector(mx, my);

                    nodes.Add(currentNode);
                }
                else if (line.StartsWith("link: "))
                {
                    var linkID = line.Split(':')[1].Trim();
                    currentNode.links.Add(int.Parse(linkID));
                }
            }

            reader.Close();
            panel2.Invalidate();
        }

        private void panel2_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.Clear(Color.White);

            e.Graphics.DrawImage(compass, 0, 0, 1024, 1024);

            foreach (var n in nodes)
            {
                e.Graphics.FillRectangle(new SolidBrush(Color.DarkRed), n.mapOrigin.X - 4, n.mapOrigin.Y - 4, 8, 8);

                foreach (var l in n.links)
                {
                    var tn = (from node in nodes
                              where node.id == l
                              select node).FirstOrDefault();

                    if (tn != null)
                    {
                        r.Width = 10;
                        r.DrawArrow(e.Graphics, new Pen(Color.Gray), new SolidBrush(Color.Red), n.mapOrigin.X, n.mapOrigin.Y, tn.mapOrigin.X, tn.mapOrigin.Y);
                    }
                }
            }
        }
    }

    class Node
    {
        public int id;
        public Vector origin;
        public Vector mapOrigin;
        public List<int> links;

        public Node()
        {
            links = new List<int>();
        }
    }
}
