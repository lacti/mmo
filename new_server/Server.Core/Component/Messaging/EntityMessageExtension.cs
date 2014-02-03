using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using Server.Core.Messaging;
using Server.Core.Util;

namespace Server.Core.Component.Messaging
{
    public static class EntityMessageExtension
    {
        #region Execute

        public static bool Execute(this IMessage message, Entity entity)
        {
            return EntityMessageQueue.Global.SendTo(entity, message);
        }

        public static void ExecuteDelayed(this IMessage message, Entity entity, int delayedMilliSec)
        {
            Timer.Execute(() => message.Execute(entity), delayedMilliSec);
        }

        #endregion

        #region Message Conversion

        public static void Assign(this Entity entity, IMessage message)
        {
            Debug.Assert(entity != null);
            if (message == null)
                return;

            var messageType = message.GetType();
            foreach (var messageProperty in messageType.GetProperties())
            {
                var messageValue = messageProperty.GetValue(message);
                if (messageValue is IMessage)
                {
                    Assign(entity, messageValue as IMessage);
                    continue;
                }

                var attributeBinds = messageProperty.GetCustomAttributes(typeof (AttributeAttribute), false);
                if (attributeBinds.Length == 0)
                    continue;

                var attributeBind = attributeBinds[0] as AttributeAttribute;
                Debug.Assert(attributeBind != null, "attributeBind != null");

                if (attributeBind.EntityId)
                {
                    entity.Id = (int) messageValue;
                }
                else
                {
                    var attributeClassName = attributeBind.Attribute.ToUnderscored();
                    var attributeFieldName = attributeBind.Field.ToCamelCase();

                    var attributeType = AttributeHelper.TypeMap[attributeClassName];
                    var attributeProperty = attributeType.GetProperty(attributeFieldName);

                    var attributeObject = entity.Attach(attributeType);
                    attributeProperty.SetValue(attributeObject, messageValue);
                }
            }
        }

        public static void Assign(this IMessage message, Entity entity)
        {
            Debug.Assert(message != null);
            if (entity == null)
                return;

            var messageType = message.GetType();
            foreach (var messageProperty in messageType.GetProperties())
            {
                if (typeof (IMessage).IsAssignableFrom(messageProperty.PropertyType))
                {
                    var messageValue = messageProperty.GetValue(message);
                    if (messageValue == null)
                    {
                        messageValue = Activator.CreateInstance(messageProperty.PropertyType);
                        messageProperty.SetValue(message, messageValue);
                    }
                    Assign(messageValue as IMessage, entity);
                    continue;
                }

                var attributeBinds = messageProperty.GetCustomAttributes(typeof (AttributeAttribute), false);
                if (attributeBinds.Length == 0)
                    continue;

                var attributeBind = attributeBinds[0] as AttributeAttribute;
                Debug.Assert(attributeBind != null, "attributeBind != null");

                if (attributeBind.EntityId)
                {
                    messageProperty.SetValue(message, entity.Id);
                }
                else
                {
                    var attributeClassName = attributeBind.Attribute.ToUnderscored();
                    var attributeFieldName = attributeBind.Field.ToCamelCase();

                    var attributeType = AttributeHelper.TypeMap[attributeClassName];
                    var attributeProperty = attributeType.GetProperty(attributeFieldName);

                    if (!entity.Has(attributeType))
                        continue;

                    var attributeObject = entity.Get(attributeType);
                    var attributeValue = attributeProperty.GetValue(attributeObject);
                    messageProperty.SetValue(message, attributeValue);
                }
            }
        }

        public static Entity ToEntity(this IMessage message)
        {
            var entityId = message.ReadField<int>("Id");
            var entity = entityId > 0
                             ? EntityManager.Instance.CreateEntity(entityId)
                             : EntityManager.Instance.CreateEntity();

            Assign(entity, message);
            return entity;
        }

        public static T ToMessage<T>(this Entity entity) where T : IMessage, new()
        {
            if (entity == null)
                throw new InvalidOperationException("entity is null");

            var message = new T();
            Assign(message, entity);
            return message;
        }

        #endregion

        #region Trigger

        public static void InvokeTrigger(this Entity entity, TriggerType triggerType, Type attributeType)
        {
            InvokeTrigger(entity, triggerType, attributeType.GetCustomAttributes(typeof(TriggerAttribute), false).OfType<TriggerAttribute>());
        }

        public static void InvokeTrigger(this Entity entity, TriggerType triggerType, PropertyInfo propertyInfo)
        {
            InvokeTrigger(entity, triggerType, propertyInfo.GetCustomAttributes(typeof (TriggerAttribute), false).OfType<TriggerAttribute>());
        }

        private static void InvokeTrigger(this Entity entity, TriggerType triggerType, IEnumerable<TriggerAttribute> triggers)
        {
            foreach (var trigger in triggers)
            {
                if (trigger.Type != triggerType)
                    continue;

                if (trigger.Message != null)
                {
                    var message = Activator.CreateInstance(trigger.Message) as IMessage;
                    message.Assign(entity);

                    switch (trigger.Action)
                    {
                        case TriggerAction.Send:
                            EntityMessageQueue.Global.SendTo(entity, message);
                            break;

                        case TriggerAction.Broadcast:
                            EntityMessageQueue.Global.BroadcastTo(entity, message);
                            break;
                    }
                }

            }
        }

        public static bool ChangeAttributeValue(this Entity entity, string attributeName, string propertyName, object newValue)
        {
            return ChangeAttributeValue(entity, attributeName, propertyName, newValue, null);
        }

        public static bool ChangeAttributeValue(this Entity entity, string attributeName, string propertyName, object newValue, Func<float, float, float> calculator)
        {
            // 값을 변경하고 그에 맞는 ChangeEvent를 전달한다.
            var attributeType = AttributeHelper.GetType(attributeName);
            if (attributeType == null)
                return false;

            propertyName = propertyName.ToCamelCase();
            var propertyType = attributeType.GetProperty(propertyName);
            if (propertyType == null)
                return false;

            if (!entity.Has(attributeType))
                return false;

            var attribute = entity.Get(attributeType);
            var previousValue = propertyType.GetValue(attribute);

            // 연산이 필요할 경우 연산한 결과를 newValue에 넣어준다.
            if (calculator != null)
                newValue = calculator(newValue.As<float>(), previousValue.As<float>());

            // contraints를 적용한 다음, 다시 PropertyType에 맞게 바꿔서 넣어준다.
            var nextValue = ConstraintsHelper.ApplyConstraints(attribute, propertyName, newValue);
            if (Equals(previousValue, nextValue))
                return false;

            entity.InvokeTrigger(TriggerType.BeforeChange, propertyType);
            attribute.SetValue(propertyName, nextValue);
            entity.InvokeTrigger(TriggerType.AfterChange, propertyType);
            return true;
        }

        #endregion
    }
}