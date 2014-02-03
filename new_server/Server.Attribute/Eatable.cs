using Server.Core.Component;
using Server.Message.Game;

namespace Server.Attribute
{
    public class Eatable : IAttribute
    {
        public int GroupId { get; set; }

        public int MaxHunger { get; set; }

        [Constraints(Type = ConstraintsType.Min, ValueType = ConstraintsValueType.IntValue, IntValue = 0)]
        [Constraints(Type = ConstraintsType.Max, ValueType = ConstraintsValueType.Reference, FieldName = "MaxHunger")]
        public int Hunger { get; set; }

        public int IncHunger { get; set; }
        public int IncHungerTickCount { get; set; }
        public int IncHungerCurrentTick { get; set; }

        public bool IsAlive
        {
            get { return Hunger < MaxHunger; }
        }
    }
}
