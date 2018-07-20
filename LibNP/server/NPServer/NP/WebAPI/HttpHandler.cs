using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

using CookComputing.XmlRpc;
using Kayak;
using Kayak.Http;

namespace NPx
{
    public static class HttpHandler
    {
        private static Thread _thread;

        public static void Start()
        {
            _thread = new Thread(Run);
            _thread.Start();
        }

        class SchedulerDelegate : ISchedulerDelegate
        {
            public void OnException(IScheduler scheduler, Exception e)
            {
                Console.WriteLine("Error on scheduler.");
                e.DebugStackTrace();
            }

            public void OnStop(IScheduler scheduler)
            {

            }
        }

        class BufferedProducer : IDataProducer
        {
            ArraySegment<byte> data;

            public BufferedProducer(string data) : this(data, Encoding.UTF8) { }
            public BufferedProducer(string data, Encoding encoding) : this(encoding.GetBytes(data)) { }
            public BufferedProducer(byte[] data) : this(new ArraySegment<byte>(data)) { }
            public BufferedProducer(ArraySegment<byte> data)
            {
                this.data = data;
            }

            public IDisposable Connect(IDataConsumer channel)
            {
                // null continuation, consumer must swallow the data immediately.
                channel.OnData(data, null);
                channel.OnEnd();
                return null;
            }
        }


        class BufferedConsumer : IDataConsumer
        {
            MemoryStream buffer = new MemoryStream();
            Action<Stream> resultCallback;
            Action<Exception> errorCallback;

            public BufferedConsumer(Action<Stream> resultCallback,
        Action<Exception> errorCallback)
            {
                this.resultCallback = resultCallback;
                this.errorCallback = errorCallback;
            }
            public bool OnData(ArraySegment<byte> data, Action continuation)
            {
                // since we're just buffering, ignore the continuation. 
                // TODO: place an upper limit on the size of the buffer. 
                // don't want a client to take up all the RAM on our server! 
                buffer.Write(data.Array, data.Offset, data.Count);
                return false;
            }
            public void OnError(Exception error)
            {
                errorCallback(error);
            }

            public void OnEnd()
            {
                buffer.Position = 0;
                resultCallback(buffer);
            }
        } 

        class RequestDelegate : IHttpRequestDelegate
        {
            public void OnRequest(HttpRequestHead request, IDataProducer requestBody,
                IHttpResponseDelegate response)
            {
                var verror = "404 Not Found";
                var errorString = "Not Found";

                try
                {
                    var path = request.Uri;

                    if (path == "/api")
                    {
                        var method = request.Method;
                        var stream = new MemoryStream();
                        if (method == "POST")
                        {
                            requestBody.Connect(new BufferedConsumer(bufStream =>
                            {
                                HandleAPIRequest(bufStream, method, response);
                            }, error =>
                            {
                                Log.Error(error.ToString());
                            }));
                        }
                        else
                        {
                            HandleAPIRequest(stream, method, response);
                        }

                        return;
                    }
                    else if (path == "/status")
                    {
                        HandleStatusRequest(response);

                        return;
                    }
                    else if (path.StartsWith("/touch/"))
                    {
                        HandleTouchRequest(path, response);

                        return;
                    }
                    else if (path.StartsWith("/flag/"))
                    {
                        HandleFlagRequest(path, response);

                        return;
                    }
                }
                catch (Exception ex)
                {
                    verror = "500 Internal Server Error";

                    Log.Error(ex.ToString());
                    errorString = ex.ToString();
                }

                response.OnResponse(new HttpResponseHead()
                {
                    Status = verror,
                    Headers = new Dictionary<string, string>()
                    {
                        { "Content-Type",  "text/plain" },
                        { "Content-Length", errorString.Length.ToString() }
                    }
                }, new BufferedProducer(errorString));

            }
        }

        private static void HandleFlagRequest(string path, IHttpResponseDelegate response)
        {
            var comp = path.Split('/').Last();

            var result = DebugFlags.Toggle(comp);

            var responseText = string.Format("{0} is now {1}.", comp, result);

            var responseBytes = Encoding.ASCII.GetBytes(responseText);

            response.OnResponse(new HttpResponseHead()
            {
                Status = "200 OK",
                Headers = new Dictionary<string, string>()
                                    {
                                        {
                                            "Content-Length", responseBytes.Length.ToString()
                                        },
                                        {
                                            "Content-Type", "text/plain"
                                        }
                                    }
            }, new BufferedProducer(responseBytes));
        }

        private static void HandleTouchRequest(string path, IHttpResponseDelegate response)
        {
            var comp = path.Split('/').Last();
            var uid = uint.Parse(comp);
            var npid = 0x110000100000000 | uid;

            var fileID = npid.ToString("x16") + "_mpdata";

            var document = new NPFile();
            document.Id = fileID;
            document.SetData(new byte[12288]);

            CDatabase.Database.CreateDocument(document, new MindTouch.Tasking.Result<NPFile>()).Wait();

            var responseText = "Touched.";

            var responseBytes = Encoding.ASCII.GetBytes(responseText);

            response.OnResponse(new HttpResponseHead()
            {
                Status = "200 OK",
                Headers = new Dictionary<string, string>()
                                    {
                                        {
                                            "Content-Length", responseBytes.Length.ToString()
                                        },
                                        {
                                            "Content-Type", "text/plain"
                                        }
                                    }
            }, new BufferedProducer(responseBytes));
        }

        private static void HandleStatusRequest(IHttpResponseDelegate response)
        {
            var responseText = "";

            responseText += "Clients: " + NPSocket.NumClients + "\r\n";
            responseText += "Packet queue size: " + NPHandler.PacketQueueSize + "\r\n";

            var responseBytes = Encoding.ASCII.GetBytes(responseText);

            response.OnResponse(new HttpResponseHead()
            {
                Status = "200 OK",
                Headers = new Dictionary<string, string>()
                                    {
                                        {
                                            "Content-Length", responseBytes.Length.ToString()
                                        },
                                        {
                                            "Content-Type", "text/plain"
                                        }
                                    }
            }, new BufferedProducer(responseBytes));
        }

        private static void HandleAPIRequest(Stream stream, string method, IHttpResponseDelegate response)
        {
            var hrequest = new HttpRequest(stream, method);
            var hresponse = new HttpResponse();

            var protocol = new NPWebAPIService();
            protocol.HandleHttpRequest(hrequest, hresponse);

            response.OnResponse(new HttpResponseHead()
            {
                Status = (hresponse.StatusCode == 200 || hresponse.StatusCode == 0) ? "200 OK" : "405 Method Not Allowed",
                Headers = new Dictionary<string, string>()
                                    {
                                        {
                                            "Content-Length", hresponse.ContentLength.ToString()
                                        },
                                        {
                                            "Content-Type", hresponse.ContentType
                                        }
                                    }
            }, new BufferedProducer(((MemoryStream)hresponse.OutputStream).GetBuffer()));
        }

        private static void Run()
        {
            var scheduler = KayakScheduler.Factory.Create(new SchedulerDelegate());
            var server = KayakServer.Factory.CreateHttp(new RequestDelegate(), scheduler);

            using (server.Listen(new IPEndPoint(IPAddress.Any, 13050)))
            {
                // runs scheduler on calling thread. this method will block until
                // someone calls Stop() on the scheduler.
                scheduler.Start();
            }
        }
    }
    
    class HttpResponse : IHttpResponse
    {
        public HttpResponse()
        {
            OutputStream = new MemoryStream();
            Output = new StreamWriter(OutputStream);
        }

        public Int64 ContentLength { get; set; }
        public string ContentType { get; set; }
        public TextWriter Output { get; private set; }
        public Stream OutputStream { get; private set; }
        public bool SendChunked
        {
            get
            {
                return false;
            }
            set
            {
                throw new NotImplementedException();
            }
        }
        public int StatusCode { get; set; }
        public string StatusDescription { get; set; }
    }

    class HttpRequest : IHttpRequest
    {
        public HttpRequest(Stream data, string method)
        {
            InputStream = data;
            HttpMethod = method;
        }

        public Stream InputStream { get; private set; }
        public string HttpMethod { get; private set; }
    }
}
