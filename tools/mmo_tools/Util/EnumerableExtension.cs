using System;
using System.Collections.Generic;
using System.Linq;

namespace MmoTools.Util
{
    public static class EnumerableExtension
    {
        public static void ForEach<T>(this IEnumerable<T> source, Action<T> action)
        {
            foreach (var each in source)
                action(each);
        }

        public static bool IsCastable<TSource>(this IEnumerable<TSource> source, Type expectType)
        {
            try
            {
                foreach (var each in source)
// ReSharper disable ReturnValueOfPureMethodIsNotUsed
                    Convert.ChangeType(each, expectType);
// ReSharper restore ReturnValueOfPureMethodIsNotUsed
                return true;
            }
// ReSharper disable EmptyGeneralCatchClause
            catch
// ReSharper restore EmptyGeneralCatchClause
            {
            }
            return false;
        }

        public static bool IsCastableTo(this IEnumerable<string> sources, Type expectType)
        {
            return sources.All(e => IsCastableTo(e, expectType));
        }

        public static bool IsCastableTo(this string source, Type exceptType)
        {
            if (exceptType == typeof (bool))
            {
                bool dummy;
                return bool.TryParse(source, out dummy);
            }
            if (exceptType == typeof (short))
            {
                short dummy;
                return short.TryParse(source, out dummy);
            }
            if (exceptType == typeof (int))
            {
                int dummy;
                return int.TryParse(source, out dummy);
            }
            if (exceptType == typeof (long))
            {
                long dummy;
                return long.TryParse(source, out dummy);
            }
            if (exceptType == typeof (float))
            {
                float dummy;
                return float.TryParse(source, out dummy);
            }
            if (exceptType == typeof (double))
            {
                double dummy;
                return double.TryParse(source, out dummy);
            }
            return exceptType == typeof (string);
        }

        public static bool Exists<TSource>(this IEnumerable<TSource> source, Func<TSource, bool> predicate)
        {
            return source.Count(predicate) > 0;
        }
    }
}