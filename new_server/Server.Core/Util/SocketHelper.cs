using System;
using System.Net;
using System.Net.Http;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public static class SocketHelper
    {
        public static bool IsDisconnected(Exception exception)
        {
            if (!(exception is SocketException))
                return false;

            return IsDisconnected(exception as SocketException);
        }

        public static bool IsDisconnected(SocketException exception)
        {
            switch ((SocketError) exception.ErrorCode)
            {
                case SocketError.ConnectionAborted:
                case SocketError.ConnectionReset:
                case SocketError.Fault:
                case SocketError.NetworkReset:
                case SocketError.OperationAborted:
                case SocketError.Shutdown:
                case SocketError.SocketError:
                case SocketError.TimedOut:
                    return true;
            }
            return false;
        }

        public static string GetHostIpAddress()
        {
            var task = RetrieveHostIpAddress();
            task.Wait();

            return task.Result;
        }

        private static async Task<string> RetrieveHostIpAddress()
        {
            var cookies = new CookieContainer();
            var handler = new HttpClientHandler
            {
                CookieContainer = cookies,
                UseCookies = true,
                UseDefaultCredentials = false,
            };
            var client = new HttpClient(handler);
            var response = await client.GetAsync("http://checkip.dyndns.org/");
            try
            {
                var content = Encoding.Default.GetString(await response.Content.ReadAsByteArrayAsync());
                return content.Extract("IP Address: ", "</body>");
            }
            catch (Exception e)
            {
                Logger.Write(e);
            }
            return "localhost";
        }
    }
}