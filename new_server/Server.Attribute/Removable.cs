using Server.Core.Component;

namespace Server.Attribute
{
    public class Removable : IAttribute
    {
        public Removable()
        {
            Action = true;
            Touch = true;
        }

        public bool Action { get; set; }
        public bool Touch { get; set; }
    }
}