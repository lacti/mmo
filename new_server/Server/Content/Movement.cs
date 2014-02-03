using System;
using Server.Attribute;
using Server.Core.Command;
using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Extension;
using Server.Extension.Handler;
using Server.Message.Game;
using Server.Subsystem;

namespace Server.Content
{
    public class Movement : IHandler
    {
        [MessageHandler]
        internal void OnMove(Entity entity, MessageSession session, MoveMsg msg)
        {
            entity.Assign(msg);
            entity.Get<Motion>().StartTime = DateTime.Now;
            PositionSystem.Global.Move(entity);

            entity.Broadcast<UpdatePositionMsg>(entity.Id);
        }

        [CommandHandler("pos", "위치를 변경합니다")]
        internal bool SetPos(Entity admin,
                             [CommandArgument("새로운 X 위치", 0)] double newX,
                             [CommandArgument("새로운 Y 위치", 0)] double newY)
        {
            if (!admin.Has<Pos>())
                return false;

            var msg = new UpdatePositionMsg {X = newX, Y = newY, InstanceMove = true};
            admin.Assign(msg);

            PositionSystem.Global.Move(admin);
            return true;
        }
    }
}