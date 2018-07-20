using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace InfinityScript.Examples
{
    public class Sharpshooter : BaseScript
    {
        private int _switchTime;
        private int _cycleRemaining;
        private string _currentWeapon;
        private Random _rng = new Random();
        private List<string> _usedWeapons = new List<string>();
        private string[] _weaponList;
        private Dictionary<string, string[]> _validAttachments;

        private SharpshooterPerk[] _perkList;

        #region SharpshooterPerk
        private class SharpshooterPerk
        {
            public string perkName;
            public string[] perks;
            public bool isWeapon;
        }
        #endregion

        public Sharpshooter()
            : base()
        {
            // is a no-op, it crashes if enabled, likely because script doesn't check for undefined
            //Utilities.SetDropItemEnabled(false);

            _switchTime = Call<int>("getDvarInt", "shrp_switchTime", 45);
            _cycleRemaining = _switchTime;

            _perkList = new[]
            {
                new SharpshooterPerk()
                {
                    perkName = "Sleight of Hand",
                    perks = new[]
                    {
                        "specialty_fastreload",
                        "specialty_quickswap"
                    }
                },
                new SharpshooterPerk()
                {
                    perkName = "Quickdraw",
                    perks = new[]
                    {
                        "specialty_quickdraw"
                    }
                },
                new SharpshooterPerk()
                {
                    perkName = "Stalker",
                    perks = new[]
                    {
                        "specialty_stalker"
                    }
                },
                new SharpshooterPerk()
                {
                    perkName = "Marathon",
                    perks = new[]
                    {
                        "specialty_longersprint",
                        "specialty_fastmantle"
                    }
                },
                new SharpshooterPerk()
                {
                    perkName = "Range",
                    perks = new[]
                    {
                        "specialty_longerrange"
                    },
                    isWeapon = true
                },
                new SharpshooterPerk()
                {
                    perkName = "Stability",
                    perks = new[]
                    {
                        "specialty_reducedsway"
                    },
                    isWeapon = true
                },
                new SharpshooterPerk()
                {
                    perkName = "Speed",
                    perks = new[]
                    {
                        "specialty_lightweight"
                    },
                    isWeapon = true
                },
            };

            foreach (var perk in _perkList)
            {
                var shaderName = perk.perks[0] + (perk.isWeapon ? "" : "_upgrade");
                Call("precacheShader", shaderName);
            }

            PrepareWeaponLists();

            _currentWeapon = GetRandomWeapon();

            OnInterval(1000, () =>
            {
                _cycleRemaining -= 1;

                if (_cycleRemaining == 0)
                {
                    _cycleRemaining = _switchTime;
                }

                foreach (var player in Players)
                {
                    if (player.HasField("bohud_created"))
                    {
                        var scoreText = player.GetField<HudElem[]>("bohud_messageText")[0];
                        scoreText.SetText("Weapons cycling in " + FormatTime(_cycleRemaining));
                    }
                }

                return true;
            });

            OnInterval(_switchTime * 1000, () =>
            {
                _currentWeapon = GetRandomWeapon();
                _cycleRemaining = _switchTime;

                foreach (var player in Players)
                {
                    if (player.IsAlive)
                    {
                        player.TakeAllWeapons();

                        player.GiveWeapon(_currentWeapon);
                        player.Call("giveMaxAmmo", _currentWeapon);

                        player.AfterDelay(100, entity =>
                        {
                            entity.SwitchToWeaponImmediate(_currentWeapon);
                        });
                    }
                }

                return true;
            });

            PlayerConnected += new Action<Entity>(entity =>
            {
                CreatePerkHUD(entity);

                entity.OnNotify("joined_team", player =>
                {
                    entity.Call("closePopupMenu");
                    entity.Call("closeIngameMenu");
                    entity.Notify("menuresponse", "changeclass", "class1");
                });

                entity.OnInterval(250, player =>
                {
                    if (player.IsAlive)
                    {
                        var weapon = player.CurrentWeapon;

                        var usedName = GetRealWeaponName(_currentWeapon);

                        if (weapon != "none" && weapon != "" && !weapon.Contains(usedName))
                        {
                            player.Call("suicide");
                        }
                    }

                    return true;
                });

                entity.OnInterval(3500, player =>
                {
                    if (player.IsAlive)
                    {
                        var weapon = player.CurrentWeapon;

                        if (weapon.StartsWith("rpg") || weapon.StartsWith("iw5_smaw"))
                        {
                            player.Call("giveMaxAmmo", weapon);
                        }
                    }

                    return true;
                });

                // you cheater
                /*entity.Call("notifyOnPlayerCommand", "ohnoes", "+actionslot 4");

                entity.OnNotify("ohnoes", player =>
                {
                    Log.Write(LogLevel.Trace, "ohnoes triggered!");
                    GiveRandomPerk(player);
                });*/

                entity.SpawnedPlayer += new Action(() =>
                {
                    entity.TakeAllWeapons();
                    entity.Call("clearPerks");

                    ResetPerkHUD(entity);

                    entity.SetField("shrp_perkc", 0);
                    entity.SetField("shrp_perks", new Parameter(new string[3]));

                    entity.GiveWeapon(_currentWeapon);
                    entity.Call("giveMaxAmmo", _currentWeapon);

                    entity.AfterDelay(100, player =>
                    {
                        player.SwitchToWeaponImmediate(_currentWeapon);
                    });
                });
            });
        }

        public override void OnPlayerKilled(Entity player, Entity inflictor, Entity attacker, int damage, string mod, string weapon, Vector3 dir, string hitLoc)
        {
            if (attacker != player && attacker.GetField<string>("classname") == "player" && attacker.IsAlive)
            {
                GiveRandomPerk(attacker);
            }
        }

        private string GetRealWeaponName(string weapon)
        {
            var tokens = weapon.Split('_');

            if (tokens[0] == "iw5")
            {
                return tokens[1];
            }
            else
            {
                return tokens[0];
            }
        }

        private string FormatTime(int seconds)
        {
            return string.Format("{0}:{1}", seconds / 60, (seconds % 60).ToString().PadLeft(2, '0'));
        }

        private void PrepareWeaponLists()
        {
            _weaponList = new[]
            {
                "iw5_44magnum",
                "iw5_usp45",
                "iw5_deserteagle",
                "iw5_mp412",
                "iw5_p99",
                "iw5_fnfiveseven",
                "iw5_fmg9",
                "iw5_skorpion",
                "iw5_mp9",
                "iw5_g18",
                "iw5_mp5",
                "iw5_m9",
                "iw5_p90",
                "iw5_pp90m1",
                "iw5_ump45",
                "iw5_mp7",
                "iw5_ak47",
                "iw5_m16",
                "iw5_m4",
                "iw5_fad",
                "iw5_acr",
                "iw5_type95",
                "iw5_mk14",
                "iw5_scar",
                "iw5_g36c",
                "iw5_cm901",
                "rpg",
                "iw5_smaw",
                "xm25",
                "iw5_dragunov",
                "iw5_msr",
                "iw5_barrett",
                "iw5_rsass",
                "iw5_as50",
                "iw5_l96a1",
                "iw5_ksg",
                "iw5_1887",
                "iw5_striker",
                "iw5_aa12",
                "iw5_usas12",
                "iw5_spas12",
                "iw5_m60",
                "iw5_mk46",
                "iw5_pecheneg",
                "iw5_sa80",
                "iw5_mg36",
            };

            _validAttachments = new Dictionary<string,string[]>();

            foreach (var weapon in _weaponList)
            {
                var attachments = new List<string>();

                for (int column = 11; column <= 21; column++)
                {
                    var attachment = Call<string>("tableLookup", "mp/statstable.csv", 4, weapon, column);

                    if (attachment != "")
                    {
                        attachments.Add(attachment);
                    }
                }

                _validAttachments.Add(weapon, attachments.ToArray());
            }
        }

        private string GetRandomWeapon()
        {
            var weapon = (from w in _weaponList
                          where !_usedWeapons.Contains(w)
                          orderby _rng.Next()
                          select w).FirstOrDefault();

            if (weapon == null)
            {
                _usedWeapons.Clear();

                weapon = _weaponList[_rng.Next(0, _weaponList.Length)];
            }

            _usedWeapons.Add(weapon);

            return AddRandomAttachmentToWeapon(weapon);
        }

        private string[] GetDesiredAttachments()
        {
            var attachmentTypes = new[]
            {
                "none",
                "sight",
                "sight",
                "sight",
                "sight",
                "sight",
                "other",
                "other",
                "other",
                "final"
            };

            switch (attachmentTypes[_rng.Next(0, attachmentTypes.Length)])
            {
                case "none":
                    return new string[0];
                case "sight":
                    return new[]
                    {
                        "acog",
                        "reflex",
                        "hamrhybrid",
                        "hybrid",
                        "zoomscope",
                        "eotech",
                        "vzscope"
                    };
                case "other":
                    return new[]
                    {
                        "silencer",
                        "silencer02",
                        "silencer03",
                        "grip",
                        "gl",
                        "gp25",
                        "m320",
                        "shotgun"
                    };
                case "final":
                    return new[]
                    {
                        "thermal",
                        "heartbeat"
                    };
            }

            return new string[0];
        }

        private string AddRandomAttachmentToWeapon(string baseWeapon)
        {
            var attachmentList = _validAttachments[baseWeapon];

            if (attachmentList.Length == 0)
            {
                return Utilities.BuildWeaponName(baseWeapon, "none", "none", _rng.Next(0, 14), _rng.Next(0, 7));
            }

            var validDesireList = false;
            string[] desiredAttachments = null;

            while (!validDesireList)
            {
                desiredAttachments = GetDesiredAttachments();

                if (desiredAttachments.Length == 0)
                {
                    return Utilities.BuildWeaponName(baseWeapon, "none", "none", _rng.Next(0, 14), _rng.Next(0, 7));
                }

                foreach (var attachment in desiredAttachments)
                {
                    if (attachmentList.Contains(attachment))
                    {
                        validDesireList = true;
                        break;
                    }
                }
            }

            string usedAttachment = "none";
            var validAttachment = false;

            while (!validAttachment)
            {
                usedAttachment = desiredAttachments[_rng.Next(0, desiredAttachments.Length)];

                if (attachmentList.Contains(usedAttachment))
                {
                    validAttachment = true;
                }
            }

            Log.Write(LogLevel.Trace, "weapon {0} with a {1}", baseWeapon, usedAttachment);

            return Utilities.BuildWeaponName(baseWeapon, usedAttachment, "none", _rng.Next(0, 14), _rng.Next(0, 7));
        }

        private void GiveRandomPerk(Entity player)
        {
            var pc = player.GetField<int>("shrp_perkc");

            if (pc < 3)
            {
                var usedPerks = player.GetField<string[]>("shrp_perks");

                var validPerk = false;
                SharpshooterPerk perk = null;

                while (!validPerk)
                {
                    perk = _perkList[_rng.Next(0, _perkList.Length)];

                    if (!usedPerks.Contains(perk.perks[0]))
                    {
                        validPerk = true;
                    }
                }

                foreach (var p in perk.perks)
                {
                    player.SetPerk(p, true, false);
                }

                UpdatePerkHUD(player, pc, perk);

                usedPerks[pc] = perk.perks[0];
                player.SetField("shrp_perkc", pc + 1);
            }
        }

        private void CreatePerkHUD(Entity player)
        {
            var icons = new HudElem[3];

            new[] { -300, -250, -200 }.ToList().ForEach(y =>
            {
                var i = (y + 300) / 50;

                var elem = HudElem.CreateIcon(player, "specialty_quickdraw_upgrade", 40, 40);
                elem.SetPoint("bottom right", "bottom right", -120, y);
                elem.Alpha = 0;
                elem.HideWhenInMenu = true;
                elem.Foreground = true;

                icons[i] = elem;
            });

            player.SetField("shrp_perkIcons", new Parameter(icons));

            var names = new HudElem[3];

            new[] { -275, -225, -175 }.ToList().ForEach(y =>
            {
                var i = (y + 275) / 50;

                var elem = HudElem.NewClientHudElem(player);
                elem.X = 40;
                elem.Y = y;
                elem.AlignX = "right";
                elem.AlignY = "bottom";
                elem.HorzAlign = "right";
                elem.VertAlign = "bottom";
                elem.FontScale = 1.5f;
                elem.SetText("Quickdraw");
                elem.Alpha = 0;
                elem.HideWhenInMenu = true;
                elem.Foreground = true;

                names[i] = elem;
            });

            player.SetField("shrp_perkNames", new Parameter(names));
        }

        private void ResetPerkHUD(Entity player)
        {
            var icons = player.GetField<HudElem[]>("shrp_perkIcons");

            foreach (var icon in icons)
            {
                icon.Alpha = 0;
            }

            var texts = player.GetField<HudElem[]>("shrp_perkNames");

            foreach (var text in texts)
            {
                text.Alpha = 0;
            }
        }

        private void UpdatePerkHUD(Entity player, int index, SharpshooterPerk perk)
        {
            var icons = player.GetField<HudElem[]>("shrp_perkIcons");
            icons[index].SetShader(perk.perks[0] + (perk.isWeapon ? "" : "_upgrade"), 40, 40);
            icons[index].Alpha = 1;

            var texts = player.GetField<HudElem[]>("shrp_perkNames");
            texts[index].SetText(perk.perkName);
            texts[index].Alpha = 1;
        }
    }
}
