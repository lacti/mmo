using System;
using System.Collections.Generic;
using System.Xml;

namespace MmoTools.Util
{
    public static class XmlExtensions
    {
        public static bool IsXmlElement(this XmlNode node, string tagName)
        {
            if (node == null) return false;
            if (!(node is XmlElement)) return false;
            if (!node.Name.Equals(tagName)) return false;
            return true;
        }

        public static IEnumerable<XmlElement> FindChildren(this XmlNode node, string tagName)
        {
            if (node != null)
            {
                foreach (XmlNode eachChild in node.ChildNodes)
                {
                    if (eachChild.IsXmlElement(tagName))
                        yield return eachChild as XmlElement;
                }
            }
        }

        public static T GetAttribute<T>(this XmlNode node, string attrName)
        {
            if (node.Attributes[attrName] == null)
                return default(T);

            var strValue = node.Attributes[attrName].Value;
            return (T) Convert.ChangeType(strValue, typeof (T));
        }
    }
}