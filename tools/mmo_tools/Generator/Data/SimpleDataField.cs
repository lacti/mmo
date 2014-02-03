using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using MmoTools.Util;

namespace MmoTools.Generator.Data
{
    internal class SimpleDataField : DataField
    {
        public HashSet<string> Values = new HashSet<string>();


        public void AddValue(string value)
        {
            if (!Values.Contains(value))
                Values.Add(value);
        }

        #region Names

        public string ReferenceTypeName
        {
            get { return ReferenceDataClass.TypeName; }
        }

        public string EnumTypeName
        {
            get { return Name + "_t"; }
        }

        public string TypeString
        {
            get
            {
                switch (ValueType)
                {
                    case TypeEnum.DATA_REFERENCE:
                        return string.Format("const {0}*", ReferenceTypeName);

                    case TypeEnum.DATA_ENUM:
                        return EnumTypeName;
                }
                return TypeUtil.ToDeclareTypeName(ValueType);
            }
        }

        #endregion

        #region Type Properties

        public bool IsId
        {
            get { return ValueType == TypeEnum.DATA_ID; }
        }

        public bool IsReferenceType
        {
            get { return ValueType == TypeEnum.DATA_REFERENCE; }
        }

        public TypeEnum ValueType { get; private set; }

        public DataClass ReferenceDataClass
        {
            get
            {
                var refName = Name.RemoveEnd("-ref");
                if (DataCenter.Instance.ContainsKey(refName))
                    return DataCenter.Instance[refName];
                throw new InvalidDataException(refName + " doesn't exist");
            }
        }

        public void InferenceValueType()
        {
            if (Values.All(each => each.StartsWith("_")))
                ValueType = TypeEnum.DATA_ENUM;

            else if (Name.EndsWith("-ref", StringComparison.OrdinalIgnoreCase))
                ValueType = TypeEnum.DATA_REFERENCE;

            else if (Values.IsCastableTo(typeof (bool)))
                ValueType = TypeEnum.BOOL;

            else if (Name.Equals("id", StringComparison.OrdinalIgnoreCase))
                ValueType = TypeEnum.DATA_ID;

            else if (Values.IsCastableTo(typeof (int)))
                ValueType = TypeEnum.INT;

            else if (Values.IsCastableTo(typeof (double)))
                ValueType = TypeEnum.DOUBLE;

            else if (!Values.Exists(e => !e.Split(',').IsCastableTo(typeof (double))))
                ValueType = TypeEnum.DATA_POS;

            else if (Values.Exists(e => e.Exists(ch => "+*/()".Contains(ch))))
                ValueType = TypeEnum.DATA_EXPRESSION;

            else ValueType = TypeEnum.STRING;
        }

        #endregion

        #region Code Generator

        public override SourceCode GenerateModelCode()
        {
            var code = new SourceCode();
            if (ValueType == TypeEnum.DATA_ENUM)
            {
                code.BracketStart("enum {0}", EnumTypeName);
                foreach (var value in Values.Distinct())
                    code.Append("{0},", value.Substring(1).ToEnumName()); // remove leading _ char
                code.BracketEnd(";");
            }
            code.Append("{0} {1};", TypeString, CanonicalName);
            return code;
        }

        public override SourceCode GenerateParseCode()
        {
            var code = new SourceCode();
            switch (ValueType)
            {
                case TypeEnum.BOOL:
                    code.BracketStart();
                    code.Append("const char* _temp = each_node->Attribute(\"{0}\");", CanonicalName);
                    code.Append("ptr->{0} = _temp == nullptr? false: boost::lexical_cast<bool>(_temp);", CanonicalName);
                    code.BracketEnd();
                    break;
                case TypeEnum.DATA_ENUM:
                    code.BracketStart();
                    code.Append("const char* _temp = each_node->Attribute(\"{0}\");", CanonicalName);
                    foreach (var value in Values.Distinct())
                        code.Append("if (stricmp(_temp, \"{2}\") == 0) ptr->{0} = {1}::{2};", CanonicalName, OwnedClass.QualifiedTypeName, value.Substring(1).ToEnumName());
                    // remove leading _ char
                    code.BracketEnd();
                    break;
                case TypeEnum.DATA_ID:
                case TypeEnum.INT:
                case TypeEnum.DOUBLE:
                    code.Append("each_node->Attribute(\"{0}\", &ptr->{0});", CanonicalName);
                    break;
                case TypeEnum.DATA_POS:
                    code.Append("parse_data_xyz(each_node->Attribute(\"{0}\"), &ptr->{0});", CanonicalName);
                    break;
                case TypeEnum.STRING:
                    code.Append("ptr->{0} = std::string(each_node->Attribute(\"{0}\"));", CanonicalName);
                    break;
                case TypeEnum.DATA_REFERENCE:
                    code.Append("ptr->{0} = nullptr;", CanonicalName);
                    break;
                case TypeEnum.DATA_EXPRESSION:
                    code.Append("ptr->{0} = expression_parser_t(each_node->Attribute(\"{0}\")).result();", CanonicalName);
                    break;
            }
            return code;
        }

        public SourceCode GenerateParseRefCode()
        {
            if (ValueType != TypeEnum.DATA_REFERENCE)
                return new SourceCode();

            var code = new SourceCode();

            var nodeName = Name.Replace('_', '-');
            var varName = nodeName.Replace('-', '_');
            code.Append("id_t {0};", varName);
            code.Append("each_node->Attribute(\"{0}\", &{1});", nodeName, varName);
            code.Append("{0}_data::linker.add<{1}>(&ptr->{2}, {3});", OwnedClass.Ancestor.SimpleName, ReferenceTypeName, CanonicalName, varName);
            return code;
        }

        #endregion
    }
}