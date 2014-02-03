/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Game
{
    public class UpdateHpMsg : IMessage
    {
        public const int TypeId = 1009;
        
        [Attribute(EntityId = true)]
        public int Id { get; set; }
        
        [Attribute(Attribute = "Fightable", Field = "MaxHp")]
        public int MaxHp { get; set; }
        
        [Attribute(Attribute = "Fightable", Field = "Hp")]
        public int Hp { get; set; }
        
    }
    
}
