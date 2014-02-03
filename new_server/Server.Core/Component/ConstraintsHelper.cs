using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Server.Core.Util;

namespace Server.Core.Component
{
    public static class ConstraintsHelper
    {
        public static object ApplyConstraints(IAttribute attribute, string propertyName, object newValue)
        {
            Debug.Assert(attribute != null);
            var attributeType = attribute.GetType();
            var propertyInfo = attributeType.GetProperty(propertyName);
            Debug.Assert(propertyInfo != null);

            foreach (var constraints in propertyInfo.GetCustomAttributes(typeof(ConstraintsAttribute), false).OfType<ConstraintsAttribute>())
            {
                switch (constraints.Type)
                {
                    case ConstraintsType.Min:
                        newValue = ApplyMinConstraints(constraints, newValue, attribute, propertyInfo);
                        break;

                    case ConstraintsType.Max:
                        newValue = ApplyMaxConstraints(constraints, newValue, attribute, propertyInfo);
                        break;
                }
            }
            return Convert.ChangeType(newValue, propertyInfo.PropertyType);
        }

        private static object ApplyMaxConstraints(ConstraintsAttribute constraints, object newValue, IAttribute attribute, PropertyInfo propertyInfo)
        {
            switch (constraints.ValueType)
            {
                case ConstraintsValueType.IntValue:
                    return Math.Min(newValue.As<int>(), constraints.IntValue);

                case ConstraintsValueType.FloatValue:
                    return Math.Min(newValue.As<float>(), constraints.FloatValue);

                case ConstraintsValueType.Reference:
                    {
                        var attributeType = attribute.GetType();
                        var referenceProperty = attributeType.GetProperty(constraints.FieldName.ToCamelCase());
                        return Math.Min(newValue.As<float>(), referenceProperty.GetValue(attribute).As<float>());
                    }
            }
            throw new InvalidOperationException("invalid value-type for max constraints: " + constraints.ValueType);
        }

        private static object ApplyMinConstraints(ConstraintsAttribute constraints, object newValue, IAttribute attribute, PropertyInfo propertyInfo)
        {
            switch (constraints.ValueType)
            {
                case ConstraintsValueType.IntValue:
                    return Math.Max(newValue.As<int>(), constraints.IntValue);

                case ConstraintsValueType.FloatValue:
                    return Math.Max(newValue.As<float>(), constraints.FloatValue);

                case ConstraintsValueType.Reference:
                    return Math.Max(newValue.As<float>(), propertyInfo.GetValue(attribute).As<float>());
            }
            throw new InvalidOperationException("invalid value-type for max constraints: " + constraints.ValueType);
        }
    }
}
