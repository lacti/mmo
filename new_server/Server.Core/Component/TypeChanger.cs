using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace Server.Core.Component
{
    public class TypeChanger
    {
        public interface IChangable
        {
            string ToString(object value);
            object ChangeType(object value, Type type);
        }

        private static readonly Dictionary<Type, IChangable> CustomChangableMap = new Dictionary<Type, IChangable>();

        public static void AddCustomChangable(Type targetType, IChangable changable)
        {
            CustomChangableMap.Add(targetType, changable);
        }

        private static readonly Type ListType = typeof (IList);

        internal static string ToString(object value)
        {
            if (value == null)
                return "";

            var valueType = value.GetType();
            IChangable changable;
            if (CustomChangableMap.TryGetValue(valueType, out changable))
                return changable.ToString(value);

            if (ListType.IsAssignableFrom(valueType))
                return ToListString(value);

            return Convert.ToString(value);
        }

        internal static string ToListString(object value)
        {
            return string.Join(",", ((IList) value).OfType<object>().Select(ToString));
        }

        internal static object ChangeType(object value, Type type)
        {
            if (value == null)
                return null;

            IChangable changable;
            if (CustomChangableMap.TryGetValue(type, out changable))
                return changable.ChangeType(value, type);

            if (ListType.IsAssignableFrom(type))
                return ChangeListType(value, type);

            return Convert.ChangeType(value, type);
        }

        internal static object ChangeListType(object value, Type listType)
        {
            var elementType = listType.GetGenericArguments()[0];
            var list = (IList)Activator.CreateInstance(listType);

            var stringValue = Convert.ToString(value);
            if (string.IsNullOrEmpty(stringValue))
                return list;

            foreach (var each in stringValue.Split(',').Select(e => ChangeType(e, elementType)))
                list.Add(each);
            return list;
        }
    }
}
