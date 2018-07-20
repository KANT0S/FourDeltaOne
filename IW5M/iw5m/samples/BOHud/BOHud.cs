using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using InfinityScript;

namespace InfinityScript.Examples
{
    public class BOHud : BaseScript
    {
        public BOHud()
            : base()
        {
            Call("precacheShader", "cardicon_radiation");

            Call("setDvar", "g_hardcore", "1");
            Call("setDvar", "cg_drawCrosshair", "1");

            PlayerConnected += new Action<Entity>(entity =>
            {
                entity.SetClientDvar("g_hardcore", "1");
                entity.SetClientDvar("g_compassForceDisplay", "1");

                CreateHUD(entity);

                entity.OnInterval(300, player =>
                {
                    UpdateHUDAmmo(player);
                    return true;
                });

                entity.OnNotify("weapon_fired", (player, weapon) =>
                {
                    UpdateHUDAmmo(player);
                });

                entity.SpawnedPlayer += new Action(() =>
                {
                    entity.SetClientDvar("g_hardcore", "1");
                    entity.SetClientDvar("g_compassForceDisplay", "1");

                    // update all players' ranking
                    UpdateScores();
                });
            });
        }

        public override void OnPlayerKilled(Entity player, Entity inflictor, Entity attacker, int damage, string mod, string weapon, Vector3 dir, string hitLoc)
        {
            // update the own score
            if (attacker.HasField("bohud_created"))
            {
                attacker.AfterDelay(10, entity =>
                {
                    var scoreText = entity.GetField<HudElem[]>("bohud_messageText")[1];
                    scoreText.SetText(entity.GetField<int>("score").ToString());
                });
            }

            // update all players' ranking
            UpdateScores();
        }

        private void UpdateScores()
        {
            var scoreList = (from p in Players
                             orderby p.GetField<int>("score") descending, p.GetField<int>("deaths") ascending
                             select p).ToArray();

            for (int i = 0; i < scoreList.Length; i++)
            {
                UpdateDisplayScore(scoreList[i], i + 1);
            }
        }

        private void UpdateDisplayScore(Entity player, int rank)
        {
            if (!player.HasField("bohud_created"))
            {
                return;
            }

            var rankTexts = player.GetField<HudElem[]>("bohud_rankText");
            rankTexts[0].SetText("^0" + rank.ToString());
            rankTexts[1].SetText("^0" + rank.ToString());
            rankTexts[2].SetText(rank.ToString());

            if (rank > 9)
            {
                rankTexts[0].X = (-26 - 14);
                rankTexts[1].X = (-26 - 12);
                rankTexts[2].X = (-26 - 13);
            }
            else
            {
                rankTexts[0].X = (-26 - 1);
                rankTexts[1].X = (-26);
                rankTexts[2].X = (-26 + 1);
            }
        }

        private void CreateHUD(Entity player)
        {
            if (player.HasField("bohud_created"))
            {
                return;
            }

            // background bars, left side
            var bars = new HudElem[2];

            new[] { new { X = -20, Y = 15, i = 0 }, new { X = -20, Y = -4, i = 1 } }.ToList().ForEach(coord =>
            {
                var bar = HudElem.NewClientHudElem(player);
                bar.X = coord.X;
                bar.Y = coord.Y;
                bar.AlignX = "left";
                bar.AlignY = "bottom";
                bar.HorzAlign = "left";
                bar.VertAlign = "bottom";
                bar.SetShader("white", 190, 15);
                bar.Alpha = 0.3f;
                bar.HideWhenInMenu = true;
                bar.Foreground = false;

                bars[coord.i] = bar;
            });

            player.SetField("bohud_bars", new Parameter(bars));

            // ranking background
            var circle = HudElem.CreateIcon(player, "cardicon_radiation", 60, 60);
            circle.SetPoint("bottom left", "bottom left", 15, -15);
            circle.HideWhenInMenu = true;
            circle.Foreground = true;
            circle.Alpha = 1;

            player.SetField("bohud_circle", new Parameter(circle));

            // rank text/shadow
            var rankX = -26;
            player.SetField("bohud_rankX", rankX);

            var rankText = new HudElem[3];

            new[] {
                new { X = rankX - 1, Sort = 3 },
                new { X = rankX + 1, Sort = 4 },
                new { X = rankX, Sort = 5 }
            }.ToList().ForEach(entry =>
            {
                var text = HudElem.NewClientHudElem(player);
                text.X = entry.X;
                text.Y = 18;
                text.AlignX = "left";
                text.AlignY = "bottom";
                text.HorzAlign = "left";
                text.VertAlign = "bottom";
                text.FontScale = 5;
                text.Sort = entry.Sort;
                text.HideWhenInMenu = true;
                text.SetText((entry.Sort == 5) ? "1" : "^01");

                rankText[entry.Sort - 3] = text;
            });

            player.SetField("bohud_rankText", new Parameter(rankText));

            // message texts
            var messageText = new HudElem[2];

            new[] {
                new { X = 30, Y = 16, Text = "", i = 0 },
                new { X = 30, Y = -3, Text = "0", i = 1 },
            }.ToList().ForEach(entry =>
            {
                var text = HudElem.NewClientHudElem(player);
                text.X = entry.X;
                text.Y = entry.Y;
                text.AlignX = "left";
                text.AlignY = "bottom";
                text.HorzAlign = "left";
                text.VertAlign = "bottom";
                text.FontScale = 1.5f;
                text.Alpha = 0.5f;
                text.HideWhenInMenu = true;
                text.Sort = 5;
                text.SetText(entry.Text);

                messageText[entry.i] = text;
            });

            player.SetField("bohud_messageText", new Parameter(messageText));

            // ammo stuff
            var ammoSlash = HudElem.CreateFontString(player, "default", 1.25f);
            ammoSlash.SetPoint("bottom right", "bottom right", -85, -35);
            ammoSlash.GlowAlpha = 0;
            ammoSlash.HideWhenInMenu = true;
            ammoSlash.Archived = false;
            ammoSlash.SetText("/");

            player.SetField("bohud_ammoSlash", new Parameter(ammoSlash));

            var ammoStock = HudElem.CreateFontString(player, "default", 1.25f);
            ammoStock.Parent = ammoSlash;
            ammoStock.SetPoint("bottom left", "bottom left", 3, 0);
            ammoStock.GlowAlpha = 0;
            ammoStock.HideWhenInMenu = true;
            ammoStock.Archived = false;
            ammoStock.SetText("48");

            player.SetField("bohud_ammoStock", new Parameter(ammoStock));

            var ammoClip = HudElem.CreateFontString(player, "default", 1.95f);
            ammoClip.Parent = ammoSlash;
            ammoClip.SetPoint("right", "right", -7, -4);
            ammoClip.GlowAlpha = 0;
            ammoClip.HideWhenInMenu = true;
            ammoClip.Archived = false;
            ammoClip.SetText("12");

            player.SetField("bohud_ammoClip", new Parameter(ammoClip));

            player.SetField("bohud_created", true);
        }

        private void UpdateHUDAmmo(Entity player)
        {
            if (!player.HasField("bohud_created"))
            {
                return;
            }

            if (!player.IsAlive)
            {
                return;
            }

            var ammoStock = player.GetField<HudElem>("bohud_ammoStock");
            var ammoClip = player.GetField<HudElem>("bohud_ammoClip");
            var currentWeapon = player.CurrentWeapon;

            ammoStock.SetText(player.GetWeaponAmmoStock(currentWeapon).ToString());
            ammoClip.SetText(player.GetWeaponAmmoClip(currentWeapon).ToString());
        }
    }
}
