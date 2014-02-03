using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using Server.Core.Util;

namespace Server.Core.Data
{
    public class DataTemplateDefine
    {
        private readonly DataTemplate _rootTemplate = new DataTemplate();

        public List<DataTemplate> Templates
        {
            get { return _rootTemplate.Children; }
        }

        public void Load(string dataPath)
        {
            var document = new XmlDocument();
            document.Load(dataPath);

            if (document.DocumentElement == null)
                return;

            foreach (var dataListNode in document.DocumentElement.ChildNodes.OfType<XmlElement>())
            {
                LoadOrFillChildTemplateFromXml(_rootTemplate, dataListNode);
            }
        }

        public void Clear()
        {
            _rootTemplate.Children.Clear();
        }

        public static string GetDataTemplateNameFromDataNode(XmlNode dataNode)
        {
            return dataNode.Name.ToCamelCase();
        }

        public static string GetDataTemplateNameFromListNode(XmlNode dataListNode)
        {
            return GetDataTemplateNameFromDataNode(dataListNode.FirstChild);
        }

        private static void LoadOrFillChildTemplateFromXml(DataTemplate dataTemplate, XmlNode dataListNode)
        {
            var childName = GetDataTemplateNameFromListNode(dataListNode);
            var childTemplate = dataTemplate.Children.SingleOrDefault(e => e.Name.Equals(childName));
            if (childTemplate != null)
                FillTemplateFromXml(dataListNode, childTemplate);
            else
                dataTemplate.AddChild(LoadTemplateFromXml(dataListNode));
        }

        private static DataTemplate LoadTemplateFromXml(XmlNode dataListNode)
        {
            var template = new DataTemplate {Name = GetDataTemplateNameFromListNode(dataListNode)};
            FillTemplateFromXml(dataListNode, template);
            return template;
        }

        private static void FillTemplateFromXml(XmlNode dataListNode, DataTemplate dataTemplate)
        {
            foreach (var dataNode in dataListNode.ChildNodes.OfType<XmlElement>())
            {
                foreach (var attribute in dataNode.Attributes.OfType<XmlAttribute>())
                    dataTemplate.AddAttribute(attribute.Name.ToCamelCase(), attribute.Value);

                if (!dataNode.HasChildNodes)
                    continue;

                var childNode = dataNode.FirstChild;
                if (childNode is XmlText)
                    dataTemplate.AddAttribute("Text", dataNode.FirstChild.Value);
                else
                {
                    LoadOrFillChildTemplateFromXml(dataTemplate, childNode);
                }
            }
        }

        public string GetCanonicalName(DataTemplate dataTemplate)
        {
            if (dataTemplate.Parent.IsRoot)
                return dataTemplate.Name;

            var concatenated = GetCanonicalName(dataTemplate.Parent).ToUnderscored() + '_' +
                               dataTemplate.Name.ToUnderscored();
            var firstCandidate = dataTemplate.Name;
            var secondCandidate = string.Join("_", concatenated.Split('_').Distinct()).ToCamelCase();
            var thirdCandiate = concatenated.ToCamelCase();

            var usingNames = new HashSet<string>();
            for (var iterator = dataTemplate.Parent; iterator != null; iterator = iterator.Parent)
                usingNames.Add(iterator.Name);

            foreach (var each in _rootTemplate.Children)
                usingNames.Add(each.Name);

            if (!usingNames.Contains(firstCandidate)) return firstCandidate;
            if (!usingNames.Contains(secondCandidate)) return secondCandidate;
            if (!usingNames.Contains(thirdCandiate)) return thirdCandiate;
            throw new InvalidDataException("Cannot make acceptable name(" + dataTemplate.Name + ").");
        }
    }
}