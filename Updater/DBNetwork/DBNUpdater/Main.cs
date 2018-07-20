using System;
using System.Collections.Generic;
using System.Xml.Linq;
using System.Linq;
using System.Net;
using System.IO;
using System.Threading;
using SevenZip.Compression;

namespace DBNUpdater
{
    public class Updater
    {
        public string Location { get; set; }
        private string _server = "";
        private string _localCacheLocation = "";
        private Caches _remoteCache;
        private Caches _localCache;
        private static CustomWebClient _wc = new CustomWebClient();
        private Thread _thread;
        private bool _usecpart = true;
        private Exception _lastException;
        public event EventHandler<StatusChangedEventArgs> StatusChanged;

        static Updater()
        {
            Log.Initialize("DBNUpdater.log", LogLevel.All, false);
        }
        public Updater(string remoteserver, string localCacheLocation, bool useCpartFiles)
        {
            _usecpart = useCpartFiles;
            Location = Directory.GetCurrentDirectory();
            _server = remoteserver;
            _localCacheLocation = localCacheLocation;
            Log.Info("--------------");
            Log.Info(String.Format("Initializing DBNUpdater with {0}, {1} and {2}. Using cpart: {3}", _server, _localCacheLocation, Location, _usecpart));
            Log.Info("--------------");
        }
        public void Kill()
        {
            _thread.Abort();
        }
        public void Start(string[] wantedCaches)
        {
            _thread = new Thread(delegate()
            {
#if !DEBUG
                try
                {
#endif
                    SendStatus(StatusChangedEnum.Start);
                    SendStatus(StatusChangedEnum.CacheDownloading, "caches.xml", 0);
                    Log.Info(String.Format("Starting Update process. Wanted caches: {0}",
                                           String.Join(", ", wantedCaches)));
                    //Step 1. Get the remote file.
                    var file = _wc.DownloadString(_server + "caches.xml");
                    var xml = XDocument.Parse(file);
                    _remoteCache = new Caches(xml);
                    Log.Info("Remote caches have been downloaded.");
                    var versions = _remoteCache.Versions();
                    foreach (var a in versions)
                    {
                        Log.Info(String.Format("Cache: {0} Version: {1}", a.Key, a.Value));
                    }
                    SendStatus(StatusChangedEnum.CacheDownloaded, new object[] {"caches.xml", versions}, 0);
                    //populate this with all possible caches
                    var cachesToUpdate = new List<string>(wantedCaches);

                    //step 2. Get the local file, and compare
                    if (_localCacheLocation != "" && File.Exists(_localCacheLocation))
                    {
                        file = File.ReadAllText(_localCacheLocation);
                        xml = XDocument.Parse(file);
                        _localCache = new Caches(xml);
                        Log.Info("Local caches:");
                        foreach (var a in _localCache.Versions())
                        {
                            Log.Info(String.Format("Cache: {0} Version: {1}", a.Key, a.Value));
                        }
                        //step 2a. Compare version numbers.
                        var localVersions = _localCache.Versions();
                        foreach (var wantedCache in wantedCaches)
                        {
                            if (versions.ContainsKey(wantedCache))
                            {
                                if (localVersions.ContainsKey(wantedCache) &&
                                    versions[wantedCache] <= localVersions[wantedCache])
                                {
                                    //update is not needed for this cache.
                                    cachesToUpdate.Remove(wantedCache);
                                }
                            }
                            else
                            {
                                cachesToUpdate.Remove(wantedCache);
                            }
                        }
                    }
                    else
                    {
                        Log.Info("Local caches are not found or unused. Downloading all wanted caches.");
                        SendStatus(StatusChangedEnum.LocalCachesNotFound);
                    }
                    //cachesToUpdate now contains a list of all caches to get.

                    if (cachesToUpdate.Count == 0)
                    {
                        //success - no update needed
                        tasksDone = 0;
                        tasksTotal = 1;
                        Log.Info("no update needed");
                        SendStatus(StatusChangedEnum.NoUpdateNeeded);
                        SendStatus(StatusChangedEnum.Finish);
                        return;
                    }
                    tasksDone = 0;
                    tasksTotal = cachesToUpdate.Count;
                    Log.Info(String.Format("Caches to download: {0}", String.Join(", ", cachesToUpdate.ToArray())));
                    //step 3. Get the CacheInfo for each wanted cache.
                    var cacheContentFileList = new List<CacheContentFile>();
                    foreach (var cache in cachesToUpdate)
                    {
                        SendStatus(StatusChangedEnum.CacheDownloading, cache, 1);
                        file = _wc.DownloadString(_server + cache + "/info.xml");
                        xml = XDocument.Parse(file);
                        var ci = new CacheInfo(this, Location, cache, xml);
                        var files = ci.Files();
                        cacheContentFileList.AddRange(files);
                        SendStatus(StatusChangedEnum.CacheDownloaded, new object[] {cache, files}, 0);
                    }
                    tasksDone = 0;
                    tasksTotal = cacheContentFileList.Sum(cifile => cifile.CompressedSize);
                    if (tasksTotal != 0)
                    {
                        SendStatus(StatusChangedEnum.DownloadsStart);
                        foreach (var cifile in cacheContentFileList)
                        {
                            //step 4. Download and decompress.
                            Log.Info(String.Format("Downloading {0}", cifile.Cache + "/" + cifile.Name));

                            int retries = 0;
                            int maxRetries = 3;
                            int rampUpTime = 2;
                            bool success = false;

                            while (!success && retries <= maxRetries)
                            {
                                //totalProgress = cifile.CompressedSize;
                                SendStatus(StatusChangedEnum.FileStart, cifile.Cache + "/" + cifile.Name, 0);

                                success = Download(cifile);

                                if (!success)
                                {
                                    SendStatus(StatusChangedEnum.FileFailed, cifile.Cache + "/" + cifile.Name,
                                               -cifile.CompressedSize);
                                    //backtrack the progress

                                    retries++;
                                    if (retries <= maxRetries)
                                    {
                                        int waitTime = (int) (Math.Pow(rampUpTime, retries))*1000;
                                        SendStatus(StatusChangedEnum.FileRedownload,
                                                   new object[] {cifile.Cache + "/" + cifile.Name, waitTime}, 0);
                                        Log.Warn(string.Format("Redownloading {0} (sleeping {1} ms)", cifile.Name,
                                                               waitTime));
                                        Thread.Sleep(waitTime);
                                    }
                                }
                            }

                            if (!success)
                            {
                                SendStatus(StatusChangedEnum.Fail, _lastException, 0);
                                return;
                            }
                            SendStatus(StatusChangedEnum.FileFinish, cifile.Cache + "/" + cifile.Name, 0);
                        }
                        SendStatus(StatusChangedEnum.DownloadsFinish);
                    }
                    //step 5. Save remote cache as local cache.
                    if(_localCacheLocation != "")
                    {
                        file = _remoteCache.ToString();
                        var path = Path.Combine(Location, _localCacheLocation);
                        File.WriteAllText(path, file);
                        SendStatus(StatusChangedEnum.LocalCacheSaved);
                    }
                    SendStatus(StatusChangedEnum.Finish);
#if !DEBUG
                }
                catch (Exception e){
                    SendStatus(StatusChangedEnum.Fail, e, 0);
                }
#endif
                
                
            });
            _thread.Start();
        }

        private long tasksDone = 0;
        private long tasksTotal = 1;
        private bool Download(CacheContentFile file)
        {
            var fileName = Path.Combine(Location, file.Name);
            var outputDirectory = Path.GetDirectoryName(fileName);
            var temporaryFile = Path.Combine(outputDirectory, file.SHA1Hash + ".cpart");
            var URL = _server + file.Cache + "/" + file.Name;
            if (!Directory.Exists(outputDirectory)) Directory.CreateDirectory(outputDirectory);
            int startAt = 0;

            Log.Info(String.Format("Starting to download {0}, {1} bytes.", file.Name, file.Size));
            if(File.Exists(temporaryFile))
            {
                //resume the download
                if(_usecpart)
                {
                    var fi = new FileInfo(temporaryFile);
                    startAt = (int) fi.Length;
                    SendStatus(StatusChangedEnum.FileDownloading, new object[] { file.Name, file.CompressedSize, startAt}, startAt);
                    Log.Info(String.Format("Partial file for {1} found, starting download at {0} bytes.", startAt, file.Name));
                }
                else
                {
                    File.Delete(temporaryFile);
                }
            }
            bool compressed = false;
#if !DEBUG
            try
            {
#endif
                if(Utilities.URLExists(URL + ".lzma"))
                {
                    compressed = true;
                    URL += ".lzma";
                    SendStatus(StatusChangedEnum.FileDownloadingLzma, file.Name, 0);
                    Log.Info(String.Format("Found a compressed file for {0}.", fileName));
                }
                Stream inStream, outStream;
                if (startAt != file.Size && startAt != file.CompressedSize) //if they equal, ._.
                {
                    // initialize request
                    var webRequest = (HttpWebRequest) WebRequest.Create(URL);
                    webRequest.AddRange(startAt);

                    var webResponse = (HttpWebResponse) webRequest.GetResponse();

                    inStream = webResponse.GetResponseStream();
                    outStream = File.Open(temporaryFile, FileMode.Append, FileAccess.Write, FileShare.Read);

                    var buffer = new byte[2048];
                    var lastBytes = 0;

                    while ((lastBytes = inStream.Read(buffer, 0, buffer.Length)) > 0)
                    {
                        outStream.Write(buffer, 0, lastBytes);
                        SendStatus(StatusChangedEnum.FileDownloading, new object[] { file.Name, file.CompressedSize, lastBytes }, lastBytes);
                    }

                    // clean up the streams
                    inStream.Close();
                    outStream.Close();
                    SendStatus(StatusChangedEnum.FileDownloadFinish, file.Name, 0);
                }
                if (compressed)
                {
                    try
                    {
                        // decompress the temp file
                        var tempDecPath = fileName + ".deco";
                        var decoder = new LZMACoder();
                        SendStatus(StatusChangedEnum.FileDecompressStart, file.Name, 0);
                        Log.Info("decompressing " + file.Name);

                        inStream = File.OpenRead(temporaryFile);
                        outStream = File.Open(tempDecPath, FileMode.Create);

                        try
                        {
                            decoder.Decompress(inStream, outStream);
                        }
                        catch (ApplicationException ex)
                        {
                            inStream.Close();
                            outStream.Close();

                            throw ex;
                        }

                        inStream.Close();
                        outStream.Close();

                        File.Delete(temporaryFile);
                        File.Move(tempDecPath, temporaryFile);
                        SendStatus(StatusChangedEnum.FileDecompressFinish, file.Name, 0);
                    }
                    catch (ApplicationException e)
                    {
                        // lzma sdk threw an error.
                        File.Delete(temporaryFile);
                        Log.Error(String.Format("LZMA file failed to decompress. {0}", e.ToString()));
                        _lastException = e;
                        return false;
                    }
                }
                // check the temp file's validity
                SendStatus(StatusChangedEnum.FileVerifyStart, file.Name, 0);
                var outHash = Utilities.GetFileSHA1(temporaryFile);
                var inHash = file.SHA1Hash;

                if (inHash != outHash)
                {
                    _lastException = new Exception("Hash verification failed for " + fileName);
                    Log.Error("Hash verification failed for " + fileName);
                    File.Delete(temporaryFile);
                    SendStatus(StatusChangedEnum.FileVerifyFailed, file.Name, 0);
                    return false;
                }
                SendStatus(StatusChangedEnum.FileVerifyFinish, file.Name, 0);
                // and rename the temp file, and then delete it
                File.Copy(temporaryFile, fileName, true);
                File.Delete(temporaryFile);
                return true;
#if !DEBUG
            }
            catch (Exception e) {
                Log.Error(String.Format("Error while downloading {0}.", e.ToString()));
                _lastException = e;
            }
            return false;
#endif
        }

        private long lastTDone = 0;
        public void SendStatus(StatusChangedEnum type)
        {
            SendStatus(type, null, 0);
        }
        public void SendStatus(StatusChangedEnum type, object data, long tasksToAdd)
        {
            tasksDone += tasksToAdd;
            lastTDone = tasksDone;

            if (tasksDone < 0) tasksDone = 0;
            if (tasksTotal == 0) tasksTotal = 1;
            if (StatusChanged != null)
            {
                StatusChanged(this, new StatusChangedEventArgs
                                        {
                    TotalTasks = tasksTotal,
                    TasksDone = tasksDone,
                    Percentage = (double)tasksDone / (double)tasksTotal * 100.00,
                    Type = type,
                    data = data
                });
            }
        }
    }
    public enum StatusChangedEnum
    {
        LocalCachesNotFound,
        Start,
        FileFinish,
        FileStart,
        CacheDownloading,
        CacheDownloaded,
        LocalCacheSaved,
        FileDownloading,
        FileDecompressStart,
        FileDecompressFinish,
        FileVerifyStart,
        FileVerifyFinish,
        FileVerifyFailed,
        FileFailed,
        FileFailedPerm,
        FileRedownload,
        FileDownloadingLzma,
        FileDownloadFinish,
        DownloadsStart,
        DownloadsFinish,
        Fail,
        Finish,
        NoUpdateNeeded
    }
    public class StatusChangedEventArgs : EventArgs
    {
        public StatusChangedEnum Type { get; set; }
        public object data { get; set; }
        public long TotalTasks { get; set; }
        public long TasksDone { get; set; }
        public double Percentage { get; set; }
    }

    [System.ComponentModel.DesignerCategory("")] //stop the fucking designer
    public class CustomWebClient : WebClient
    {
        protected override WebRequest GetWebRequest(Uri address)
        {
            var request = HttpWebRequest.Create(address);
            request.Proxy = new WebProxy();

            return request;
        }
    }
}
