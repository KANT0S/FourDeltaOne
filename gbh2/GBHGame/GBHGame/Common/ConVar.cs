using System;
using System.Collections.Generic;

namespace GBH
{
    public class ConVar
    {
        #region static members
        private static Dictionary<string, ConVar> _convars = new Dictionary<string, ConVar>();

        public static void Initialize()
        {
            ConVar.Register("cheats", false, "Enable cheats", ConVarFlags.None);
        }

        public static IEnumerable<ConVar> GetConVars()
        {
            return _convars.Values;
        }

        public static ConVar Find(string name)
        {
            name = name.ToLowerInvariant();

            if (_convars.ContainsKey(name))
            {
                return _convars[name];
            }

            return null;
        }

        public static ConVar Register<T>(string name, T defaultValue, string description, ConVarFlags flags)
        {
            var existingVar = Find(name);

            if (existingVar != null)
            {
                return Reregister(name, typeof(T), defaultValue, description, flags);
            }
            else
            {
                return RegisterNew(name, typeof(T), defaultValue, description, flags);
            }
        }

        private static ConVar Reregister(string name, Type type, object defaultValue, string description, ConVarFlags flags)
        {
            var convar = Find(name);

            if (convar != null)
            {
                try
                {
                    convar._value = Convert.ChangeType(convar._value, type);
                    convar._defaultValue = defaultValue;
                    convar._type = type;
                    convar._description = description;
                    convar._flags = flags;
                }
                catch (InvalidCastException)
                {
                    Log.Write(LogLevel.Warning, "InvalidCastException while converting ConVar from {0} to {1}", convar._type.Name, type.Name);
                }
                catch (ArgumentException)
                {
                    Log.Write(LogLevel.Warning, "ArgumentException while converting ConVar from {0} to {1}", convar._type.Name, type.Name);
                }
            }

            return convar;
        }

        private static ConVar RegisterNew(string name, Type type, object defaultValue, string description, ConVarFlags flags)
        {
            var convar = new ConVar(name);
            convar._type = type;
            convar._value = defaultValue;
            convar._defaultValue = defaultValue;
            convar._description = description;
            convar._flags = flags;

            _convars.Add(name.ToLowerInvariant(), convar);

            return convar;
        }

        public static T GetValue<T>(string name)
        {
            var convar = Find(name);

            if (convar != null)
            {
                return convar.GetValue<T>();
            }

            return default(T);
        }

        public static ConVar SetValue<T>(string name, T value)
        {
            return SetValue<T>(name, value, true);
        }

        public static ConVar SetValue<T>(string name, T value, bool force)
        {
            var convar = Find(name);

            if (convar == null)
            {
                convar = RegisterNew(name, typeof(T), value, "External ConVar", (force) ? ConVarFlags.None : ConVarFlags.UserCreated);
            }
            else
            {
                convar.SetValue<T>(value, force);
            }

            return convar;
        }

        public static void HandleCommand(string[] args)
        {
            ConVar command = Find(args[0]);

            if (command == null)
            {
                return;
            }

            if (args.Length == 1)
            {
                Log.Write(LogLevel.Info, "\"{0}\" is:\"{1}^7\" default:\"{2}^7\"", command.Name, command.GetValue<string>(), command._defaultValue.ToString());
                return;
            }

            command.SetValue<string>(args[1], false);
        }
        #endregion

        #region instance members
        private string _name;
        private Type _type;
        private object _value;
        private object _defaultValue;
        private string _description;
        private ConVarFlags _flags;

        private ConVar(string name)
        {
            _name = name;
            _description = "";
            _type = typeof(string);
            _value = "";
            _defaultValue = "";
        }

        public string Name
        {
            get
            {
                return _name;
            }
        }

        public string Description
        {
            get
            {
                return _description;
            }
        }

        public ConVarFlags Flags
        {
            get
            {
                return _flags;
            }
        }

        public T GetValue<T>()
        {
            try
            {
                var value = _value;

                if (typeof(T) != _type)
                {
                    value = Convert.ChangeType(_value, typeof(T));
                }

                // special-case null strings caused by error conditions
                if (value == null && typeof(T) == typeof(string))
                {
                    return (T)((object)string.Empty);
                }

                return (T)value;
            }
            catch (InvalidCastException)
            {
                Log.Write(LogLevel.Warning, "InvalidCastException while converting ConVar from {0} to {1}", _type.Name, typeof(T).Name);
            }
            catch (ArgumentException)
            {
                Log.Write(LogLevel.Warning, "ArgumentException while converting ConVar from {0} to {1}", _type.Name, typeof(T).Name);
            }
            catch (OverflowException)
            {
                Log.Write(LogLevel.Warning, "OverflowException while converting ConVar from {0} to {1}", _type.Name, typeof(T).Name);
            }

            return default(T);
        }

        public void SetValue<T>(T value, bool force)
        {
            if (!force)
            {
                if (TestFlag(ConVarFlags.ReadOnly))
                {
                    Log.Write(LogLevel.Info, "{0} is read only.", _name);
                    return;
                }

                if (TestFlag(ConVarFlags.WriteProtected))
                {
                    Log.Write(LogLevel.Info, "{0} is write protected.", _name);
                    return;
                }

                if (TestFlag(ConVarFlags.Cheat) && !GetValue<bool>("cheats"))
                {
                    Log.Write(LogLevel.Info, "{0} is cheat protected.", _name);
                    return;
                }
            }

            try
            {
                if (typeof(T) != _type)
                {
                    _value = Convert.ChangeType(value, _type);
                }
                else
                {
                    _value = value;
                }
            }
            catch (FormatException)
            {
                Log.Write(LogLevel.Warning, "{0} is an invalid value for variable {1} (type {2}).", value, _name, _type.Name);
                //_value = _defaultValue;
            }
            catch (InvalidCastException)
            {
                Log.Write(LogLevel.Warning, "{0} cannot be converted to {1}.", typeof(T).Name, _type.Name);
                //_value = _defaultValue;
            }
            catch (OverflowException)
            {
                Log.Write(LogLevel.Warning, "{0} cannot be converted to {1}.", typeof(T).Name, _type.Name);
            }
        }

        private bool TestFlag(ConVarFlags flag)
        {
            return ((_flags & flag) == flag);
        }
        #endregion
    }

    [Flags]
    public enum ConVarFlags
    {
        None = 0,
        Archived = 1,
        ReadOnly = 2,
        WriteProtected = 4,
        Cheat = 8,
        UserCreated = 16
    }
}
