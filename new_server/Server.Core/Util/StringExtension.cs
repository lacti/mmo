#region

using System.Linq;
using System.Text;

#endregion

namespace Server.Core.Util
{
    public static class StringExtension
    {
        public static int CountTab(this string line)
        {
            if (line == null) return 0;
            return line.Count(e => e == '\t');
        }

        public static string Strip(this string line, params string[] removals)
        {
            return removals.Aggregate(line, (current, removal) => current.Replace(removal, ""));
        }

        public static string ToCamelCase(this string source, bool upperFirst = true)
        {
            if (string.IsNullOrWhiteSpace(source))
                return source;

            var makeUpper = upperFirst;
            var builder = new StringBuilder();
            foreach (var ch in source.Replace('-', '_'))
            {
                if (ch == '_') makeUpper = true;
                else
                {
                    builder.Append(makeUpper ? char.ToUpper(ch) : ch);
                    makeUpper = false;
                }
            }
            return builder.ToString();
        }

        public static string ToUnderscored(this string source)
        {
            if (string.IsNullOrWhiteSpace(source))
                return source;

            var builder = new StringBuilder();
            foreach (var ch in source)
            {
                if (char.IsUpper(ch) && builder.Length > 0)
                    builder.Append('_');
                builder.Append(char.ToLower(ch));
            }
            return builder.ToString();
        }

        // ReSharper disable StringIndexOfIsCultureSpecific.1
        // ReSharper disable StringIndexOfIsCultureSpecific.2
        public static string Extract(this string source, string start, string end)
        {
            var startPos = source.IndexOf(start) + start.Length;
            var endPos = source.IndexOf(end, startPos);
            return source.Substring(startPos, endPos - startPos);
        }

        public static string Substring(this string source, string needle)
        {
            return source.Substring(source.IndexOf(needle)) + needle.Length;
        }

        // ReSharper restore StringIndexOfIsCultureSpecific.1
        // ReSharper restore StringIndexOfIsCultureSpecific.2
    }
}