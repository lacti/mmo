using Server.Core.Component;

namespace Server.Attribute
{
    public class Displayable : IAttribute
    {
        public Displayable()
        {
            Visible = true;
        }

        public bool Visible { get; set; }
        public int ResourceId { get; set; }
    }
}