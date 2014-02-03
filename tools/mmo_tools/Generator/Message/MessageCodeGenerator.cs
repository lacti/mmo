using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Xml;
using MmoTools.Base;
using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    public class MessageCodeGenerator : IGenerator
    {
        public const string NamespaceName = "msg";
        private readonly List<MessageGroup> _groups = new List<MessageGroup>();

        private readonly string _options = "";
        private readonly string _outputDirectory;

        public MessageCodeGenerator(string outputDirectory, string options)
        {
            _outputDirectory = outputDirectory;
            _options = options;
        }

        private string Postfix { get; set; }

        public string DocumentElementName
        {
            get { return "mmo-msgs"; }
        }

        public void Load(string fileName, XmlDocument doc)
        {
            var rootNode = doc.DocumentElement;
            if (rootNode == null)
                return;

            if (!rootNode.Name.Equals(DocumentElementName))
                return;

            Postfix = rootNode.GetAttribute("postfix");
            foreach (var groupNode in rootNode.ChildNodes.OfType<XmlElement>().Where(e => e.Name == "group"))
            {
                _groups.Add(ParseGroupNode(groupNode));
            }
        }

        public void Generate()
        {
            Parallel.ForEach(_groups, e => e.Generate(_outputDirectory, _options));
        }

        private MessageGroup ParseGroupNode(XmlElement groupNode)
        {
            var group = new MessageGroup {Name = groupNode.GetAttribute("name")};
            foreach (var msgNode in groupNode.ChildNodes.OfType<XmlElement>().Where(e => e.Name.Equals("msg")))
            {
                group.Messages.Add(ParseMessageNode(msgNode));
            }
            group.Messages.ForEach(e => e.Group = group);
            return group;
        }

        private Message ParseMessageNode(XmlElement msgNode)
        {
            var msg = new Message
                {
                    From = msgNode.GetAttribute("from"),
                    To = msgNode.GetAttribute("to"),
                    Name = msgNode.GetAttribute("name"),
                    Postfix = Postfix,
                    Namespace = NamespaceName
                };

            ParseEachField(msgNode, msg.Fields);
            msg.Fields.ForEach(e => e.Message = msg);
            return msg;
        }

        private void ParseEachField(XmlElement msgNode, ICollection<Field> fields)
        {
            foreach (var fieldNode in msgNode.ChildNodes.OfType<XmlElement>())
            {
                var fieldType = fieldNode.Name;
                switch (fieldType)
                {
                    case "field":
                        fields.Add(ParseSimpleFieldNode(fieldNode));
                        break;
                    case "list":
                        fields.Add(ParseListFieldNode(fieldNode));
                        break;
                    case "ref":
                        fields.Add(ParseRefFieldNode(fieldNode));
                        break;
                }
            }
        }

        private SimpleField ParseSimpleFieldNode(XmlElement fieldNode)
        {
            return new SimpleField
                {
                    Name = fieldNode.GetAttribute("name"),
                    Type = TypeUtil.Parse(fieldNode.GetAttribute("type")),
                    Optional = fieldNode.GetAttribute<bool>("optional"),
                    BindAttribute = AttributeBind.Parse(fieldNode.GetAttribute("attribute"))
                };
        }

        private ListField ParseListFieldNode(XmlElement listNode)
        {
            var list = new ListField
                {
                    Name = listNode.GetAttribute("name")
                };

            ParseEachField(listNode, list.Fields);
            return list;
        }

        private RefField ParseRefFieldNode(XmlElement refNode)
        {
            var once = refNode.GetAttribute<bool>("once");
            var target = refNode.GetAttribute("msg");
            var name = refNode.HasAttribute("name") ? refNode.GetAttribute("name") : target;

            return once
                       ? new RefField {Name = name, TargetName = target}
                       : new RefListField {Name = name, TargetName = target};
        }
    }
}