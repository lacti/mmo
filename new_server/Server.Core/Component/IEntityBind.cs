using System;

namespace Server.Core.Component
{
    public interface IEntityBind
    {
        bool Add(Entity entity);
        bool Update(Entity entity);
        bool Remove(Entity entity);
        bool ForEach(Action<Entity> action);
        void Clear();
    }
}