using System;
using System.Collections.Generic;
using System.Reflection;

namespace Server.Core.Util
{
    public class TypeMap<TKey> : Dictionary<TKey, Type>
    {
        private readonly Func<Type, TKey> _keyFinder;
        private readonly Func<TKey, bool> _predicate;

        public TypeMap(Assembly targetAssembly, Type interfaceType, Func<Type, TKey> keyFinder,
                       Func<TKey, bool> predicate = null)
        {
            TargetAssemblies = new[] {targetAssembly};
            InterfaceType = interfaceType;

            _keyFinder = keyFinder;
            _predicate = predicate;

            Initialize();
        }

        public TypeMap(Assembly[] targetAssemblies, Type interfaceType, Func<Type, TKey> keyFinder,
                       Func<TKey, bool> predicate = null)
        {
            TargetAssemblies = targetAssemblies;
            InterfaceType = interfaceType;

            _keyFinder = keyFinder;
            _predicate = predicate;

            Initialize();
        }

        public Assembly[] TargetAssemblies { get; private set; }
        public Type InterfaceType { get; private set; }

        private void Initialize()
        {
            foreach (var targetAssembly in TargetAssemblies)
            {
                foreach (var eachType in targetAssembly.GetTypes())
                {
                    if (!InterfaceType.IsAssignableFrom(eachType))
                        continue;

                    var typeId = _keyFinder(eachType);
                    if (_predicate != null && !_predicate(typeId))
                        continue;

                    Add(typeId, eachType);
                }
            }
        }
    }
}