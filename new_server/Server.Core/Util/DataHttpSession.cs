using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;

namespace Server.Core.Util
{
    public class DataHttpSession
    {
        public string SessionKey { get; private set; }
        private readonly Dictionary<string, object> _objectMap = new Dictionary<string, object>();

        internal DataHttpSession()
        {
            SessionKey = GenerateKey();
        }

        public object this[string key]
        {
            get { return _objectMap.ContainsKey(key) ? _objectMap[key] : null; }
            set
            {
                if (_objectMap.ContainsKey(key))
                    _objectMap.Remove(key);
                _objectMap.Add(key, value);
            }
        }

        public T GetObject<T>(string key) where T : class
        {
            return this[key] as T;
        }

        private static readonly Random KeyRandom = new Random();
        private static readonly MD5 Hasher = MD5.Create();

        public static string GenerateKey()
        {
            var key = KeyRandom.Next() ^ KeyRandom.Next() ^ KeyRandom.Next();
            var bytes = Hasher.ComputeHash(BitConverter.GetBytes(key));
            return string.Join("", bytes.Select(e => e.ToString("X2")));
        }
    }
}
