using Server.Core.Component;
using Server.Message.Game;

namespace Server.Attribute
{
    public class Fightable : IAttribute
    {
        public int MaxHp { get; set; }

        [Constraints(Type = ConstraintsType.Min, ValueType = ConstraintsValueType.IntValue, IntValue = 0)]
        [Constraints(Type = ConstraintsType.Max, ValueType = ConstraintsValueType.Reference, FieldName = "MaxHp")]
        [Trigger(Type = TriggerType.AfterChange, Message = typeof(UpdateHpMsg), Action = TriggerAction.Broadcast)]
        public int Hp { get; set; }

        public int Recovery { get; set; }

        public int RecoveryTickCount { get; set; }
        public int RecoveryCurrentTick { get; set; }

        public bool IsAlive
        {
            get { return Hp > 0; }
        }
    }
}