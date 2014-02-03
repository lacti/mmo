namespace MmoTools.Util
{
    public class CodeUtil
    {
        public static string GetCanonicalName(params string[] names)
        {
            return string.Join("::", names);
        }

        public static string ForEachCode(string typeName, string varAccessor, string iteratorName, bool isConst)
        {
            return string.Format("for ({0}::{3} {2} = {1}begin(); {2} != {1}end(); ++{2}) {{",
                                 typeName, varAccessor, iteratorName, isConst ? "const_iterator" : "iterator");
        }
    }
}