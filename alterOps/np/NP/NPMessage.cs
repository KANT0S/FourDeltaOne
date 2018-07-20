using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using NPx;

namespace NPx
{
    public class NPMessage
    {
        private NPHandler _client;

        public NPMessage(NPHandler client)
        {
            _client = client;
        }

        public int Type { get; set; }
        public int ID { get; set; }
        public MemoryStream Buffer { get; set; }

        public void Process()
        {
            try
            {
                var message = NPMessageFactory.CreateMessage(Type);

                Log.Debug("Dispatching a " + message.GetType().Name);

                message.ID = ID;
                message.Deserialize(Buffer);
                message.Process(_client);
            }
            catch (Exception e)
            {
                Log.Error(e.ToString());
            }
        }
    }
}
