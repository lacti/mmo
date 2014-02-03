using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Core.Component;
using Server.Core.Data;
using Server.Core.Messaging;
using Server.Data;

namespace Server.Extension
{
    public static class EntityExtension
    {
        public static MessageSession GetSession(this Entity entity)
        {
            return entity == null ? null : MessageSessionManager.Instance.At(entity.Id);
        }

        public static Entity Create(this EntityManager manager, EntityTemplate.Ids id)
        {
            return manager.Create(DataCenter.Instance.GetCreateInfo(id));
        }
    }
}
