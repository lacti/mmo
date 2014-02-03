using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using MmoTools.Util;

namespace MmoTools.Generator.Data
{
    internal class DataClass
    {
        public static int TypeIdSerial = 0;
        public int TypeId = TypeIdSerial++;

        public DataClass()
        {
            Fields = new Dictionary<string, DataField>();
        }

        #region Names

        private string _canonicalName;
        private string _collectionName;
        private string _collectionTypeName;
        private string _headerFileName;
        private string _includeFileName;
        private string _parserName;
        private string _qualifiedTypeName;
        private string _simpleName;
        private string _typeName;
        private string _xmlNodeName;

        public string XmlNodeName
        {
            get { return _xmlNodeName ?? (_xmlNodeName = Name.Replace('_', '-')); }
        }

        public string SimpleName
        {
            get { return _simpleName ?? (_simpleName = Name.ToEnumName().ToLower()); }
        }

        public string CanonicalName
        {
            get { return _canonicalName ?? (_canonicalName = MakeCanonicalName()); }
        }

        public string HeaderFileName
        {
            get { return _headerFileName ?? (_headerFileName = SimpleName + "_data.h"); }
        }

        public string TypeName
        {
            get { return _typeName ?? (_typeName = CanonicalName + "_t"); }
        }

        public string CollectionTypeName
        {
            get { return _collectionTypeName ?? (_collectionTypeName = CanonicalName + (HasId ? "_map_t" : "_list_t")); }
        }

        public string CollectionName
        {
            get { return _collectionName ?? (_collectionName = CanonicalName + (HasId ? "_map" : "_list")); }
        }

        public string IncludeFileName
        {
            get { return _includeFileName ?? (_includeFileName = SimpleName.Replace("-", "_").ToLower()); }
        }

        public string ParserName
        {
            get { return _parserName ?? (_parserName = IsTopMost ? CanonicalName : OwnedField.OwnedClass.ParserName + "_" + CanonicalName); }
        }

        public string QualifiedTypeName
        {
            get { return _qualifiedTypeName ?? (_qualifiedTypeName = IsTopMost ? TypeName : OwnedField.OwnedClass.QualifiedTypeName + "::" + TypeName); }
        }

        private string MakeCanonicalName()
        {
            if (IsTopMost)
                return SimpleName;

            var parentName = "";
            if (OwnedField != null)
                parentName = OwnedField.OwnedClass.CanonicalName + "_";
            var firstCandidateName = (parentName + Name).ToEnumName().ToLower();
            var secondCandidateName = string.Join("_", firstCandidateName.Split('_').Distinct());
            var thirdCandidateName = Name.ToEnumName().ToLower();

            var names = new List<string>();
            for (var field = OwnedField; field != null; field = field.OwnedClass.OwnedField)
                names.Add(field.OwnedClass.CanonicalName);

            names.AddRange(DataCenter.Instance.Values.Select(clazz => clazz.SimpleName));

            if (!names.Contains(thirdCandidateName)) return thirdCandidateName;
            if (!names.Contains(secondCandidateName)) return secondCandidateName;
            if (!names.Contains(firstCandidateName)) return firstCandidateName;
            throw new InvalidDataException("Cannot make acceptable name(" + Name + ").");
        }

        #endregion

        #region Properties

        public bool IsTopMost
        {
            get { return OwnedField == null; }
        }

        public bool HasId
        {
            get { return Fields.Values.Count(field => field is SimpleDataField && (field as SimpleDataField).IsId) > 0; }
        }

        public bool IsNameContent
        {
            get { return Name != null && Name.ToLower().Contains("name"); }
        }

        public DataClass Ancestor
        {
            get { return IsTopMost ? this : OwnedField.OwnedClass.Ancestor; }
        }

        public string ParentClassType
        {
            get { return OwnedField.OwnedClass.QualifiedTypeName; }
        }

        public bool NeedLinker
        {
            get { return Fields.Values.Count(field => field is SimpleDataField && (field as SimpleDataField).ValueType == TypeEnum.DATA_REFERENCE) > 0; }
        }

        public IEnumerable<DataClassField> DataClassFields
        {
            get { return from f in Fields.Values where f is DataClassField select f as DataClassField; }
        }

        public IEnumerable<SimpleDataField> ReferenceFields
        {
            get { return from field in Fields.Values.OfType<SimpleDataField>() where field.ValueType == TypeEnum.DATA_REFERENCE select field; }
        }

        public IEnumerable<DataClass> ReferencingChildClass
        {
            get { return from field in Fields.Values.OfType<SimpleDataField>() where field.IsReferenceType select field.ReferenceDataClass; }
        }

        public IEnumerable<DataClass> ReferencingDescentClass
        {
            get
            {
                foreach (var each in ReferencingChildClass) yield return each;
                foreach (var refer in ReferencingChildClass)
                    foreach (var each in refer.ReferencingChildClass) yield return each;
            }
        }

        #endregion

        #region Mutator

        public SimpleDataField CreateOrGetSimpleField(string name)
        {
            if (Fields.ContainsKey(name))
                return Fields[name] as SimpleDataField;
            Fields.Add(name, new SimpleDataField {Name = name, OwnedClass = this});
            return Fields[name] as SimpleDataField;
        }

        public DataClassField CreateOrGetClassField(string name)
        {
            if (Fields.ContainsKey(name))
                return Fields[name] as DataClassField;
            Fields.Add(name, new DataClassField {Name = name, OwnedClass = this});
            return Fields[name] as DataClassField;
        }

        public void InferenceValueType()
        {
            Parallel.ForEach(Fields.Values.OfType<SimpleDataField>(), simpleField => simpleField.InferenceValueType());
            Parallel.ForEach(Fields.Values.OfType<DataClassField>(), classField => classField.Class.InferenceValueType());
        }

        #endregion

        #region Code Generator

        public SourceCode GenerateModelCode()
        {
            var code = new SourceCode();
            code.BracketStart("struct {0}", TypeName);
            code.IndentRight();
            foreach (var field in Fields.Values)
                code.Append(field.GenerateModelCode());
            if (HasContent)
                code.Append("std::string {0};", IsNameContent ? "name" : "content");

            if (NodeCount > 1)
            {
                code.NewLine();
                code.Append("~{0}()", TypeName);
                code.BracketStart();
                foreach (var field in DataClassFields)
                {
                    if (field.Class.HasId) code.Append("CLEAR_POINTER_MAP({0}, {1});", field.Class.CollectionTypeName, field.Class.CollectionName);
                    else code.Append("CLEAR_POINTER_LIST({0}, {1});", field.Class.TypeName, field.Class.CollectionName);
                }
                code.BracketEnd();
            }
            code.BracketEnd(";");
            return code;
        }

        public string GenerateParsePrototypeCode()
        {
            var declaration = string.Format("static void parse_{0}(TiXmlElement* root_node", ParserName);
            if (IsTopMost) declaration += ")";
            else declaration += string.Format(", {0}* parent)", ParentClassType);
            return declaration;
        }

        public SourceCode GenerateParseDeclCode()
        {
            return new SourceCode(GenerateParsePrototypeCode() + ";");
        }

        public SourceCode GenerateParseDeclsCode()
        {
            var code = new SourceCode();
            code.Append(GenerateParseDeclCode());
            foreach (var field in DataClassFields)
                code.Append(field.Class.GenerateParseDeclCode());
            return code;
        }

        public SourceCode GenerateParseCode()
        {
            var code = new SourceCode();
            code.Append(GenerateParsePrototypeCode());
            code.BracketStart();
            {
                code.Append("if (root_node == nullptr) return;");
                code.BracketStart("for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement())");
                {
                    code.Append("{0}* ptr = new {0};", QualifiedTypeName);

                    foreach (var field in Fields.Values)
                        code.Append(field.GenerateParseCode());

                    if (HasContent)
                        code.Append("ptr->{0} = std::string(each_node->GetText() != nullptr? each_node->GetText(): \"\");", IsNameContent ? "name" : "content");

                    code.NewLine();
                    if (IsTopMost)
                        code.Append("{0}_data::add(ptr);", SimpleName);
                    else
                    {
                        code.Append(HasId ? "parent->{0}.insert(std::make_pair(ptr->id, ptr));" : "parent->{0}.push_back(ptr);", CollectionName);
                    }

                    if (NeedLinker)
                        code.NewLine();

                    foreach (var field in ReferenceFields)
                        code.Append(field.GenerateParseRefCode());
                }
                code.BracketEnd();
            }
            code.BracketEnd();

            foreach (var field in DataClassFields)
                code.Append(field.GenerateParseBodyCode());
            return code;
        }

        public SourceCode GenerateDataCenterTypedefCode()
        {
            return new SourceCode().Append("typedef data_center<{0}> {1}_data;", TypeName, SimpleName);
        }

        public SourceCode GenerateDependencyIncludeCode()
        {
            var code = new SourceCode();
            foreach (var clazz in ReferencingChildClass)
                code.Append("#include \"{0}\"", clazz.HeaderFileName);
            return code;
        }

        public SourceCode GenerateDependMapCheckCode()
        {
            return new SourceCode().Append("data_depend_map::instance().is_depend(data::type_id::{0})", SimpleName);
        }

        public SourceCode GenerateReloaderImplCode()
        {
            var code = new SourceCode();
            code.Append("template <>");
            code.Append("inline void data_referer<{0}>::add()", TypeName);
            code.BracketStart();
            code.Append("data_depend_map::instance().add(data::type_id::{0});", SimpleName);
            code.BracketEnd();
            code.NewLine();

            code.Append("template <>");
            code.Append("inline void data_referer<{0}>::release()", TypeName);
            code.BracketStart();
            code.Append("data_depend_map::instance().release(data::type_id::{0});", SimpleName);
            code.BracketEnd();
            code.NewLine();

            code.Append("template <>");
            code.Append("inline bool data_reloader_impl<{0}>::is_reloadable()", TypeName);
            code.BracketStart();
            code.Append("return !({0});", string.Join(" \n    && ", from clazz in DataDepender.Instance[this] select clazz.GenerateDependMapCheckCode().Generate()));
            code.BracketEnd();
            code.NewLine();

            code.Append("template <>");
            code.Append("inline void data_reloader_impl<{0}>::reload()", TypeName);
            code.BracketStart();
            code.Append("data_center<{0}>::clear();", TypeName);
            code.Append("data_center<{0}>::load();", TypeName);
            code.Append("data_center<{0}>::link();", TypeName);
            code.BracketEnd();
            return code;
        }

        #endregion

        public string XmlFileName { get; set; }
        public string XmlRootName { get; set; }
        public DataField OwnedField { get; set; }
        public string Name { get; set; }

        public Dictionary<string, DataField> Fields { get; private set; }
        public bool HasContent { get; set; }
        public int NodeCount { get; set; }
    }
}