/* this code is auto-generated. */

using System.Collections.Generic;
using Server.Attribute;
using Server.Core.Entity;
using Server.Message.Game;
using Server.Message.System;
using Server.Message.Server;
using Server.Message.Resource;

namespace Server.Subsystem
{
    public static class MessageEntityExtension
    {
        public static void Assign(this Entity entity, SpawnMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
            entity.Get<Nameplate>().Name = msg.Name;
            Assign(entity, msg.CharacterResource);
            Assign(entity, msg.UpdatePosition);
        }
        
        public static void Assign(this SpawnMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
            msg.Name = entity.Get<Nameplate>().Name;
            Assign(msg.CharacterResource, entity);
            Assign(msg.UpdatePosition, entity);
        }
        
        public static SpawnMsg ToSpawnMsg(this Entity entity)
        {
            var msg = new SpawnMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this SpawnMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, MoveMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
            entity.Get<Pos>().X = msg.X;
            entity.Get<Pos>().Y = msg.Y;
            entity.Get<Motion>().Dir = msg.Dir;
            entity.Get<Motion>().Speed = msg.Speed;
        }
        
        public static void Assign(this MoveMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
            msg.X = entity.Get<Pos>().X;
            msg.Y = entity.Get<Pos>().Y;
            msg.Dir = entity.Get<Motion>().Dir;
            msg.Speed = entity.Get<Motion>().Speed;
        }
        
        public static MoveMsg ToMoveMsg(this Entity entity)
        {
            var msg = new MoveMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this MoveMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, UpdatePositionMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
            entity.Get<Pos>().X = msg.X;
            entity.Get<Pos>().Y = msg.Y;
            entity.Get<Motion>().Dir = msg.Dir;
            entity.Get<Motion>().Dir = msg.Speed;
        }
        
        public static void Assign(this UpdatePositionMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
            msg.X = entity.Get<Pos>().X;
            msg.Y = entity.Get<Pos>().Y;
            msg.Dir = entity.Get<Motion>().Dir;
            msg.Speed = entity.Get<Motion>().Dir;
        }
        
        public static UpdatePositionMsg ToUpdatePositionMsg(this Entity entity)
        {
            var msg = new UpdatePositionMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this UpdatePositionMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, DespawnMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
        }
        
        public static void Assign(this DespawnMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
        }
        
        public static DespawnMsg ToDespawnMsg(this Entity entity)
        {
            var msg = new DespawnMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this DespawnMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, WorldInfoMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
        }
        
        public static void Assign(this WorldInfoMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
        }
        
        public static WorldInfoMsg ToWorldInfoMsg(this Entity entity)
        {
            var msg = new WorldInfoMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this WorldInfoMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, ChatMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
        }
        
        public static void Assign(this ChatMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
        }
        
        public static ChatMsg ToChatMsg(this Entity entity)
        {
            var msg = new ChatMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this ChatMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, CharacterResourceMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
            entity.Get<Displayable>().ResourceId = msg.ResourceId;
        }
        
        public static void Assign(this CharacterResourceMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
            msg.ResourceId = entity.Get<Displayable>().ResourceId;
        }
        
        public static CharacterResourceMsg ToCharacterResourceMsg(this Entity entity)
        {
            var msg = new CharacterResourceMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this CharacterResourceMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, DisconnectedMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
        }
        
        public static void Assign(this DisconnectedMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
        }
        
        public static DisconnectedMsg ToDisconnectedMsg(this Entity entity)
        {
            var msg = new DisconnectedMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this DisconnectedMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
        public static void Assign(this Entity entity, VoiceMsg msg)
        {
            EntityHelper.SetEntityId(entity, msg.Id);
        }
        
        public static void Assign(this VoiceMsg msg, Entity entity)
        {
            msg.Id = entity.Id;
        }
        
        public static VoiceMsg ToVoiceMsg(this Entity entity)
        {
            var msg = new VoiceMsg();
            Assign(msg, entity);
            return msg;
        }
        
        public static Entity ToEntity(this VoiceMsg msg)
        {
            var entity = EntityManager.Instance.CreateEntity(msg.Id);
            Assign(entity, msg);
            return entity;
        }
        
    }
}
