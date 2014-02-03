using System.Linq;

namespace MmoTools.Generator.Message
{
    internal class AttributeBind
    {
        public string AttributeName { get; set; }
        public string FieldName { get; set; }

        public static AttributeBind Parse(string value)
        {
            if (value == null || value.Count(e => e == '.') != 1)
                return null;
            var data = value.Trim().Split('.');
            return new AttributeBind {AttributeName = data[0], FieldName = data[1]};
        }
    }
}