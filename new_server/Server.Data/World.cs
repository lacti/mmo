/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class World : IData
    {
        public class WorldNpc
        {
            public int Id { get; set; }
            public int NpcRef { get; set; }
            public string Loc { get; set; }
            public string Rot { get; set; }
            public Npc Npc { get { return DataCenter.Instance[typeof (Npc), NpcRef] as Npc; } }
        }
        
        public int Id { get; set; }
        public int WorldNameRef { get; set; }
        public int SpriteRef { get; set; }
        public WorldName WorldName { get { return DataCenter.Instance[typeof (WorldName), WorldNameRef] as WorldName; } }
        public Sprite Sprite { get { return DataCenter.Instance[typeof (Sprite), SpriteRef] as Sprite; } }
        public List<WorldNpc> WorldNpcs { get; private set; }
        
        public World()
        {
            WorldNpcs = new List<WorldNpc>();
        }
    }
    
}
