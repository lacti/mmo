using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal abstract class Field
    {
        public Message Message { get; set; }
        public string Name { get; set; }

        public string CanonicalName
        {
            get { return CodeUtil.GetCanonicalName(Message.CanonicalName, Name); }
        }

        public abstract SourceCode GenerateWriter(CodeContext context);
        public abstract SourceCode GenerateReader(CodeContext context);
        public abstract SourceCode GenerateHandler(CodeContext context);
        public abstract SourceCode GenerateField(CodeContext context);
    }
}