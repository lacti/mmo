#region

using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;

#endregion

namespace Server.Core.Util
{
    public static class XmlExtension
    {
        public static T GetAttribute<T>(this XmlNode node, string name)
        {
            return GetAttribute(node, name, default(T));
        }

        public static T GetAttribute<T>(this XmlNode node, string name, T defaultValue)
        {
            if (node == null || node.Attributes == null || node.Attributes[name] == null)
                return defaultValue;

            return (T) Convert.ChangeType(node.Attributes[name].Value, typeof (T));
        }

        public static IEnumerable<XmlElement> FindChildren(this XmlNode node, string name)
        {
            return from e in node.ChildNodes.OfType<XmlElement>() where e.Name.Equals(name) select e;
        }

        public static T SelectSingleValue<T>(this XmlNode node, string xpath)
        {
            if (node == null)
                return default(T);

            var selectedNode = node.SelectSingleNode(xpath);
            if (selectedNode == null)
                return default(T);

            return selectedNode.Value.As<T>();
        }
    }
}