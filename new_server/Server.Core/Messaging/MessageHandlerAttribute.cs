using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Core.Messaging
{
    [AttributeUsage(AttributeTargets.Method, Inherited = false)]
    public class MessageHandlerAttribute : Attribute
    {
        public const int DeduceFromParameter = -1;

        public int MessageId { get; set; }

        public MessageHandlerAttribute()
            : this(DeduceFromParameter)
        {
        }

        public MessageHandlerAttribute(int messageId)
        {
            MessageId = messageId;
        }
    }
}
