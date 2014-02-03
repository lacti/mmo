using System;
using System.Collections.Generic;
using System.Linq;
using Server.Attribute;
using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Message.Game;

namespace Server.Extension
{
    public class PositionSystem : Actor<Action>
    {
        private const int DefaultViewRange = 2000;
        public static readonly PositionSystem Global = new PositionSystem();

        private readonly Dictionary<Entity, HashSet<Entity>> _viewMap = new Dictionary<Entity, HashSet<Entity>>();

        protected override void ProcessMessage(Action action)
        {
            action();
        }

        public void Appear(Entity entity)
        {
            if (!entity.Has<Pos>())
                return;

            Post(() => RebuildViewOfEntity(entity));
        }

        private void RebuildViewOfEntity(Entity entity)
        {
            var currentPos = GetCurrentPos(entity);
            var viewRange = entity.Has<Player>()
                                ? Math.Max(entity.Get<Player>().ViewRange, DefaultViewRange)
                                : 0;

            if (!_viewMap.ContainsKey(entity))
                _viewMap.Add(entity, new HashSet<Entity>());

            var observers = _viewMap[entity];
            foreach (var peer in _viewMap.Keys.Where(e => e.Has<Player>()))
            {
                if (peer == entity)
                    continue;

                var peerViewRange = Math.Max(peer.Get<Player>().ViewRange, DefaultViewRange);
                var peerPos = GetCurrentPos(peer);
                var distance = Pos.Distance(currentPos, peerPos);
                Logger.Write("{0} <-> {1} : {2}", currentPos, peerPos, distance);

                if (distance <= peerViewRange)
                {
                    if (!observers.Contains(peer))
                    {
                        observers.Add(peer);
                        SendSpawnMessage(peer, entity);
                    }
                }
                else
                {
                    if (observers.Contains(peer))
                    {
                        observers.Remove(peer);
                        SendDespawnMessage(peer, entity);
                    }
                }

                var peerObservers = _viewMap[peer];
                if (distance < viewRange)
                {
                    if (!peerObservers.Contains(entity))
                    {
                        peerObservers.Add(entity);
                        SendSpawnMessage(entity, peer);
                    }
                }
                else
                {
                    if (peerObservers.Contains(entity))
                    {
                        peerObservers.Remove(entity);
                        SendDespawnMessage(entity, peer);
                    }
                }
            }
        }

        public void Disappear(Entity entity)
        {
            if (!entity.Has<Pos>())
                return;

            Post(() => RemoveViewOfEntity(entity));
        }

        private void RemoveViewOfEntity(Entity entity)
        {
            foreach (var peer in _viewMap[entity])
                SendDespawnMessage(peer, entity);

            _viewMap.Remove(entity);

            foreach (var pair in _viewMap.Where(e => e.Value.Contains(entity)))
            {
                var peer = pair.Key;
                var observers = pair.Value;

                observers.Remove(entity);
                SendDespawnMessage(entity, peer);
            }
        }

        private static void SendSpawnMessage(Entity observer, Entity observee)
        {
            SendTo<SpawnMsg>(observer, observee);
        }

        private static void SendDespawnMessage(Entity observer, Entity observee)
        {
            SendTo<DespawnMsg>(observer, observee);
        }

        private static void SendTo<T>(Entity receiver, Entity sender) where T : IMessage, new()
        {
            if (!receiver.Has<Player>())
                return;

            var session = MessageSessionManager.Instance.At(receiver.Id);
            if (session == null)
                return;

            session.Send(sender.ToMessage<T>());
        }

        public void Move(Entity entity)
        {
            if (!entity.Has<Pos>())
                return;

            Post(() => RebuildViewOfEntity(entity));
        }

        public void Execute(Entity entity, Func<Entity, Pos, Entity, Pos, bool> rangeSelector,
                            Action<IEnumerable<Entity>> callback)
        {
            if (!entity.Has<Pos>())
                return;

            Post(() =>
                {
                    var currentPos = GetCurrentPos(entity);
                    var peers = new HashSet<Entity>();
                    foreach (var peer in _viewMap.Keys)
                    {
                        if (peer == entity)
                            continue;

                        var peerPos = GetCurrentPos(peer);
                        if (rangeSelector(entity, currentPos, peer, peerPos))
                            peers.Add(peer);
                    }
                    callback(peers);
                });
        }

        public void Interact(Entity entity, Action<IEnumerable<Entity>> callback)
        {
            Execute(entity, InteractRangeSelector, callback);
        }

        private static bool InteractRangeSelector(Entity entity, Pos entityPos, Entity peer, Pos peerPos)
        {
            if (!entity.Has<Motion>())
                return false;

            const int defaultRange = 50;
            const int defaultDegree = 30;
            var dir = entity.Get<Motion>().Dir;
            var ax = Math.Cos(dir);
            var ay = Math.Sin(dir);
            var sizeOfA = Math.Sqrt(ax*ax + ay*ay);

            const double deltaRadian = defaultDegree*Math.PI/180.0;
            const double epsilon = 1.0;

            var dx = peerPos.X - entityPos.X;
            var dy = peerPos.Y - entityPos.Y;
            var sizeOfD = Math.Sqrt(dx*dx + dy*dy);
            if (Math.Abs(sizeOfD - 0) < epsilon)
                return true;

            var cross = ax*dx + ay*dy;
            var thetaRadian = Math.Acos(cross/(sizeOfA*sizeOfD));
            if (!(-deltaRadian <= thetaRadian && thetaRadian <= deltaRadian))
                return false;

            var oppositeDistance = Pos.Distance(entityPos.X, entityPos.Y, peerPos.X, peerPos.Y);
            return !(oppositeDistance > defaultRange);
        }

        public void Broadcast(Entity sender, IMessage message, int exceptEntityId = Entity.InvalidEntityId)
        {
            if (!sender.Has<Pos>())
                return;

            Post(() =>
                {
                    if (!_viewMap.ContainsKey(sender))
                        return;

                    var receiverIds = _viewMap[sender].Select(e => e.Id).Concat(new[] { sender.Id }).Where(e => e != exceptEntityId);
                    foreach (var peerSession in MessageSessionManager.Instance[receiverIds].Where(e => e != null))
                    {
                        peerSession.Send(message);
                    }
                });
        }

        public static Pos GetCurrentPos(Entity entity)
        {
            // 이 함수는 해당 entity의 context에서 호출되어야 한다.
            if (!entity.Has<Pos>())
                return Pos.Empty;

            var currentPos = entity.Get<Pos>();
            if (!entity.Has<Motion>())
                return currentPos;

            var motion = entity.Get<Motion>();

            const double zeroSpeed = 0.01;
            if (Math.Abs(motion.Speed - 0) < zeroSpeed)
                return currentPos;

            const int maxSeconds = 120;
            var seconds = Math.Min((DateTime.Now - motion.StartTime).Seconds, maxSeconds);
            var realPos = new Pos
                {
                    X = currentPos.X + motion.Speed*seconds*Math.Cos(motion.Dir),
                    Y = currentPos.Y + motion.Speed*seconds*Math.Sin(motion.Dir),
                    Z = currentPos.Z
                };
            return realPos;
        }
    }
}