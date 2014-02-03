/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Game
{
    public class CharacterResourceMsg : IMessage
    {
        public const int TypeId = 1007;
        
        [Attribute(EntityId = true)]
        public int Id { get; set; }
        
        [Attribute(Attribute = "Displayable", Field = "ResourceId")]
        public int ResourceId { get; set; }
        
    }
    
}
