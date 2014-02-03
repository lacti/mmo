using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using Server.Core.Util;

namespace Server.CodeGenerator.Generator
{
    internal class MessageGenerator : IGenerator
    {
        public void Generate()
        {
            // ReSharper disable AssignNullToNotNullAttribute
            // ReSharper disable PossibleNullReferenceException
            var document = new XmlDocument();
            document.Load(PathConfig.MessageXmlPath);

            var postfix = document.SelectSingleNode("/mmo-msgs/@postfix").Value.ToCamelCase();

            var generatedFiles = new List<string>();
            var groupIndex = 0;
            foreach (var groupNode in document.SelectNodes("//group").OfType<XmlElement>())
            {
                ++groupIndex;

                var groupName = groupNode.GetAttribute("name").ToCamelCase();
                var groupPath = Path.Combine(PathConfig.MessageProjectPath, groupName);
                IoHelper.CreateDirectory(groupPath);

                var typeIndex = 0;
                foreach (var msgNode in groupNode.SelectNodes("msg").OfType<XmlElement>())
                {
                    var typeId = groupIndex*1000 + typeIndex;
                    ++typeIndex;

                    if (msgNode.GetAttribute("internal").Equals("true"))
                        continue;

                    var messageName = msgNode.GetAttribute("name");
                    var messageClassName = messageName.ToCamelCase() + postfix;

                    var code = new SourceCode();
                    code.Append("using System.Collections.Generic;");
                    code.Append("using Server.Core.Component;");
                    code.Append("using Server.Core.Messaging;");
                    code.NewLine();
                    code.Append("namespace Server.Message.{0}", groupName);
                    code.BracketStart();

                    code.Append("public class {0} : IMessage", messageClassName);
                    code.BracketStart();
                    code.Append("public const int TypeId = {0};", typeId);
                    code.NewLine();

                    var listMembers = new Dictionary<string, string>();
                    foreach (var fieldNode in msgNode.SelectNodes("field").OfType<XmlElement>())
                    {
                        var fieldType = fieldNode.GetAttribute("type");
                        var fieldName = fieldNode.GetAttribute("name").ToCamelCase();
                        if (fieldType.Equals("bin", StringComparison.OrdinalIgnoreCase))
                            fieldType = "byte[]";

                        if (fieldName.Equals("id", StringComparison.OrdinalIgnoreCase))
                        {
                            code.Append("[Attribute(EntityId = true)]");
                        }

                        if (fieldNode.HasAttribute("attribute"))
                        {
                            var attributeBind = fieldNode.GetAttribute("attribute").Split('.');
                            var attributeClassName = attributeBind[0].ToCamelCase();
                            var attributeFieldName = attributeBind[1].ToCamelCase();
                            code.Append("[Attribute(Attribute = \"{0}\", Field = \"{1}\")]", attributeClassName, attributeFieldName);
                        }

                        code.Append("public {0} {1} {{ get; set; }}", fieldType, fieldName);
                        code.NewLine();
                    }

                    foreach (var refNode in msgNode.SelectNodes("ref").OfType<XmlElement>())
                    {
                        var refName = refNode.GetAttribute("msg").ToCamelCase();
                        var onceRef = refNode.GetAttribute<bool>("once");
                        if (onceRef)
                        {
                            code.Append("public {0}{1} {0} {{ get; set; }}", refName, postfix);
                        }
                        else
                        {
                            var listTypeName = string.Format("List<{0}{1}>", refName, postfix);
                            var listMemberName = string.Format("{0}List", refName);
                            code.Append("public {0} {1} {{ get; private set; }}", listTypeName, listMemberName);
                            listMembers.Add(listMemberName, listTypeName);
                        }
                    }

                    if (listMembers.Count > 0)
                    {
                        code.NewLine();
                        code.Append("public {0}()", messageClassName);
                        code.BracketStart();
                        foreach (var pair in listMembers)
                        {
                            code.Append("{0} = new {1}();", pair.Key, pair.Value);
                        }
                        code.BracketEnd();
                    }
                    code.BracketEnd();
                    code.NewLine();

                    code.BracketEnd();
                    code.NewLine();

                    var messageFileName = messageClassName + ".cs";
                    var messageFilePath = Path.Combine(groupPath, messageFileName);
                    code.WriteToFile(messageFilePath);

                    generatedFiles.Add(Path.Combine(groupName, messageFileName));
                }
            }

            GeneratorHelper.UpdateProjectFile(PathConfig.MessageProjectFilePath, generatedFiles);

            // ReSharper restore AssignNullToNotNullAttribute
            // ReSharper restore PossibleNullReferenceException
        }
    }
}