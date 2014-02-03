using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using Server.Core.Util;

namespace Server.Extension
{
    public class NameCache
    {
        public static readonly NameCache Instance = new NameCache();

        private readonly Dictionary<string, int> _cacheMap = new Dictionary<string, int>();
        private readonly ReaderWriterLockSlim _lock = new ReaderWriterLockSlim();

        private NameCache()
        {
        }

        public int this[string name]
        {
            get { return _lock.DoRead(() => _cacheMap[name]); }
        }

        public bool Add(string name, int entityId)
        {
            return _lock.DoWrite(() =>
                {
                    if (_cacheMap.ContainsKey(name))
                        return false;

                    _cacheMap.Add(name, entityId);
                    return true;
                });
        }

        public void Remove(string name)
        {
            _lock.DoWrite(() =>
                {
                    Debug.Assert(_cacheMap.ContainsKey(name));
                    _cacheMap.Remove(name);
                });
        }

        public bool Contains(string name)
        {
            return _lock.DoRead(() => _cacheMap.ContainsKey(name));
        }

        public int Count
        {
            get { return _lock.DoRead(() => _cacheMap.Count); }
        }
    }
}