using System;

namespace Server.Core.Messaging
{
    public class MessageQueue
    {
        public static readonly MessageQueue Global = new MessageQueue();

        private MessageQueue()
        {
        }

        public event Func<MessageSession, IMessage, bool> Send;

        public bool SendTo(IMessage message)
        {
            if (Send == null || message == null)
                return false;

            return Send(null, message);
        }

        public bool SendTo(MessageSession session, IMessage message)
        {
            if (Send == null || message == null)
                return false;

            return Send(session, message);
        }
    }
}