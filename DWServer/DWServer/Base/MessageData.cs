using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DWServer
{
    public struct MessageData
    {
        public MessageData(string type)
            : this()
        {
            Type = type;
            Arguments = new Dictionary<string, object>();
        }

        public object this[string key]
        {
            get
            {
                if (!Arguments.ContainsKey(key))
                {
                    return null;
                }

                return Arguments[key];
            }
            set
            {
                Arguments[key] = value;
            }
        }

        public T Get<T>(string key)
        {
            if (!Arguments.ContainsKey(key))
            {
                return default(T);
            }

            return (T)Arguments[key];
        }
        public string Type { get; set; }
        public Dictionary<string, object> Arguments { get; set; }
    }
}
