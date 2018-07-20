using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using CookComputing.XmlRpc;

namespace DWServer
{
    public class DWWebAPIService : XmlRpcHttpServerProtocol
    {
        private string PassPhrase = "USAIsDominate";

        [XmlRpcMethod("ao.get-wad", Description="Retrieves the specified WAD file.")]
        public string GetWAD(string filename, string password)
        {
            if (password != PassPhrase)
                return "";

            var WADFile = "data/pub/" + filename + ".wad";

            if (!File.Exists(WADFile))
                return "";

            return Convert.ToBase64String(File.ReadAllBytes(WADFile));
        }

        [XmlRpcMethod("ao.add-wad", Description="Add a new WAD file and flush the pub cache.")]
        public bool AddWAD(string filename, string fileData, string password)
        {
            if (password != PassPhrase)
                return false;

            byte[] FileData = Convert.FromBase64String(fileData);
            File.WriteAllBytes(@"data/pub/" + filename, FileData);

            DWStorage.FlushPublisherFiles();

            return true;
        }

        [XmlRpcMethod("ao.get-motd", Description = "Retrieves the MOTD.")]
        public string GetMOTD(string language, string password)
        {
            if (password != PassPhrase)
                return "Invalid Password";

            var WADFile = "data/pub/online_tu12_mp_" + language + ".wad";

            if (!File.Exists(WADFile))
                return "Error";

            WAD.TWADFile ReqFile = WAD.Manager.GetFileFromWAD("motd-mp-" + language + ".txt", language);

            if (ReqFile.FileName == "error")
                return "Error";

            return Convert.ToBase64String(ReqFile.Content);
        }


        [XmlRpcMethod("ao.set-motd", Description = "Sets the MOTD.")]
        public bool SetMOTD(string data, string language, string password)
        {
            if (password != PassPhrase)
                return false;

            WAD.TWADFile WADFile = new WAD.TWADFile(Convert.FromBase64String(data), "motd-mp-" + language + ".txt");

            DWStorage.FlushPublisherFiles();

            return WAD.Manager.AddFileToWAD(WADFile, language);
        }

    }
}
