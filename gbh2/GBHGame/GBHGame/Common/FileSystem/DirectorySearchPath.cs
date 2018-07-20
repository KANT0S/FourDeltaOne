using System.Collections.Generic;
using System.IO;

namespace GBH
{
    public class DirectorySearchPath : ISearchPath
    {
        private string _directory;

        public DirectorySearchPath(string directory)
        {
            _directory = directory;

            if (_directory.EndsWith("\\") || _directory.EndsWith("/"))
            {
                _directory = _directory.Substring(0, _directory.Length - 1);
            }
        }

        public string Description
        {
            get
            {
                return _directory;
            }
        }

        private string BuildPath(string path)
        {
            // FIXME: security considerations?
            return Path.Combine(_directory, (path.StartsWith("/")) ? path.Substring(1) : path);
        }

        public IEnumerable<string> ListFiles(string sub, string extension)
        {
            var directory = BuildPath(sub);

            if (!Directory.Exists(directory))
            {
                return new List<string>(0);
            }

            var files = Directory.GetFiles(directory, "*." + extension);
            var retval = new List<string>();

            foreach (var file in files)
            {
                var ifile = file.Replace(_directory, "").Replace('\\', '/');
                retval.Add(ifile);
            }

            return retval;
        }

        public bool FileExists(string path)
        {
            return File.Exists(BuildPath(path));
        }

        public Stream OpenRead(string path)
        {
            return File.OpenRead(BuildPath(path));
        }
    }
}
