using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Xml;
using MmoTools.Base;
using MmoTools.Util;

namespace MmoTools.Generator.Attribute
{
    public class AttributeCodeGenerator : IGenerator
    {
        private readonly List<AttributeClass> _attributes = new List<AttributeClass>();

        private readonly string _outputDirectory;
        private SourceCode _cppCode;

        public AttributeCodeGenerator(string outputDirectory)
        {
            _outputDirectory = outputDirectory;
        }

        public string DocumentElementName
        {
            get { return "mmo-attributes"; }
        }

        public void Load(string fileName, XmlDocument document)
        {
            var rootNode = document.DocumentElement;
            if (rootNode == null)
                return;

            if (!rootNode.Name.Equals(DocumentElementName))
                return;

            // Configuration 읽기
            var stdafxNode = rootNode.SelectSingleNode("configuration/stdafx/text()");
            if (stdafxNode != null)
                Configuration.Stdafx = stdafxNode.Value;

            // Attribute 정의 읽기
            var attributeNodes = rootNode.SelectNodes("attribute");
            if (attributeNodes == null)
                return;

            foreach (XmlNode eachAttrNode in attributeNodes)
            {
                var attributeClass = new AttributeClass
                    {
                        Name = eachAttrNode.GetAttribute<string>("name")
                    };
                if (eachAttrNode["custom"] != null)
                    attributeClass.CustomCode = eachAttrNode["custom"].FirstChild.Value.Trim();

                foreach (var eachFieldNode in eachAttrNode.FindChildren("field"))
                {
                    attributeClass.Fields.Add(new AttributeField
                        {
                            Name = eachFieldNode.GetAttribute<string>("name"),
                            Type = TypeUtil.Parse(eachFieldNode.GetAttribute<string>("type")),
                            Default = eachFieldNode.GetAttribute<string>("default"),
                            Volatile = eachFieldNode.GetAttribute<bool>("volatile"),
                        });
                }
                _attributes.Add(attributeClass);
            }
        }

        public void Generate()
        {
            Parallel.ForEach(_attributes, GenerateHeaders);
            GenerateCpp();
        }

        private void GenerateHeaders(AttributeClass attributeClass)
        {
            var code = new SourceCode();
            code.Append("#pragma once");
            code.NewLine();
            code.Append("#include \"cbes/attribute.h\"");
            code.NewLine();
            code.BracketStart("struct {0} : public attribute_t<{0}>", attributeClass.StructName);
            foreach (var attributeField in attributeClass.Fields)
            {
                code.Append("{2}{0} {1};", TypeUtil.ToDeclareTypeName(attributeField.Type), attributeField.Name, attributeField.Volatile ? "volatile " : "");
            }

            code.NewLine();
            code.Append("// default constructor");
            var constructorArgs = attributeClass.Fields.Select(
                e => string.Format("{0}({1})", e.Name, e.Default ?? TypeUtil.ToDefaultValueInInitializer(e.Type))).ToList();

            if (constructorArgs.Count > 0)
            {
                code.Append("{0}()", attributeClass.StructName);
                code.IndentRight();
                code.Append(": {0} {{}}", string.Join(", ", constructorArgs));
                code.IndentLeft();
            }
            else code.Append("{0}() {{}}", attributeClass.StructName);

            if (attributeClass.NonDefaultFields.Any())
            {
                code.NewLine();
                code.Append("// argumented constructor");
                var paramArgs = new List<string>();
                var initializeArgs = new List<string>();
                // default가 없는 field를 대상으로만 argumented constructor를 만들어준다.
                foreach (var attributeField in attributeClass.Fields)
                {
                    if (attributeField.Default == null)
                    {
                        paramArgs.Add(string.Format("{0} _{1}", TypeUtil.ToArgumentTypeName(attributeField.Type), attributeField.Name));
                        initializeArgs.Add(string.Format("{0}(_{0})", attributeField.Name));
                    }
                    else
                    {
                        initializeArgs.Add(string.Format("{0}({1})", attributeField.Name, attributeField.Default));
                    }
                }

                if (initializeArgs.Count > 0)
                {
                    code.Append("{0}({1})", attributeClass.StructName, string.Join(", ", paramArgs));
                    code.IndentRight();
                    code.Append(": {0} {{}}", string.Join(", ", initializeArgs));
                    code.IndentLeft();
                }
            }

            code.NewLine();
            code.Append(SourceCode.Parse(@"
virtual void from_bson(bson_iterator*);
virtual void to_bson(bson*);
virtual void from_xml(TiXmlElement*);
virtual void to_xml(std::ostream&);".Trim()));

            if (attributeClass.CustomCode != null)
            {
                code.NewLine();
                code.Append(SourceCode.Parse(attributeClass.CustomCode));
            }

            code.BracketEnd(";");

            code.Append("typedef boost::shared_ptr<{0}> {1};", attributeClass.StructName, attributeClass.ReferenceName);
            code.NewLine();

            var headerPath = Path.Combine(_outputDirectory, attributeClass.HeaderFileName);
            code.WriteToFile(headerPath);
        }

        private void GenerateCpp()
        {
            _cppCode = new SourceCode();
            _cppCode.Append("#include \"{0}\"", Configuration.Stdafx);
            foreach (var dependancy in new[] {"singleton.h", "cbes/attribute.h"})
                _cppCode.Append("#include \"{0}\"", dependancy);
            _cppCode.NewLine();
            _cppCode.Append("#pragma warning (disable: 4189 4100 4505)");
            _cppCode.NewLine();

            foreach (var attributeClass in _attributes)
                _cppCode.Append("#include \"{0}\"", attributeClass.HeaderFileName);
            _cppCode.NewLine();

            #region factory

            _cppCode.Append("#pragma region /* factory-register */");
            _cppCode.StaticInitializerStart();
            foreach (var attributeClass in _attributes)
                _cppCode.Append("sys<attribute_factory>().factory_map.insert(std::make_pair(\"{0}\", [=] () {{ return attribute_ref(new {1}); }}));", attributeClass.Name,
                                attributeClass.StructName);
            _cppCode.StaticInitializerEnd();

            _cppCode.Append("#pragma endregion");
            _cppCode.NewLine();

            #endregion

            foreach (var attributeClass in _attributes)
            {
                _cppCode.Append("#pragma region /* {0} */", attributeClass.Name);

                #region from_bson

                {
                    var nonVolatiles = attributeClass.NonVolatileFields.ToArray();
                    if (nonVolatiles.Any())
                    {
                        _cppCode.Append("void {0}::from_bson(bson_iterator*)", attributeClass.StructName);
                        _cppCode.Append("{");
                        _cppCode.Append("}");
                    }
                    else
                    {
                        _cppCode.Append("void {0}::from_bson(bson_iterator* iterator)", attributeClass.StructName);
                        _cppCode.BracketStart();
                        _cppCode.BracketStart("while (bson_iterator_next(iterator))");
                        _cppCode.Append("const char* field_name = bson_iterator_key(iterator);");
                        _cppCode.Append("const char* field_value = bson_iterator_string(iterator);");
                        foreach (var attributeField in nonVolatiles)
                        {
                            _cppCode.Append("if (stricmp(\"{0}\", field_name) == 0) {0} = boost::lexical_cast<{1}>(field_value);",
                                            attributeField.Name, TypeUtil.ToDeclareTypeName(attributeField.Type));
                        }
                        _cppCode.BracketEnd();
                        _cppCode.BracketEnd();
                    }
                }

                #endregion

                #region to_bson

                {
                    _cppCode.Append("void {0}::to_bson(bson* obj)", attributeClass.StructName);
                    _cppCode.BracketStart();
                    _cppCode.Append("bson_append_start_object(obj, \"{0}\");", attributeClass.Name);
                    foreach (var attributeField in attributeClass.NonVolatileFields)
                    {
                        _cppCode.Append("bson_append_string(obj, \"{0}\", boost::lexical_cast<std::string>({0}).c_str());",
                                        attributeField.Name);
                    }
                    _cppCode.Append("bson_append_finish_object(obj);");
                    _cppCode.BracketEnd();
                }

                #endregion

                #region from_xml

                {
                    var nonVolatileFields = attributeClass.NonVolatileFields.ToArray();
                    var primitives = nonVolatileFields.Where(e => TypeUtil.IsPrimitiveType(e.Type)).ToArray();
                    var nonPrimitives = nonVolatileFields.Where(e => !TypeUtil.IsPrimitiveType(e.Type)).ToArray();

                    if (!primitives.Any() && !nonPrimitives.Any())
                    {
                        _cppCode.Append("void {0}::from_xml(TiXmlElement*)", attributeClass.StructName);
                        _cppCode.Append("{");
                        _cppCode.Append("}");
                    }
                    else
                    {
                        _cppCode.Append("void {0}::from_xml(TiXmlElement* node)", attributeClass.StructName);
                        _cppCode.BracketStart();
                        // primitive type은 attribute로 값을 넣어준다.
                        _cppCode.Append("const char* attr_value = nullptr;");
                        foreach (var attributeField in primitives)
                        {
                            if (attributeField.Type == TypeEnum.INT || attributeField.Type == TypeEnum.DOUBLE)
                            {
                                _cppCode.Append("node->Attribute(\"{0}\", &{0});", attributeField.Name);
                            }
                            else
                            {
                                _cppCode.Append("attr_value = node->Attribute(\"{0}\");", attributeField.Name);
                                _cppCode.Append("if (attr_value != nullptr) {0} = boost::lexical_cast<{1}>(attr_value);", attributeField.Name,
                                                TypeUtil.ToDeclareTypeName(attributeField.Type));
                            }
                        }

                        // non-primitive type은 child element로 값을 넣어준다.
                        if (nonPrimitives.Any())
                        {
                            _cppCode.BracketStart("for (TiXmlElement* each_node = node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement())");
                            _cppCode.Append("const char* node_name = each_node->Value();");
                            foreach (var attributeField in nonPrimitives)
                            {
                                _cppCode.BracketStart("if (stricmp(\"{0}\", node_name) == 0)", attributeField.Name);
                                _cppCode.Append(
                                    attributeField.Type == TypeEnum.STRING
                                        ? "{0} = std::string(each_node->GetText() != nullptr? each_node->GetText(): \"\");"
                                        : "xml_custom_convert(each_node, &{0});", attributeField.Name);
                                _cppCode.BracketEnd();
                            }
                            _cppCode.BracketEnd();
                        }
                        _cppCode.BracketEnd();
                    }
                }

                #endregion

                #region to_xml

                {
                    _cppCode.Append("void {0}::to_xml(std::ostream& out)", attributeClass.StructName);
                    _cppCode.BracketStart();

                    var nonVolatileFields = attributeClass.NonVolatileFields.ToArray();
                    var primitives = nonVolatileFields.Where(e => TypeUtil.IsPrimitiveType(e.Type)).ToArray();
                    var nonPrimitives = nonVolatileFields.Where(e => !TypeUtil.IsPrimitiveType(e.Type)).ToArray();

                    if (!primitives.Any() && !nonPrimitives.Any())
                        _cppCode.Append("out << \"<{0}/>\";", attributeClass.Name);
                    else
                    {
                        _cppCode.Append("out << \"<{0}\";", attributeClass.Name);

                        if (primitives.Any())
                        {
                            _cppCode.IndentRight();
                            // primitive type은 attribute로 값을 넣어준다.
                            foreach (var attributeField in primitives)
                                _cppCode.Append(@"out << "" {0}=\"""" << {0} << ""\"""";", attributeField.Name);
                            _cppCode.IndentLeft();
                        }
                        if (!nonPrimitives.Any())
                            _cppCode.Append("out << \"/>\";");
                        else
                        {
                            _cppCode.Append("out << \">\";");
                            _cppCode.IndentRight();
                            // non-primitive type은 child element로 값을 넣어준다.
                            foreach (var attributeField in nonPrimitives)
                                _cppCode.Append(@"out << ""<{0}>"" << {0} << ""</{0}>"";", attributeField.Name);
                            _cppCode.IndentLeft();
                            _cppCode.Append("out << \"</{0}>\";", attributeClass.Name);
                        }
                    }
                    _cppCode.Append("out << std::endl;");
                    _cppCode.BracketEnd();
                }

                #endregion

                _cppCode.Append("#pragma endregion");
                _cppCode.NewLine();
            }

            var cppFileName = Path.Combine(_outputDirectory, "bind_attributes.cpp");
            _cppCode.WriteToFile(cppFileName);
        }
    }
}