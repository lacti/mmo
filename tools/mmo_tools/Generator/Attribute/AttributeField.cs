using MmoTools.Util;

namespace MmoTools.Generator.Attribute
{
    internal class AttributeField
    {
        public string Name { get; set; }
        public TypeEnum Type { get; set; }
        public string Default { get; set; }
        public bool Volatile { get; set; }
    }
}