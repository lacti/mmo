/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class AttributeTemplate : IData
    {
        public class Field
        {
            public string Name { get; set; }
            public string Value { get; set; }
        }
        
        public int Id { get; set; }
        public string Name { get; set; }
        public List<Field> Fields { get; private set; }
        
        public AttributeTemplate()
        {
            Fields = new List<Field>();
        }
    }
    
}
