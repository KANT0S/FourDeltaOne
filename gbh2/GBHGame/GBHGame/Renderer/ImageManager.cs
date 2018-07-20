using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public static class ImageManager
    {
        private static Dictionary<string, Image> _images = new Dictionary<string, Image>();

        public static Image FindImage(string name)
        {
            if (_images.ContainsKey(name))
            {
                return _images[name];
            }

            if (!FileSystem.FileExists(name))
            {
                return null;
            }

            var image = new Image(name);
            if (!image.Load())
            {
                image = new Image("Images/default.png");

                if (!image.Load())
                {
                    throw new Exception("Couldn't load default image...");
                }
            }

            _images[name] = image;

            return image;
        }
    }

    public class Image
    {
        public string Name { get; private set; }
        public bool Loaded
        {
            get
            {
                return Texture != null;
            }
        }

        public Texture2D Texture { get; private set; }

        public Image(string name)
        {
            Name = name;
        }

        public bool Load()
        {
            if (Loaded)
            {
                return true;
            }

            var extension = Path.GetExtension(Name);

            switch (extension)
            {
                case ".png":
                case ".jpg":
                case ".gif":
                    {
                        var stream = FileSystem.OpenCopy(Name);

                        try
                        {
                            Texture = Texture2D.FromStream(Renderer.Device, stream);
                        }
                        catch (Exception ex)
                        {
                            stream.Close();
                            Log.Write(LogLevel.Error, ex.ToString());
                            return false;
                        }

                        stream.Close();
                        return true;
                    }
                case ".dds":
                    {
                        var stream = FileSystem.OpenCopy(Name);

                        try
                        {
                            Texture2D texture;
                            DDSLib.DDSFromStream(stream, Renderer.Device, 0, true, out texture);

                            Texture = texture;
                        }
                        catch (Exception ex)
                        {
                            stream.Close();
                            Log.Write(LogLevel.Error, ex.ToString());
                            return false;
                        }

                        stream.Close();
                        return true;
                    }
            }

            Log.Write(LogLevel.Error, "Image {0} - not of a supported type", Name);

            return false;
        }
    }
}
