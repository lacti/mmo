using System.Collections.Generic;
using System.Globalization;

namespace MmoTools.Util
{
    public class AccessorStack
    {
        private readonly Dictionary<string, int> _nameCounter = new Dictionary<string, int>();
        private readonly Stack<Accessor> _stack = new Stack<Accessor>();

        public string Accessor
        {
            get { return _stack.Peek().ToString(); }
        }

        private string IncreaseNameCount(string name)
        {
            if (_nameCounter.ContainsKey(name))
            {
                var count = _nameCounter[name];
                _nameCounter[name] = ++count;

                return name + (count == 1 ? "" : count.ToString(CultureInfo.InvariantCulture));
            }
            _nameCounter.Add(name, 0);
            return name;
        }

        private void DecreaseNameCount(string varName)
        {
            var lastPos = varName.Length - 1;
            for (; lastPos > 0; --lastPos)
            {
                int dummy;
                if (!int.TryParse(varName.Substring(lastPos), out dummy))
                    break;
            }
            var name = varName.Substring(0, lastPos + 1);
            if (_nameCounter.ContainsKey(name))
            {
                var count = _nameCounter[name];
                --count;
                if (count <= 0)
                    _nameCounter.Remove(name);
                else _nameCounter[name] = count;
            }
        }

        public string StartAccessor(string varName, bool isPointer = false)
        {
            var newName = IncreaseNameCount(varName);
            _stack.Push(new Accessor {Name = newName, Pointer = isPointer});
            return newName;
        }

        public void EndAccessor()
        {
            DecreaseNameCount(_stack.Pop().Name);
        }
    }
}