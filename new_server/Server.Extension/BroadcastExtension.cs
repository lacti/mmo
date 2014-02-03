using System.Linq;
using Server.Attribute;
using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Messaging;

namespace Server.Extension
{
    public static class BroadcastExtension
    {
        static BroadcastExtension()
        {
            EntityMessageQueue.Global.Broadcast += (entity, message) => entity.Broadcast(message);
        }

        #region MessageSessionManager

        public static void Broadcast<T>(this MessageSessionManager manager, Entity entity,
                                        int exceptEntityId = Entity.InvalidEntityId) where T : IMessage, new()
        {
            Broadcast(manager, entity, entity.ToMessage<T>(), exceptEntityId);
        }

        public static void Broadcast(this MessageSessionManager manager, Entity entity, IMessage message,
                                     int exceptEntityId = Entity.InvalidEntityId)
        {
            if (!entity.Has<Pos>())
                return;

        }

        #endregion

        #region Entity

        public static void Broadcast<T>(this Entity entity, int exceptEntityId = Entity.InvalidEntityId) where T : IMessage, new()
        {
            PositionSystem.Global.Broadcast(entity, entity.ToMessage<T>(), exceptEntityId);
        }

        public static void Broadcast(this Entity entity, IMessage message, int exceptEntityId = Entity.InvalidEntityId)
        {
            PositionSystem.Global.Broadcast(entity, message, exceptEntityId);
        }

        #endregion
    }
}