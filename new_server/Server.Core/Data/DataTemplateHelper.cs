using System;
using System.Collections.Generic;
using System.Linq;

namespace Server.Core.Data
{
    public class DataTemplateHelper
    {
        public static string InferenceType(IEnumerable<string> values)
        {
            // ReSharper disable PossibleMultipleEnumeration
            if (IsCastable<bool>(values)) return "bool";
            if (IsCastable<int>(values)) return "int";
            if (IsCastable<double>(values)) return "double";
            if (IsStartsWith(values, "_")) return "enum";
            return "string";
            // ReSharper restore PossibleMultipleEnumeration
        }

        public static bool IsCastable<T>(IEnumerable<string> source)
        {
            // ReSharper disable ReturnValueOfPureMethodIsNotUsed
            try
            {
                foreach (var each in source)
                    Convert.ChangeType(each, typeof (T));
                return true;
            }
            catch (Exception)
            {
            }
            return false;
            // ReSharper restore ReturnValueOfPureMethodIsNotUsed
        }

        public static bool IsStartsWith(IEnumerable<string> source, string prefix)
        {
            return source.All(each => each.StartsWith(prefix));
        }
    }
}