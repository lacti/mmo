using System.Collections.Generic;
using System.Linq;
using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal class ListField : Field
    {
        public ListField()
        {
            Fields = new List<Field>();
        }

        public List<Field> Fields { get; private set; }

        public string TypeName
        {
            get { return Name + "_list"; }
        }

        public string CanonicalType
        {
            get { return CodeUtil.GetCanonicalName(Message.CanonicalName, TypeName); }
        }

        public string VarName
        {
            get { return Name + "s"; }
        }

        private string GetVarAccessor(CodeContext context)
        {
            return context.Accessor + VarName + ".";
        }

        public override SourceCode GenerateWriter(CodeContext context)
        {
            var code = new SourceCode();
            var listAccessor = GetVarAccessor(context);
            code.Append("(*this) << static_cast<uint32_t>({0}size());", listAccessor);

            var iteratorName = context.StartIterator();
            code.Append(CodeUtil.ForEachCode(CanonicalType, listAccessor, iteratorName, true));
            code.IndentRight();
            Fields.ForEach(field => code.Append(field.GenerateWriter(context)));
            code.BracketEnd();
            context.EndIterator();
            return code;
        }

        public override SourceCode GenerateReader(CodeContext context)
        {
            var code = new SourceCode();
            var countVar = Name + "_count";
            code.Append("uint32_t {0};", countVar);
            code.Append("(*this) >> {0};", countVar);

            code.Append("for (uint32_t index = 0; index < {0}; ++index) {{", countVar);
            code.IndentRight();

            var listAccessor = GetVarAccessor(context);
            var eachVariable = context.StartAccessor("each");
            code.Append("{0} {1};", CanonicalName, eachVariable);
            Fields.ForEach(field => code.Append(field.GenerateReader(context)));
            code.Append("{0}push_back({1});", listAccessor, eachVariable);
            code.BracketEnd();
            context.EndAccessor();
            return code;
        }

        public override SourceCode GenerateHandler(CodeContext context)
        {
            var code = new SourceCode();
            if (Fields.Count(field => field is RefField) == 0)
                return code;

            var listAccessor = GetVarAccessor(context);
            var iteratorName = context.StartIterator();
            code.Append(CodeUtil.ForEachCode(TypeName, listAccessor, iteratorName, true));
            code.IndentRight();
            Fields.ForEach(field => code.Append(field.GenerateHandler(context)));
            code.BracketEnd();
            context.EndIterator();
            return code;
        }

        public override SourceCode GenerateField(CodeContext context)
        {
            var code = new SourceCode();
            code.Append("struct {0} {{", Name);
            code.IndentRight();
            Fields.ForEach(field => code.Append(field.GenerateField(context)));
            code.NewLine();
            code.Append(Fields.GenerateDefaultConstructor(Name));
            code.BracketEnd(";");
            code.Append("typedef std::vector<{0}> {1};", Name, TypeName);
            code.Append("{0} {1};", TypeName, VarName);
            return code;
        }
    }
}