using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Server.Core.Util;

namespace Server.Core.Component
{
    public class EntityBinder : IEntityBind
    {
        public static readonly EntityBinder Instance = new EntityBinder();

        private readonly List<IEntityBind> _impls = new List<IEntityBind>();

        private EntityBinder()
        {
        }

        public bool Add(Entity entity)
        {
            return _impls.Aggregate(true, (current, each) => each.Add(entity) && current);
        }

        public bool Update(Entity entity)
        {
            return _impls.Aggregate(true, (current, each) => each.Update(entity) && current);
        }

        public bool Remove(Entity entity)
        {
            return _impls.Aggregate(true, (current, each) => each.Remove(entity) && current);
        }

        public bool ForEach(Action<Entity> action)
        {
            return _impls.Aggregate(true, (current, each) => each.ForEach(action) && current);
        }

        public void Clear()
        {
            _impls.ForEach(each => each.Clear());
        }

        public void AddBinder(IEntityBind binder)
        {
            _impls.Add(binder);
        }

        public void RemoveBinder(IEntityBind binder)
        {
            _impls.Remove(binder);
        }

        public T GetBinder<T>() where T : class, IEntityBind
        {
            return _impls.SingleOrDefault(e => e is T) as T;
        }

        public void Sync(string syncFromDatabaseType, string syncToDatabaseType)
        {
            var fromBindType = Type.GetType(syncFromDatabaseType, false);
            var toBindType = Type.GetType(syncToDatabaseType, false);

            Debug.Assert(fromBindType != null && _impls.Exists(e => e.GetType() == fromBindType));
            Debug.Assert(toBindType != null && _impls.Exists(e => e.GetType() == toBindType));

            var fromBind = _impls.Single(e => e.GetType() == fromBindType);
            var toBind = _impls.Single(e => e.GetType() == toBindType);

            try
            {
                toBind.Clear();
                fromBind.ForEach(e => toBind.Add(e));
            }
            catch (Exception e)
            {
                Logger.Write(e);
            }
        }
    }
}