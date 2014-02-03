/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Message.Game
{
    public class WorldInfoMsg : IMessage
    {
        public const int TypeId = 1005;
        
        [Attribute(EntityId = true)]
        public int Id { get; set; }
        
        public int WorldId { get; set; }
        
        public List<SpawnMsg> SpawnList { get; private set; }
        
        public WorldInfoMsg()
        {
            SpawnList = new List<SpawnMsg>();
        }
    }
    
}
