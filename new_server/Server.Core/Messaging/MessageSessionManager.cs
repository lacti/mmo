using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Threading;
using Server.Core.Util;

namespace Server.Core.Messaging
{
    public class MessageSessionManager : IDisposable
    {
        public static readonly MessageSessionManager Instance = new MessageSessionManager();
        private readonly Dictionary<MessageSession, int> _idMap = new Dictionary<MessageSession, int>();

        private readonly ReaderWriterLockSlim _lock = new ReaderWriterLockSlim();
        private readonly Dictionary<int, MessageSession> _sessionMap = new Dictionary<int, MessageSession>();

        private MessageSessionManager()
        {
        }

        #region Object Managing

        public MessageSession CreateSession()
        {
            return new MessageSession(this);
        }

        public MessageSession CreateSession(Socket socket)
        {
            return new MessageSession(this, socket);
        }

        public MessageListener CreateListener()
        {
            return new MessageListener(this);
        }

        internal bool Add(int sessionId, MessageSession session)
        {
            _lock.DoWrite(() =>
                {
                    _sessionMap.Add(sessionId, session);
                    _idMap.Add(session, sessionId);
                });
            return true;
        }

        internal bool Remove(int sessionId)
        {
            return _lock.DoWrite(() =>
                {
                    if (!_sessionMap.ContainsKey(sessionId))
                        return false;

                    var removal = _sessionMap[sessionId];
                    _sessionMap.Remove(sessionId);
                    _idMap.Remove(removal);
                    return true;
                });
        }

        #endregion

        public MessageSession this[int sessionId]
        {
            get { return _lock.DoRead(() => _sessionMap[sessionId]); }
        }

        public IEnumerable<MessageSession> this[IEnumerable<int> sessionIds]
        {
            get { return _lock.DoRead(() => from id in sessionIds where _sessionMap.ContainsKey(id) select _sessionMap[id]); }
        }

        public int this[MessageSession session]
        {
            get { return _lock.DoRead(() => _idMap.ContainsKey(session) ? _idMap[session] : -1); }
        }

        public void Dispose()
        {
            foreach (var session in _sessionMap.Values)
                session.Dispose();

            _sessionMap.Clear();
            _idMap.Clear();
        }

        public MessageSession At(int sessionId)
        {
            return _lock.DoRead(() => _sessionMap.ContainsKey(sessionId) ? _sessionMap[sessionId] : null);
        }

        public void Broadcast(IMessage message, IEnumerable<int> sessionIds)
        {
            foreach (var session in this[sessionIds])
                session.Send(message);
        }

        public void Broadcast(IMessage message)
        {
            _lock.DoRead(() =>
                {
                    foreach (var session in _sessionMap.Values)
                        session.Send(message);
                });
        }
    }
}