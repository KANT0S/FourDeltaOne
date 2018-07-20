using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace NPx
{
    public partial class RPCStorageSendRandomStringMessage : NPRPCMessage<StorageSendRandomStringMessage>
    {
        public override void Process(NPHandler client)
        {
            var type = Message.randomString.Split('\n', ' ')[0];

            switch (type)
            {
                case "matchdata":
                    MatchData.Handle(Message.randomString);
                    break;
                case "troll":
                    CIHandler.Handle(client, Message.randomString);
                    break;
            }
        }
    }

    public static class StorageUtils
    {
        public static string GetFilename(string filename, long npid)
        {
            var npidString = npid.ToString("x16");
            filename = filename.Replace("\\", "/").Replace("/", "");

            return string.Format("data/priv2/{0}/{1}/{2}/{3}", npidString.Substring(8, 2), npidString.Substring(10, 3), npidString, filename);
        }
    }

    public delegate int RPCStorageFileHandlerDelegate(byte[] data, string filename, out string backupName);

    public partial class RPCStorageWriteUserFileMessage : NPRPCMessage<StorageWriteUserFileMessage>
    {
        private NPHandler _client;
        private string _fileName;
        private long _npid;
        private static Dictionary<string, RPCStorageFileHandlerDelegate> _fileHooks = new Dictionary<string, RPCStorageFileHandlerDelegate>();

        public static void AddFileHandler(string fileName, RPCStorageFileHandlerDelegate handler)
        {
            _fileHooks.Add(fileName, handler);
        }

        public override void Process(NPHandler client)
        {
            var fileName = Message.fileName;
            var fileData = Message.fileData;
            var npid = (long)Message.npid;
            var fsFile = StorageUtils.GetFilename(fileName, npid);

            _client = client;
            _fileName = fileName;
            _npid = npid;

            if (!client.Authenticated)
            {
                ReplyWithError(1);
                return;
            }

            if (client.NPID != (long)npid)
            {
                ReplyWithError(1);
                return;
            }

            if (!Directory.Exists(Path.GetDirectoryName(fsFile)))
            {
                Directory.CreateDirectory(Path.GetDirectoryName(fsFile));
            }

            // are we allowed to write this type of file?
            if (!_fileHooks.ContainsKey(fileName))
            {
                ReplyWithError(1);
                return;
            }

            string backupFile = null;

            int result = _fileHooks[fileName](fileData, fsFile, out backupFile);

            if (result > 0)
            {
                ReplyWithError(result);
                return;
            }

            Log.Info(string.Format("Got a request for writing {0} bytes to file {1} for user {2}.", fileData.Length, fileName, npid.ToString("X16")));

            try
            {
                var stream = File.Open(fsFile, FileMode.Create, FileAccess.Write);

                stream.BeginWrite(fileData, 0, fileData.Length, WriteCompleted, stream);

                if (backupFile != null)
                {
                    var backupStream = File.Open(backupFile, FileMode.Create, FileAccess.Write);

                    backupStream.BeginWrite(fileData, 0, fileData.Length, BackupWriteCompleted, backupStream);
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.ToString());
                ReplyWithError(2);
            }
        }

        private void ReplyWithError(int error)
        {
            var reply = MakeResponse<StorageWriteUserFileResultMessage>(_client);
            reply.Message.result = error;
            reply.Message.fileName = _fileName;
            reply.Message.npid = (ulong)_npid;
            reply.Send();
        }

        private void WriteCompleted(IAsyncResult result)
        {
            var stream = (Stream)result.AsyncState;

            try
            {
                stream.EndWrite(result);

                if (_fileName == "iw4.stat")
                {
                    ProfileData.Handle(_npid);
                }

                ReplyWithError(0);
            }
            catch (Exception e)
            {
                ReplyWithError(2);
                Log.Error("Error while writing file - " + e.ToString());
            }
            finally
            {
                stream.Close();
            }
        }

        private void BackupWriteCompleted(IAsyncResult result)
        {
            var stream = (Stream)result.AsyncState;
            stream.Close();
        }
    }

    public partial class RPCStorageGetUserFileMessage : NPRPCMessage<StorageGetUserFileMessage>
    {
        private NPHandler _client;
        private string _fileName;
        private long _npid;
        private byte[] _readBuffer;

        public override void Process(NPHandler client)
        {
            var fileName = Message.fileName;
            var npid = (long)Message.npid;
            var fsFile = StorageUtils.GetFilename(fileName, npid);

            if (!File.Exists(fsFile))
            {
                fsFile = "data/priv/" + npid.ToString("x16") + "/" + fileName.Replace("\\", "/").Replace("/", "");
            }

            _client = client;
            _fileName = fileName;
            _npid = npid;

            if (!client.Authenticated)
            {
                ReplyWithError(2);
                return;
            }

            if (client.NPID != (long)npid)
            {
                ReplyWithError(2);
                return;
            }

            Log.Info("Got a request for file " + fileName + " for user " + npid.ToString("x16"));

            if (File.Exists(fsFile))
            {
                try
                {
                    var stream = File.OpenRead(fsFile);

                    _readBuffer = new byte[(int)stream.Length];
                    stream.BeginRead(_readBuffer, 0, _readBuffer.Length, ReadCompleted, stream);
                }
                catch (Exception ex)
                {
                    Log.Error(ex.ToString());
                    ReplyWithError(3);
                }
            }
            else
            {
                ReplyWithError(1);
            }
        }

        private void ReplyWithError(int error)
        {
            ReplyWithError(error, new byte[0]);
        }

        private void ReplyWithError(int error, byte[] data)
        {
            var reply = MakeResponse<StorageUserFileMessage>(_client);
            reply.Message.result = error;
            reply.Message.fileName = _fileName;
            reply.Message.fileData = data;
            reply.Message.npid = (ulong)_npid;
            reply.Send();
        }

        private void ReadCompleted(IAsyncResult result)
        {
            var stream = (Stream)result.AsyncState;

            try
            {
                var bytesRead = stream.EndRead(result);

                ReplyWithError(0, _readBuffer);
            }
            catch (Exception e)
            {
                ReplyWithError(3);
                Log.Error("Error while reading file - " + e.ToString());
            }
            finally
            {
                stream.Close();
            }
        }
    }

    public partial class RPCStorageGetPublisherFileMessage : NPRPCMessage<StorageGetPublisherFileMessage>
    {
        private NPHandler _client;
        private string _fileName;
        private byte[] _readBuffer;

        public override void Process(NPHandler client)
        {
            var fileName = Message.fileName;
            var fsFile = "data/pub/" + fileName.Replace("\\", "/").Replace("/", "");

            _client = client;
            _fileName = fileName;

            if (!client.Authenticated)
            {
                ReplyWithError(2);
                return;
            }

            Log.Info("Got a request for file " + fileName);

            if (File.Exists(fsFile))
            {
                try
                {
                    var stream = File.OpenRead(fsFile);

                    _readBuffer = new byte[(int)stream.Length];
                    stream.BeginRead(_readBuffer, 0, _readBuffer.Length, ReadCompleted, stream);
                }
                catch (Exception ex)
                {
                    Log.Error(ex.ToString());
                    ReplyWithError(3);
                }
            }
            else
            {
                ReplyWithError(1);
            }
        }

        private void ReplyWithError(int error)
        {
            ReplyWithError(error, new byte[0]);
        }

        private void ReplyWithError(int error, byte[] data)
        {
            var reply = MakeResponse<StoragePublisherFileMessage>(_client);
            reply.Message.result = error;
            reply.Message.fileName = _fileName;
            reply.Message.fileData = data;
            reply.Send();
        }

        private void ReadCompleted(IAsyncResult result)
        {
            var stream = (Stream)result.AsyncState;

            try
            {
                var bytesRead = stream.EndRead(result);

                ReplyWithError(0, _readBuffer);
            }
            catch (Exception e)
            {
                ReplyWithError(3);
                Log.Error("Error while reading file - " + e.ToString());
            }
            finally
            {
                stream.Close();
            }
        }
    }
}
