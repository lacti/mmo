using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Threading;
using System.IO;
using System.Text.RegularExpressions;

namespace Server.Core.Util
{
    public class DataHttpServer
    {
        private const string DefaultIndexFile = "index.html";

        private readonly HttpListener _listener = new HttpListener();
        public bool IsRunning { get; set; }

        private class DelegationInfo
        {
            public object Instance { get; set; }
            public MethodInfo Method { get; set; }
            public bool SynchronizeInstance { get; set; }
            public bool RedirectIfSessionIsNull { get; set; }
        }

        private readonly Dictionary<string, DelegationInfo> _delegationMap = new Dictionary<string, DelegationInfo>();
        public string ApiResourceName { get; set; }
        public string BaseDirectoryPath { get; set; }
        public string RedirectUrlIfSessionIsNull { get; set; }

        public event Action<string> OnLog;
        public event Action<Exception> OnError;
        private readonly object _logLock = new object();

        private readonly Dictionary<string, DataHttpSession> _sessionMap = new Dictionary<string, DataHttpSession>();
        private readonly object _sessionLock = new object();
        private const string DataSessionKeyCookieName = "__DataSessionKey__";

        public int MaxConcurrentRequestCount { get; set; }

        public DataHttpServer(int port, bool localhostOnly)
        {
            if (!localhostOnly)
            {
                _listener.Prefixes.Add(string.Format("http://*:{0}/", port));
            }
            else
            {
                _listener.Prefixes.Add(string.Format("http://localhost:{0}/", port));
            }

            ApiResourceName = "/api";
            BaseDirectoryPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

            MaxConcurrentRequestCount = int.MaxValue;
            RedirectUrlIfSessionIsNull = "/";
        }

        public void RegisterHandler(object handler, bool synchronizeInstance = false)
        {
            if (handler == null)
                return;

            RegisterHandlerInternal(handler, handler.GetType(), synchronizeInstance);
        }

        public void RegisterHandler(Type handlerType)
        {
            RegisterHandlerInternal(null, handlerType, false);
        }

        private void RegisterHandlerInternal(object handlerInstance, Type handlerType, bool synchronizeInstance)
        {
            if (handlerType == null)
                return;

            var flags = BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic;
            if (handlerInstance != null)
                flags |= BindingFlags.Instance;

            foreach (var method in handlerType.GetMethods(flags))
            {
                // DataHandler Attribute가 있는 Method만 등록한다.
                var handleAttribute = method.GetCustomAttributes(false).OfType<DataHandleAttribute>().FirstOrDefault();
                if (handleAttribute == null)
                    continue;

                _delegationMap.Add(method.Name.ToLower(), new DelegationInfo
                {
                    Instance = handlerInstance,
                    Method = method,
                    SynchronizeInstance = synchronizeInstance,
                    RedirectIfSessionIsNull = handleAttribute.RedirectIfSessionIsNull
                });
            }
        }

        private void FireLogMessage(string format, params object[] args)
        {
            if (OnLog != null)
            {
                lock (_logLock)
                {
                    if (args == null || args.Length == 0)
                        OnLog(format);
                    else OnLog(string.Format(format, args));
                }
            }
        }

        private void FireExceptionMessage(Exception e)
        {
            if (OnError != null)
            {
                lock (_logLock)
                {
                    OnError(e);
                }
            }
        }

        public void Start()
        {
            new Thread(Run).Start();
        }

        public void Run()
        {
            IsRunning = true;
            _listener.Start();

            var concurrentRequestCount = 0;
            while (IsRunning)
            {
                var context = _listener.GetContext();
                FireLogMessage(context.Request.RawUrl);

                // 최대 연결 개수를 초과하면 연결을 끊어버린다.
                if (concurrentRequestCount >= MaxConcurrentRequestCount)
                {
                    context.Response.OutputStream.Close();
                    continue;
                }

                // 요청이 많아지면 시스템이 망가진다.
                new Thread(() =>
                    {
                        Interlocked.Increment(ref concurrentRequestCount);

                        try
                        {
                            HandleContext(context);
                        }
                        catch (Exception e)
                        {
                            FireExceptionMessage(e);
                        }
                        context.Response.OutputStream.Close();
                        Interlocked.Decrement(ref concurrentRequestCount);
                    }).Start();
            }
            _listener.Close();
        }

        private static readonly Regex NameRegex = new Regex("name=\"([0-9_A-Za-z]+)\"", RegexOptions.Compiled);

        private Dictionary<string, string> ReadPostData(HttpListenerContext context)
        {
            var result = new Dictionary<string, string>();

            const string boundaryPrefix = "boundary=";
            var contentType = context.Request.ContentType;
            var boundary = contentType.Contains(boundaryPrefix)? contentType.Substring(contentType.IndexOf(boundaryPrefix) + boundaryPrefix.Length): "";

            using (var reader = new StreamReader(context.Request.InputStream))
            {
                if (string.IsNullOrWhiteSpace(boundary))
                {
                    result.Add("", reader.ReadToEnd());
                }
                else
                {
                    var name = "";
                    var buffer = new StringBuilder();
                    boundary = "--" + boundary;
                    while (!reader.EndOfStream)
                    {
                        var line = reader.ReadLine();
                        if (!line.StartsWith(boundary))
                        {
                            if (buffer.Length > 0)
                                buffer.Append(Environment.NewLine);
                            buffer.Append(line);
                            continue;
                        }

                        if (line.EndsWith("--"))
                        {
                            result.Add(name, buffer.ToString());
                            break;
                        }

                        if (!string.IsNullOrWhiteSpace(name))
                        {
                            result.Add(name, buffer.ToString());
                        }
                        buffer.Clear();

                        line = reader.ReadLine();
                        name = NameRegex.Match(line).Groups[1].Value;
                        reader.ReadLine();
                    }
                }
            }
            return result;
        }

        private void HandleContext(HttpListenerContext context)
        {
            var url = context.Request.RawUrl;
            if (url.StartsWith(ApiResourceName, StringComparison.OrdinalIgnoreCase))
            {
                var methodName = url.Contains('?')
                    ? url.Substring(url.LastIndexOf('/') + 1, url.IndexOf('?') - url.LastIndexOf('/') - 1).ToLower()
                    : url.Substring(url.LastIndexOf('/') + 1).ToLower();

                // 등록된 파일이 없다면 Handler가 있는지 찾아보고 실행한 결과를 반환해준다.
                var httpObject = TryExecuteHandler(context, methodName);
                FireLogMessage(httpObject.ContentType);

                // Cookie 추가 및 삭제
                foreach (var newCookie in httpObject.CookiesToAdd)
                    context.Response.AppendCookie(newCookie);

                foreach (
                    var removalCookie in
                        context.Response.Cookies.OfType<Cookie>().Where(e => httpObject.CookiesToRemove.Contains(e.Name)))
                    removalCookie.Expires = DateTime.Now.Subtract(TimeSpan.FromDays(1));

                if (httpObject.Session != null)
                {
                    // Session이 추가되었다면 Session을 설정해준다.
                    if (!httpObject.NeedToDeleteSession)
                    {
                        UpdateSessionMap(httpObject.Session);
                        context.Response.AppendCookie(new Cookie(DataSessionKeyCookieName, httpObject.Session.SessionKey));
                    }
                    else
                    {
                        // 필요에 따라 Session을 파괴한다.
                        RemoveSessionMap(httpObject.Session);
                        var sessionCookie =
                            context.Response.Cookies.OfType<Cookie>()
                                   .FirstOrDefault(e => e.Name == DataSessionKeyCookieName);
                        if (sessionCookie != null)
                            sessionCookie.Expires = DateTime.Now.Subtract(TimeSpan.FromDays(1));
                    }
                }

                if (!string.IsNullOrWhiteSpace(httpObject.RedirectUrl))
                {
                    context.Response.Redirect(httpObject.RedirectUrl);
                }
                else
                {
                    context.Response.AddHeader("Content-Type", httpObject.ContentType);
                    if (!string.IsNullOrWhiteSpace(httpObject.ContentDisposition))
                        context.Response.AddHeader("Content-Disposition", httpObject.ContentDisposition);

                    context.Response.OutputStream.Write(httpObject.ReturnBytes, 0, httpObject.ReturnBytes.Length);
                }
            }
            else
            {
                // QueryString을 제거하고 맨 앞의 / 문자를 제거한다.
                var resourceUrl = url.Contains('?') ? url.Substring(0, url.IndexOf('?')) : url;
                resourceUrl = resourceUrl.Substring(1);

                // 주소를 입력하지 않았다면 기본 파일을 보여준다.
                if (resourceUrl.Length == 0)
                    resourceUrl = DefaultIndexFile;

                var resourcePath = Path.Combine(BaseDirectoryPath, resourceUrl.Replace('/', Path.DirectorySeparatorChar));
                if (!File.Exists(resourcePath))
                    return;

                var outputStream = context.Response.OutputStream;
                using (var reader = new FileStream(resourcePath, FileMode.Open, FileAccess.Read, FileShare.Read))
                {
                    var buffer = new byte[4096];
                    int read;
                    while ((read = reader.Read(buffer, 0, buffer.Length)) > 0)
                    {
                        outputStream.Write(buffer, 0, read);
                    }
                }
            }
        }

        private HttpResponseObject TryExecuteHandler(HttpListenerContext context, string methodName)
        {
            try
            {
                var requestData = new Dictionary<string, string>();
                if (context.Request.HttpMethod == "POST")
                {
                    requestData = ReadPostData(context);
                }
                else if (context.Request.HttpMethod == "GET")
                {
                    var url = context.Request.Url.ToString();
                    if (url.Contains('?'))
                    {
                        var query = url.Substring(url.LastIndexOf('?') + 1);
                        foreach (var part in query.Split('&'))
                        {
                            if (string.IsNullOrWhiteSpace(part))
                                continue;

                            var keyValue = part.Split('=');
                            var key = keyValue[0];
                            var value = keyValue.Length > 1? keyValue[1]: "";
                            requestData.Add(key, value);
                        }
                    }
                }
                else throw new InvalidOperationException("Invalid Request: " + context.Request.HttpMethod);

                return ExecuteHandler(context.Request, methodName, requestData);
            }
            catch (Exception ex)
            {
                FireExceptionMessage(ex);
                return HttpResponseObject.FromJson(new { Error = ex.Message });
            }
        }

        private HttpResponseObject ExecuteHandler(HttpListenerRequest request, string methodName, Dictionary<string, string> requestData)
        {
            if (!_delegationMap.ContainsKey(methodName))
                return HttpResponseObject.Empty;

            // 검색의 편의를 위해 key를 모두 소문자로 변경한다.
            var data = requestData.ToDictionary(pair => pair.Key.ToLower(), pair => pair.Value);

            var delegator = _delegationMap[methodName];
            var parameters = new List<object>();
            foreach (var param in delegator.Method.GetParameters())
            {
                var paramName = param.Name.ToLower();
                if (param.ParameterType == typeof(HttpListenerRequest))
                    parameters.Add(request);
                else if (param.ParameterType == typeof(CookieCollection))
                    parameters.Add(request.Cookies);
                else if (param.ParameterType == typeof (DataHttpSession))
                {
                    var session = FindDataSession(request.Cookies[DataSessionKeyCookieName].Value);
                    if (session == null && delegator.RedirectIfSessionIsNull)
                        return HttpResponseObject.Redirect(RedirectUrlIfSessionIsNull);
                    parameters.Add(session);
                }
                else if (!data.ContainsKey(paramName))
                    parameters.Add(param.ParameterType == typeof(string) ? "" : Activator.CreateInstance(param.ParameterType));
                else
                {
                    // 잘못된 인자가 전달될 경우 예외가 발생할 수 있다.
                    parameters.Add(Convert.ChangeType(data[paramName], param.ParameterType));
                }
            }

            object returnObject;
            if (delegator.SynchronizeInstance && delegator.Instance != null)
            {
                // Lock 옵션을 사용한 경우에는 등록된 객체 전역에 lock을 걸고 method를 호출한다.
                lock (delegator.Instance)
                {
                    returnObject = delegator.Method.Invoke(delegator.Instance, parameters.ToArray());
                }
            }
            else
            {
                // 그 외의 경우에는 그냥 호출한다.
                returnObject = delegator.Method.Invoke(delegator.Instance, parameters.ToArray());
            }
            
            if (returnObject == null)
                return HttpResponseObject.Empty;

            var returnObjectType = returnObject.GetType();
            if (returnObjectType == typeof(string))
                return HttpResponseObject.FromText(Convert.ToString(returnObject));

            if (returnObjectType == typeof (DataHttpSession))
                return HttpResponseObject.Empty.SetSession((DataHttpSession) returnObject);

            if (returnObjectType == typeof(HttpResponseObject))
                return (HttpResponseObject)returnObject;

            return HttpResponseObject.FromJson(returnObject);
        }

        public void Stop()
        {
            IsRunning = false;
        }

        private static string HostName
        {
            get { return Environment.MachineName; }
        }

        private static string LocalIpAddress
        {
            get
            {
                foreach (var ip in Dns.GetHostEntry(Dns.GetHostName()).AddressList)
                {
                    if (ip.AddressFamily == AddressFamily.InterNetwork)
                        return ip.ToString();
                }
                return "127.0.0.1";
            }
        }

        private void UpdateSessionMap(DataHttpSession session)
        {
            if (session == null)
                return;

            lock (_sessionLock)
            {
                if (_sessionMap.ContainsKey(session.SessionKey))
                    _sessionMap.Remove(session.SessionKey);
                _sessionMap.Add(session.SessionKey, session);
            }
        }

        private void RemoveSessionMap(DataHttpSession session)
        {
            if (session == null)
                return;

            lock (_sessionLock)
            {
                if (_sessionMap.ContainsKey(session.SessionKey))
                    _sessionMap.Remove(session.SessionKey);
            }
        }

        private DataHttpSession FindDataSession(string dataSessionKey)
        {
            if (string.IsNullOrWhiteSpace(dataSessionKey))
                return null;

            lock (_sessionLock)
            {
                return _sessionMap.ContainsKey(dataSessionKey) ? _sessionMap[dataSessionKey] : null;
            }
        }
    }
}
