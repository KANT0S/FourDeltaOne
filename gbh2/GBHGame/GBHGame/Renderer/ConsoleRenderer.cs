using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;

namespace GBH
{
    public static class ConsoleRenderer
    {
        public static bool Active { get; set; }
        public static bool LargeConsole { get; set; }
        public static bool Invalidated { get; set; }

        private static int _screenX;
        private static int _screenY;

        private static int _screenSize;
        private static int _screenTop = 0;

        private static string _inputBuffer;
        private static string _typedInputBuffer;
        private static List<string> _commandHistory;
        public static List<string> _screenBuffer;

        [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
        public static extern int ToAscii(
            uint uVirtKey, // virtual-key code
            uint uScanCode, // scan code
            byte[] lpKeyState, // key-state array
            StringBuilder lpChar, // buffer for translated key
            uint flags // active-menu flag
        );

        static ConsoleRenderer()
        {
            _screenBuffer = new List<string>();
            _commandHistory = new List<string>();
        }

        public static void Initialize()
        {
            LargeConsole = true;

            _screenX = 1280;
            _screenY = 720;
        }

        public static void HandleKey(Keys key)
        {
            if (key == Keys.F11)
            {
                Active = !Active;
            }

            if (!Active)
            {
                return;
            }

            if (key == Keys.Back)
            {
                if (_inputBuffer.Length > 0)
                {
                    _inputBuffer = _inputBuffer.Substring(0, _inputBuffer.Length - 1);
                    _typedInputBuffer = _inputBuffer;

                    UpdateSuggestions();
                    Invalidated = true;
                }
            }

            if (key == Keys.Enter)
            {
                SendCommand();

                UpdateSuggestions();
                return;
            }

            if (key == Keys.Up)
            {
                if (_suggestions.Count > 0)
                {
                    _currentSuggestion--;

                    if (_currentSuggestion < 0)
                    {
                        _currentSuggestion = _suggestions.Count - 1;
                    }

                    _inputBuffer = _suggestions[_currentSuggestion] + " ";

                    Invalidated = true;
                }

                return;
            }

            if (key == Keys.Down)
            {
                if (_suggestions.Count > 0)
                {
                    _currentSuggestion++;

                    if (_currentSuggestion > (_suggestions.Count - 1))
                    {
                        _currentSuggestion = 0;
                    }

                    _inputBuffer = _suggestions[_currentSuggestion] + " ";

                    Invalidated = true;
                }

                return;
            }

            if (key == Keys.PageUp)
            {
                _screenTop -= 1;

                if (_screenTop < 0)
                {
                    _screenTop = 0;
                }

                Invalidated = true;

                return;
            }

            if (key == Keys.PageDown)
            {
                _screenTop += 1;

                if (_screenTop > (_screenBuffer.Count - _screenSize))
                {
                    _screenTop = (_screenBuffer.Count - _screenSize);
                }

                Invalidated = true;

                return;
            }
        }

        public static void HandleChar(char character)
        {
            if (!Active)
            {
                return;
            }

            if (char.IsControl(character))
            {
                return;
            }

            if (character == '`')
            {
                return;
            }

            _inputBuffer += character;
            _typedInputBuffer = _inputBuffer;

            UpdateSuggestions();
        }

        public static void Process()
        {
            if (Active)
            {
                RenderConsole();
            }
        }

        private static void SendCommand()
        {
            Print("^7]" + _inputBuffer);

            Command.AddToBuffer(_typedInputBuffer);
            Command.AddToBuffer("\n");

            _inputBuffer = "";
            _typedInputBuffer = "";
            UpdateSuggestions();
        }

        public static void Print(string text)
        {
            var lines = text.Split('\n');

            foreach (var line in lines)
            {
                _screenBuffer.Add(line);
            }

            ResetTop();

            if (_screenBuffer.Count > 500)
            {
                _screenBuffer = _screenBuffer.Skip(_screenBuffer.Count - 300).ToList();
            }
        }

        private static void ResetTop()
        {
            _screenTop = (_screenBuffer.Count - _screenSize);
        }

        private static float p(float value)
        {
            return ((short)(value * 16f)) / 16f;
        }

        private static int _inputBoxBottom = 0;
        private static int _inputBoxRight = 0;

        private static void RenderConsole()
        {
            FontManager.SetFont(FontStyle.Console);

            RenderInputBox();

            if (LargeConsole)
            {
                RenderOutputBox();
            }

            RenderAutoComplete();
        }

        private static void RenderInputBox()
        {
            var text = "GBH2>^7 " + _inputBuffer;
            var tSize = FontManager.MeasureString(text);

            int x = 15;
            int y = 15;

            int w = _screenX - 30;
            int h = 16 + 3 + 3;

            Renderer2D.FillRectangle(new Color(0x44, 0x44, 0x44), x, y, w + 2, h + 2);
            Renderer2D.DrawRectangle(new Color(0x22, 0x22, 0x22), x, y, w + 2, h + 2);

            FontManager.SetColor(Color.Yellow);
            FontManager.PrintString(new Vector2(x + 7, y + 3), text);

            _inputBoxRight = x + 7 + tSize;
            _inputBoxBottom = y + h + 2;
        }

        private static List<string> _suggestions = new List<string>();
        private static string _suggestionSource = "";
        private static int _currentSuggestion = 0;

        private static void UpdateSuggestions()
        {
            if (_typedInputBuffer.Contains(' ') || _typedInputBuffer == "")
            {
                _suggestions.Clear();
                _currentSuggestion = 0;
                return;
            }

            if (_typedInputBuffer != _suggestionSource)
            {
                var suggestions = from command in ConVar.GetConVars()
                                  where command.Name.ToLower().StartsWith(_typedInputBuffer.ToLower()) && command.Name.ToLower() != _typedInputBuffer.ToLower()
                                  select command.Name;

                _suggestions = suggestions.ToList();
                _currentSuggestion = -1;
            }
        }

        private static void RenderAutoComplete()
        {
            if (_suggestions.Count > 0)
            {
                var longestSuggestion = (from command in _suggestions
                                         orderby command.Length descending
                                         select command).First();

                var size = FontManager.MeasureString(longestSuggestion);

                var x = _inputBoxRight;
                var y = _inputBoxBottom - 2;

                var lines = Math.Min(_suggestions.Count, 20);
                var h = (lines * 16) + 4 + 6;
                var w = size + 4 + 6;

                Renderer2D.FillRectangle(new Color(0x44, 0x44, 0x44), x, y, w, h);
                Renderer2D.DrawRectangle(new Color(0x22, 0x22, 0x22), x, y, w, h);

                foreach (var line in _suggestions)
                {
                    FontManager.SetColor(Color.White);
                    FontManager.PrintString(new Vector2(x + 5, y + 5), line);

                    y += 16;
                }
            }
        }

        private static void RenderOutputBox()
        {
            int x = 15;
            int y = _inputBoxBottom + 5;

            int w = _screenX - 30;
            int h = _screenY - y - 15 - 2;

            var size = 16;

            Renderer2D.FillRectangle(new Color(0x44, 0x44, 0x44, 0xcc), x, y, w + 2, h + 2);
            Renderer2D.DrawRectangle(new Color(0x22, 0x22, 0x22, 0xcc), x, y, w + 2, h + 2);

            bool reset = false;

            if (_screenSize == 0)
            {
                reset = true;
            }

            _screenSize = (h - 4) / 16;

            if (reset)
            {
                ResetTop();
            }

            var list = _screenBuffer.Skip(_screenTop);
            int i = 0;

            y += 5;

            foreach (var line in list)
            {
                FontManager.SetColor(Color.White);
                FontManager.PrintString(new Vector2(x + 5, y), line);

                y += size;
                i++;

                if (i >= _screenSize)
                {
                    break;
                }
            }
        }
    }

}
