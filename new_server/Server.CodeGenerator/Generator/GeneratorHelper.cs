using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Server.Core.Util;

namespace Server.CodeGenerator.Generator
{
    public static class GeneratorHelper
    {
        public static void UpdateProjectFile(string projectFilePath, IEnumerable<string> generatedFiles)
        {
            var projDoc = new XmlDocument();
            projDoc.Load(projectFilePath);

            var edited = false;
            var parentNodeToAdd = FindItemGroupNode(projDoc);
            var includedItems = FindCompileIncludes(projDoc).ToList();
            foreach (var eachFile in generatedFiles)
            {
                if (includedItems.Contains(eachFile))
                    continue;

                var newNode = parentNodeToAdd.AppendChild(projDoc.CreateElement("Compile", projDoc.DocumentElement.NamespaceURI)) as XmlElement;
                newNode.SetAttribute("Include", eachFile);
                edited = true;
            }

            if (edited)
                projDoc.Save(projectFilePath);

        }

        private static XmlNode FindItemGroupNode(XmlDocument projDoc)
        {
            var rootNode = projDoc.DocumentElement;
            if (rootNode == null)
                throw new InvalidOperationException();

            var itemGroupNodes = rootNode.FindChildren("ItemGroup").ToArray();
            if (!itemGroupNodes.Any())
            {
                return rootNode.AppendChild(projDoc.CreateElement("ItemGroup", rootNode.NamespaceURI));
            }

            foreach (var itemGroupNode in itemGroupNodes)
            {
                if (itemGroupNode.FindChildren("Compile").Any())
                    return itemGroupNode;
            }
            return itemGroupNodes.First();
        }

        private static IEnumerable<string> FindCompileIncludes(XmlDocument projDoc)
        {
            foreach (var itemGroupNode in projDoc.DocumentElement.FindChildren("ItemGroup"))
            {
                foreach (var compileNode in itemGroupNode.FindChildren("Compile"))
                {
                    var include = compileNode.GetAttribute("Include");
                    if (!string.IsNullOrWhiteSpace(include))
                        yield return include;
                }
            }
        }
    }
}
