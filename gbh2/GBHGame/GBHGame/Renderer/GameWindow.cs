using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace GBH
{
    class GameWindow : Form
    {
        public GameWindow()
        {
            this.ClientSize = new System.Drawing.Size(1280, 720);
            this.Text = "GBH2";
            this.FormBorderStyle = FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;

            this.Show();
        }

        private IntPtr hIMC;

        protected override void WndProc(ref Message m)
        {
            switch ((Win32.WindowsMessage)m.Msg)
            {
                case Win32.WindowsMessage.WM_KEYDOWN:
                    EventSystem.QueueEvent(new KeyEvent() { Time = Game.Time, Key = (Microsoft.Xna.Framework.Input.Keys)m.WParam, Down = true });
                    break;

                case Win32.WindowsMessage.WM_KEYUP:
                    EventSystem.QueueEvent(new KeyEvent() { Time = Game.Time, Key = (Microsoft.Xna.Framework.Input.Keys)m.WParam, Down = false });
                    break;

                case Win32.WindowsMessage.WM_CHAR:
                    EventSystem.QueueEvent(new CharEvent() { Time = Game.Time, Character = (char)m.WParam });
                    break;

                case Win32.WindowsMessage.WM_CLOSE:
                    Client.Shutdown();
                    Environment.Exit(0);
                    break;
            }

            base.WndProc(ref m);
        }

        private static Win32.NativeMessage _msg;
        private static GameWindow _gw;

        public static void Initialize()
        {
            _gw = new GameWindow();
        }

        public static IntPtr NativeHandle
        {
            get
            {
                return _gw.Handle;
            }
        }

        public static void RunEvents()
        {
            while (Win32.PeekMessage(out _msg, _gw.Handle, 0, 0, (uint)Win32.PM.NOREMOVE))
            {
                if (!Win32.GetMessage(out _msg, _gw.Handle, 0, 0))
                {
                    Environment.Exit(0);
                }

                Win32.TranslateMessage(ref _msg);
                Win32.DispatchMessage(ref _msg);
            }
        }
    }

    static class Win32
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct NativeMessage
        {
            public IntPtr hwnd;
            public uint message;
            public IntPtr wParam;
            public IntPtr lParam;
            public uint time;
            public System.Drawing.Point point;
        }

        [DllImport("imm32.dll")]
        public static extern IntPtr ImmGetContext(IntPtr hWnd);

        [DllImport("imm32.dll")]
        public static extern IntPtr ImmAssociateContext(IntPtr hWnd, IntPtr hIMC);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetMessage(out NativeMessage lpMsg, IntPtr hwnd, uint wMsgFilterMin, uint wMsgFilterMax);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool PeekMessage(out NativeMessage lpMsg, IntPtr hwnd, uint wMsgFilterMin, uint wMsgFilterMax, uint wRemoveMsg);

        public enum PM : uint
        {
            NOREMOVE = 0,
            REMOVE = 1
        }

        [DllImport("user32.dll")]
        public static extern bool TranslateMessage([In] ref NativeMessage lpMsg);

        [DllImport("user32.dll")]
        public static extern IntPtr DispatchMessage([In] ref NativeMessage lpmsg);

        #region SM_* and WM_*
        // from winuser.h
        /*
         * GetSystemMetrics() codes
         */
        public enum SystemMetrics : uint
        {
            SM_CXSCREEN = 0,
            SM_CYSCREEN = 1,
            SM_CXVSCROLL = 2,
            SM_CYHSCROLL = 3,
            SM_CYCAPTION = 4,
            SM_CXBORDER = 5,
            SM_CYBORDER = 6,
            SM_CXDLGFRAME = 7,
            SM_CXFIXEDFRAME = 7,/* ;win40 name change */
            SM_CYDLGFRAME = 8,
            SM_CYFIXEDFRAME = 8,/* ;win40 name change */
            SM_CYVTHUMB = 9,
            SM_CXHTHUMB = 10,
            SM_CXICON = 11,
            SM_CYICON = 12,
            SM_CXCURSOR = 13,
            SM_CYCURSOR = 14,
            SM_CYMENU = 15,
            SM_CXFULLSCREEN = 16,
            SM_CYFULLSCREEN = 17,
            SM_CYKANJIWINDOW = 18,
            SM_MOUSEPRESENT = 19,
            SM_CYVSCROLL = 20,
            SM_CXHSCROLL = 21,
            SM_DEBUG = 22,
            SM_SWAPBUTTON = 23,
            SM_RESERVED1 = 24,
            SM_RESERVED2 = 25,
            SM_RESERVED3 = 26,
            SM_RESERVED4 = 27,
            SM_CXMIN = 28,
            SM_CYMIN = 29,
            SM_CXSIZE = 30,
            SM_CYSIZE = 31,
            SM_CXFRAME = 32,
            SM_CXSIZEFRAME = 32, /* ;win40 name change */
            SM_CYFRAME = 33,
            SM_CYSIZEFRAME = 33,/* ;win40 name change */
            SM_CXMINTRACK = 34,
            SM_CYMINTRACK = 35,
            SM_CXDOUBLECLK = 36,
            SM_CYDOUBLECLK = 37,
            SM_CXICONSPACING = 38,
            SM_CYICONSPACING = 39,
            SM_MENUDROPALIGNMENT = 40,
            SM_PENWINDOWS = 41,
            SM_DBCSENABLED = 42,
            SM_CMOUSEBUTTONS = 43,
            SM_SECURE = 44,
            SM_CXEDGE = 45,
            SM_CYEDGE = 46,
            SM_CXMINSPACING = 47,
            SM_CYMINSPACING = 48,
            SM_CXSMICON = 49,
            SM_CYSMICON = 50,
            SM_CYSMCAPTION = 51,
            SM_CXSMSIZE = 52,
            SM_CYSMSIZE = 53,
            SM_CXMENUSIZE = 54,
            SM_CYMENUSIZE = 55,
            SM_ARRANGE = 56,
            SM_CXMINIMIZED = 57,
            SM_CYMINIMIZED = 58,
            SM_CXMAXTRACK = 59,
            SM_CYMAXTRACK = 60,
            SM_CXMAXIMIZED = 61,
            SM_CYMAXIMIZED = 62,
            SM_NETWORK = 63,
            SM_CLEANBOOT = 67,
            SM_CXDRAG = 68,
            SM_CYDRAG = 69,

            SM_SHOWSOUNDS = 70,
            SM_CXMENUCHECK = 71,   /* Use instead of GetMenuCheckMarkDimensions()! */
            SM_CYMENUCHECK = 72,
            SM_SLOWMACHINE = 73,
            SM_MIDEASTENABLED = 74,
            SM_MOUSEWHEELPRESENT = 75,
            SM_XVIRTUALSCREEN = 76,
            SM_YVIRTUALSCREEN = 77,
            SM_CXVIRTUALSCREEN = 78,
            SM_CYVIRTUALSCREEN = 79,
            SM_CMONITORS = 80,
            SM_SAMEDISPLAYFORMAT = 81,
            SM_IMMENABLED = 82,
            SM_CXFOCUSBORDER = 83,
            SM_CYFOCUSBORDER = 84,

            SM_TABLETPC = 86,
            SM_MEDIACENTER = 87,
            SM_STARTER = 88,
            SM_SERVERR2 = 89,
            SM_CMETRICS = 90,
            SM_REMOTESESSION = 0x1000,
            SM_SHUTTINGDOWN = 0x2000,
            SM_REMOTECONTROL = 0x2001,
            SM_CARETBLINKINGENABLED = 0x2002

        }

        public enum WindowsMessage : uint
        {
            /*
             * Window Messages
             */

            WM_NULL = 0x0000,
            WM_CREATE = 0x0001,
            WM_DESTROY = 0x0002,
            WM_MOVE = 0x0003,
            WM_SIZE = 0x0005,

            WM_ACTIVATE = 0x0006,

            WM_SETFOCUS = 0x0007,
            WM_KILLFOCUS = 0x0008,
            WM_ENABLE = 0x000A,
            WM_SETREDRAW = 0x000B,
            WM_SETTEXT = 0x000C,
            WM_GETTEXT = 0x000D,
            WM_GETTEXTLENGTH = 0x000E,
            WM_PAINT = 0x000F,
            WM_CLOSE = 0x0010,

            //#ifndef _WIN32_WCE
            WM_QUERYENDSESSION = 0x0011,
            WM_QUERYOPEN = 0x0013,
            WM_ENDSESSION = 0x0016,
            //#endif
            WM_QUIT = 0x0012,
            WM_ERASEBKGND = 0x0014,
            WM_SYSCOLORCHANGE = 0x0015,
            WM_SHOWWINDOW = 0x0018,
            WM_WININICHANGE = 0x001A,

            WM_DEVMODECHANGE = 0x001B,
            WM_ACTIVATEAPP = 0x001C,
            WM_FONTCHANGE = 0x001D,
            WM_TIMECHANGE = 0x001E,
            WM_CANCELMODE = 0x001F,
            WM_SETCURSOR = 0x0020,
            WM_MOUSEACTIVATE = 0x0021,
            WM_CHILDACTIVATE = 0x0022,
            WM_QUEUESYNC = 0x0023,

            WM_GETMINMAXINFO = 0x0024,

            WM_PAINTICON = 0x0026,
            WM_ICONERASEBKGND = 0x0027,
            WM_NEXTDLGCTL = 0x0028,
            WM_SPOOLERSTATUS = 0x002A,
            WM_DRAWITEM = 0x002B,
            WM_MEASUREITEM = 0x002C,
            WM_DELETEITEM = 0x002D,
            WM_VKEYTOITEM = 0x002E,
            WM_CHARTOITEM = 0x002F,
            WM_SETFONT = 0x0030,
            WM_GETFONT = 0x0031,
            WM_SETHOTKEY = 0x0032,
            WM_GETHOTKEY = 0x0033,
            WM_QUERYDRAGICON = 0x0037,
            WM_COMPAREITEM = 0x0039,
            WM_COMPACTING = 0x0041,
            WM_COMMNOTIFY = 0x0044,  /* no longer suported */
            WM_WINDOWPOSCHANGING = 0x0046,
            WM_WINDOWPOSCHANGED = 0x0047,

            WM_POWER = 0x0048,
            /*
             * wParam for WM_POWER window message and DRV_POWER driver notification
            #define PWR_OK              1
            #define PWR_FAIL            (-1)
            #define PWR_SUSPENDREQUEST  1
            #define PWR_SUSPENDRESUME   2
            #define PWR_CRITICALRESUME  3
             */

            WM_COPYDATA = 0x004A,
            WM_CANCELJOURNAL = 0x004B,

            /*
             * lParam of WM_COPYDATA message points to...
            typedef struct tagCOPYDATASTRUCT {
                ULONG_PTR dwData;
                DWORD cbData;
                PVOID lpData;
            } COPYDATASTRUCT, *PCOPYDATASTRUCT;
             */

            //#if(WINVER >= 0x0400)
            WM_NOTIFY = 0x004E,
            WM_INPUTLANGCHANGEREQUEST = 0x0050,
            WM_INPUTLANGCHANGE = 0x0051,
            WM_TCARD = 0x0052,
            WM_HELP = 0x0053,
            WM_USERCHANGED = 0x0054,
            WM_NOTIFYFORMAT = 0x0055,

            WM_CONTEXTMENU = 0x007B,
            WM_STYLECHANGING = 0x007C,
            WM_STYLECHANGED = 0x007D,
            WM_DISPLAYCHANGE = 0x007E,
            WM_GETICON = 0x007F,
            WM_SETICON = 0x0080,
            //#endif /* WINVER >= 0x0400 */

            WM_NCCREATE = 0x0081,
            WM_NCDESTROY = 0x0082,
            WM_NCCALCSIZE = 0x0083,
            WM_NCHITTEST = 0x0084,
            WM_NCPAINT = 0x0085,
            WM_NCACTIVATE = 0x0086,
            WM_GETDLGCODE = 0x0087,
            //#ifndef _WIN32_WCE
            WM_SYNCPAINT = 0x0088,
            //#endif
            WM_NCMOUSEMOVE = 0x00A0,
            WM_NCLBUTTONDOWN = 0x00A1,
            WM_NCLBUTTONUP = 0x00A2,
            WM_NCLBUTTONDBLCLK = 0x00A3,
            WM_NCRBUTTONDOWN = 0x00A4,
            WM_NCRBUTTONUP = 0x00A5,
            WM_NCRBUTTONDBLCLK = 0x00A6,
            WM_NCMBUTTONDOWN = 0x00A7,
            WM_NCMBUTTONUP = 0x00A8,
            WM_NCMBUTTONDBLCLK = 0x00A9,

            //#if(_WIN32_WINNT >= 0x0500)
            WM_NCXBUTTONDOWN = 0x00AB,
            WM_NCXBUTTONUP = 0x00AC,
            WM_NCXBUTTONDBLCLK = 0x00AD,
            //#endif /* _WIN32_WINNT >= 0x0500 */

            //#if(_WIN32_WINNT >= 0x0501)
            WM_INPUT = 0x00FF,
            //#endif /* _WIN32_WINNT >= 0x0501 */

            WM_KEYFIRST = 0x0100,
            WM_KEYDOWN = 0x0100,
            WM_KEYUP = 0x0101,
            WM_CHAR = 0x0102,
            WM_DEADCHAR = 0x0103,
            WM_SYSKEYDOWN = 0x0104,
            WM_SYSKEYUP = 0x0105,
            WM_SYSCHAR = 0x0106,
            WM_SYSDEADCHAR = 0x0107,
            //#if(_WIN32_WINNT >= 0x0501)
            WM_UNICHAR = 0x0109,
            WM_KEYLAST = 0x0109,
            //#define UNICODE_NOCHAR                  0xFFFF

            //#if(WINVER >= 0x0400)
            WM_IME_STARTCOMPOSITION = 0x010D,
            WM_IME_ENDCOMPOSITION = 0x010E,
            WM_IME_COMPOSITION = 0x010F,
            WM_IME_KEYLAST = 0x010F,
            //#endif /* WINVER >= 0x0400 */

            WM_INITDIALOG = 0x0110,
            WM_COMMAND = 0x0111,
            WM_SYSCOMMAND = 0x0112,
            WM_TIMER = 0x0113,
            WM_HSCROLL = 0x0114,
            WM_VSCROLL = 0x0115,
            WM_INITMENU = 0x0116,
            WM_INITMENUPOPUP = 0x0117,
            WM_MENUSELECT = 0x011F,
            WM_MENUCHAR = 0x0120,
            WM_ENTERIDLE = 0x0121,
            //#if(WINVER >= 0x0500)
            //#ifndef _WIN32_WCE
            WM_MENURBUTTONUP = 0x0122,
            WM_MENUDRAG = 0x0123,
            WM_MENUGETOBJECT = 0x0124,
            WM_UNINITMENUPOPUP = 0x0125,
            WM_MENUCOMMAND = 0x0126,

            //#ifndef _WIN32_WCE
            //#if(_WIN32_WINNT >= 0x0500)
            WM_CHANGEUISTATE = 0x0127,
            WM_UPDATEUISTATE = 0x0128,
            WM_QUERYUISTATE = 0x0129,

            WM_CTLCOLORMSGBOX = 0x0132,
            WM_CTLCOLOREDIT = 0x0133,
            WM_CTLCOLORLISTBOX = 0x0134,
            WM_CTLCOLORBTN = 0x0135,
            WM_CTLCOLORDLG = 0x0136,
            WM_CTLCOLORSCROLLBAR = 0x0137,
            WM_CTLCOLORSTATIC = 0x0138,
            //#define MN_GETHMENU                     0x01E1

            WM_MOUSEFIRST = 0x0200,
            WM_MOUSEMOVE = 0x0200,
            WM_LBUTTONDOWN = 0x0201,
            WM_LBUTTONUP = 0x0202,
            WM_LBUTTONDBLCLK = 0x0203,
            WM_RBUTTONDOWN = 0x0204,
            WM_RBUTTONUP = 0x0205,
            WM_RBUTTONDBLCLK = 0x0206,
            WM_MBUTTONDOWN = 0x0207,
            WM_MBUTTONUP = 0x0208,
            WM_MBUTTONDBLCLK = 0x0209,
            //#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
            WM_MOUSEWHEEL = 0x020A,
            //#endif
            //#if (_WIN32_WINNT >= 0x0500)
            WM_XBUTTONDOWN = 0x020B,
            WM_XBUTTONUP = 0x020C,
            WM_XBUTTONDBLCLK = 0x020D,
            //#endif
            //#if (_WIN32_WINNT >= 0x0500)
            WM_MOUSELAST = 0x020D,

            //#if(_WIN32_WINNT >= 0x0400)
            /* Value for rolling one detent */
            //#define WHEEL_DELTA                     120
            //#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

            /* Setting to scroll one page for SPI_GET/SETWHEELSCROLLLINES */
            //#define WHEEL_PAGESCROLL                (UINT_MAX)
            //#endif /* _WIN32_WINNT >= 0x0400 */

            WM_PARENTNOTIFY = 0x0210,
            WM_ENTERMENULOOP = 0x0211,
            WM_EXITMENULOOP = 0x0212,

            //#if(WINVER >= 0x0400)
            WM_NEXTMENU = 0x0213,
            WM_SIZING = 0x0214,
            WM_CAPTURECHANGED = 0x0215,
            WM_MOVING = 0x0216,
            //#endif /* WINVER >= 0x0400 */

            //#if(WINVER >= 0x0400)
            WM_POWERBROADCAST = 0x0218,
            /*
            #ifndef _WIN32_WCE
            #define PBT_APMQUERYSUSPEND             0x0000
            #define PBT_APMQUERYSTANDBY             0x0001

            #define PBT_APMQUERYSUSPENDFAILED       0x0002
            #define PBT_APMQUERYSTANDBYFAILED       0x0003

            #define PBT_APMSUSPEND                  0x0004
            #define PBT_APMSTANDBY                  0x0005

            #define PBT_APMRESUMECRITICAL           0x0006
            #define PBT_APMRESUMESUSPEND            0x0007
            #define PBT_APMRESUMESTANDBY            0x0008

            #define PBTF_APMRESUMEFROMFAILURE       0x00000001

            #define PBT_APMBATTERYLOW               0x0009
            #define PBT_APMPOWERSTATUSCHANGE        0x000A

            #define PBT_APMOEMEVENT                 0x000B
            #define PBT_APMRESUMEAUTOMATIC          0x0012
            #endif
            */
            //#endif /* WINVER >= 0x0400 */

            //#if(WINVER >= 0x0400)
            WM_DEVICECHANGE = 0x0219,
            //#endif /* WINVER >= 0x0400 */

            WM_MDICREATE = 0x0220,
            WM_MDIDESTROY = 0x0221,
            WM_MDIACTIVATE = 0x0222,
            WM_MDIRESTORE = 0x0223,
            WM_MDINEXT = 0x0224,
            WM_MDIMAXIMIZE = 0x0225,
            WM_MDITILE = 0x0226,
            WM_MDICASCADE = 0x0227,
            WM_MDIICONARRANGE = 0x0228,
            WM_MDIGETACTIVE = 0x0229,

            WM_MDISETMENU = 0x0230,
            WM_ENTERSIZEMOVE = 0x0231,
            WM_EXITSIZEMOVE = 0x0232,
            WM_DROPFILES = 0x0233,
            WM_MDIREFRESHMENU = 0x0234,

            //#if(WINVER >= 0x0400)
            WM_IME_SETCONTEXT = 0x0281,
            WM_IME_NOTIFY = 0x0282,
            WM_IME_CONTROL = 0x0283,
            WM_IME_COMPOSITIONFULL = 0x0284,
            WM_IME_SELECT = 0x0285,
            WM_IME_CHAR = 0x0286,
            //#endif /* WINVER >= 0x0400 */
            //#if(WINVER >= 0x0500)
            WM_IME_REQUEST = 0x0288,
            //#endif /* WINVER >= 0x0500 */
            //#if(WINVER >= 0x0400)
            WM_IME_KEYDOWN = 0x0290,
            WM_IME_KEYUP = 0x0291,
            //#endif /* WINVER >= 0x0400 */

            //#if((_WIN32_WINNT >= 0x0400) || (WINVER >= 0x0500))
            WM_MOUSEHOVER = 0x02A1,
            WM_MOUSELEAVE = 0x02A3,
            //#endif
            //#if(WINVER >= 0x0500)
            WM_NCMOUSEHOVER = 0x02A0,
            WM_NCMOUSELEAVE = 0x02A2,
            //#endif /* WINVER >= 0x0500 */

            //#if(_WIN32_WINNT >= 0x0501)
            WM_WTSSESSION_CHANGE = 0x02B1,

            WM_TABLET_FIRST = 0x02c0,
            WM_TABLET_LAST = 0x02df,
            //#endif /* _WIN32_WINNT >= 0x0501 */

            WM_CUT = 0x0300,
            WM_COPY = 0x0301,
            WM_PASTE = 0x0302,
            WM_CLEAR = 0x0303,
            WM_UNDO = 0x0304,
            WM_RENDERFORMAT = 0x0305,
            WM_RENDERALLFORMATS = 0x0306,
            WM_DESTROYCLIPBOARD = 0x0307,
            WM_DRAWCLIPBOARD = 0x0308,
            WM_PAINTCLIPBOARD = 0x0309,
            WM_VSCROLLCLIPBOARD = 0x030A,
            WM_SIZECLIPBOARD = 0x030B,
            WM_ASKCBFORMATNAME = 0x030C,
            WM_CHANGECBCHAIN = 0x030D,
            WM_HSCROLLCLIPBOARD = 0x030E,
            WM_QUERYNEWPALETTE = 0x030F,
            WM_PALETTEISCHANGING = 0x0310,
            WM_PALETTECHANGED = 0x0311,
            WM_HOTKEY = 0x0312,

            //#if(WINVER >= 0x0400)
            WM_PRINT = 0x0317,
            WM_PRINTCLIENT = 0x0318,
            //#endif /* WINVER >= 0x0400 */

            //#if(_WIN32_WINNT >= 0x0500)
            WM_APPCOMMAND = 0x0319,
            //#endif /* _WIN32_WINNT >= 0x0500 */

            //#if(_WIN32_WINNT >= 0x0501)
            WM_THEMECHANGED = 0x031A,
            //#endif /* _WIN32_WINNT >= 0x0501 */

            //#if(WINVER >= 0x0400)

            WM_HANDHELDFIRST = 0x0358,
            WM_HANDHELDLAST = 0x035F,

            WM_AFXFIRST = 0x0360,
            WM_AFXLAST = 0x037F,
            //#endif /* WINVER >= 0x0400 */

            WM_PENWINFIRST = 0x0380,
            WM_PENWINLAST = 0x038F,

            //#if(WINVER >= 0x0400)
            WM_APP = 0x8000,
            //#endif /* WINVER >= 0x0400 */

            /*
             * NOTE: All Message Numbers below 0x0400 are RESERVED.
             *
             * Private Window Messages Start Here:
             */
            WM_USER = 0x0400
        }

        #endregion

    }
}
