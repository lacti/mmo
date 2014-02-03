using System;
using System.Diagnostics;
using MongoDB.Bson;
using MongoDB.Driver;
using Server.Core.Util;

namespace Server.Core.Component
{
    public class MongoBind : IEntityBind, IDisposable
    {
        private const string EntityIdName = "entity_id";

        public MongoBind(string host, int port, string database, string collection)
        {
            var builder = new MongoConnectionStringBuilder {Server = new MongoServerAddress(host, port)};
            Client = new MongoClient(builder.ConnectionString);
            Database = Client.GetServer().GetDatabase(database);
            Collection = Database.GetCollection(collection);
        }

        public MongoClient Client { get; private set; }
        public MongoDatabase Database { get; private set; }
        public MongoCollection Collection { get; private set; }

        public void Dispose()
        {
            Client.GetServer().Disconnect();
        }

        public bool Add(Entity entity)
        {
            var entityDoc = SerializeEntity(entity);

            var result = Collection.Insert(entityDoc);
            return result.Ok;
        }

        public bool Update(Entity entity)
        {
            var queryDoc = new QueryDocument {{EntityIdName, entity.Id}};
            var updateDoc = new UpdateDocument {{"$set", SerializeEntity(entity)}};

            var result = Collection.Update(queryDoc, updateDoc);
            return result.Ok;
        }

        public bool Remove(Entity entity)
        {
            var queryDoc = new QueryDocument {{EntityIdName, entity.Id}};

            var result = Collection.Remove(queryDoc);
            return result.Ok;
        }

        public bool ForEach(Action<Entity> action)
        {
            foreach (var eachDoc in Collection.FindAllAs<BsonDocument>())
            {
                if (!eachDoc.Contains(EntityIdName))
                    continue;

                var entity = DeserializeEntity(eachDoc);
                action(entity);
            }
            return true;
        }

        public void Clear()
        {
            Collection.RemoveAll();
        }

        private static BsonDocument SerializeEntity(Entity entity)
        {
            var entityDoc = new BsonDocument {{EntityIdName, entity.Id}};
            foreach (var attributePair in entity.Attributes)
            {
                var attributeType = attributePair.Key;

                // skip when volatile class
                var classVolatiles = attributeType.GetCustomAttributes(typeof (VolatileAttribute), false);
                if (classVolatiles.Length > 0)
                    continue;

                var attribute = attributePair.Value;

                var attributeDoc = new BsonDocument();
                foreach (var eachProperty in attributeType.GetProperties())
                {
                    // skip when volatile property
                    var propertyVolatiles = eachProperty.GetCustomAttributes(typeof (VolatileAttribute), false);
                    if (propertyVolatiles.Length > 0)
                        continue;

                    var eachValue = eachProperty.GetValue(attribute);
                    attributeDoc.Add(eachProperty.Name.ToUnderscored(), TypeChanger.ToString(eachValue));
                }
                entityDoc.Add(attributeType.Name.ToUnderscored(), attributeDoc);
            }
            return entityDoc;
        }

        private static Entity DeserializeEntity(BsonDocument entityDoc)
        {
            var entityId = entityDoc[EntityIdName].AsInt32;
            var entity = new Entity {Id = entityId};
            foreach (var each in entityDoc)
            {
                if (!AttributeHelper.TypeMap.ContainsKey(each.Name))
                    continue;

                Debug.Assert(each.Value.IsBsonDocument, "AttributeDoc should be BsonDocument");

                var attributeType = AttributeHelper.TypeMap[each.Name];
                var attribute = entity.Attach(attributeType);
                var attributeDoc = each.Value.AsBsonDocument;

                foreach (var eachProperty in attributeType.GetProperties())
                {
                    if (!eachProperty.CanWrite)
                        continue;

                    var propertyName = eachProperty.Name.ToUnderscored();
                    if (!attributeDoc.Contains(propertyName))
                        continue;

                    var propertyValue = attributeDoc[propertyName].AsString;
                    propertyValue = TryTypeCorrection(eachProperty.PropertyType, propertyValue);

                    eachProperty.SetValue(attribute, TypeChanger.ChangeType(propertyValue, eachProperty.PropertyType));
                }
            }
            return entity;
        }

        private static string TryTypeCorrection(Type propertyType, string propertyValue)
        {
            if (propertyType == typeof (bool))
            {
                if (propertyValue == "1") propertyValue = "true";
                else if (propertyValue == "0") propertyValue = "false";
            }
            return propertyValue;
        }
    }
}