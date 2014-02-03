using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public static class CastExtension
    {
        public static T As<T>(this object value)
        {
            if (value == null)
                return default(T);

            object result = Convert.ChangeType(value, typeof (T));
            Debug.Assert(result != null);

            return (T) result;
        }

        public static object As(this object value, Type type)
        {
            if (value == null)
                return type.IsValueType ? Activator.CreateInstance(type) : null;

            return Convert.ChangeType(value, type);
        }
    }
}
