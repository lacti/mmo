using Server.Core.Component;

namespace Server.Attribute
{
    public class Player : IAttribute
    {
        [Volatile]
        public bool Logged { get; set; }

        public int ViewRange { get; set; }
    }
}