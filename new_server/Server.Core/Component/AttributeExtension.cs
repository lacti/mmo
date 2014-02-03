using System;
using System.Diagnostics;

namespace Server.Core.Component
{
    public static class AttributeExtension
    {
        public static void Assign(this IAttribute attribute, IAttribute other)
        {
            Debug.Assert(attribute != null);
            if (other == null)
                return;

            Debug.Assert(attribute.GetType() == other.GetType());
            foreach (var eachProperty in attribute.GetType().GetProperties())
            {
                eachProperty.SetValue(attribute, eachProperty.GetValue(other));
            }
        }

        public static void SetValue(this IAttribute attribute, string name, object value)
        {
            Debug.Assert(attribute != null);
            if (string.IsNullOrWhiteSpace(name))
                return;

            var property = attribute.GetType().GetProperty(name);
            if (property == null)
                return;

            if (value == null)
            {
                property.SetValue(attribute, null);
            }
            else
            {
                property.SetValue(attribute, property.PropertyType == value.GetType()
                                                 ? value
                                                 : Convert.ChangeType(value, property.PropertyType));
            }
        }

        public static object GetValue(this IAttribute attribute, string name)
        {
            Debug.Assert(attribute != null);
            if (string.IsNullOrWhiteSpace(name))
                return null;

            var property = attribute.GetType().GetProperty(name);
            return property == null ? null : property.GetValue(attribute);
        }
    }
}