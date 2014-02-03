using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Xml;
using MmoTools.Base;
using MmoTools.Util;

namespace MmoTools.Generator.Data
{
    public class DataCodeGenerator : IGenerator
    {
        private readonly string _outputDirectory;

        public DataCodeGenerator(string outputDirectory)
        {
            _outputDirectory = outputDirectory;
        }

        public string DocumentElementName
        {
            get { return "mmo-data"; }
        }

        public void Load(string fileName, XmlDocument document)
        {
            var rootNode = document.DocumentElement;
            if (rootNode == null)
                return;

            // mmo-database 파일을 대상으로만 작업을 진행한다.
            if (!rootNode.Name.Equals(DocumentElementName))
                return;

            var info = new FileInfo(fileName);
            var rootName = info.Name.Substring(0, info.Name.IndexOf('.'));

            // parse all
            foreach (var dataNode in rootNode.ChildNodes.OfType<XmlElement>())
            {
                var data = ParseEntityListXml(dataNode, null);
                data.XmlFileName = fileName;
                data.XmlRootName = rootName;

                DataCenter.Instance.Add(data.Name, data);
            }

            // make depend map
            foreach (var clazz in DataCenter.Instance.Values)
            {
                foreach (var refered in clazz.ReferencingDescentClass)
                {
                    if (!DataDepender.Instance.ContainsKey(refered))
                        DataDepender.Instance.Add(refered, new List<DataClass>());

                    DataDepender.Instance[refered].Add(clazz);
                }

                if (!DataDepender.Instance.ContainsKey(clazz))
                    DataDepender.Instance.Add(clazz, new List<DataClass>());
                DataDepender.Instance[clazz].Add(clazz);
            }
        }

        public void Generate()
        {
            // inference value type
            Parallel.ForEach(DataCenter.Instance.Values, clazz => clazz.InferenceValueType());

            GenerateTypeIdFile();
            GenerateModelFile();
            GenerateParserFile();
        }

        #region Code Generator

        private void GenerateTypeIdFile()
        {
            var typeIdCode = new SourceCode();
            typeIdCode.Append("#pragma once");
            typeIdCode.NewLine();
            typeIdCode.Append("namespace data { namespace type_id { ;");
            foreach (var clazz in DataCenter.Instance.Values)
            {
                typeIdCode.Append("const int {0} = {1};", clazz.SimpleName, clazz.TypeId);
            }
            typeIdCode.Append("}}");
            typeIdCode.NewLine();
            typeIdCode.WriteToFile(Path.Combine(_outputDirectory, "data_type_id.h"));
        }

        private void GenerateModelFile()
        {
            Parallel.ForEach(DataCenter.Instance.Values, clazz =>
                {
                    // generate header file
                    var header = new SourceCode();
                    header.Append("#pragma once");
                    header.Append("#include <data_def.h>");
                    header.Append("#include \"data_type_id.h\"");
                    header.Append("#include <data_center.h>");
                    header.Append(clazz.GenerateDependencyIncludeCode());
                    header.NewLine();
                    header.Append("namespace data { ;");
                    header.Append(clazz.GenerateModelCode());
                    header.NewLine();
                    header.Append("typedef data_center<{0}> {1}_data;", clazz.TypeName, clazz.SimpleName);
                    header.NewLine();
                    header.Append("#if DATA_RELOADABLE");
                    header.Append(clazz.GenerateReloaderImplCode());
                    header.Append("#endif");
                    header.NewLine();
                    header.Append("void __data_load(data_type_t<{0}>);", clazz.TypeName);
                    header.NewLine();
                    header.Append("}");
                    header.NewLine();
                    header.WriteToFile(Path.Combine(_outputDirectory, clazz.HeaderFileName));
                });
        }

        private void GenerateParserFile()
        {
            const string clientCppFileName = "data_center_client.cpp";
            const string serverCppFileName = "data_center_server.cpp";

            // generate parser file
            var parser = new SourceCode();
            parser.Append("#include <data_expression.h>");
            parser.Append("#include <data_center.h>");
            foreach (var clazz in DataCenter.Instance.Values)
                parser.Append("#include \"{0}\"", clazz.HeaderFileName);

            parser.NewLine();
            parser.Append("#pragma warning( disable : 4996 )");
            parser.NewLine();
            parser.Append("using namespace data;");
            parser.NewLine();
            foreach (var clazz in DataCenter.Instance.Values)
                parser.Append("template <> typename data_center<{0}>::storage_t* data_center<{0}>::storage = nullptr;", clazz.TypeName);

            foreach (var clazz in DataCenter.Instance.Values)
                parser.Append("template <> data_linker_t data_center<{0}>::linker;", clazz.TypeName);

            parser.NewLine();

            foreach (var clazz in DataCenter.Instance.Values)
                parser.Append(clazz.GenerateParseDeclsCode().Generate());
            parser.NewLine();

            foreach (var clazz in DataCenter.Instance.Values)
            {
                parser.Append(clazz.GenerateParseCode().Generate());
                parser.NewLine();
            }
            parser.NewLine();

            foreach (var clazz in DataCenter.Instance.Values)
            {
                parser.Append("void data::__data_load(data_type_t<{0}>)", clazz.TypeName);
                parser.BracketStart();

                parser.Append("TiXmlDocument document;");
                parser.Append("document.LoadFile(user_defined_path_resolver(\"{0}\"));", clazz.XmlFileName.Replace("\\", "\\\\"));
                parser.Append("TiXmlElement* root_node = document.FirstChildElement(\"{0}\");", clazz.XmlRootName);
                parser.Append("parse_{0}(root_node->FirstChildElement(\"{1}-list\"));", clazz.ParserName, clazz.XmlNodeName);

                parser.BracketEnd();
                parser.NewLine();
            }

            SourceCode.StaticInitializerSerial = 99;
            parser.StaticInitializerStart();
            {
                foreach (var clazz in DataCenter.Instance.Values)
                    parser.Append("data::__data_load(data_type_t<{0}>());", clazz.TypeName);

                parser.NewLine();

                foreach (var clazz in DataCenter.Instance.Values)
                    parser.Append("{0}_data::linker.link();", clazz.SimpleName);
            }
            parser.StaticInitializerEnd();
            parser.NewLine();
            parser.Append("#pragma warning( default : 4996 )");
            parser.NewLine();

            parser.WriteToFile(Path.Combine(_outputDirectory, clientCppFileName), "#include <clientpch.h>");
            parser.WriteToFile(Path.Combine(_outputDirectory, serverCppFileName), "#include <serverpch.h>");
        }

        #endregion

        #region Parse Xml

        private static DataClass ParseEntityListXml(XmlNode node, DataClass previousClass)
        {
            var data = previousClass ?? new DataClass {Name = node.FirstChild.Name};
            foreach (XmlNode entityNode in node.ChildNodes)
            {
                ParseEntityXml(entityNode, data);
                ++data.NodeCount;
            }
            return data;
        }

        private static void ParseEntityXml(XmlNode node, DataClass data)
        {
            if (node.InnerText.Length > 0)
                data.HasContent = true;

            if (node.Attributes != null)
            {
                foreach (var attr in node.Attributes.OfType<XmlAttribute>())
                {
                    var key = attr.Name;
                    var value = attr.Value;

                    var field = data.CreateOrGetSimpleField(key);
                    field.Values.Add(value);
                }
            }

            foreach (XmlNode childNode in node.ChildNodes)
            {
                if (!(childNode is XmlElement))
                    continue;

                var field = data.CreateOrGetClassField(childNode.FirstChild.Name);
                field.Class = ParseEntityListXml(childNode, field.Class);
                field.Class.OwnedField = field;
            }
        }

        #endregion
    }
}