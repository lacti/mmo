using System;
using System.Collections;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Threading.Tasks;
using Server.Core.Util;

namespace Server.Core.Messaging
{
    public class MessageSession : IDisposable
    {
        internal MessageSession(MessageSessionManager sessionManager)
            : this(sessionManager, new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp))
        {
        }

        internal MessageSession(MessageSessionManager sessionManager, Socket clientSocket)
        {
            SessionManager = sessionManager;
            ClientSocket = clientSocket;
            SessionId = -1;

            _sendQueue = new SinglyAccessQueue<byte[]>(this, ProcessSend);
        }

        #region SessionId

        public const int InvalidSessionId = -1;
        private int _sessionId = InvalidSessionId;

        public int SessionId
        {
            get { return _sessionId; }
            set
            {
                if (_sessionId != InvalidSessionId)
                    SessionManager.Remove(_sessionId);

                _sessionId = value;

                if (_sessionId != InvalidSessionId)
                    SessionManager.Add(_sessionId, this);
            }
        }

        #endregion

        #region Send

        private readonly SinglyAccessQueue<byte[]> _sendQueue;
        private bool _disconnectProcessed;

        public DateTime LastSendTime { get; private set; }

        public void Send(IMessage message)
        {
            byte[] messageBytes;
            using (var memoryStream = new MemoryStream())
            {
                using (var memoryWriter = new BinaryWriter(memoryStream))
                {
                    memoryWriter.Write(0); // for writing message-length
                    message.WriteTo(memoryWriter);
                    messageBytes = memoryStream.ToArray();
                }
            }
            var lengthBytes = BitConverter.GetBytes(messageBytes.Length);
            Array.Copy(lengthBytes, messageBytes, lengthBytes.Length);

            Send(messageBytes);
        }

        public void Send(byte[] messageBytes)
        {
            _sendQueue.EnqueueOrProcess(messageBytes);
        }

        private void ProcessSend(byte[] bytes)
        {
            try
            {
                ClientSocket.SendAsync(bytes).Wait();
                LastSendTime = DateTime.Now;
            }
            catch (Exception e)
            {
                if (!SocketHelper.IsDisconnected(e))
                    Logger.Write(e);

                var errorCode = e is SocketException ? (e as SocketException).ErrorCode : -1;
                ProcessDisconnect(errorCode);
            }
        }

        private void ProcessDisconnect(int errorCode)
        {
            if (_disconnectProcessed)
                return;

            if (_sessionId != InvalidSessionId)
                SessionManager.Remove(_sessionId);

            Dispatch(new ResetPeerMessage { ErrorCode = errorCode, Id = _sessionId });

            _disconnectProcessed = true;
        }

        #endregion

        #region Receive

        public DateTime LastReceiveTime { get; private set; }

        public void StartReceive()
        {
            Task.Factory.StartNew(ReceiveLoop, TaskCreationOptions.LongRunning);
        }

        private async void ReceiveLoop()
        {
            int errorCode;
            try
            {
                while (true)
                {
                    var sizeBytes = await ClientSocket.ReceiveAsync(sizeof (int));
                    var messageSize = BitConverter.ToInt32(sizeBytes, 0) - sizeof (int);
                    var messageBytes = await ClientSocket.ReceiveAsync(messageSize);

                    ProcessMessage(messageBytes);
                }
            }
            catch (SocketException e)
            {
                errorCode = e.ErrorCode;
                if (!SocketHelper.IsDisconnected(e))
                    Logger.Write(e);
            }
            catch (Exception e)
            {
                errorCode = -1;
                Logger.Write(e);
            }

            ProcessDisconnect(errorCode);
        }

        private void ProcessMessage(byte[] messageBytes)
        {
            try
            {
                var message = MessageHelper.ReadMessage(messageBytes);
                if (message == null)
                    return;

                if (!Dispatch(message))
                    Logger.Write("no handler: " + message.GetType());

                LastReceiveTime = DateTime.Now;
            }
            catch (Exception e)
            {
                Logger.Write(e);
            }
        }

        private bool Dispatch(IMessage message)
        {
            var dispatched = false;
            try
            {
                if (MessageQueue.Global.SendTo(this, message))
                    dispatched = true;
            }
            catch (Exception e)
            {
                Logger.Write(e);
            }

            var messageType = message.GetType();
            foreach (var eachProperty in messageType.GetProperties())
            {
                var propertyType = eachProperty.PropertyType;
                var propertyValue = eachProperty.GetValue(message, null);

                if (typeof (IMessage).IsAssignableFrom(propertyType))
                {
                    var childMessage = propertyValue as IMessage;
                    Debug.Assert(childMessage != null, "childMessage != null");

                    if (Dispatch(childMessage))
                        dispatched = true;
                }
                else if (typeof (IList).IsAssignableFrom(propertyType))
                {
                    var list = propertyValue as IList;
                    Debug.Assert(list != null, "list != null");

                    foreach (var childMessage in list.OfType<IMessage>())
                    {
                        Debug.Assert(childMessage != null, "childMessage != null");

                        if (Dispatch(childMessage))
                            dispatched = true;
                    }
                }
            }
            return dispatched;
        }

        #endregion

        #region Connectivity

        public void Dispose()
        {
            Shutdown(SocketShutdown.Both);
            ClientSocket.Close();
        }

        public void Connect(string host, int port)
        {
            ClientSocket.Connect(host, port);
        }

        public void Shutdown(SocketShutdown how)
        {
            ClientSocket.Shutdown(how);
        }

        #endregion

        public MessageSessionManager SessionManager { get; private set; }
        public Socket ClientSocket { get; private set; }
    }
}