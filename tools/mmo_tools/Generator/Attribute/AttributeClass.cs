using System.Collections.Generic;
using System.Linq;

namespace MmoTools.Generator.Attribute
{
    internal class AttributeClass
    {
        public AttributeClass()
        {
            Fields = new List<AttributeField>();
        }

        public string Name { get; set; }
        public string CustomCode { get; set; }
        public List<AttributeField> Fields { get; private set; }

        public string StructName
        {
            get { return Name + "_t"; }
        }

        public string ReferenceName
        {
            get { return Name + "_ref"; }
        }

        public string HeaderFileName
        {
            get { return Name + "_attribute.h"; }
        }

        public IEnumerable<AttributeField> NonDefaultFields
        {
            get { return from e in Fields where e.Default == null select e; }
        }

        public IEnumerable<AttributeField> NonVolatileFields
        {
            get { return from e in Fields where !e.Volatile select e; }
        }
    }
}