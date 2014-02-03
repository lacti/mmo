using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Core.Component;

namespace Server.Actor.Controller
{
    public class BaseController
    {
        public BaseActor Actor { get; private set; }

        public BaseController(BaseActor actor)
        {
            Actor = actor;
            Actor.Attach(this);
        }

        public virtual IEnumerable<int> OnTick()
        {
            yield break;
        }

        public virtual void Write(Dictionary<string, object> objectMap)
        {
            var thisType = GetType();
            var thisMap =
                GetType()
                    .GetProperties()
                    .Where(
                        e =>
                        e.CanRead && e.CanWrite && e.GetCustomAttributes(typeof (VolatileAttribute), false).Length == 0)
                    .ToDictionary(property => property.Name, property => property.GetValue(this, null));
            objectMap.Add(thisType.Name, thisMap);
        }

        public virtual void Load(Dictionary<string, object> objectMap)
        {
            var thisType = GetType();
            object thisMapObj;
            if (!objectMap.TryGetValue(thisType.Name, out thisMapObj))
                return;
            
            var thisMap = (Dictionary<string, object>)thisMapObj;
            foreach (var property in thisType.GetProperties()
                                             .Where(
                                                 e =>
                                                 e.CanRead && e.CanWrite &&
                                                 e.GetCustomAttributes(typeof (VolatileAttribute), false).Length == 0 &&
                                                 thisMap.ContainsKey(e.Name)))
            {
                property.SetValue(this, thisMap[property.Name], null);
            }
        }
    }
}
