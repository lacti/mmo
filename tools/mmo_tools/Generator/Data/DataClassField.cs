using MmoTools.Util;

namespace MmoTools.Generator.Data
{
    internal class DataClassField : DataField
    {
        public DataClass Class { get; set; }

        #region Code Generator

        public override SourceCode GenerateModelCode()
        {
            var code = new SourceCode();
            code.Append(Class.GenerateModelCode());
            if (Class.NodeCount > 1)
            {
                code.Append(Class.HasId ? "typedef std::map<data::id_t, {0}*> {1};" : "typedef std::vector<{0}*> {1};", Class.TypeName, Class.CollectionTypeName);
                code.Append("{0} {1};", Class.CollectionTypeName, Class.CollectionName);
            }
            else
                code.Append("{0} {1};", Class.TypeName, Class.CanonicalName);
            return code;
        }

        public override SourceCode GenerateParseCode()
        {
            var code = new SourceCode();
            code.Append("parse_{0}(each_node->FirstChildElement(), ptr);", Class.ParserName);
            return code;
        }

        public SourceCode GenerateParseBodyCode()
        {
            return Class.GenerateParseCode();
        }

        #endregion
    }
}