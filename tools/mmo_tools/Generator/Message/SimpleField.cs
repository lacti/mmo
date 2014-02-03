using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal class SimpleField : Field
    {
        public TypeEnum Type { get; set; }
        public bool Optional { get; set; }
        public AttributeBind BindAttribute { get; set; }


        private string GetVarAccessor(CodeContext context)
        {
            return context.Accessor + Name;
        }

        public override SourceCode GenerateWriter(CodeContext context)
        {
            return new SourceCode().Append("(*this) << {0};", GetVarAccessor(context));
        }

        public override SourceCode GenerateReader(CodeContext context)
        {
            return new SourceCode().Append("(*this) >> {0};", GetVarAccessor(context));
        }

        public override SourceCode GenerateHandler(CodeContext context)
        {
            return new SourceCode();
        }

        public override SourceCode GenerateField(CodeContext context)
        {
            return new SourceCode().Append("{0} {1};", TypeUtil.ToDeclareTypeName(Type), Name);
        }
    }
}