using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GBH
{
    public static class FileSystem
    {
        private static List<ISearchPath> _searchPaths = new List<ISearchPath>();

        public static void Initialize()
        {
            // initializes the filesystem
            Log.Write(LogLevel.Info, "Initializing filesystem...");

            // add game search directories
            AddSearchDirectory(Path.Combine(Environment.CurrentDirectory, "Data"));

            // list the existing search paths
            DumpSearchPaths();
        }

        public static IEnumerable<string> ListFiles(string subdir, string extension)
        {
            var returnList = new List<string>();

            foreach (var searchPath in _searchPaths)
            {
                var files = searchPath.ListFiles(subdir, extension);

                foreach (var file in files)
                {
                    if (!returnList.Contains(file))
                    {
                        returnList.Add(file);
                    }
                }
            }

            return returnList;
        }

        public static bool FileExists(string path)
        {
            foreach (var searchPath in _searchPaths)
            {
                if (searchPath.FileExists(path))
                {
                    return true;
                }
            }

            return false;
        }

        public static Stream OpenRead(string path)
        {
            foreach (var searchPath in _searchPaths)
            {
                if (searchPath.FileExists(path))
                {
                    return searchPath.OpenRead(path);
                }
            }

            return null;
        }

        public static MemoryStream OpenCopy(string path)
        {
            var stream = OpenRead(path);

            if (stream != null)
            {
                var memorystream = new MemoryStream();
                stream.CopyTo(memorystream);
                stream.Close();

                memorystream.Position = 0;

                return memorystream;
            }

            return null;
        }

        public static void DumpSearchPaths()
        {
            Log.Write(LogLevel.Info, "{0} search paths", _searchPaths.Count);

            var i = 0;

            foreach (var path in _searchPaths)
            {
                Log.Write(LogLevel.Info, "{0}: {1}", i, path.Description);

                i++;
            }
        }

        public static void AddSearchDirectory(string path)
        {
            // add the search path to the list
            var directorySearchPath = new DirectorySearchPath(path);

            _searchPaths.Add(directorySearchPath);

            // search for .pak files
            // TODO: find out if OrderBy is needed
            var files = Directory.GetFiles(path, "*.pak").OrderByDescending(p => p);

            foreach (var file in files)
            {
                var zipSearchPath = new ZipSearchPath(file);
                _searchPaths.Add(zipSearchPath);
            }
        }
    }
}
