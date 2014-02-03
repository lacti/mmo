using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Web.Script.Serialization;

namespace Server.Core.Util
{
    public class HttpResponseObject
    {
        public static readonly HttpResponseObject Empty = new HttpResponseObject { ContentType = "text/plain", ReturnBytes = Encoding.UTF8.GetBytes("") };

        public string ContentType { get; set; }
        public string ContentDisposition { get; set; }
        public byte[] ReturnBytes { get; set; }

        internal readonly List<Cookie> CookiesToAdd = new List<Cookie>();
        internal readonly List<string> CookiesToRemove = new List<string>();

        internal DataHttpSession Session { get; set; }
        internal bool NeedToDeleteSession { get; set; }
        internal string RedirectUrl { get; set; }

        public HttpResponseObject AppendCookie(string key, string value)
        {
            return AppendCookie(new Cookie(key, value));
        }

        public HttpResponseObject AppendCookie(Cookie cookie)
        {
            CookiesToAdd.Add(cookie);
            return this;
        }

        public HttpResponseObject RemoveCookie(string key)
        {
            CookiesToRemove.Add(key);
            return this;
        }

        public HttpResponseObject SetSession(DataHttpSession session)
        {
            Session = session;
            return this;
        }

        public HttpResponseObject CreateSession(params object[] parameters)
        {
            var newSession = new DataHttpSession();
            for (var index = 0; index < parameters.Length; index += 2)
            {
                newSession[parameters[index].ToString()] = parameters[index + 1];
            }

            return SetSession(newSession);
        }

        public HttpResponseObject DeleteSession(DataHttpSession session)
        {
            NeedToDeleteSession = true;
            return SetSession(session);
        }

        public static HttpResponseObject FromBytes(string contentType, byte[] bytes)
        {
            return FromBytes(contentType, bytes, "");
        }

        public static HttpResponseObject FromBytes(string contentType, byte[] bytes, string contentDisposition)
        {
            return new HttpResponseObject { ContentType = contentType, ReturnBytes = bytes, ContentDisposition = contentDisposition };
        }

        public static HttpResponseObject FromString(string contentType, string value)
        {
            return FromBytes(contentType, Encoding.Default.GetBytes(value));
        }

        public static HttpResponseObject FromText(string value)
        {
            return FromString("text/plain", value);
        }

        public static HttpResponseObject FromHtml(string value)
        {
            return FromString("text/html", value);
        }

        public static HttpResponseObject FromJson(object jsonObject)
        {
            var jsonString = ToJson(jsonObject);
            return FromString("application/json", jsonString);
        }

        public static HttpResponseObject Redirect(string url)
        {
            return new HttpResponseObject {RedirectUrl = url};
        }

        private static string ToJson(object value)
        {
            return (new JavaScriptSerializer()).Serialize(value);
        }
    }
}
