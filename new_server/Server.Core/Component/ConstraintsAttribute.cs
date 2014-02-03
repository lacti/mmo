using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Core.Component
{
    [AttributeUsage(AttributeTargets.Property, Inherited = false, AllowMultiple = true)]
    public class ConstraintsAttribute : Attribute
    {
        public ConstraintsType Type { get; set; }
        public ConstraintsValueType ValueType { get; set; }

        public int IntValue { get; set; }
        public float FloatValue { get; set; }
        public string StringValue { get; set; }

        public string FieldName { get; set; }
    }
}
