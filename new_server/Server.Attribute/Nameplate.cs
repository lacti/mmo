using Server.Core.Component;

namespace Server.Attribute
{
    public class Nameplate : IAttribute
    {
        public string Title { get; set; }
        public string Name { get; set; }
    }
}