using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public static class MaterialManager
    {
        private static Dictionary<string, Material> _materials = new Dictionary<string, Material>();

        public static void ReadMaterialFile(string path)
        {
            // read out the file
            var stream = FileSystem.OpenRead(path);
            var reader = new StreamReader(stream);
            var data = reader.ReadToEnd();
            reader.Close();

            // create a new tokenizer
            var tokenizer = new Tokenizer(data);
            
            while (true)
            {
                var token = tokenizer.ReadToken();

                if (token == "")
                {
                    break;
                }

                var materialName = token;

                var material = new Material(materialName);
                if (material.Initialize(tokenizer))
                {
                    _materials.Add(materialName.ToLowerInvariant(), material);
                    _materials[material.BaseName] = material; // add a basename variant as well; the UV in such should not be used
                }
            }
        }

        public static Material FindMaterial(string name)
        {
            if (_materials.ContainsKey(name))
            {
                return _materials[name];
            }

            return _materials["default"];
        }
    }

    public class Material
    {
        public string Name { get; private set; }
        public Image DiffuseImage { get; private set; }
        public Image NormalImage { get; private set; }
        public Vector3 DiffuseColor { get; private set; }
        public bool UseVertexColors { get; private set; }
        
        // a name to use to identify the set of textures/parameters without the atlas specifiers
        public string BaseName { get; private set; }
        
        private string _texturePath;
        private string _normalPath;

        // uv coordinate range
        private float _x1, _x2, _y1, _y2;

        public Material(string name)
        {
            Name = name;

            _x1 = 0.0f;
            _y1 = 0.0f;
            _x2 = 1.0f;
            _y2 = 1.0f;

            BaseName = "";

            DiffuseColor = new Vector3(1.0f, 1.0f, 1.0f);
        }

        public bool Initialize(Tokenizer tokenizer)
        {
            try
            {
            var token = tokenizer.ReadToken();

            if (token != "{")
            {
                Log.Write(LogLevel.Error, "Material {0} - expected '{{', found '{1}'", Name, token);
                return false;
            }

            while (true)
            {
                token = tokenizer.ReadToken();

                if (token == "")
                {
                    Log.Write(LogLevel.Error, "Material {0} - no closing '}'", Name);
                    return false;
                }

                if (token == "}")
                {
                    break;
                }

                switch (token)
                {
                    case "texture":
                        _texturePath = tokenizer.ReadToken(false);

                        BaseName += "_" + _texturePath;

                        DiffuseImage = ImageManager.FindImage(_texturePath);

                        if (DiffuseImage == null)
                        {
                            Log.Write(LogLevel.Error, "Material {0} - failed to load diffuse image {1}", Name, _texturePath);
                            return false;
                        }
                        break;
                    case "normalTexture":
                        _normalPath = tokenizer.ReadToken(false);

                        BaseName += "_n" + _normalPath;

                        NormalImage = ImageManager.FindImage(_normalPath);

                        if (NormalImage == null)
                        {
                            Log.Write(LogLevel.Error, "Material {0} - failed to load normal image {1}", Name, _normalPath);
                            return false;
                        }
                        break;
                    case "diffuseColor":
                        Vector3 color = new Vector3(float.Parse(tokenizer.ReadToken(false)), float.Parse(tokenizer.ReadToken(false)), float.Parse(tokenizer.ReadToken(false)));

                        DiffuseColor = color;

                        BaseName += "_dc";
                        break;
                    case "useVertexColors":
                        UseVertexColors = true;

                        BaseName += "_uvc";
                        break;
                    case "uv":
                        var x1s = tokenizer.ReadToken(false);
                        var y1s = tokenizer.ReadToken(false);
                        var x2s = tokenizer.ReadToken(false);
                        var y2s = tokenizer.ReadToken(false);

                        float x1, x2, y1, y2;

                        if (!float.TryParse(x1s, out x1))
                        {
                            Log.Write(LogLevel.Error, "Material {0} - couldn't parse 'uv' value {1}", Name, x1s);
                            return false;
                        }

                        if (!float.TryParse(x2s, out x2))
                        {
                            Log.Write(LogLevel.Error, "Material {0} - couldn't parse 'uv' value {1}", Name, x2s);
                            return false;
                        }

                        if (!float.TryParse(y1s, out y1))
                        {
                            Log.Write(LogLevel.Error, "Material {0} - couldn't parse 'uv' value {1}", Name, y1s);
                            return false;
                        }

                        if (!float.TryParse(y2s, out y2))
                        {
                            Log.Write(LogLevel.Error, "Material {0} - couldn't parse 'uv' value {1}", Name, y2s);
                            return false;
                        }

                        _x1 = x1;
                        _y1 = y1;
                        _x2 = x2;
                        _y2 = y2;
                        break;
                }
            }

            if (_texturePath == "")
            {
                Log.Write(LogLevel.Error, "Material {0} - no texture path set", Name);
                return false;
            }

            return true;
            }
            catch (Exception e)
            {
                Log.Write(LogLevel.Error, "Material {0} - {1}", Name, e.Message);

                return false;
            }
        }

        public Vector2 TransformUV(Vector2 uv)
        {
            // first scale the coordinates
            // TODO: wrapping/clamping?
            var x = uv.X * (_x2 - _x1);
            var y = uv.Y * (_y2 - _y1);

            // then add the offset
            x += _x1;
            y += _y1;

            // and return the value
            return new Vector2(x, y);
        }
    }
}
