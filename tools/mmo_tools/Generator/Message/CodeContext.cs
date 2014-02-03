using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal class CodeContext
    {
        private readonly AccessorStack _accessorStack = new AccessorStack();

        public string Accessor
        {
            get { return _accessorStack.Accessor; }
        }

        public string StartIterator()
        {
            return _accessorStack.StartAccessor("iter", true);
        }

        public void EndIterator()
        {
            _accessorStack.EndAccessor();
        }

        public string StartAccessor(string varName)
        {
            return _accessorStack.StartAccessor(varName);
        }

        public void EndAccessor()
        {
            _accessorStack.EndAccessor();
        }
    }
}