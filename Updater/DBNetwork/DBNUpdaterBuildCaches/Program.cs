using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Xml.Linq;
using System.Xml;
using System.IO;
using SevenZip.Compression;
using System.Security.Cryptography;
using System.Reflection;
namespace BuildCaches
{
    class CacheContentFile
    {
        public long Size;
        public long CompressedSize;
        public string SHA1Hash;
    }
    class Program
    {
        
        static bool askWhetherToCompress = false;
        static void Main(string[] args)
        {
            //some logical steps to help my mind code :>
            //1. read Caches folder, if a caches.xml file is found, go to step 3, if not, 2
            //2. get all the directories inside the Caches folder, add those to a list with version number being 0
            //3. get all the caches inside the cache file, add those to a list with its version number
            //4. read all cache folders recursively, if an info.xml file exists, use that file to verify if any files have changed.
            //5. any files that haven't changed (or all if an info.xml file doesn't exist) or are new should be compressed and added to the info.xml file
            //6. build the caches.xml file with an incremented version (if the info.xml file has changed)
            //7. copy all files to be uploaded (caches.xml + directories with info.xml and only files to be uploaded (lzma's)) to the CacheUpload folder

            Environment.CurrentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

            var cf = CombinePaths(Environment.CurrentDirectory, "Caches", "caches.xml");
            try
            {
                Directory.Delete(CombinePaths(Environment.CurrentDirectory, "CachesUpload"), true);
            }
            catch (DirectoryNotFoundException)
            {
            
            }

            var cacheDict = new SortedDictionary<string, int>();
            var cacheChanged = new SortedDictionary<string, bool>();
            if(File.Exists(cf))
            {
                foreach (var el in XDocument.Parse(File.ReadAllText(cf, Encoding.UTF8)).Element("Caches").Elements())
                {
                    cacheDict.Add(el.Attribute("ID").Value, int.Parse(el.Attribute("Version").Value));
                }

            }
            foreach(var dir in Directory.GetDirectories(Path.Combine(Environment.CurrentDirectory, "Caches")))
            {
               if(!cacheDict.ContainsKey(Path.GetFileName(dir)))
               {
                   cacheDict.Add(Path.GetFileName(dir), 0);
               }
            }
            foreach(var keyval in cacheDict)
            {
                var changed = false;
                var CCFList = new SortedDictionary<string, CacheContentFile>();
                var infoXML = new XDocument();
                var currentFolder = CombinePaths(Environment.CurrentDirectory, "Caches", keyval.Key);
                if(File.Exists(CombinePaths(currentFolder, "info.xml")))
                {
                    foreach (var el in XDocument.Parse(File.ReadAllText(CombinePaths(currentFolder, "info.xml"), Encoding.UTF8)).Element("CacheInfo").Elements())
                    {
                        var name = el.Attribute("Name").Value;
                        if(File.Exists(CombinePaths(currentFolder, name)))
                        {
                            var size = long.Parse(el.Attribute("Size").Value);
                            var sizeCompressed = long.Parse(el.Attribute("CompressedSize").Value);
                            var hash = el.Attribute("SHA1Hash").Value;
                            CCFList.Add(name, new CacheContentFile
                                                  {
                                                      SHA1Hash = hash,
                                                      Size = size,
                                                      CompressedSize = sizeCompressed
                                                  });
                        }
                    }
                }
                foreach (var file in GetFilesRecursive(currentFolder))
                {
                    var afile = file.Replace(currentFolder, "").Substring(1).Replace("\\", "/"); //remove current folder and the slash, replace \ with / for linux (still works on windows)

                    if (Path.GetFileName(afile) == "info.xml" || Path.GetExtension(afile) == ".lzma") continue; //both files shouldn't be here!!!1
                    var length = new FileInfo(Path.Combine(currentFolder, afile)).Length;
                    var lengthCompressed = length;
                    var hash = GetFileSHA1(Path.Combine(currentFolder, afile));
                    bool compressed = true;
                    if (!CCFList.ContainsKey(afile) || CCFList[afile].SHA1Hash != hash || CCFList[afile].Size != length)
                    {
                        changed = true;
                        //doesn't exist in info.xml, hash changed or size changed
                        var pathToGo = CombinePaths(Environment.CurrentDirectory, "CacheUpload", keyval.Key, afile);
                        if (length < 75 * 1024 * 1024) //bigger than 75mb = no compress (should work for most situations)
                        {
                            var lz = new LZMACoder();
                            var inStream = File.Open(Path.Combine(currentFolder, afile), FileMode.Open, FileAccess.Read);
                            Directory.CreateDirectory(Path.GetDirectoryName(pathToGo));
                            var outStream = File.Open(pathToGo + ".lzma", FileMode.Create, FileAccess.Write);
                            try
                            {
                                Console.WriteLine("Compressing {0} to {1}....", Path.Combine(currentFolder, afile), pathToGo + ".lzma");
                                lz.Compress(inStream, outStream);
                                
                                inStream.Close();
                                outStream.Close();
                                lengthCompressed = new FileInfo(pathToGo + ".lzma").Length;
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine("Failed to compress {0}, {1}", file, e.ToString());
                                inStream.Close();
                                outStream.Close();
                                compressed = false;
                                File.Delete(pathToGo + ".lzma");
                            }
                        }
                        if(!compressed)
                        {
                            File.Copy(Path.Combine(currentFolder, afile), pathToGo, true);
                        }
                        if (CCFList.ContainsKey(afile))
                        {
                            CCFList[afile].Size = length;
                            CCFList[afile].CompressedSize = lengthCompressed;
                            CCFList[afile].SHA1Hash = hash;
                        }
                        else
                        {
                            CCFList.Add(afile, new CacheContentFile
                                                   {
                                                       Size = length,
                                                       CompressedSize = lengthCompressed,
                                                       SHA1Hash = hash
                                                   });
                        }
                    }
                }
                infoXML = new XDocument(
                    new XElement("CacheInfo")
                );

                foreach(var keyval2 in CCFList)
                {
                    infoXML.Element("CacheInfo").Add(
                        new XElement("CacheFile",
                            new XAttribute("Name", keyval2.Key),
                            new XAttribute("Size", keyval2.Value.Size.ToString()),
                            new XAttribute("CompressedSize", keyval2.Value.CompressedSize.ToString()),
                            new XAttribute("SHA1Hash", keyval2.Value.SHA1Hash)
                        )
                    );
                }
                cacheChanged.Add(keyval.Key, changed);
              //  using (XmlTextWriter writer = new XmlTextWriter(Path.Combine(currentFolder, "info.xml"), null))
              //  {
              //      infoXML.Save(writer);
             ////   }
                Store(infoXML, Path.Combine(currentFolder, "info.xml"));
                Directory.CreateDirectory(Path.GetDirectoryName(CombinePaths(Environment.CurrentDirectory, "CacheUpload", keyval.Key, "info.xml")));
                File.Copy(Path.Combine(currentFolder, "info.xml"), CombinePaths(Environment.CurrentDirectory, "CacheUpload", keyval.Key, "info.xml"), true);
            }
            var caches = new XDocument(
                new XElement("Caches")
            );
            foreach(var keyval3 in cacheDict)
            {
                caches.Element("Caches").Add(
                    new XElement("Cache",
                        new XAttribute("ID", keyval3.Key),
                        new XAttribute("Version", (keyval3.Value + (cacheChanged[keyval3.Key] ? 1 : 0)).ToString())
                    )
                );
            }
          //  using (XmlTextWriter writer = new XmlTextWriter(CombinePaths(Environment.CurrentDirectory, "Caches", "caches.xml"), null))
          //  {
          //      caches.Save(writer);
         //   }
            Store(caches, CombinePaths(Environment.CurrentDirectory, "Caches", "caches.xml"));
            Directory.CreateDirectory(Path.GetDirectoryName(CombinePaths(Environment.CurrentDirectory, "Caches", "caches.xml")));
            File.Copy(CombinePaths(Environment.CurrentDirectory, "Caches", "caches.xml"), CombinePaths(Environment.CurrentDirectory, "CacheUpload", "caches.xml"), true);
        }
        static string GetFileSHA1(string fileName)
        {
            return GetFileHash(new SHA1Managed(), fileName).ToUpper();
        }
        static string GetFileHash(HashAlgorithm algorithm, string fileName)
        {
            var stream = File.OpenRead(fileName);
            var hashResult = algorithm.ComputeHash(stream);
            stream.Close();

            return BitConverter.ToString(hashResult).Replace("-", "");
        }
        static string CombinePaths(string first, params string[] others)
        {
            // from http://stackoverflow.com/questions/760612/how-can-path-combine-be-used-with-more-than-2-arguments

            // Put error checking in here :)
            string path = first;
            foreach (string section in others)
            {
                path = Path.Combine(path, section);
            }
            return path;
        }
        static List<string> GetFilesRecursive(string b)
        {
            //probably better ways to do it, but pssh
            //from http://www.dotnetperls.com/recursively-find-files

            // 1.
            // Store results in the file results list.
            List<string> result = new List<string>();

            // 2.
            // Store a stack of our directories.
            Stack<string> stack = new Stack<string>();

            // 3.
            // Add initial directory.
            stack.Push(b);

            // 4.
            // Continue while there are directories to process
            while (stack.Count > 0)
            {
                // A.
                // Get top directory
                string dir = stack.Pop();

                try
                {
                    // B
                    // Add all files at this directory to the result List.
                    result.AddRange(Directory.GetFiles(dir, "*.*"));

                    // C
                    // Add all directories at this directory.
                    foreach (string dn in Directory.GetDirectories(dir))
                    {
                        stack.Push(dn);
                    }
                }
                catch
                {
                    // D
                    // Could not open the directory
                }
            }
            return result;
        }

        static void Store(XDocument xml, string filename)
        {
            var stream = File.Open(filename, FileMode.Create);
            var writer = new StreamWriter(stream, new UTF8Encoding(false));
            xml.Save(writer);
            stream.Close();
        }
    }
}
