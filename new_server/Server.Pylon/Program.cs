using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Extension;
using Server.Extension.Handler;
using Server.Message.Pylon;
using System.Web;

namespace Server.Pylon
{
    class Program : IHandler
    {
        static void Main()
        {
            // 모든 handler 등록
            HandlerHelper.Register(Assembly.GetExecutingAssembly());

            // Network 시작
            var listener = MessageSessionManager.Instance.CreateListener();
            Task.Factory.StartNew(() => listener.Start(Configuration.PylonPort), TaskCreationOptions.LongRunning);

            try
            {
                // Web Service 시작
                var webServer = new DataHttpServer(Configuration.PylonWebPort, false);
                webServer.RegisterHandler(_instance);
                webServer.ApiResourceName = "/";
                // webServer.OnLog += Logger.Write;
                webServer.OnError += Logger.Write;
                webServer.Start();
            }
            catch (Exception e)
            {
                Logger.Write("Cannot start web-server.");
                Logger.Write(e);
            }

            Logger.Write("Server is ready.");
            Console.ReadLine();

            if (_instance != null)
            {
                _instance.SaveInterChatUsers();
            }
        }

        private static Program _instance;

        public Program()
        {
            // Handler 생성될 때 Instance를 등록한다.
            _instance = this;

            // InterChat User 읽기
            LoadInterChatUsers();

            // ping & pong
            SendPingPacket();
            CheckInvalidConnection();
        }

        const string UserFileName = "users.txt";
        private void LoadInterChatUsers()
        {
            if (!File.Exists(UserFileName))
                return;

            foreach (var line in File.ReadAllLines(UserFileName, Encoding.UTF8))
            {
                var user = InterChatUser.Parse(line.Trim());
                if (user != null && !_userMap.ContainsKey(user.Name))
                    _userMap.Add(user.Name, user);
            }
        }

        private void SaveInterChatUsers()
        {
            var result = _userMap.Values.Select(e => e.ToString()).ToArray();
            File.WriteAllLines(UserFileName, result, Encoding.UTF8);
        }

        private class InterChatUser
        {
            public const string Delimiter = "|";
            private const string ColorDelimiter = "#";

            public string Name { get; set; }
            public Color TextColor { private get; set; }

            public InterChatUser()
            {
                TextColor = Color.Black;
            }

            public override string ToString()
            {
                return Name + ColorDelimiter + TextColor.ToArgb().ToString(CultureInfo.InvariantCulture);
            }

            public static InterChatUser Parse(string content)
            {
                var argbIndex = content.LastIndexOf(ColorDelimiter, StringComparison.Ordinal);
                if (argbIndex < 0)
                    return null;

                int argbValue;
                var argbString = content.Substring(argbIndex + 1);
                if (string.IsNullOrWhiteSpace(argbString) || !int.TryParse(argbString, out argbValue))
                    return null;

                var userName = content.Substring(0, argbIndex);
                var textColor = Color.FromArgb(argbValue);
                return new InterChatUser {Name = userName, TextColor = textColor};
            }
        }

        #region Inter-server Context

        private readonly Dictionary<string, InterChatUser> _userMap = new Dictionary<string, InterChatUser>();
        private readonly ReaderWriterLockSlim _userMapLock = new ReaderWriterLockSlim();

        private readonly Dictionary<MessageSession, ServerMsg> _serverMap = new Dictionary<MessageSession, ServerMsg>();
        private readonly ReaderWriterLockSlim _serverMapLock = new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion);

        private readonly Dictionary<MessageSession, string> _interChatUserMap = new Dictionary<MessageSession, string>();
        private readonly ReaderWriterLockSlim _interChatLock = new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion);

        private readonly Queue<InterChatMsg> _lastMessages = new Queue<InterChatMsg>();
        private readonly ReaderWriterLockSlim _interChatMessageLock = new ReaderWriterLockSlim();

        private const int PreserveMessageCount = 240;
        private const int LastWebMessageCount = 48;
        private const int LastClientMessageCount = 48;

        #endregion

        #region Check Valid-Connection

        internal async void SendPingPacket()
        {
            while (true)
            {
                const int delayMillisecs = 1 * 60 * 1000;
                await Task.Delay(delayMillisecs);

                // 1분에 한 번씩 ping 패킷을 보낸다.
                SendPingPacket(_serverMapLock, _serverMap);
                SendPingPacket(_interChatLock, _interChatUserMap);
            }
        }

        private static void SendPingPacket<T>(ReaderWriterLockSlim locker, Dictionary<MessageSession, T> sessionMap)
        {
            foreach (var session in locker.DoRead(() => sessionMap.Keys.ToArray()))
                session.Send(new AlivePingMsg());
        }

        internal async void CheckInvalidConnection()
        {
            while (true)
            {
                const int delayMillisecs = 5 * 60 * 1000;
                await Task.Delay(delayMillisecs);

                // 일정 시간동안 패킷 교환이 없는 서버는 목록에서 제거한다.
                CheckAndDisconnect(_serverMapLock, _serverMap);
                CheckAndDisconnect(_interChatLock, _interChatUserMap);
            }
        }

        private void CheckAndDisconnect<T>(ReaderWriterLockSlim locker, Dictionary<MessageSession, T> sessionMap)
        {
            const int tolerantInterval = 3 * 60 * 1000;

            var removalSessions = locker.DoWrite(() =>
                    sessionMap.Keys.Where(e => (DateTime.Now - e.LastReceiveTime).TotalMilliseconds >= tolerantInterval)
                              .ToArray());

            foreach (var session in removalSessions)
                ResetPeer(session, new ResetPeerMessage());

            // 해당 연결을 모두 제거한다.
            foreach (var session in removalSessions)
            {
                try
                {
                    session.Dispose();
                }
                catch
                {
                }
            }
        }

        #endregion

        [MessageHandler]
        internal void RegisterServer(MessageSession session, ServerMsg msg)
        {
            _serverMapLock.DoWrite(() =>
                {
                    _serverMap.Add(session, msg);
                    Logger.Write("Connected: [{0}] {1}:{2}", msg.Name, msg.Host, msg.Port);
                });
        }

        [MessageHandler]
        internal void RequestServer(MessageSession session, RequestServerMsg msg)
        {
            _serverMapLock.DoRead(() =>
                {
                    var response = new ServersMsg();
                    response.ServerList.AddRange(_serverMap.Values);

                    session.Send(response);
                    Logger.Write("Response To: {0} -> {1}", session.ClientSocket.RemoteEndPoint, _serverMap.Count);
                });
        }

        [MessageHandler]
        internal void ResetPeer(MessageSession session, ResetPeerMessage msg)
        {
            _serverMapLock.DoWrite(() =>
                {
                    if (!_serverMap.ContainsKey(session))
                        return;

                    var server = _serverMap[session];
                    Logger.Write("Disconnected: [{0}] {1}:{2}", server.Name, server.Host, server.Port);

                    _serverMap.Remove(session);
                });

            _interChatLock.DoWrite(() =>
                {
                    if (!_interChatUserMap.ContainsKey(session))
                        return;

                    var userName = _interChatUserMap[session];
                    Logger.Write("Disconnected: InterChat [{0}]", userName);

                    _interChatUserMap.Remove(session);

                    BroadcastChatMsgWithNoLock("system", userName + "께서 접속을 종료하였습니다.");
                    BroadcastCommandMsgWithNoLock(InterChatCommandType.InformLogoutUser, userName);
                });
        }

        private InterChatUser FindOrCreateUserWithLock(string userName)
        {
            return _userMapLock.DoWrite(() =>
                {
                    if (!_userMap.ContainsKey(userName))
                        _userMap.Add(userName, new InterChatUser {Name = userName});
                    return _userMap[userName];
                });
        }

        private string SerializeUserListWithNoLock()
        {
            var result = _interChatUserMap.Values.Select(FindOrCreateUserWithLock).Select(user => user.ToString()).ToList();
            return string.Join(InterChatUser.Delimiter, result.ToArray());
        }

        [MessageHandler]
        internal void LoginInterChat(MessageSession session, InterChatLoginMsg msg)
        {
            _interChatLock.DoWrite(() =>
                {
                    // 중복 로그인 방지
                    if (_interChatUserMap.ContainsValue(msg.Name))
                    {
                        Logger.Write("Connected: Duplicate Login [{0}]", msg.Name);
                        session.Send(new InterChatCommandMsg {TypeCode = (int) InterChatCommandType.CheckUserName, Content = "false"});
                        return;
                    }

                    session.Send(new InterChatCommandMsg { TypeCode = (int)InterChatCommandType.CheckUserName, Content = "true" });
                    Logger.Write("Connected: InterChat [{0}]", msg.Name);

                    // 먼저 목록을 보내주고,
                    session.Send(new InterChatCommandMsg
                        {
                            TypeCode = (int)InterChatCommandType.InformLoggedUsers,
                            Content = SerializeUserListWithNoLock()
                        });

                    // 예전 채팅 내역이 있으면 보내준 뒤,
                    // ReSharper disable ImplicitlyCapturedClosure
                    _interChatMessageLock.DoRead(() =>
                        {
                            var skipCount = Math.Max(0, _lastMessages.Count - LastClientMessageCount);
                            foreach (var lastMessage in _lastMessages.Skip(skipCount).Take(LastClientMessageCount))
                                session.Send(lastMessage);
                        });
                    // ReSharper restore ImplicitlyCapturedClosure

                    // 맵에 자기 자신을 추가한 뒤, 자신에 대한 정보도 같이 처리한다.
                    _interChatUserMap.Add(session, msg.Name);
                    BroadcastChatMsgWithNoLock("system", msg.Name + "께서 접속하였습니다.");
                    BroadcastCommandMsgWithNoLock(InterChatCommandType.InformLoggedUser, FindOrCreateUserWithLock(msg.Name).ToString());
                });
        }

        [MessageHandler]
        internal void BroadcastInterChat(MessageSession session, InterChatMsg msg)
        {
            _interChatLock.DoRead(() =>
                {
                    var userName = "unknown";
                    if (_interChatUserMap.ContainsKey(session))
                        userName = _interChatUserMap[session];

                    BroadcastChatMsgWithNoLock(userName, msg.Message);
                });
        }

        [MessageHandler]
        internal void DispatchCommand(MessageSession session, InterChatCommandMsg msg)
        {
            var user = FindUserFromSessionWithLock(session);
            if (user == null)
                return;

            switch ((InterChatCommandType)msg.TypeCode)
            {
                case InterChatCommandType.CheckUserName:
                    // 중복된 유저 이름이 있는지 검사해서 알려준다.
                    session.Send(new InterChatCommandMsg
                        {
                            TypeCode = (int)InterChatCommandType.CheckUserName,
                            Content = user.ToString()
                        });
                    break;

                case InterChatCommandType.ChangeColor:
                    // 자신의 텍스트 색상을 변경한다.
                    user.TextColor = Color.FromArgb(int.Parse(msg.Content));
                    BroadcastCommandMsgWithLock(InterChatCommandType.ChangeColor, user.ToString());
                    break;

                case InterChatCommandType.Speech:
                    // Speech를 수행한다.
                    Task.Factory.StartNew(() => DoSpeech(user.Name, msg.Content));
                    break;
            }
        }

        private string FindUserNameFromSessionWithLock(MessageSession session)
        {
            return _interChatLock.DoRead(() => _interChatUserMap.ContainsKey(session) ? _interChatUserMap[session] : "");
        }

        private InterChatUser FindUserFromSessionWithLock(MessageSession session)
        {
            var userName = FindUserNameFromSessionWithLock(session);
            return _userMapLock.DoRead(() => _userMap.ContainsKey(userName) ? _userMap[userName] : null);
        }

        private async void DoSpeech(string userName, string message)
        {
            try
            {
                // 먼저 채팅 메시지를 전파한 후,
                BroadcastChatMsgWithLock(userName, message);

                var mp3Bytes = await VoiceReader.Read(message);
                var mp3Base64 = Convert.ToBase64String(mp3Bytes);

                // 시간 차이가 좀 크겠지만,  Mp3 메시지를 전달한다.
                BroadcastCommandMsgWithLock(InterChatCommandType.Speech, mp3Base64);
            }
            catch (Exception e)
            {
                Logger.Write(e);
            }
        }

        internal void BroadcastChatMsgWithLock(string userName, string message)
        {
            _interChatLock.DoRead(() => BroadcastChatMsgWithNoLock(userName, message));
        }

        internal void BroadcastChatMsgWithNoLock(string userName, string message)
        {
            var msg = new InterChatMsg
                {
                    Name = userName,
                    Message = message,
                    Ticks = DateTime.Now.Ticks
                };

            _interChatMessageLock.DoWrite(() =>
                {
                    _lastMessages.Enqueue(msg);
                    while (_lastMessages.Count > PreserveMessageCount)
                        _lastMessages.Dequeue();
                });

            foreach (var eachSession in _interChatUserMap.Keys)
                eachSession.Send(msg);
        }
        
        internal void BroadcastCommandMsgWithLock(InterChatCommandType commandType, string content)
        {
            _interChatLock.DoRead(() => BroadcastCommandMsgWithNoLock(commandType, content));
        }

        internal void BroadcastCommandMsgWithNoLock(InterChatCommandType commandType, string content)
        {
            var msg = new InterChatCommandMsg
            {
                TypeCode = (int)commandType,
                Content = content
            };

            foreach (var eachSession in _interChatUserMap.Keys)
                eachSession.Send(msg);
        }

        [MessageHandler]
        internal void ProcessPong(MessageSession session, AlivePongMsg msg)
        {
            // 자동으로 해당 Session의 LastReceiveTime이 갱신되니 별 다른 작업은 하지 않는다.
        }

        [DataHandle]
        public HttpResponseObject Message(long baseTick)
        {
            var messages = _interChatMessageLock.DoRead(() => _lastMessages.Reverse().Take(LastWebMessageCount)
                                                                  .Where(e => e.Ticks > baseTick)
                                                                  .Select(e => new { name = e.Name, message = e.Message, time = new DateTime(e.Ticks).ToString("HH:mm:ss"), tick = e.Ticks }));
            return HttpResponseObject.FromJson(messages);
        }

        [DataHandle]
        public HttpResponseObject SendMessage(string userName, string message)
        {
            if (string.IsNullOrWhiteSpace(userName) || string.IsNullOrWhiteSpace(message))
                return HttpResponseObject.FromJson(new { result = "error" });

            message = HttpUtility.UrlDecode(message);
            BroadcastChatMsgWithLock(userName, message);
            return HttpResponseObject.FromJson(new { result = "ok" });
        }

        [DataHandle]
        public HttpResponseObject UserList()
        {
            var users = _interChatLock.DoRead(() => _interChatUserMap.Values);
            return HttpResponseObject.FromJson(users);
        }
    }
}
