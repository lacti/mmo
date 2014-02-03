using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal class RefField : Field
    {
        private Message _target;
        public string TargetName { get; set; }

        public Message Target
        {
            get { return _target ?? (_target = Message.Group[TargetName]); }
        }


        public string CanonicalTarget
        {
            get { return Target.CanonicalName; }
        }

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
            var code = new SourceCode();
            code.Append("this->skip(static_cast<msg_size_t>(sizeof(msg_type_t)));");
            code.Append("(*this) >> {0};", GetVarAccessor(context));
            return code;
        }

        public override SourceCode GenerateHandler(CodeContext context)
        {
            return new SourceCode().Append("{0}::handle(session, {1});", CanonicalTarget, GetVarAccessor(context));
        }

        public override SourceCode GenerateField(CodeContext context)
        {
            return new SourceCode().Append("{0} {1};", CanonicalTarget, Name);
        }
    }
}