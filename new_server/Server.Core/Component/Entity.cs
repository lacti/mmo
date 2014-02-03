using System;
using System.Collections.Generic;
using System.Text;
using Server.Core.Component.Messaging;
using Server.Core.Util;
using System.Collections.Concurrent;

namespace Server.Core.Component
{
    public class Entity
    {
        public const int InvalidEntityId = -1;

        private readonly ConcurrentDictionary<Type, IAttribute> _attributeMap;
        private int _id;

        public Entity()
        {
            _attributeMap = new ConcurrentDictionary<Type, IAttribute>();
            _id = InvalidEntityId;
            _actionQueue = new SinglyAccessQueue<Action>(this, action => action());
        }

        public int Id
        {
            get { return _id; }
            set
            {
                if (_id == InvalidEntityId)
                    _id = value;
                else EntityManager.Instance.UpdateEntityId(_id, value);
            }
        }

        #region Typed Accessor

        public T Attach<T>() where T : class, IAttribute, new()
        {
            return Attach(typeof (T)) as T;
        }

        public bool Detach<T>() where T : class, IAttribute
        {
            return Detach(typeof (T));
        }

        public T Get<T>() where T : class, IAttribute
        {
            return Get(typeof (T)) as T;
        }

        public bool Has<T>() where T : class, IAttribute
        {
            return Has(typeof (T));
        }

        public void Ensure<T>() where T : class, IAttribute, new()
        {
            Ensure(typeof (T));
        }

        public IEnumerable<KeyValuePair<Type, IAttribute>> Attributes
        {
            get { return _attributeMap.ToArray();  }
        }

        #endregion

        #region Reflected Accessor

        public IAttribute Attach(Type type)
        {
            var attribute = Activator.CreateInstance(type) as IAttribute;
            if (_attributeMap.TryAdd(type, attribute))
                this.InvokeTrigger(TriggerType.Attach, type);
            else attribute = _attributeMap[type];

            return attribute;
        }

        public bool Detach(Type type)
        {
            IAttribute attribute;
            if (_attributeMap.TryRemove(type, out attribute))
            {
                this.InvokeTrigger(TriggerType.Detach, type);
                return true;
            }
            return false;
        }

        public bool Has(Type type)
        {
            return _attributeMap.ContainsKey(type);
        }

        public IAttribute Get(Type type)
        {
            return Has(type) ? _attributeMap[type] : null;
        }

        public void Ensure(Type type)
        {
            if (!Has(type))
                Attach(type);
        }

        #endregion

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.Append("- ").Append(GetType().Name).AppendLine();
            builder.Append("  - entity_id=").Append(Id).AppendLine();
            foreach (var attribute in _attributeMap.Values)
                builder.Append(AttributeHelper.ToString(attribute));
            return builder.ToString();
        }

        #region Post & Process

        private readonly SinglyAccessQueue<Action> _actionQueue;

        public void Post(Action action)
        {
            _actionQueue.EnqueueOrProcess(action);
        }

        public void PostDelayed(Action action, int delayedMilliSec)
        {
            Timer.Execute(() => Post(action), delayedMilliSec);
        }

        #endregion

    }
}