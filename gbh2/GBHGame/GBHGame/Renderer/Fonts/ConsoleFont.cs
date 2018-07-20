using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;

namespace GBH
{
    public class ConsoleFont : Font
    {
        protected override string Filename
        {
            get { return "consolefont.png"; }
        }

        public override int LineHeight
        {
            get { return 14; }
        }

        public override string Name
        {
            get { return "ConsoleFont"; }
        }

        private Dictionary<int, int> _widthTable = new Dictionary<int, int>();

        public ConsoleFont()
        {
            _widthTable.Add(32, 8);
            _widthTable.Add(33, 8);
            _widthTable.Add(34, 8);
            _widthTable.Add(35, 8);
            _widthTable.Add(36, 8);
            _widthTable.Add(37, 8);
            _widthTable.Add(38, 8);
            _widthTable.Add(39, 8);
            _widthTable.Add(40, 8);
            _widthTable.Add(41, 8);
            _widthTable.Add(42, 8);
            _widthTable.Add(43, 8);
            _widthTable.Add(44, 8);
            _widthTable.Add(45, 8);
            _widthTable.Add(46, 8);
            _widthTable.Add(47, 8);
            _widthTable.Add(48, 8);
            _widthTable.Add(49, 8);
            _widthTable.Add(50, 8);
            _widthTable.Add(51, 8);
            _widthTable.Add(52, 8);
            _widthTable.Add(53, 8);
            _widthTable.Add(54, 8);
            _widthTable.Add(55, 8);
            _widthTable.Add(56, 8);
            _widthTable.Add(57, 8);
            _widthTable.Add(58, 8);
            _widthTable.Add(59, 8);
            _widthTable.Add(60, 8);
            _widthTable.Add(61, 8);
            _widthTable.Add(62, 8);
            _widthTable.Add(63, 8);
            _widthTable.Add(64, 8);
            _widthTable.Add(65, 8);
            _widthTable.Add(66, 8);
            _widthTable.Add(67, 8);
            _widthTable.Add(68, 8);
            _widthTable.Add(69, 8);
            _widthTable.Add(70, 8);
            _widthTable.Add(71, 8);
            _widthTable.Add(72, 8);
            _widthTable.Add(73, 8);
            _widthTable.Add(74, 8);
            _widthTable.Add(75, 8);
            _widthTable.Add(76, 8);
            _widthTable.Add(77, 8);
            _widthTable.Add(78, 8);
            _widthTable.Add(79, 8);
            _widthTable.Add(80, 8);
            _widthTable.Add(81, 8);
            _widthTable.Add(82, 8);
            _widthTable.Add(83, 8);
            _widthTable.Add(84, 8);
            _widthTable.Add(85, 8);
            _widthTable.Add(86, 8);
            _widthTable.Add(87, 8);
            _widthTable.Add(88, 8);
            _widthTable.Add(89, 8);
            _widthTable.Add(90, 8);
            _widthTable.Add(91, 8);
            _widthTable.Add(92, 8);
            _widthTable.Add(93, 8);
            _widthTable.Add(94, 8);
            _widthTable.Add(95, 8);
            _widthTable.Add(96, 8);
            _widthTable.Add(97, 8);
            _widthTable.Add(98, 8);
            _widthTable.Add(99, 8);
            _widthTable.Add(100, 8);
            _widthTable.Add(101, 8);
            _widthTable.Add(102, 8);
            _widthTable.Add(103, 8);
            _widthTable.Add(104, 8);
            _widthTable.Add(105, 8);
            _widthTable.Add(106, 8);
            _widthTable.Add(107, 8);
            _widthTable.Add(108, 8);
            _widthTable.Add(109, 8);
            _widthTable.Add(110, 8);
            _widthTable.Add(111, 8);
            _widthTable.Add(112, 8);
            _widthTable.Add(113, 8);
            _widthTable.Add(114, 8);
            _widthTable.Add(115, 8);
            _widthTable.Add(116, 8);
            _widthTable.Add(117, 8);
            _widthTable.Add(118, 8);
            _widthTable.Add(119, 8);
            _widthTable.Add(120, 8);
            _widthTable.Add(121, 8);
            _widthTable.Add(122, 8);
            _widthTable.Add(123, 8);
            _widthTable.Add(124, 8);
            _widthTable.Add(125, 8);
            _widthTable.Add(126, 8);
            _widthTable.Add(127, 5);
        }

        public override int GetWidth(char character)
        {
            if (!_widthTable.ContainsKey((int)character))
            {
                return _widthTable['?'];
            }

            return _widthTable[(int)character];
        }

        public override Rectangle GetCoordinates(char character)
        {
            var c = ((int)character) - 32;

            if (c > (127 - 32) || c < 0)
            {
                return GetCoordinates('?');
            }

            var x = (c % 16) * 16;
            var y = (c / 16) * 16;

            return new Rectangle(x, y, _widthTable[(int)character], LineHeight);
        }
    }
}
