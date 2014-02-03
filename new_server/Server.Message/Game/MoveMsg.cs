/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Game
{
    public class MoveMsg : IMessage
    {
        public const int TypeId = 1002;
        
        [Attribute(EntityId = true)]
        public int Id { get; set; }
        
        [Attribute(Attribute = "Pos", Field = "X")]
        public double X { get; set; }
        
        [Attribute(Attribute = "Pos", Field = "Y")]
        public double Y { get; set; }
        
        [Attribute(Attribute = "Motion", Field = "Dir")]
        public double Dir { get; set; }
        
        [Attribute(Attribute = "Motion", Field = "Speed")]
        public double Speed { get; set; }
        
    }
    
}
