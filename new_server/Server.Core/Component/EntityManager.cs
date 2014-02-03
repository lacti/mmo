using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using Server.Core.Util;

namespace Server.Core.Component
{
    public class EntityManager
    {
        public static readonly EntityManager Instance = new EntityManager();

        private readonly ConcurrentDictionary<int, Entity> _entityMap = new ConcurrentDictionary<int, Entity>();
        private int _idSerial;

        private EntityManager()
        {
            _idSerial = 0;
        }

        public int IdSerial
        {
            get { return _idSerial; }
            private set { Interlocked.Exchange(ref _idSerial, value); }
        }

        public Entity this[int entityId]
        {
            get { return _entityMap[entityId]; }
        }

        public List<Entity> this[List<int> entityIds]
        {
            get { return new List<Entity>(from e in _entityMap where entityIds.Contains(e.Key) select e.Value); }
        }

        public void TrimIdSerial()
        {
            IdSerial = _entityMap.Keys.Max() + 1;
        }

        public Entity CreateEntity(bool add = true)
        {
            return CreateEntity(IssueEntityId(), add);
        }

        public Entity CreateEntity(int entityId, bool add = true)
        {
            var entity = new Entity {Id = entityId};
            if (add)
                Add(entity);

            return entity;
        }

        public int IssueEntityId()
        {
            return Interlocked.Increment(ref _idSerial);
        }

        public bool UpdateEntityId(int oldId, int newId)
        {
            if (oldId == newId)
                return true;

            if (!Contains(oldId) || Contains(newId))
                return false;

            var entity = this[oldId];
            lock (entity)
            {
                entity.Id = newId;
                if (!Add(entity))
                {
                    entity.Id = oldId;
                    return false;
                }
                Remove(oldId);
            }
            return true;
        }

        public bool Add(Entity entity)
        {
            const int retryCount = 1000;
            var tryCount = 0;

            while (!_entityMap.TryAdd(entity.Id, entity))
            {
                if (_entityMap.ContainsKey(entity.Id))
                    return false;

                if (++tryCount > retryCount)
                    return false;
            }
            return true;
        }

        public bool Remove(int entityId)
        {
            const int retryCount = 1000;
            var tryCount = 0;

            Entity removal;
            while (!_entityMap.TryRemove(entityId, out removal))
            {
                if (!_entityMap.ContainsKey(entityId))
                    return true;

                if (++tryCount > retryCount)
                    return false;
            }
            return true;
        }

        public bool Contains(int entityId)
        {
            return _entityMap.ContainsKey(entityId);
        }

        public Entity At(int entityId)
        {
            return Contains(entityId) ? this[entityId] : null;
        }

        #region Factory

        // SystemId는 0으로 고정한다.
        public static readonly int SystemId = 0;

        public Entity Create(CreateInfo createInfo)
        {
            return Create(CreateEntity(), createInfo);
        }

        public Entity Create(int entityId, CreateInfo createInfo)
        {
            return Create(CreateEntity(entityId), createInfo);
        }

        public Entity Create(Entity entity, CreateInfo createInfo)
        {
            foreach (var pair in createInfo)
            {
                var attributeName = pair.Key;
                var typeName = attributeName.ToUnderscored();
                Debug.Assert(AttributeHelper.TypeMap.ContainsKey(typeName));

                var attribute = entity.Attach(AttributeHelper.TypeMap[typeName]);
                if (pair.Value != null)
                {
                    foreach (var each in pair.Value)
                        attribute.SetValue(each.Key.ToCamelCase(), each.Value);
                }
            }
            return entity;
        }

        public class CreateInfo : Dictionary<string, Dictionary<string, string>>
        {
        }

        #endregion
    }
}