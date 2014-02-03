using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Messaging;

namespace Server.Attribute
{
    [Trigger(Type = TriggerType.Attach, Message = typeof(TickMessage))]
    public class Tickable : IAttribute
    {
        public int Interval { get; set; }
    }
}