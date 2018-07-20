using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;
using MongoDB.Driver;
using MongoDB.Driver.Builders;

namespace DWServer
{
    public class DWStorage
    {
        private class PerfState
        {
            public int Min { get; set; }
            public int Max { get; set; }

            public void Add(int time)
            {
                if (time < Min || Min == 0)
                {
                    Min = time;
                }

                if (time > Max)
                {
                    Max = time;
                }
            }
        }

        private static Dictionary<int, PerfState> _states = new Dictionary<int, PerfState>();

        private static PerfState GetPerfState(int type)
        {
            if (!_states.ContainsKey(type))
            {
                _states.Add(type, new PerfState());
            }

            return _states[type];
        }

        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            var weHandle = ((type == 10) || (type == 18) || (type == 8 || (crypt && type == 28) || type == 50 || type == 4 || type == 58) || (type == 23));

            try
            {
                if (type == 10)
                {
                    var packet = DWRouter.GetMessage(data);
                    var call = packet.ByteBuffer.ReadByte();
                    var initTime = DateTime.Now;

                    switch (call)
                    {
                        case 1:
                            UploadFile(data, packet);
                            break;
                        case 3:
                            GetFile(data, packet);
                            break;
                        case 7:
                            GetPublisherFile(data, packet);
                            break;
                        default:
                            Log.Debug("unknown packet " + call + " in bdStorage");
                            DWRouter.Unknown(data, packet);
                            break;
                    }
                }
                else if (type == 18)
                {
                    try
                    {
                        var packet = DWRouter.GetMessage(data);
                        var reply = packet.MakeReply(5, false);
                        reply.ByteBuffer.Write(new byte[] { 0x2d, 0xb3, 0xa2, 0x4f, 0x1a, 0x36, 0xd0, 0xd2, 0x00, 0x00, 0x04, 0x00, 0x00, 0xee, 0x02, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0xd0, 0x07, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x88, 0x13, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x02, 0x0c, 0xd1, 0xaa, 0x7a, 0xb1, 0x31, 0xb1, 0xaa, 0x6b, 0x83, 0x51, 0x00, 0x00 });
                        reply.Send(true);
                    }
                    catch { }
                }
                else if (type == 8 || (crypt && type == 28) || type == 50 || type == 4 || type == 58)
                {
                    try
                    {
                        var packet = DWRouter.GetMessage(data);

                        try
                        {
                            var call = packet.ByteBuffer.ReadByte();
                            //Log.Debug("Service call identifier is " + call + ".");
                        }
                        catch { }

                        //DWRouter.Unknown(data, packet);
                    }
                    catch { }
                }
                else if (type == 23)
                {
                    try
                    {
                        var packet = DWRouter.GetMessage(data);
                        var call = packet.ByteBuffer.ReadByte();

                        switch (call)
                        {
                            case 2:
                                GetCounterTotals(data, packet);
                                break;
                            default:
                                Log.Debug("unknown packet " + call + " in bdCounter");
                                //DWRouter.Unknown(data, packet);
                                break;
                        }
                    }
                    catch { }
                }
                /*else if (type != 12 && crypt)
                {
                    var packet = DWRouter.GetMessage(data);
                    DWRouter.Unknown(data, packet);
                }*/
            }
            catch (Exception e)
            {
                Log.Error(e.ToString());
            }

            if (weHandle)
            {
                var upacket = DWRouter.GetMessage(data);
            }
        }

        private static void GetCounterTotals(MessageData mdata, DWMessage packet)
        {
            try
            {
                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000002);
                reply.ByteBuffer.Write((uint)0);
                reply.ByteBuffer.Write((byte)2);
                reply.ByteBuffer.Write((uint)100);
                reply.ByteBuffer.Write((uint)100);

                for (int i = 0; i < 100; i++)
                {
                    var id = packet.ByteBuffer.ReadUInt32();

                    reply.ByteBuffer.Write(id);
                    byte[] buffer = new byte[sizeof(Int64)]; // from internLabs DWServer, random number generator
                    Random random = new Random();

                    random.NextBytes(buffer);
                    long signed = BitConverter.ToInt64(buffer, 0);

                    reply.ByteBuffer.Write((long)signed); // random number, mmk?
                    //reply.ByteBuffer.Write((long)9223372036854775807); pffft

                    /* from NN DWServer
                    //reply.ByteBuffer.Write((long)1337);
                    reply.ByteBuffer.Write((long)9223372036854775807);
                    */
                }

                reply.Send(true);
            }
            catch { }
        }

        public class File
        {
            [BsonId]
            public ObjectId id;
            public int userid;
            public string filename;
            public byte[] data;
            public int filesize;
        }

        private static readonly Random r = new Random();

        private static void UploadFile(MessageData mdata, DWMessage packet)
        {
            var filename = packet.ByteBuffer.ReadString();
            var test = packet.ByteBuffer.ReadBool();
            var data = packet.ByteBuffer.ReadBlob();
            // NOTE: AN UINT64 MIGHT FOLLOW HERE. THIS COULD BE USED BY DEDICATED SERVER-STYLE LICENSES FOR WRITING USER STATS.
            var user = (ulong)0;
            try
            {
                user = packet.ByteBuffer.ReadUInt64();
            }
            catch { }

            if (user == 0)
            {
                /*lock (DWRouter.Connections)
                {
                    user = (from conn in DWRouter.Connections
                            where conn.Value == mdata.Get<string>("cid")
                            select conn.Key).FirstOrDefault();
                }*/
                user = DWRouter.GetIDForData(mdata);
            }
            //var user = 0x1100001ded1ca7e;//packet.ByteBuffer.ReadUInt64();
            var path = filename + "_" + user.ToString("x16");
            /*MySqlDataReader query = Database.Query("SELECT * FROM files WHERE filename='" + path + "'");
            if (query.HasRows)
            {
                query.Close();
                Log.Debug("Trying to update " + path + "... for user ");
                Database.UploadQuery("UPDATE files SET data = ?filedata, filesize = '" + data.Length + "', userid='" + Convert.ToString((int)user) + "' WHERE filename='" + path + "'", data);
            }
            else
            {
                query.Close();
                Log.Debug("Trying to write " + path + "... for user ");
                Database.UploadQuery("INSERT INTO files(userid, filename, data, filesize) VALUES('" + Convert.ToString((int)user) + "', '" + path + "', ?filedata, '" + data.Length + "')", data);
            }*/

            Log.Debug("Trying to write " + path + "...");
            
            var file = new File()
                {
                    filename = path,
                    userid = (int)user,
                    filesize = data.Length,
                    data = data
                };

            var query = Query.EQ("filename", path);
            var files = Database.AFiles.Find(query);

            if (files.Count() > 0)
            {
                file.id = files.First().id;
            }

            Database.AFiles.Save(file);
            

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)1);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((uint)0);
            reply.Send(true);
        }

        private static void GetFile(MessageData mdata, DWMessage packet)
        {
            var filename = packet.ByteBuffer.ReadString();
            var user = packet.ByteBuffer.ReadUInt64();

            if (user == 0)
            {
                user = DWRouter.GetIDForData(mdata);
            }

            var path = filename + "_" + user.ToString("x16");
            var e = r.Next(1, 164);

            Log.Debug("Trying to send " + path + "... (originID " + e + ")");

            var query = Query.EQ("filename", path);
            var files = Database.AFiles.Find(query);

            if (files.Count() > 0)
            {
                var file = files.First();
                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000001);
                reply.ByteBuffer.Write((uint)0);
                reply.ByteBuffer.Write((byte)7);
                reply.ByteBuffer.Write((uint)1);
                reply.ByteBuffer.Write((uint)1);
                reply.ByteBuffer.WriteBlob(file.data);
                reply.Send(true);
            }
            else
            {
                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000001);
                reply.ByteBuffer.Write((uint)0x3E8);
                reply.Send(true);
            }
        }

        private static Dictionary<string, byte[]> _defaultFiles = new Dictionary<string, byte[]>();

        private static void GetPublisherFile(MessageData mdata, DWMessage packet)
        {
            try
            {

                var filename = packet.ByteBuffer.ReadString();

                Log.Debug("Trying to send " + filename + "...");

                //var data = File.ReadAllBytes(@"data/pub/" + filename);

                if (!_defaultFiles.ContainsKey(filename))
                {
                    FileStream fileStream = new FileStream(@"data/pub/" + filename, FileMode.Open, FileAccess.Read);

                    int offset = 0;
                    byte[] data = new byte[fileStream.Length];
                    int remaining = (int)fileStream.Length;
                    while (remaining > 0)
                    {
                        int read = fileStream.Read(data, offset, remaining);
                        if (read <= 0)
                            throw new EndOfStreamException
                                (String.Format("End of stream reached with {0} bytes left to read", remaining));
                        remaining -= read;
                        offset += read;
                    }

                    fileStream.Close();
                    fileStream.Dispose();

                    _defaultFiles.Add(filename, data);
                }

                var reply = packet.MakeReply(1, false);
                reply.ByteBuffer.Write(0x8000000000000001);
                reply.ByteBuffer.Write((uint)0);
                reply.ByteBuffer.Write((byte)7);
                reply.ByteBuffer.Write((uint)1);
                reply.ByteBuffer.Write((uint)1);
                reply.ByteBuffer.WriteBlob(_defaultFiles[filename]);
                reply.Send(true);
            }
            catch { }
        }

        public static void FlushPublisherFiles()
        {
            lock (_defaultFiles)
            {
                _defaultFiles.Clear();
            }
        }
    }
}
