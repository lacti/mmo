/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class InteractTemplate : IData
    {
        public class Effect
        {
            public Types Type { get; set; }
            public enum Types
            {
                Attribute,
            }
            public Directions Direction { get; set; }
            public enum Directions
            {
                Target,
                Owner,
            }
            public string Attribute { get; set; }
            public string Field { get; set; }
            public Functions Function { get; set; }
            public enum Functions
            {
                Minus,
                Plus,
            }
            public int Value { get; set; }
        }
        
        public int Id { get; set; }
        public enum Ids
        {
            Attack,
            Heal,
            Recovery,
        }
        public List<Effect> Effects { get; private set; }
        
        public InteractTemplate()
        {
            Effects = new List<Effect>();
        }
    }
    
}
