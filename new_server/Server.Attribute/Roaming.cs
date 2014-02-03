using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Core.Component;

namespace Server.Attribute
{
    class Roaming : IAttribute
    {
        public Pos StartPos { get; set; }
        public Pos EndPos { get; set; }
    }
}
