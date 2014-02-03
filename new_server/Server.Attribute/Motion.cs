using Server.Core.Component;
using System;

namespace Server.Attribute
{
    public class Motion : IAttribute
    {
        public double Dir { get; set; }
        public double Speed { get; set; }

        [Volatile]
        public DateTime StartTime { get; set; }
    }
}