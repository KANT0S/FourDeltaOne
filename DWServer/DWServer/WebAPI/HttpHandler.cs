using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

using Kayak;
using Kayak.Http;
using Razor = RazorEngine.Razor;
using System.Reflection;
using CookComputing.XmlRpc;

namespace DWServer
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
            List<ArraySegment<byte>> buffer = new List<ArraySegment<byte>>();
            Action<byte[]> resultCallback;
            Action<Exception> errorCallback;

            public BufferedConsumer(Action<byte[]> resultCallback,
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
                buffer.Add(data);
                return false;
            }
            public void OnError(Exception error)
            {
                errorCallback(error);
            }

            public void OnEnd()
            {
                foreach (var segment in buffer)
                {
                    resultCallback(segment.Array);
                }
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

                    if (path == "/servers")
                    {
                        HandleServersRequest(response);

                        return;
                    }

                    if (path == "/api")
                    {
                        var method = request.Method;
                        var stream = new MemoryStream();
                        if (method == "POST")
                        {
                            requestBody.Connect(new BufferedConsumer(bufferedBody =>
                            {
                                var data = Encoding.UTF8.GetBytes(Encoding.UTF8.GetString(bufferedBody).Split(new[] { "\r\n\r\n" }, 2, StringSplitOptions.RemoveEmptyEntries)[1]);
                                stream.Write(data, 0, data.Length);
                                stream.Position = 0;

                                HandleAPIRequest(stream, method, response);
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

        private static void HandleServersRequest(IHttpResponseDelegate response)
        {
            var status = "200 OK";

            var outputStream = new MemoryStream();
            var outputWriter = new StreamWriter(outputStream);

            try
            {
                var templateStream = Assembly.GetExecutingAssembly().GetManifestResourceStream("DWServer.Templates.Servers.cshtml");
                var templateReader = new StreamReader(templateStream);
                var template = templateReader.ReadToEnd();
                templateReader.Close();

                var sessions = DWMatch.Sessions;
                outputWriter.Write(Razor.Parse(template, sessions));
                outputWriter.Flush();
            }
            catch (RazorEngine.Templating.TemplateCompilationException e)
            {
                status = "500 Internal Server Error";
                outputWriter.Write("<pre>");
                outputWriter.Write(e.ToString());

                outputWriter.Write("\n");
                foreach (var err in e.Errors)
                {
                    outputWriter.Write(err.ToString() + "\n");
                    outputWriter.Write(err.ErrorText + "\n");
                }

                outputWriter.Write(e.SourceCode);

                outputWriter.Write("</pre>");
                outputWriter.Flush();
            }

            response.OnResponse(new HttpResponseHead()
            {
                Status = status,
                Headers = new Dictionary<string, string>()
                                    {
                                        {
                                            "Content-Length", outputStream.Length.ToString()
                                        },
                                        {
                                            "Content-Type", "text/html"
                                        }
                                    }
            }, new BufferedProducer(((MemoryStream)outputStream).GetBuffer()));
        }

        private static void HandleAPIRequest(Stream stream, string method, IHttpResponseDelegate response)
        {
            var hrequest = new HttpRequest(stream, method);
            var hresponse = new HttpResponse();

            var protocol = new DWWebAPIService();
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
            int port = 3076;
            if (Program.Game == TitleID.T5)
            {
                port = 3076;
            }
            else if (Program.Game == TitleID.IW5)
            {
                port = 3080;
            }

            using (server.Listen(new IPEndPoint(IPAddress.Any, port)))
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
