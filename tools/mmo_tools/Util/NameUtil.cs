using System.Linq;
using System.Text;

namespace MmoTools.Util
{
    public static class NameUtil
    {
        public static int CountTab(this string line)
        {
            if (line == null) return 0;
            return line.Count(e => e == '\t');
        }

        public static string ToFunctionName(this string source)
        {
            source = source.Replace('-', '_');
            while (source.Contains("__"))
                source = source.Replace("__", "_");
            source = source.Replace('-', '_');
            var isCamelcase = false;
            var firstUpperIndex = -1;
            for (var i = 0; i < source.Length; ++i)
            {
                if (char.IsUpper(source[i]))
                {
                    firstUpperIndex = i;
                    break;
                }
            }
            if (firstUpperIndex >= 0)
            {
                isCamelcase = source.Length > 1 && (firstUpperIndex > 0 || char.IsLower(source[firstUpperIndex + 1]));
            }
            if (!isCamelcase)
                source = source.ToLower();
            var builder = new StringBuilder();
            var toUpper = true;
            foreach (var ch in source)
            {
                if (toUpper)
                {
                    builder.Append(char.ToUpper(ch));
                    toUpper = false;
                }
                else if (ch == '-') toUpper = true;
                else builder.Append(ch);
            }
            return builder.ToString();
        }

        public static string ToVariableName(this string source)
        {
            var functionName = ToFunctionName(source);
            return char.ToLower(functionName[0]) + functionName.Substring(1);
        }

        public static string ToEnumName(this string source)
        {
            var functionName = ToFunctionName(source);
            var builder = new StringBuilder();
            var isFirst = true;
            foreach (var ch in functionName)
            {
                if (!isFirst && char.IsUpper(ch)) builder.Append('_');
                builder.Append(char.ToUpper(ch));
                isFirst = false;
            }
            return builder.ToString();
        }

        public static string Strip(this string line, params string[] removals)
        {
            foreach (var removal in removals)
                line = line.Replace(removal, "");
            return line;
        }

        public static string RemoveEnd(this string source, string end)
        {
            if (source == null)
                return null;

            if (source.EndsWith(end))
                return source.Substring(0, source.Length - end.Length);
            return source;
        }
    }
}