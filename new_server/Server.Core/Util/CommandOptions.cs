using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public class CommandOptions
    {
        private Dictionary<string, string> _optionMap = new Dictionary<string, string>();

        public CommandOptions(string[] args)
        {
            for (int index = 0; index < args.Length; index += 2)
                _optionMap.Add(args[index], args[index + 1]);
        }

        public bool Has(string name)
        {
            return _optionMap.ContainsKey(name);
        }

        public T Get<T>(string name)
        {
            return Get<T>(name, default(T));
        }

        public T Get<T>(string name, T defaultValue)
        {
            return !Has(name) ? defaultValue : _optionMap[name].As<T>();
        }

        public void Override<T>(string name, ref T targetValue)
        {
            targetValue = Get(name, targetValue);
        }
    }
}
