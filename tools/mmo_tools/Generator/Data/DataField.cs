using MmoTools.Util;

namespace MmoTools.Generator.Data
{
    internal abstract class DataField
    {
        public DataClass OwnedClass { get; set; }
        public string Name { get; set; }

        public string CanonicalName
        {
            get { return Name.ToEnumName().ToLower().RemoveEnd("_ref"); }
        }

        public abstract SourceCode GenerateModelCode();
        public abstract SourceCode GenerateParseCode();
    }
}