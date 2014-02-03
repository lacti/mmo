using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Xml;
using Server.Core.Util;

namespace Server.Core.Data
{
    public class DataCenter
    {
        #region TypeMap

        public static readonly TypeMap<string> TypeMap =
            new TypeMap<string>(Configuration.DataAssembly, typeof (IData), GetTypeName,
                                id => !string.IsNullOrWhiteSpace(id));

        public static string GetTypeName(Type dataType)
        {
            return dataType.Name;
        }

        public static string GetTypeName(IData data)
        {
            return GetTypeName(data.GetType());
        }

        #endregion

        #region Singleton

        public static readonly DataCenter Instance = new DataCenter();

        private DataCenter()
        {
        }

        #endregion

        #region Accessor

        private readonly Dictionary<Type, List<IData>> _storage = new Dictionary<Type, List<IData>>();

        public object this[Type type, int index]
        {
            get { return _storage.ContainsKey(type) ? _storage[type].SingleOrDefault(e => e.Id == index) : null; }
        }

        public IEnumerable<T> GetData<T>() where T : class, IData
        {
            if (_storage.ContainsKey(typeof (T)))
                return _storage[typeof (T)].OfType<T>();

            return new T[0];
        }

        public IEnumerable<T> GetData<T>(Func<T, bool> predicate) where T : class, IData
        {
            return from e in GetData<T>() where predicate(e) select e;
        }

        public T GetDatum<T>(Func<T, bool> predicate) where T : class, IData
        {
            return GetData(predicate).FirstOrDefault();
        }

        public T GetDatum<T>(int id) where T : class, IData
        {
            return this[typeof(T), id] as T;
        }

        public void Clear()
        {
            _storage.Clear();
        }

        #endregion

        #region Load & Save

        private readonly DataTemplateDefine _define = new DataTemplateDefine();
        private ReadState _lastReadState;

        public void Load(string dataPath)
        {
            _lastReadState = new ReadState {DataPath = dataPath};
            _define.Clear();

            // define을 먼저 읽어야 Canonical 이름을 찾을 수가 있다.
            _define.Load(dataPath);
            LoadInternal(dataPath);
        }

        public void Load(string dataPath, string pattern, SearchOption option = SearchOption.TopDirectoryOnly)
        {
            _lastReadState = new ReadState {DataPath = dataPath, SearchPattern = pattern, Option = option};
            _define.Clear();

            // define을 먼저 읽어야 Canonical 이름을 찾을 수가 있다.
            foreach (var eachXml in Directory.GetFiles(dataPath, pattern, option))
                _define.Load(eachXml);

            foreach (var eachXml in Directory.GetFiles(dataPath, pattern, option))
                Load(eachXml);
        }

        public void Reload()
        {
            if (_lastReadState == null)
                return;

            if (!string.IsNullOrWhiteSpace(_lastReadState.SearchPattern))
                Load(_lastReadState.DataPath, _lastReadState.SearchPattern, _lastReadState.Option);
            else Load(_lastReadState.DataPath);
        }

        private void LoadInternal(string dataPath)
        {
            if (!File.Exists(dataPath))
                return;

            var document = new XmlDocument();
            document.Load(dataPath);

            if (document.DocumentElement == null)
                return;

            if (!document.DocumentElement.HasChildNodes)
                return;

            foreach (var dataListNode in document.DocumentElement.ChildNodes.OfType<XmlElement>())
            {
                foreach (var dataNode in dataListNode.ChildNodes.OfType<XmlElement>())
                {
                    var dataNodeName = DataTemplateDefine.GetDataTemplateNameFromDataNode(dataNode);
                    var dataTemplate = _define.Templates.Single(e => e.Name.Equals(dataNodeName));

                    var dataObject = ReadObjectFromXml(dataNode, null, dataTemplate);
                    if (dataObject == null)
                        continue;

                    var dataClassName = _define.GetCanonicalName(dataTemplate);
                    var dataType = TypeMap[dataClassName];
                    Debug.Assert(dataObject is IData, "dataObject is IData");

                    if (!_storage.ContainsKey(dataType))
                        _storage.Add(dataType, new List<IData>());
                    _storage[dataType].Add(dataObject as IData);
                }
            }
        }

        private object ReadObjectFromXml(XmlNode dataNode, Type parentType, DataTemplate template)
        {
            var dataClassName = _define.GetCanonicalName(template);
            Type dataType;
            if (parentType == null)
            {
                if (!TypeMap.ContainsKey(dataClassName))
                    return null;
                dataType = TypeMap[dataClassName];
            }
            else
            {
                dataType = parentType.GetNestedType(dataClassName);
            }

            var idEnumType = dataType.GetNestedType("Ids");
            var hasIdEnum = idEnumType != null;
            var dataObject = Activator.CreateInstance(dataType);
            if (dataNode.Attributes != null)
            {
                foreach (var attribute in dataNode.Attributes.OfType<XmlAttribute>())
                {
                    var attributeName = attribute.Name.ToCamelCase();
                    var attributeProperty = dataType.GetProperty(attributeName);
                    if (attributeProperty == null)
                        continue;

                    var isIdAttribute = attributeProperty.Name.Equals("Id");
                    if (attributeProperty.PropertyType.IsEnum ||
                        (isIdAttribute && hasIdEnum))
                    {
                        var enumString = attribute.Value.Substring(1);   // remove leading _ character.
                        var enumType = isIdAttribute ? idEnumType : attributeProperty.PropertyType;
                        Debug.Assert(enumType != null);

                        var enumValue = Enum.Parse(enumType, enumString, true);
                        attributeProperty.SetValue(dataObject,
                                                   Convert.ChangeType(enumValue, attributeProperty.PropertyType));
                    }
                    else
                    {
                        attributeProperty.SetValue(dataObject,
                                                   Convert.ChangeType(attribute.Value, attributeProperty.PropertyType));
                    }
                }
            }

            if (dataNode.HasChildNodes)
            {
                var childNode = dataNode.FirstChild;
                if (childNode is XmlText)
                {
                    var textProperty = dataType.GetProperty("Text");
                    textProperty.SetValue(dataObject, dataNode.FirstChild.Value);
                }
                else
                {
                    var listMap = new Dictionary<PropertyInfo, IList>();
                    foreach (var childDataNode in childNode.ChildNodes.OfType<XmlElement>())
                    {
                        var childNodeName = DataTemplateDefine.GetDataTemplateNameFromDataNode(childDataNode);
                        var childTemplate = template.Children.Single(e => e.Name.Equals(childNodeName));
                        var childClassName = _define.GetCanonicalName(childTemplate);

                        var listProperty = dataType.GetProperty(childClassName + "s");
                        if (!listMap.ContainsKey(listProperty))
                            listMap.Add(listProperty, Activator.CreateInstance(listProperty.PropertyType) as IList);

                        var childObject = ReadObjectFromXml(childDataNode, dataType, childTemplate);
                        if (childObject == null)
                            continue;

                        listMap[listProperty].Add(childObject);
                    }

                    foreach (var pair in listMap)
                        pair.Key.SetValue(dataObject, pair.Value);
                }
            }

            return dataObject;
        }

        private class ReadState
        {
            public string DataPath { get; set; }
            public string SearchPattern { get; set; }
            public SearchOption Option { get; set; }
        }

        #endregion
    }
}