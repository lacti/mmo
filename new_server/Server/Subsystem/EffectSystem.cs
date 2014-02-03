using System;
using System.Collections.Generic;
using System.Linq;
using Server.Core.Component;
using Server.Core.Component.Messaging;
using Server.Core.Util;
using Server.Data;
using Server.Extension;

namespace Server.Subsystem
{
    public static class EffectSystem
    {
        public static void Affect(Entity entity, InteractTemplate template)
        {
            var targetsNeeded = template.Effects.Any(e => e.Direction == InteractTemplate.Effect.Directions.Target);
            if (!targetsNeeded)
                Affect(entity, new Entity[0], template);
            else
            {
                PositionSystem.Global.Interact(entity, targets => Affect(entity, targets.ToArray(), template));
            }
        }

        public static void Affect(Entity entity, Entity[] targets, InteractTemplate template)
        {
            foreach (var effect in template.Effects)
            {
                switch (effect.Direction)
                {
                    case InteractTemplate.Effect.Directions.Owner:
                        AffectToOwner(entity, effect);
                        break;

                    case InteractTemplate.Effect.Directions.Target:
                        AffectToTarget(entity, targets, effect);
                        break;
                }
            }
        }

        private static void AffectToOwner(Entity entity, InteractTemplate.Effect effect)
        {
            switch (effect.Type)
            {
                case InteractTemplate.Effect.Types.Attribute:
                    AffectToOwnerAttribute(entity, effect);
                    break;
            }
        }

        private static void AffectToTarget(Entity entity, IEnumerable<Entity> targets, InteractTemplate.Effect effect)
        {
            switch (effect.Type)
            {
                case InteractTemplate.Effect.Types.Attribute:
                    AffectToTargetAttribute(entity, targets, effect);
                    break;
            }
        }

        private static void AffectToOwnerAttribute(Entity entity, InteractTemplate.Effect effect)
        {
            entity.ChangeAttributeValue(
                effect.Attribute, effect.Field, effect.Value, (operand, oldValue) => CalculateAffectedValue(effect.Function, operand, oldValue));
        }

        private static void AffectToTargetAttribute(Entity entity, IEnumerable<Entity> targets, InteractTemplate.Effect effect)
        {
            var attributeType = AttributeHelper.TypeMap[AttributeHelper.GetTypeName(effect.Attribute)];
            targets.Gather(attributeType, result =>
                {
                    Logger.Write("Interact {0} -> {1}", entity.Id, string.Join(", ", (from e in result select e.Id).ToArray()));

                    if (result.Count == 0)
                        return;

                    var effectValue = (float)effect.Value / result.Count;
                    result.PostEach(attributeType, target => target.ChangeAttributeValue(
                        effect.Attribute, effect.Field, effectValue, (operand, oldValue) => CalculateAffectedValue(effect.Function, operand, oldValue)));
                });
        }

        private static float CalculateAffectedValue(InteractTemplate.Effect.Functions function, float operand, float value)
        {
            switch (function)
            {
                case InteractTemplate.Effect.Functions.Plus:
                    return value + operand;
                case InteractTemplate.Effect.Functions.Minus:
                    return value - operand;
            }
            throw new InvalidOperationException("not implemented function: " + function);
        }
    }
}
