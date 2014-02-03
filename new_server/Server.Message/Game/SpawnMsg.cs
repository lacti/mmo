/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Game
{
    public class SpawnMsg : IMessage
    {
        public const int TypeId = 1001;
        
        [Attribute(EntityId = true)]
        public int Id { get; set; }
        
        [Attribute(Attribute = "Nameplate", Field = "Name")]
        public string Name { get; set; }
        
        public CharacterResourceMsg CharacterResource { get; set; }
        public UpdatePositionMsg UpdatePosition { get; set; }
        public UpdateHpMsg UpdateHp { get; set; }
    }
    
}
