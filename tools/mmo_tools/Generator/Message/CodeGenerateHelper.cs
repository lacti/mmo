using System.Collections.Generic;
using System.Linq;
using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal static class CodeGenerateHelper
    {
        public static SourceCode GenerateDefaultConstructor(this IEnumerable<Field> fields, string className)
        {
            // msg default constructor
            var primitiveArgs = fields.OfType<SimpleField>().Select(
                e => string.Format("{0}({1})", e.Name, TypeUtil.ToDefaultValueInInitializer(e.Type))).ToArray();

            var code = new SourceCode();
            if (primitiveArgs.Any())
            {
                code.Append("#ifdef _DEBUG");
                code.Append("{0}()", className);
                code.IndentRight();
                code.Append(": {0} {{}}", string.Join(", ", primitiveArgs));
                code.IndentLeft();
                code.Append("#else");
            }
            code.Append("{0}() {{}}", className);
            if (primitiveArgs.Any())
                code.Append("#endif");
            return code;
        }
    }
}