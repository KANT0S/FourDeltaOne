namespace secretSchemes
{
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Text;

    public class ProgressDialog
    {
        private string _CancelMessage = string.Empty;
        private string _Line1 = string.Empty;
        private string _Line2 = string.Empty;
        private string _Line3 = string.Empty;
        private uint _maximum = 100;
        private IntPtr _parentHandle;
        private string _Title = string.Empty;
        private uint _value;
        private Win32IProgressDialog pd;

        public ProgressDialog(IntPtr parentHandle)
        {
            this._parentHandle = parentHandle;
        }

        public void CloseDialog()
        {
            if (this.pd != null)
            {
                this.pd.StopProgressDialog();
                this.pd = null;
            }
        }

        public void ShowDialog(params PROGDLG[] flags)
        {
            if (this.pd == null)
            {
                this.pd = (Win32IProgressDialog)new Win32ProgressDialog();
                this.pd.SetTitle(this._Title);
                this.pd.SetCancelMsg(this._CancelMessage, null);
                this.pd.SetLine(1, this._Line1, false, IntPtr.Zero);
                this.pd.SetLine(2, this._Line2, false, IntPtr.Zero);
                this.pd.SetLine(3, this._Line3, false, IntPtr.Zero);
                PROGDLG normal = PROGDLG.Normal;
                if (flags.Length != 0)
                {
                    normal = flags[0];
                    for (int i = 1; i < flags.Length; i++)
                    {
                        normal |= flags[i];
                    }
                }
                this.pd.StartProgressDialog(this._parentHandle, null, normal, IntPtr.Zero);
            }
        }

        public string CancelMessage
        {
            get
            {
                return this._CancelMessage;
            }
            set
            {
                this._CancelMessage = value;
                if (this.pd != null)
                {
                    this.pd.SetCancelMsg(this._CancelMessage, null);
                }
            }
        }

        public bool HasUserCancelled
        {
            get
            {
                return ((this.pd != null) && this.pd.HasUserCancelled());
            }
        }

        public string Line1
        {
            get
            {
                return this._Line1;
            }
            set
            {
                this._Line1 = value;
                if (this.pd != null)
                {
                    this.pd.SetLine(1, this._Line1, false, IntPtr.Zero);
                }
            }
        }

        public string Line2
        {
            get
            {
                return this._Line2;
            }
            set
            {
                this._Line2 = value;
                if (this.pd != null)
                {
                    this.pd.SetLine(2, this._Line2, false, IntPtr.Zero);
                }
            }
        }

        public string Line3
        {
            get
            {
                return this._Line3;
            }
            set
            {
                this._Line3 = value;
                if (this.pd != null)
                {
                    this.pd.SetLine(3, this._Line3, false, IntPtr.Zero);
                }
            }
        }

        public uint Maximum
        {
            get
            {
                return this._maximum;
            }
            set
            {
                this._maximum = value;
                if (this.pd != null)
                {
                    this.pd.SetProgress(this._value, this._maximum);
                }
            }
        }

        public string Title
        {
            get
            {
                return this._Title;
            }
            set
            {
                this._Title = value;
                if (this.pd != null)
                {
                    this.pd.SetTitle(this._Title);
                }
            }
        }

        public uint Value
        {
            get
            {
                return this._value;
            }
            set
            {
                this._value = value;
                if (this.pd != null)
                {
                    this.pd.SetProgress(this._value, this._maximum);
                }
            }
        }

        public enum PDTIMER : uint
        {
            Pause = 2,
            Reset = 1,
            Resume = 3
        }

        [Flags]
        public enum PROGDLG : uint
        {
            AutoTime = 2,
            Modal = 1,
            NoMinimize = 8,
            NoProgressBar = 0x10,
            Normal = 0,
            NoTime = 4
        }

        public static class shlwapi
        {
            [DllImport("shlwapi.dll", CharSet = CharSet.Auto)]
            private static extern bool PathCompactPath(IntPtr hDC, [In, Out] StringBuilder pszPath, int dx);
        }

        [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("EBBC7C04-315E-11d2-B62F-006097DF5BD4")]
        public interface Win32IProgressDialog
        {
            void StartProgressDialog(IntPtr hwndParent, [MarshalAs(UnmanagedType.IUnknown)] object punkEnableModless, ProgressDialog.PROGDLG dwFlags, IntPtr pvResevered);
            void StopProgressDialog();
            void SetTitle([MarshalAs(UnmanagedType.LPWStr)] string pwzTitle);
            void SetAnimation(IntPtr hInstAnimation, ushort idAnimation);
            [return: MarshalAs(UnmanagedType.Bool)]
            [PreserveSig]
            bool HasUserCancelled();
            void SetProgress(uint dwCompleted, uint dwTotal);
            void SetProgress64(ulong ullCompleted, ulong ullTotal);
            void SetLine(uint dwLineNum, [MarshalAs(UnmanagedType.LPWStr)] string pwzString, [MarshalAs(UnmanagedType.VariantBool)] bool fCompactPath, IntPtr pvResevered);
            void SetCancelMsg([MarshalAs(UnmanagedType.LPWStr)] string pwzCancelMsg, object pvResevered);
            void Timer(ProgressDialog.PDTIMER dwTimerAction, object pvResevered);
        }

        [ComImport, Guid("F8383852-FCD3-11d1-A6B9-006097DF5BD4")]
        public class Win32ProgressDialog
        {
        }
    }
}

