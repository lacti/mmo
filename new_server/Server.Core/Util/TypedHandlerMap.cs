using System;
using System.Threading;

namespace Server.Core.Util
{
    public class TypedHandlerMap<THandler>
    {
        private readonly MultiDictionary<int, THandler> _handlerMap = new MultiDictionary<int, THandler>();
        private readonly ReaderWriterLockSlim _lock = new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion);

        public void AddHandler(int typeId, THandler handler)
        {
            _lock.DoWrite(() => _handlerMap.Add(typeId, handler));
        }

        public bool ExecuteHandler(int typeId, Action<THandler> action)
        {
            return _lock.DoRead(() =>
                {
                    if (!_handlerMap.ContainsKey(typeId))
                        return false;

                    _handlerMap[typeId].ForEach(action);
                    return true;
                });
        }
    }
}