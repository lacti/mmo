/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Pylon
{
    public class InterChatMsg : IMessage
    {
        public const int TypeId = 4004;
        
        public string Name { get; set; }
        
        public string Message { get; set; }
        
        public long Ticks { get; set; }
        
    }
    
}
