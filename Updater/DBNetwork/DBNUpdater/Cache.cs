using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Xml.Linq;
using System.IO;
namespace DBNUpdater
{
    public class Caches
    {
        //bit simple
        private XDocument _xml;
        public Caches(XDocument xml)
        {
            _xml = xml;
        }
        public Dictionary<string, int> Versions()
        {
            var IE = from cache in _xml.Element("Caches").Elements()
                           select new KeyValuePair<string, int>(
                               cache.Attribute("ID").Value,
                               int.Parse(cache.Attribute("Version").Value)
                           );
            var versions = IE.ToDictionary(kvp=>kvp.Key,kvp=>kvp.Value);
            return versions;
        }
        public override string ToString()
        {
            return _xml.ToString();
        }
    }

    public class CacheInfo
    {
        private XDocument _xml;
        private string _location;
        private string _cache;
        private Updater _updater;
        public CacheInfo(Updater updater, string location, string cache, XDocument xml)
        {
            _updater = updater;
            _cache = cache;
            _location = location;
            _xml = xml;
        }
        public List<CacheContentFile> Files()
        {
            var IE = from cacheinfo in _xml.Element("CacheInfo").Elements()
                        select new CacheContentFile(_location,
                                                    _cache,
                                                    cacheinfo.Attribute("Name").Value,
                                                    cacheinfo.Attribute("SHA1Hash").Value,
                                                    cacheinfo.Attribute("Size").Value,
                                                    cacheinfo.Attribute("CompressedSize").Value
                        );
            var files = IE.ToList();
            for (var i = 0; i < files.Count; i++)
            {
                if (!files[i].NeedsUpdate())
                {
                    _updater.SendStatus(StatusChangedEnum.NoUpdateNeeded, files[i].Name, 0);
                    files.RemoveAt(i);
                    i--;
                }
            }
            return files;
        }
    }

    public class CacheContentFile
    {
        private string _location;
        public CacheContentFile(string location, string cache, string name, string sha1, string size, string compressed)
        {
            _location = location;
            Cache = cache;
            Name = name;
            SHA1Hash = sha1;
            Size = int.Parse(size);
            CompressedSize = int.Parse(compressed);
        }
        public bool NeedsUpdate()
        {
            var path = Path.Combine(_location, Name);
            //check if exists
            if (!File.Exists(path)) return true;
            //check if size is the same
            var fi = new FileInfo(path);
            if (fi.Length != Size) return true;
            //check if hash is the same
            if (Utilities.GetFileSHA1(path) != SHA1Hash) return true;
            Log.Info(String.Format("{0} does not need updating", Name));
            return false;
        }

        public string Cache { get; set; }
        public long Size { get; set; }
        public long CompressedSize { get; set; }
        public string Name { get; set; }
        public string SHA1Hash { get; set; }
    }
}