using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using CookComputing.XmlRpc;

namespace NPx
{
    public class NPWebAPIService : XmlRpcHttpServerProtocol
    {
        [XmlRpcMethod("aiw.get-stats", Description="Gets stat data for a specified user.")]
        public string GetStats(string userID, string titleID)
        {
            var filename = "";

            if (titleID == "iw4")
            {
                filename = "iw4.stat";
            }
            else if (titleID == "iw5")
            {
                filename = "mpdata";
            }

            var npid = (uint.Parse(userID) | 0x110000100000000);
            var fsFile = StorageUtils.GetFilename(filename, npid);

            if (!File.Exists(fsFile))
            {
                return "";
            }

            return Convert.ToBase64String(File.ReadAllBytes(fsFile));
        }

        [XmlRpcMethod("aiw.set-stats", Description = "Sets stat data for a specified user.")]
        public bool SetStats(string userID, string titleID, string base64Data)
        {
            try
            {
                var filename = "";

                if (titleID == "iw4")
                {
                    filename = "iw4.stat";
                }
                else if (titleID == "iw5")
                {
                    filename = "mpdata";
                }

                var npid = (uint.Parse(userID) | 0x110000100000000);
                var fsFile = StorageUtils.GetFilename(filename, npid);

                var data = Convert.FromBase64String(base64Data);

                if (!File.Exists(fsFile))
                {
                    return false;
                }

                string backupFile = null;
                //var result = RPCStorageWriteUserFileMessage.PrepareWriteFile(npid, "iw4.stat", data, out backupFile);
                var result = false;

                if (!result)
                {
                    return false;
                }

                // recalculate checksum
                var checksumBytes = new byte[data.Length - 4];
                Array.Copy(data, 4, checksumBytes, 0, checksumBytes.Length);
                var checksum = Crc32.Compute(checksumBytes);

                Array.Copy(BitConverter.GetBytes(checksum), 0, data, 0, 4);

                try
                {
                    File.WriteAllBytes(fsFile, data);
                }
                catch (Exception ex)
                {
                    Log.Error(ex.ToString());
                    return false;
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
                throw ex;
            }

            return true;
        }

        [XmlRpcMethod("aiw.add-key", Description="Add a server license key to the allowed key list.")]
        public bool AddKey(string key, string password)
        {
            if (password != "678452056")
            {
                return false;
            }

            var licenseKey = new LicenseKeys()
            {
                Key = key,
                Type = 2
            };

            var database = XNP.Create();
            database.LicenseKeys.InsertOnSubmit(licenseKey);
            database.SubmitChanges();

            return true;
        }

        [XmlRpcMethod("match.get-match", Description="Gets a recorded match")]
        public MatchXmlData GetMatch(string matchGuid)
        {
            var database = XNP.Create();
            var matches = from m in database.Matches // m? MINEN!
                          where m.MatchGuiD == matchGuid
                          select m;

            if (matches.Count() == 0)
            {
                throw new Exception("No such match exists.");
            }

            var match = matches.First();

            var affectedPlayers = from u in database.MatchUsers
                                  where u.MatchID == match.MatchGuiD
                                  select u.MatchUser;
            
            var matchData = new MatchXmlData();
            matchData.guid = match.MatchGuiD;
            matchData.time = match.MatchTime;
            matchData.map = match.MatchMap;
            matchData.gameType = match.MatchGameType;
            matchData.data = Convert.ToBase64String(File.ReadAllBytes(match.MatchFilename));
            matchData.players = affectedPlayers.ToArray();

            return matchData;
        }

        [XmlRpcMethod("match.get-recent-matches", Description = "Gets recent matches for a user")]
        public MatchMetaData[] GetRecentMatches(string userID, int numMatches)
        {
            if (numMatches > 25)
            {
                throw new ArgumentException("a maximum of 25 matches can be returned");
            }

            var user = int.Parse(userID);
            var database = XNP.Create();
            var matches = (from match in database.Matches
                           join matchUser in database.MatchUsers on match.MatchGuiD equals matchUser.MatchID
                           where matchUser.MatchUser == user
                           orderby match.MatchTime descending
                           select new MatchMetaData()
                           {
                               guid = match.MatchGuiD,
                               time = match.MatchTime,
                               map = match.MatchMap,
                               gameType = match.MatchGameType
                           }).Take(numMatches);

            return matches.ToArray();
        }
    }

    public class MatchMetaData
    {
        public string gameType;
        public string map;
        public int time;
        public string guid;
    }

    public class MatchXmlData : MatchMetaData
    {
        public string data;
        public int[] players;
    }
}
