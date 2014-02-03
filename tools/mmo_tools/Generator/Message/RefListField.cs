using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal class RefListField : RefField
    {
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
            var iteratorName = context.StartIterator();
            code.Append("(*this) << static_cast<uint32_t>({0}size());", listAccessor);
            code.Append(CodeUtil.ForEachCode(CanonicalType, listAccessor, iteratorName, true));
            code.IndentRight();
            code.Append("(*this) << (*{0});", iteratorName);
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

            var listAccessor = GetVarAccessor(context);
            code.Append("for (uint32_t index = 0; index < {0}; ++index) {{", countVar);
            code.IndentRight();
            var eachVariable = context.StartAccessor("each");
            code.Append("{0} {1};", CanonicalTarget, eachVariable);
            code.Append("this->skip(static_cast<msg_size_t>(sizeof(msg_type_t)));");
            code.Append("(*this) >> {0};", eachVariable);
            code.Append("{0}push_back({1});", listAccessor, eachVariable);
            context.EndAccessor();
            code.BracketEnd();
            return code;
        }

        public override SourceCode GenerateHandler(CodeContext context)
        {
            var code = new SourceCode();
            var listAccessor = GetVarAccessor(context);
            var iteratorName = context.StartIterator();
            code.Append(CodeUtil.ForEachCode(TypeName, listAccessor, iteratorName, true));
            code.IndentRight();
            code.Append("{0}::handle(session, *{1});", CanonicalTarget, iteratorName);
            code.BracketEnd();
            context.EndIterator();
            return code;
        }

        public override SourceCode GenerateField(CodeContext context)
        {
            var code = new SourceCode();
            code.Append("typedef std::vector<{0}> {1};", CanonicalTarget, TypeName);
            code.Append("{0} {1};", TypeName, VarName);
            return code;
        }
    }
}