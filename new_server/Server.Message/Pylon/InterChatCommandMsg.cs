/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Pylon
{
    public class InterChatCommandMsg : IMessage
    {
        public const int TypeId = 4005;
        
        public int TypeCode { get; set; }
        
        public string Content { get; set; }
        
    }
    
}
