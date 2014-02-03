using System;
using System.Net;
using System.Net.Sockets;
using Server.Core.Util;

namespace Server.Core.Messaging
{
    public class MessageListener
    {
        internal MessageListener(MessageSessionManager sessionManager)
        {
            SessionManager = sessionManager;
        }

        public MessageSessionManager SessionManager { get; private set; }

        public async void Start(int port)
        {
            var listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            var localEndPoint = new IPEndPoint(IPAddress.Any, port);

            try
            {
                listener.Bind(localEndPoint);
                listener.Listen(100);

                while (true)
                {
                    var clientSocket = await listener.AcceptAsync();
                    var session = SessionManager.CreateSession(clientSocket);
                    session.StartReceive();
                }
            }
            catch (Exception e)
            {
                Logger.Write(e);
            }
        }
    }
}