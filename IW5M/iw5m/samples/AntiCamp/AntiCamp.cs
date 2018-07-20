using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using InfinityScript;

namespace InfinityScript.Examples
{
    public class AntiCamp : BaseScript
    {
        private bool _donePrematch = false;
        private List<Entity> _safeTriggers = new List<Entity>();

        public AntiCamp()
        {
            var gameType = Call<string>("getDvar", "g_gametype").ToLower();

            for (int i = 0; i < 2048; i++)
            {
                var entity = Call<Entity>("getEntByNum", i);

                if (entity != null)
                {
                    var targetname = entity.GetField<string>("targetname");

                    if (gameType == "dom")
                    {
                        if (targetname == "flag_primary" || targetname == "flag_secondary")
                        {
                            _safeTriggers.Add(entity);
                        }
                    }
                    else if (gameType == "koth")
                    {
                        if (targetname == "radiotrigger")
                        {
                            _safeTriggers.Add(entity);
                        }
                    }
                }
            }

            // only trigger anticamp after the game started
            OnNotify("prematch_done", () =>
            {
                _donePrematch = true;
            });

            // don't trigger anticamp after the game ended
            OnNotify("game_over", () =>
            {
                _donePrematch = false;
            });

            PlayerConnecting += new Action<Entity>(entity =>
            {
                // bombsite using
                entity.SetField("ac_using", 0);

                entity.OnNotify("use_hold", player =>
                {
                    player.SetField("ac_using", 1);
                });

                entity.OnNotify("done_using", player =>
                {
                    player.SetField("ac_using", 0);
                });

                entity.OnInterval(4000, player =>
                {
                    if (!player.IsAlive)
                    {
                        return true;
                    }

                    if (!_donePrematch)
                    {
                        return true;
                    }

                    if (player.GetField<int>("ac_using") != 0)
                    {
                        return true;
                    }

                    foreach (var safeEntity in _safeTriggers)
                    {
                        if (player.Call<int>("istouching", safeEntity) != 0)
                        {
                            Log.Write(LogLevel.Trace, "touching safe entity");
                            return true;
                        }
                    }

                    if (player.Call<int>("istalking") != 0)
                    {
                        Log.Write(LogLevel.Trace, "talking");
                        return true;
                    }

                    if (player.HasField("ac_lastPos"))
                    {
                        var lastPos = player.GetField<Vector3>("ac_lastPos");

                        if (lastPos.DistanceTo2D(player.Origin) < 50)
                        {
                            player.Call("iprintlnbold", "You will be killed if you do not move.");

                            var oldHealth = player.Health;
                            player.Health /= 3;
                            player.Notify("damage", (oldHealth - player.Health), player, new Vector3(0, 0, 0), new Vector3(0, 0, 0), "MOD_EXPLOSIVE", "", "", "", 0, "frag_grenade_mp");

                            if (player.Health < 5)
                            {
                                player.Call("suicide");
                            }
                        }
                    }

                    player.SetField("ac_lastPos", player.Origin);

                    return true;
                });
            });
        }
    }
}
