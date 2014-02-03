using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Server.Attribute;
using Server.Core.Component;
using Server.Core.Data;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Data;
using Server.Extension;
using Server.Extension.Handler;
using Server.Message.Game;
using Server.Subsystem;

namespace Server.Content
{
    public class Interaction : IHandler
    {
        [MessageHandler]
        internal void OnInteract(Entity entity, MessageSession session, InteractMsg msg)
        {
            if (entity.Has<Fightable>())
            {
                var template = DataCenter.Instance.GetDatum<InteractTemplate>(msg.InteractId);
                if (template == null)
                {
                    Logger.Write("Request a invalid interact[{0}]", msg.InteractId);
                    return;
                }

                EffectSystem.Affect(entity, template);
            }
            else
            {
                PositionSystem.Global.Interact(entity, targets =>
                    {
                        // default interaction - remove removables
                        foreach (var target in targets.Where(target => target.Has<Removable>() && target.Get<Removable>().Action))
                        {
                            // despawn target
                            PositionSystem.Global.Disappear(target);
                            EntityBinder.Instance.Remove(target);
                            EntityManager.Instance.Remove(target.Id);
                        }
                    });
            }

        }
    }
}