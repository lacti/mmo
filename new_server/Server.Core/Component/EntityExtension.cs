using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace Server.Core.Component
{
    public static class EntityExtension
    {
        public static void Gather<T>(this IEnumerable<Entity> entities, Action<List<Entity>> callback) where T : class, IAttribute
        {
            var result = new List<Entity>();
            PostEach<T>(entities, each => result.Add(each), () => callback(result));
        }

        public static void PostEach<T>(this IEnumerable<Entity> entities, Action<Entity> callback) where T : class, IAttribute
        {
            PostEach<T>(entities, callback, null);
        }

        public static void PostEach<T>(this IEnumerable<Entity> entities, Action<Entity> iteratingCallback, Action finalCallback) where T : class, IAttribute
        {
            PostEach(entities, typeof (T), iteratingCallback, finalCallback);
        }

        #region Reflection Helper

        public static void Gather(this IEnumerable<Entity> entities, Type attributeType, Action<List<Entity>> callback)
        {
            var result = new List<Entity>();
            PostEach(entities, attributeType, each => result.Add(each), () => callback(result));
        }

        public static void PostEach(this IEnumerable<Entity> entities, Action<Entity> callback)
        {
            PostEach(entities, null, callback, null);
        }

        public static void PostEach(this IEnumerable<Entity> entities, Type attributeType, Action<Entity> callback)
        {
            PostEach(entities, attributeType, callback, null);
        }

        public static void PostEach(this IEnumerable<Entity> entities, Type attributeType, Action<Entity> iteratingCallback, Action finalCallback)
        {
            Debug.Assert(iteratingCallback != null);

            // ReSharper disable PossibleMultipleEnumeration
            if (entities == null || !entities.Any())
            {
                if (finalCallback != null)
                    finalCallback();
                return;
            }

            var entity = entities.First();
            entity.Post(() =>
                {
                    if (attributeType != null && entity.Has(attributeType))
                        iteratingCallback(entity);
                    PostEach(entities.Skip(1), attributeType, iteratingCallback, finalCallback);
                });
            // ReSharper restore PossibleMultipleEnumeration
        }

        #endregion

    }
}