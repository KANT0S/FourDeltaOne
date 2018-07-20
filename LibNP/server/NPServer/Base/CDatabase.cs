using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

using DreamSeat;

namespace NPx
{
    public static class CDatabase
    {
        public static CouchClient Client { get; set; }
        public static CouchDatabase Database { get; set; }

        public static void Start()
        {
            Log.Info("CDatabase starting...");

            Client = new CouchClient("127.0.0.1");

            if (!Client.HasDatabase("xnp_files"))
            {
                Client.CreateDatabase("xnp_files");
            }

            Database = Client.GetDatabase("xnp_files");

            Log.Info("CDatabase started...");

            MigrateFiles();

            Log.Info("CDatabase completed.");
        }

        private static void MigrateFiles()
        {
            if (File.Exists("data/migration_files_002_done"))
            {
                return;
            }

            //var files = Directory.GetFiles("data/priv2", "*.*", SearchOption.AllDirectories);
            var directories = Directory.GetDirectories("data/priv2/00");

            foreach (var dir in directories)
            {
                var subdirectories = Directory.GetDirectories(dir);

                foreach (var subdir in subdirectories)
                {
                    var files = Directory.GetFiles(subdir);

                    foreach (var file in files)
                    {
                        var match = Regex.Match(file.Replace("\\", "/"), "00/([0-9a-f]{3})/([0-9a-f]{16})/(.*)$");

                        if (!match.Success)
                        {
                            continue;
                        }

                        if (file.Contains("iw4_"))
                        {
                            continue;
                        }

                        var npid = long.Parse(match.Groups[2].Value, System.Globalization.NumberStyles.AllowHexSpecifier);
                        var filename = match.Groups[3].Value;

                        var id = npid.ToString("x16") + "_" + filename;
                        var isNew = true;
                        var npfile = new NPFile();

                        if (Database.DocumentExists(id))
                        {
                            isNew = false;
                            npfile = Database.GetDocument<NPFile>(id);
                        }

                        npfile.Id = id;
                        npfile.SetData(File.ReadAllBytes(file));

                        if (isNew)
                        {
                            Database.CreateDocument<NPFile>(npfile);
                        }
                        else
                        {
                            Database.UpdateDocument<NPFile>(npfile);
                        }

                        Console.WriteLine(id);
                    }
                }
            }

            File.WriteAllText("data/migration_files_002_done", DateTime.Now.ToString());
        }
    }

    public class NPFile : CouchDocument
    {
        public string FileData { get; set; }

        public NPFile()
        {
            
        }

        public NPFile(byte[] data)
        {
            FileData = Convert.ToBase64String(data);
        }

        public void SetData(byte[] data)
        {
            FileData = Convert.ToBase64String(data);
        }
    }
}
