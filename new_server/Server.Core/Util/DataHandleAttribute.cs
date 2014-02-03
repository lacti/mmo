using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Server.Core.Util
{
    public class DataHandleAttribute : Attribute
    {
        public bool RedirectIfSessionIsNull { get; set; }
    }
}
