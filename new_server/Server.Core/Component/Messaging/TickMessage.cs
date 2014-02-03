using System;
using Server.Core.Messaging;

namespace Server.Core.Component.Messaging
{
    public class TickMessage : IMessage
    {
        public const int TypeId = 65534;

        public TickMessage()
        {
            StartTime = DateTime.Now;
        }

        [Component.Attribute(EntityId = true)]
        public int Id { get; set; }

        public DateTime StartTime { get; set; }
    }
}