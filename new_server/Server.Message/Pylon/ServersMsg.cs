/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Pylon
{
    public class ServersMsg : IMessage
    {
        public const int TypeId = 4002;
        
        public List<ServerMsg> ServerList { get; private set; }
        
        public ServersMsg()
        {
            ServerList = new List<ServerMsg>();
        }
    }
    
}
