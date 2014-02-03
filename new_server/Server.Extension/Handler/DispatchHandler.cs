using System;
using System.Diagnostics;
using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Messaging;
using Server.Core.Util;

namespace Server.Extension.Handler
{
    public class DispatchHandler
    {
        #region Delegate

        public delegate void EntityHandler(Entity entity, IMessage message);

        public delegate void EntitySessionHandler(Entity entity, MessageSession session, IMessage message);

        public delegate void Handler(IMessage message);

        public delegate void SessionHandler(MessageSession session, IMessage message);

        #endregion

        public static readonly DispatchHandler Instance = new DispatchHandler();

        #region DispatchHandler

        private readonly TypedHandlerMap<EntityHandler> _entityHandlerMap = new TypedHandlerMap<EntityHandler>();

        private readonly TypedHandlerMap<EntitySessionHandler> _entitySessionHandlerMap = new TypedHandlerMap<EntitySessionHandler>();

        private readonly TypedHandlerMap<Handler> _handlerMap = new TypedHandlerMap<Handler>();

        private readonly TypedHandlerMap<SessionHandler> _sessionHandlerMap = new TypedHandlerMap<SessionHandler>();

        #endregion

        private DispatchHandler()
        {
            MessageQueue.Global.Send += Execute;
            EntityMessageQueue.Global.Send += Execute;
        }

        public DispatchHandler AddHandler<T>(Action<T> handler) where T : class, IMessage
        {
            return AddHandler(typeof (T), message => handler(message as T));
        }

        public DispatchHandler AddHandler(Type messageType, Handler handler)
        {
            _handlerMap.AddHandler(MessageHelper.GetTypeId(messageType), handler);
            return this;
        }

        public DispatchHandler AddHandler<T>(Action<MessageSession, T> handler) where T : class, IMessage
        {
            return AddHandler(typeof (T), (session, message) => handler(session, message as T));
        }

        public DispatchHandler AddHandler(Type messageType, SessionHandler handler)
        {
            _sessionHandlerMap.AddHandler(MessageHelper.GetTypeId(messageType), handler);
            return this;
        }

        public DispatchHandler AddHandler<T>(Action<Entity, T> handler)
            where T : class, IMessage
        {
            return AddHandler(typeof (T), (entity, message) => handler(entity, message as T));
        }

        public DispatchHandler AddHandler(Type messageType, EntityHandler handler)
        {
            _entityHandlerMap.AddHandler(MessageHelper.GetTypeId(messageType), handler);
            return this;
        }

        public DispatchHandler AddHandler<T>(Action<Entity, MessageSession, T> handler)
            where T : class, IMessage
        {
            return AddHandler(typeof (T), (entity, session, message) => handler(entity, session, message as T));
        }

        public DispatchHandler AddHandler(Type messageType, EntitySessionHandler handler)
        {
            _entitySessionHandlerMap.AddHandler(MessageHelper.GetTypeId(messageType), handler);
            return this;
        }

        public bool Execute(IMessage message)
        {
            return Execute(null, null, message);
        }

        public bool Execute(Entity entity, IMessage message)
        {
            return Execute(entity, entity != null ? MessageSessionManager.Instance.At(entity.Id) : null, message);
        }

        public bool Execute(MessageSession session, IMessage message)
        {
            return Execute(session != null ? EntityManager.Instance.At(session.SessionId) : null, session, message);
        }

        public bool Execute(Entity entity, MessageSession session, IMessage message)
        {
            Debug.Assert(message != null);

            var executed = ExecuteInternal(message);
            if (session != null) executed = ExecuteInternal(session, message) || executed;
            if (entity != null) executed = ExecuteInternal(entity, message) || executed;
            if (entity != null && session != null) executed = ExecuteInternal(entity, session, message) || executed;
            return executed;
        }

        private bool ExecuteInternal(MessageSession session, IMessage message)
        {
            return _sessionHandlerMap.ExecuteHandler(message.GetTypeId(), handler => handler(session, message));
        }

        private bool ExecuteInternal(IMessage message)
        {
            return _handlerMap.ExecuteHandler(message.GetTypeId(), handler => handler(message));
        }

        private bool ExecuteInternal(Entity entity, MessageSession session, IMessage message)
        {
            return _entitySessionHandlerMap.ExecuteHandler(message.GetTypeId(),
                                                           handler => entity.Post(() => handler(entity, session, message)));
        }

        private bool ExecuteInternal(Entity entity, IMessage message)
        {
            return _entityHandlerMap.ExecuteHandler(message.GetTypeId(), handler => entity.Post(() => handler(entity, message)));
        }
    }
}