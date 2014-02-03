using Server.Core.Component;
using Server.Message.Game;

namespace Server.Attribute
{
    public class EntityGenerate : IAttribute
    {
        public int MaxHp { get; set; }

        [Constraints(Type = ConstraintsType.Min, ValueType = ConstraintsValueType.IntValue, IntValue = 0)]
        [Constraints(Type = ConstraintsType.Max, ValueType = ConstraintsValueType.Reference, FieldName = "MaxHp")]
        //[Trigger(Type = TriggerType.AfterChange, Message = typeof(UpdateFactoryHpMsg), Action = TriggerAction.Broadcast)]
        public int Hp { get; set; }

        //[Trigger(Type = TriggerType.AfterChange, Message = typeof(UpdateFactoryResourceMsg), Action = TriggerAction.Broadcast)]
        public int Resource { get; set; }

        public bool IsGeneratable
        {
            get { return Hp > 0; }
        }

        public int IncHp { get; set; }
        public int IncResource { get; set; }
        public int DecHpForDeathFromHunger { get; set; }

        public int NeededResourceToGen { get; set; }
    }
}
