using Server.Attribute;
using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Data;
using Server.Core.Messaging;
using Server.Data;
using Server.Extension.Handler;
using Server.Subsystem;

namespace Server.Content
{
    public class Tick : IHandler
    {
        [MessageHandler]
        internal void OnTick(TickMessage msg)
        {
        }

        [MessageHandler]
        internal void OnTick(Entity entity, TickMessage tick)
        {
            // 로그아웃 한 상태에서 Tickable이 남아있는 유저는 Tick을 수행하지 않도록 한다.
            if (entity.Has<Player>() && !entity.Get<Player>().Logged)
            {
                entity.Detach<Tickable>();
                return;
            }

            if (entity.Has<Fightable>())
            {
                var fightable = entity.Get<Fightable>();
                if (fightable.IsAlive)
                {
                    if (++fightable.RecoveryCurrentTick >= fightable.RecoveryTickCount)
                    {
                        var recovery = DataCenter.Instance.GetDatum<InteractTemplate>((int)InteractTemplate.Ids.Recovery);
                        EffectSystem.Affect(entity, recovery);

                        fightable.RecoveryCurrentTick = 0;
                    }
                }
            }

            if (entity.Has<Eatable>())
            {
            }

            if (entity.Has<EntityGenerate>())
            {
                var entityGenerate = entity.Get<EntityGenerate>();
                if (entityGenerate.IsGeneratable)
                {
                    if (entityGenerate.Resource >= entityGenerate.NeededResourceToGen)
                    {
                        // generate!
                        

                        entityGenerate.Resource -= entityGenerate.NeededResourceToGen;
                    }

                    entityGenerate.Resource += entityGenerate.IncResource;
                }
            }

            // 다음 Tick을 요청한다.
            if (entity.Has<Tickable>())
            {
                var nextMsg = new TickMessage {Id = entity.Id};
                nextMsg.ExecuteDelayed(entity, entity.Get<Tickable>().Interval);
            }
        }
    }
}