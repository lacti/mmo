using System;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using Server.Core.Util;

namespace Server.Core.Component
{
    public class XmlBind : IEntityBind
    {
        private readonly string _directory;
        public XmlBind(string directory)
        {
            _directory = directory;
            IoHelper.CreateDirectory(_directory);
        }

        public bool Add(Entity entity)
        {
            return TryAction(() =>
                {
                    var xmlPath = Path.Combine(_directory, entity.Id + ".xml");
                    WriteXml(SerializeEntity(entity), xmlPath);
                });
        }

        public bool Update(Entity entity)
        {
            return TryAction(() =>
                {
                    Remove(entity);
                    Add(entity);
                });
        }

        public bool Remove(Entity entity)
        {
            return TryAction(() =>
                {
                    var xmlPath = Path.Combine(_directory, entity.Id + ".xml");
                    File.Delete(xmlPath);
                });
        }

        public bool ForEach(Action<Entity> action)
        {
            return TryAction(() =>
                {
                    foreach (var xmlPath in Directory.GetFiles(_directory, "*.xml"))
                    {
                        var entityDoc = new XmlDocument();
                        entityDoc.Load(xmlPath);

                        var entity = DeserializeEntity(entityDoc);
                        action(entity);
                    }
                });
        }

        public void Clear()
        {
            foreach (var each in Directory.GetFiles(_directory, "*.xml"))
                File.Delete(each);
        }

        private static bool TryAction(Action action)
        {
            try
            {
                action();
                return true;
            }
            catch (Exception e)
            {
                Logger.Write(e);
            }
            return false;
        }

        private static void WriteXml(XmlDocument document, string xmlPath)
        {
            using (var writer = XmlWriter.Create(xmlPath, new XmlWriterSettings
                {
                    Encoding = Encoding.UTF8,
                    Indent = true,
                    OmitXmlDeclaration = false,
                }))
            {
                document.Save(writer);
            }
        }

        // ReSharper disable PossibleNullReferenceException
        private static XmlDocument SerializeEntity(Entity entity)
        {
            var entityDoc = new XmlDocument();
            var entityNode = entityDoc.AppendChild(entityDoc.CreateElement("entity")) as XmlElement;
            entityNode.SetAttribute("id", entity.Id.ToString(CultureInfo.InvariantCulture));
            foreach (var attributePair in entity.Attributes)
            {
                var attributeType = attributePair.Key;

                // skip when volatile class
                var classVolatiles = attributeType.GetCustomAttributes(typeof(VolatileAttribute), false);
                if (classVolatiles.Length > 0)
                    continue;

                var attribute = attributePair.Value;

                var attributeNode = entityNode.AppendChild(entityDoc.CreateElement("attribute")) as XmlElement;
                attributeNode.SetAttribute("name", attributeType.Name.ToUnderscored());

                foreach (var eachProperty in attributeType.GetProperties())
                {
                    // skip when volatile property
                    var propertyVolatiles = eachProperty.GetCustomAttributes(typeof(VolatileAttribute), false);
                    if (propertyVolatiles.Length > 0)
                        continue;

                    var eachValue = eachProperty.GetValue(attribute);
                    var valueNode = attributeNode.AppendChild(entityDoc.CreateElement("field")) as XmlElement;
                    valueNode.SetAttribute("name", eachProperty.Name.ToUnderscored());
                    valueNode.SetAttribute("value", TypeChanger.ToString(eachValue));
                }
            }
            return entityDoc;
        }

        private static Entity DeserializeEntity(XmlDocument entityDoc)
        {
            var entityNode = entityDoc.DocumentElement;
            var entityId = entityNode.GetAttribute("id").As<int>();
            var entity = new Entity { Id = entityId };
            foreach (var attributeNode in entityNode.ChildNodes.OfType<XmlElement>())
            {
                var attributeName = attributeNode.GetAttribute("name");
                if (!AttributeHelper.TypeMap.ContainsKey(attributeName))
                    continue;

                var attributeType = AttributeHelper.TypeMap[attributeName];
                var attribute = entity.Attach(attributeType);

                foreach (var eachProperty in attributeType.GetProperties())
                {
                    var propertyName = eachProperty.Name.ToUnderscored();
                    var propertyNode = attributeNode.SelectSingleNode(string.Format("field[@name='{0}']", propertyName)) as XmlElement;
                    if (propertyNode == null)
                        continue;

                    var propertyValue = propertyNode.GetAttribute("value");
                    if (!eachProperty.CanWrite)
                        continue;

                    eachProperty.SetValue(attribute, TypeChanger.ChangeType(propertyValue, eachProperty.PropertyType));
                }
            }
            return entity;
        }
        // ReSharper restore PossibleNullReferenceException

    }
}
