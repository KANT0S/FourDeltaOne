using System.Collections.Generic;
using System.IO;

using Ionic.Zip;

namespace GBH
{
    public class ZipSearchPath : ISearchPath
    {
        private ZipFile _file;
        private string _fileName;
        private int _numFiles;

        public ZipSearchPath(string zip)
        {
            _file = new ZipFile(zip);
            _fileName = zip;
            _numFiles = _file.Entries.Count;
        }

        public string Description
        {
            get
            {
                return string.Format("{0} ({1} files)", _fileName, _numFiles);
            }
        }

        private string NormalizeSeparators(string path)
        {
            var retval = "";

            foreach (var cha in path)
            {
                var ch = cha;

                if (ch == '\\')
                {
                    ch = '/';
                }

                if (ch == '/')
                {
                    if (retval.EndsWith("/"))
                    {
                        continue;
                    }
                }

                retval += ch;
            }

            return retval;
        }

        private string BuildPath(string path)
        {
            path = NormalizeSeparators(path);

            if (path.StartsWith("/"))
            {
                path = path.Substring(1);
            }

            return path;
        }

        public IEnumerable<string> ListFiles(string sub, string extension)
        {
            if (!sub.EndsWith("/"))
            {
                sub = sub + "/";
            }

            var list = new List<string>();

            foreach (var entry in _file.Entries)
            {
                if (entry.IsDirectory)
                {
                    continue;
                }

                if (entry.FileName.StartsWith(sub))
                {
                    if (entry.FileName.EndsWith("." + extension))
                    {
                        list.Add("/" + entry.FileName);
                    }
                }
            }

            return list;
        }

        public bool FileExists(string path)
        {
            var pathLower = BuildPath(path).ToLowerInvariant();

            foreach (var entry in _file.Entries)
            {
                if (entry.IsDirectory)
                {
                    continue;
                }

                if (entry.FileName.ToLowerInvariant() == pathLower)
                {
                    return true;
                }
            }

            return false;
        }

        public Stream OpenRead(string path)
        {
            var pathLower = BuildPath(path).ToLowerInvariant();

            foreach (var entry in _file.Entries)
            {
                if (entry.IsDirectory)
                {
                    continue;
                }

                if (entry.FileName.ToLowerInvariant() == pathLower)
                {
                    return entry.OpenReader();
                }
            }

            return null;
        }
    }
}
