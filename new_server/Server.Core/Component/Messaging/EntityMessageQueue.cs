using System;
using System.Linq;
using Server.Core.Messaging;

namespace Server.Core.Component.Messaging
{
    public class EntityMessageQueue
    {
        public static readonly EntityMessageQueue Global = new EntityMessageQueue();

        private EntityMessageQueue()
        {
        }

        public event Func<Entity, IMessage, bool> Send;

        public event Action<Entity, IMessage> Broadcast;

        public bool SendTo(Entity entity, IMessage message)
        {
            if (Send == null)
                return false;

            return Send(entity, message);
        }

        public void BroadcastTo(Entity entity, IMessage message)
        {
            if (Broadcast == null)
                return;

            Broadcast(entity, message);
        }
    }
}