using System.Collections.Generic;
using System.IO;

namespace GBH
{
    public interface ISearchPath
    {
        bool FileExists(string path);
        Stream OpenRead(string path);

        IEnumerable<string> ListFiles(string sub, string extension);

        string Description
        {
            get;
        }
    }
}
