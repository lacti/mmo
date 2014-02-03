using System.Linq;
using System.Net.Sockets;
using Server.Attribute;
using Server.Core.Command;
using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Data;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Data;
using Server.Extension;
using Server.Extension.Handler;
using Server.Message.Game;
using System;
using Server.Subsystem;

namespace Server.Content
{
    public class EnterLeave : IHandler
    {
        private readonly object _accountLock = new object();

        [MessageHandler]
        internal void OnEnterWorld(MessageSession session, EnterWorldMsg msg)
        {
            // get or create entity
            Entity entity;
            lock (_accountLock)
            {
                if (NameCache.Instance.Contains(msg.Name))
                    entity = EntityManager.Instance[NameCache.Instance[msg.Name]];
                else
                {
                    entity = EntityManager.Instance.Create(EntityTemplate.Ids.User);
                    entity.Get<Nameplate>().Name = msg.Name;

                    if (!EntityBinder.Instance.Add(entity))
                    {
                        Logger.Write("Cannot insert entity[{0}]", msg.Name);
                        return;
                    }

                    NameCache.Instance.Add(msg.Name, entity.Id);
                }

                // ignore duplicately send enter-world msg
                if (entity.Id == session.SessionId)
                    return;

                // prevent repeatly login
                if (entity.Get<Player>().Logged)
                {
                    Logger.Write("{0} is already logged.", msg.Name);
                    session.Shutdown(SocketShutdown.Both);
                    return;
                }

                entity.Get<Player>().Logged = true;
            }

            entity.Ensure<Tickable>();
            entity.Get<Tickable>().Interval = DataCenter.Instance.GetAttributeValue<int>(typeof(Tickable), "interval");

            session.SessionId = entity.Id;
            session.Send(new WorldInfoMsg { Id = entity.Id, WorldId = 1 });
            session.Send(entity.ToMessage<SpawnMsg>());

            PositionSystem.Global.Appear(entity);

            Logger.Write("{0} is logged.", msg.Name);
        }

        [MessageHandler]
        internal void OnResetPeer(Entity entity, MessageSession session, ResetPeerMessage msg)
        {
            PositionSystem.Global.Disappear(entity);

            if (entity.Has<Player>())
                entity.Get<Player>().Logged = false;

            if (entity.Has<Tickable>())
                entity.Detach<Tickable>();

            EntityBinder.Instance.Update(entity);

            if (entity.Has<Nameplate>())
                Logger.Write("{0} is logout.", entity.Get<Nameplate>().Name);
        }

        [CommandHandler("npc", "새로운 Npc를 생성합니다")]
        internal bool SpawnNpc(Entity admin,
                               [CommandArgument("Npc의 이름")] string npcName,
                               [CommandArgument("Npc의 X 위치", 0)] double newX,
                               [CommandArgument("Npc의 Y 위치", 0)] double newY)
        {
            if (!admin.Has<Pos>())
                return false;

            var npc = EntityManager.Instance.Create(EntityTemplate.Ids.Npc);
            npc.Get<Motion>().Dir = admin.Get<Motion>().Dir;
            npc.Get<Nameplate>().Name = npcName;
            npc.Get<Pos>().Assign(new Pos {X = newX, Y = newY});

            PositionSystem.Global.Appear(npc);
            EntityBinder.Instance.Add(npc);
            return true;
        }

        [CommandHandler("res", "지정한 Entity의 Resource를 변경합니다")]
        internal bool ChangeResource(Entity admin,
                                     [CommandArgument("Resource를 변경할 대상")] Entity target,
                                     [CommandArgument("새로운 Resource Id")] int newResourceId)
        {
            ChangeResourceAndBroadcast(target, newResourceId);
            return true;
        }

        [CommandHandler("change_me", "자신의 Resource를 변경합니다")]
        internal bool ChangeMe(Entity admin,
                               [CommandArgument("새로운 Resource Id")] int newResourceId)
        {
            if (!admin.Has<Displayable>())
                return false;

            ChangeResourceAndBroadcast(admin, newResourceId);
            return true;
        }

        private static void ChangeResourceAndBroadcast(Entity target, int newResourceId)
        {
            target.Get<Displayable>().ResourceId = newResourceId;
            EntityBinder.Instance.Update(target);

            target.Broadcast<CharacterResourceMsg>();
        }

        [CommandHandler("spawn", "현재 지점에 새로운 Entity를 생성합니다")]
        internal bool SpawnEntity(Entity admin,
                                  [CommandArgument("생성할 Entity의 TemplateId")] int templateId)
        {
            var entityTemplate = DataCenter.Instance.GetEntityTemplate(templateId);
            if (entityTemplate == null)
                return false;

            var entity = EntityManager.Instance.Create(DataCenter.Instance.GetCreateInfo(entityTemplate));
            if (entity.Has<Pos>() && admin.Has<Pos>())
            {
                entity.Get<Pos>().Assign(admin.Get<Pos>());
                PositionSystem.Global.Appear(entity);
            }

            EntityBinder.Instance.Add(entity);
            return true;
        }

        [CommandHandler("lachesis", "라.케.시.스.를 수행합니다.")]
        internal bool SimulateLachesis(Entity admin,
                                [CommandArgument("생성할 Factory의 개수")] int factoryCount)
        {
            if (!(5 <= factoryCount && factoryCount < 1000))
                return false;

            var rand = new Random(factoryCount);
            foreach (var groupId in Enumerable.Range(0, factoryCount))
            {
                var factory = EntityManager.Instance.Create(EntityTemplate.Ids.Factory);
                factory.Get<Nameplate>().Name = groupId.ToString();
                factory.Get<Pos>().Assign(new Pos { X = rand.Next(-50, 50), Y = rand.Next(-50, 50) });
                factory.Get<Displayable>().ResourceId = 6;

                PositionSystem.Global.Appear(factory);
                EntityBinder.Instance.Add(factory);
            }

            return true;
        }

        [CommandHandler("delachesis", "라.케.시.스.를 종료합니다.")]
        internal bool FinalizeLachesis(Entity admin)
        {
            for (int entityId = 0; entityId <= EntityManager.Instance.IdSerial; entityId++)
            {
                if (EntityManager.Instance.Contains(entityId) == false)
                    continue;
                
                var entity = EntityManager.Instance[entityId];
                if (entity.Has<EntityGenerate>() || entity.Has<Eatable>())
                {
                    // despawn target
                    PositionSystem.Global.Disappear(entity);
                    EntityBinder.Instance.Remove(entity);
                    EntityManager.Instance.Remove(entity.Id);
                }

            } // end for entityId

            return true;
        }

    }
}