using System;
using System.Text;
using Server.Core.Util;

namespace Server.Core.Component
{
    public class AttributeHelper
    {
        public static readonly TypeMap<string> TypeMap =
            new TypeMap<string>(Configuration.AttributeAssembly, typeof (IAttribute), GetTypeName, id => !string.IsNullOrWhiteSpace(id));

        public static Type GetType(string attributeTypeName)
        {
            return TypeMap[GetTypeName(attributeTypeName)];
        }

        public static string GetTypeName(Type attributeType)
        {
            return GetTypeName(attributeType.Name);
        }

        public static string GetTypeName(string attributeTypeName)
        {
            return attributeTypeName.ToUnderscored();
        }

        public static string GetTypeName(IAttribute attribute)
        {
            return GetTypeName(attribute.GetType());
        }

        public static string ToString(IAttribute attribute)
        {
            var builder = new StringBuilder();
            var attributeType = attribute.GetType();
            builder.Append("  - ").Append(attributeType.Name).AppendLine();
            foreach (var eachProperty in attributeType.GetProperties())
            {
                var propertyValue = eachProperty.GetValue(attribute);
                builder.Append("    - ").Append(eachProperty.Name).Append('=').Append(propertyValue).AppendLine();
            }
            return builder.ToString();
        }
    }
}