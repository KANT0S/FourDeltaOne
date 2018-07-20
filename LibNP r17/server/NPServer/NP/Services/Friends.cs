using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

using NPx;

namespace NPx
{
    public partial class RPCFriendsGetProfileDataMessage : NPRPCMessage<FriendsGetProfileDataMessage>
    {
        public override void Process(NPHandler client)
        {
            var npids = (from npid in Message.npids
                         select (int)(npid & 0xFFFFFFFF)).ToArray();

            var db = XNP.Create();

            var profiles = from profile in db.IW4Profiles
                           where npids.Contains(profile.UserID)
                           select profile;

            var reply = MakeResponse<FriendsGetProfileDataResultMessage>(client);

            foreach (var profile in profiles)
            {
                var profileResult = new ProfileDataResult { npid = (ulong)((uint)profile.UserID | 0x110000100000000) };
                profileResult.profile = new byte[8];
                Array.Copy(BitConverter.GetBytes(profile.Experience.Value), 0, profileResult.profile, 0, 4);
                Array.Copy(BitConverter.GetBytes((uint)profile.Prestige.Value), 0, profileResult.profile, 4, 4);

                reply.Message.results.Add(profileResult);
            }

            reply.Send();
        }
    }

    public partial class RPCFriendsSetSteamIDMessage : NPRPCMessage<FriendsSetSteamIDMessage>
    {
        public override void Process(NPHandler client)
        {
            var platformID = (long)Message.steamID;
            var userID = (int)(client.NPID & 0xFFFFFFFF);
            var gdb = XNP.Create();

            var currentLink = from link in gdb.ExternalPlatforms
                              where link.UserID == userID// && link.PlatformType == "steam"
                              select link;

            var newLink = new ExternalPlatforms();

            if (currentLink.Count() > 0)
            {
                newLink = currentLink.First();

                if (newLink.PlatformID == platformID && newLink.PlatformAuthenticated == 1)
                {
                    return;
                }

                //gdb.ExternalPlatforms.DeleteOnSubmit(newLink);
                gdb.SubmitChanges();

                newLink = new ExternalPlatforms();
            }

            newLink.UserID = userID;
            newLink.PlatformType = "steam";
            newLink.PlatformID = platformID;
            newLink.PlatformAuthenticated = 0;

            SteamFriendsLinker.UpdateForUser(userID, platformID);

            if (currentLink.Count() == 0)
            {
                gdb.ExternalPlatforms.InsertOnSubmit(newLink);
            }

            gdb.SubmitChanges();
            
            ThreadPool.QueueUserWorkItem(delegate(object stateo)
            {
                var state = (SteamAuthCheckState)stateo;
                var result = 3; // error occurred, magic numbers ftw

                try
                {
                    var wc = new WebClient();
                    var gameList = wc.DownloadString(string.Format("http://steamcommunity.com/profiles/{0}/games?xml=1", state.platformID));

                    if (gameList.Contains("is private"))
                    {
                        result = 4;
                    }

                    // we don't parse xml here, laziness...
                    var ownsTheGame = gameList.Contains(">10190<") || gameList.Contains(">42690<"); // >< is to prevent accidental hashes containing 10190

                    result = (ownsTheGame) ? 0 : 2; // 2 = doesn't own the game

                    // update the database too based on this
                    var db = XNP.Create();

                    var id = (state.client.NPID & 0xFFFFFFFF);

                    var clinks = from link in db.ExternalPlatforms
                                 where link.UserID == id
                                 select link;

                    if (clinks.Count() > 0)
                    {
                        var clink = clinks.First();
                        clink.PlatformAuthenticated = (sbyte)((ownsTheGame) ? 1 : 0);

                        db.SubmitChanges();
                    }
                }
                catch (Exception ex)
                {
                    Log.Error(ex.ToString());
                }

                Thread.Sleep(500);

                try
                {
                    var linkReply = new NPRPCResponse<AuthenticateExternalStatusMessage>(client);
                    linkReply.Message.status = result;
                    linkReply.Send();
                }
                catch (Exception ex)
                {
                    Log.Error(ex.ToString());
                }
            }, new SteamAuthCheckState()
            {
                client = client,
                platformID = platformID
            });
        }

        private class SteamAuthCheckState
        {
            public NPHandler client;
            public long platformID;
        }
    }
}
