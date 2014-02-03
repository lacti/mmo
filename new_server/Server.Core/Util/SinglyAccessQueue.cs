using System;
using System.Collections.Concurrent;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public class SinglyAccessQueue<T>
    {
        private readonly ExclusiveRun.Flag _flag = new ExclusiveRun.Flag();
        private Action<T> _action;
        private ConcurrentQueue<T> _queue = new ConcurrentQueue<T>();

        public SinglyAccessQueue(object owner)
            : this(owner, null)
        {
        }

        public SinglyAccessQueue(object owner, Action<T> action)
        {
            Owner = owner;
            _action = action;
        }

        public object Owner { get; private set; }

        public Action<T> Action
        {
            set { _action = value; }
        }

        public bool IsEmpty
        {
            get { return _queue.IsEmpty; }
        }

        public int Count
        {
            get { return _queue.Count; }
        }

        public void EnqueueOrProcess(T item)
        {
            _queue.Enqueue(item);

            // execute asynchronously
            ProcessItems();
        }

        private void ProcessItems()
        {
            try
            {
                do
                {
                    using (var exclusive = new ExclusiveRun(_flag))
                    {
                        if (!exclusive.IsAcquired)
                            return;

                        DequeueAndDoAction();
                    }
                } while (!_queue.IsEmpty);
            }
            catch (Exception e)
            {
                Logger.Write(e);
                Clear();
            }
        }

        private void DequeueAndDoAction()
        {
            T item;
            while (_queue.TryDequeue(out item))
            {
                try
                {
                    if (_action != null)
                        _action(item);
                }
                catch (Exception e)
                {
                    Logger.Write(e);
                }
            }
        }

        public void Clear()
        {
            _queue = new ConcurrentQueue<T>();
        }
    }
}