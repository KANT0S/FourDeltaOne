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

    public delegate int RPCStorageFileHandlerDelegate(byte[] data, byte[] filename, out string backupName);

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
            var fileID = npid.ToString("x16") + "_" + fileName;
            //var fsFile = StorageUtils.GetFilename(fileName, npid);

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

            // are we allowed to write this type of file?
            if (!_fileHooks.ContainsKey(fileName))
            {
                ReplyWithError(1);
                return;
            }

            Log.Info(string.Format("Got a request for writing {0} bytes to file {1} for user {2}.", fileData.Length, fileName, npid.ToString("X16")));

            CDatabase.Database.GetDocument<NPFile>(fileID, new MindTouch.Tasking.Result<NPFile>()).WhenDone(res =>
            {
                var document = new NPFile();
                document.Id = fileID;

                var isNew = true;

                if (res.Value != null)
                {
                    string backupFile;

                    try
                    {
                        int result = _fileHooks[fileName](fileData, Convert.FromBase64String(res.Value.FileData), out backupFile);

                        if (result > 0)
                        {
                            ReplyWithError(result);
                            return;
                        }
                    }
                    catch (Exception ex)
                    {
                        Log.Error(ex.ToString());

                        ReplyWithError(2);
                        return;
                    }

                    isNew = false;
                    document = res.Value;
                }

                document.SetData(fileData);

                MindTouch.Tasking.Result<NPFile> res2;

                if (isNew)
                {
                    res2 = CDatabase.Database.CreateDocument(document, new MindTouch.Tasking.Result<NPFile>());
                }
                else
                {
                    res2 = CDatabase.Database.UpdateDocument(document, new MindTouch.Tasking.Result<NPFile>());
                }

                res2.WhenDone(success =>
                {
                    ReplyWithError(0);
                },
                exception =>
                {
                    Log.Error(exception.ToString());
                    ReplyWithError(2);
                });
            });
        }

        /*public static bool PrepareWriteFile(long npid, string fileName, byte[] fileData, out string backupFile)
        {
            var fsFile = StorageUtils.GetFilename(fileName, npid);

            if (!_fileHooks.ContainsKey(fileName))
            {
                backupFile = null;
                return false;
            }

            int result = _fileHooks[fileName](fileData, fsFile, out backupFile);

            if (result > 0)
            {
                return false;
            }

            return true;
        }*/

        private void ReplyWithError(int error)
        {
            var reply = MakeResponse<StorageWriteUserFileResultMessage>(_client);
            reply.Message.result = error;
            reply.Message.fileName = _fileName;
            reply.Message.npid = (ulong)_npid;
            reply.Send();
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
            var fileID = npid.ToString("x16") + "_" + fileName;

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

            CDatabase.Database.GetDocument<NPFile>(fileID, new MindTouch.Tasking.Result<NPFile>()).WhenDone(
                file =>
                {
                    if (file != null)
                    {
                        ReplyWithError(0, Convert.FromBase64String(file.FileData));
                    }
                    else
                    {
                        if (Program.IW5Available || fileName != "mpdata") // IW5M block
                        {
                            ReplyWithError(1);
                        }
                    }
                },
                exception =>
                {
                    Log.Warn(exception.ToString());
                    ReplyWithError(3);
                }
            );
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
    }

    public partial class RPCStorageGetPublisherFileMessage : NPRPCMessage<StorageGetPublisherFileMessage>
    {
        private NPHandler _client;
        private string _fileName;
        private byte[] _readBuffer;

        private static Dictionary<string, byte[]> _publisherFiles = new Dictionary<string, byte[]>();

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
                if (_publisherFiles.ContainsKey(fileName))
                {
                    ReplyWithError(0, _publisherFiles[fileName]);
                    return;
                }

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

                _publisherFiles[_fileName] = _readBuffer;

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
