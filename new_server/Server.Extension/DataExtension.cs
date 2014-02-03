using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Server.Core.Component;
using Server.Core.Data;
using Server.Data;

namespace Server.Extension
{
    public static class DataExtension
    {
        #region DataCenter

        public static EntityTemplate GetEntityTemplate(this DataCenter dataCenter, int id)
        {
            return dataCenter[typeof (EntityTemplate), id] as EntityTemplate;
        }

        public static EntityTemplate GetEntityTemplate(this DataCenter dataCenter, EntityTemplate.Ids id)
        {
            return GetEntityTemplate(dataCenter, (int) id);
        }

        public static EntityManager.CreateInfo GetCreateInfo(this DataCenter dataCenter, int id)
        {
            return GetCreateInfo(dataCenter, GetEntityTemplate(dataCenter, id));
        }

        public static EntityManager.CreateInfo GetCreateInfo(this DataCenter dataCenter, EntityTemplate.Ids id)
        {
            return GetCreateInfo(dataCenter, GetEntityTemplate(dataCenter, id));
        }

        public static EntityManager.CreateInfo GetCreateInfo(this DataCenter dataCenter, EntityTemplate entityTemplate)
        {
            Debug.Assert(entityTemplate != null);

            var result = new EntityManager.CreateInfo();
            foreach (var attribute in entityTemplate.Attributes)
                result.Add(attribute.Name, null);

            foreach (var attributeTemplate in dataCenter.GetData<AttributeTemplate>(e => result.ContainsKey(e.Name)))
                result[attributeTemplate.Name] = attributeTemplate.ToDictionary();

            return result;
        }

        public static AttributeTemplate GetAttributeTemplate(this DataCenter dataCenter, Type type)
        {
            return dataCenter.GetDatum<AttributeTemplate>(e => e.Name.Equals(type.Name, StringComparison.OrdinalIgnoreCase));
        }

        public static T GetAttributeValue<T>(this DataCenter dataCenter, Type type, string fieldName)
        {
            var template = dataCenter.GetDatum<AttributeTemplate>(e => e.Name.Equals(type.Name, StringComparison.OrdinalIgnoreCase));
            return GetValue<T>(template, fieldName);
        }

        #endregion

        #region AttributeTemplate

        public static Dictionary<string, string> ToDictionary(this AttributeTemplate template)
        {
            return template.Fields.ToDictionary(e => e.Name, e => e.Value);
        }

        public static T GetValue<T>(this AttributeTemplate attributeTemplate, string fieldName)
        {
            if (attributeTemplate == null)
                return default(T);

            var field = attributeTemplate.Fields.SingleOrDefault(e => e.Name.Equals(fieldName, StringComparison.OrdinalIgnoreCase));
            if (field == null)
                return default(T);

            return (T) Convert.ChangeType(field.Value, typeof (T));
        }

        #endregion
    }
}