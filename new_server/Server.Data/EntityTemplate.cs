/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class EntityTemplate : IData
    {
        public class Attribute
        {
            public string Name { get; set; }
        }
        
        public int Id { get; set; }
        public enum Ids
        {
            System,
            User,
            Npc,
            Item,
            Monster,
            Factory,
        }
        public List<Attribute> Attributes { get; private set; }
        
        public EntityTemplate()
        {
            Attributes = new List<Attribute>();
        }
    }
    
}
