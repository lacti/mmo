/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Resource
{
    public class VoiceMsg : IMessage
    {
        public const int TypeId = 3000;
        
        [Attribute(EntityId = true)]
        public int Id { get; set; }
        
        public byte[] Mp3 { get; set; }
        
    }
    
}
