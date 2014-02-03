using System;

namespace Server.Core.Component
{
    [AttributeUsage(AttributeTargets.Property, Inherited = false)]
    public class AttributeAttribute : Attribute
    {
        public string Attribute { get; set; }
        public string Field { get; set; }

        public bool EntityId { get; set; }
    }
}