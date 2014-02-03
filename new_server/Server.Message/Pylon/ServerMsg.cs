/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Pylon
{
    public class ServerMsg : IMessage
    {
        public const int TypeId = 4000;
        
        public string Host { get; set; }
        
        public int Port { get; set; }
        
        public string Name { get; set; }
        
    }
    
}
