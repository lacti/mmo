using System;

namespace Server.Core.Component
{
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Property, Inherited = false, AllowMultiple = true)]
    public class TriggerAttribute : Attribute
    {
        /// <summary>
        /// Trigger의 종류
        /// </summary>
        public TriggerType Type { get; set; }

        /// <summary>
        /// Trigger Message의 전파 방식
        /// </summary>
        public TriggerAction Action { get; set; }

        /// <summary>
        /// Trigger에 의해 전파될 Message의 Type
        /// </summary>
        public Type Message { get; set; }

        public TriggerAttribute()
        {
            Type = TriggerType.None;
            Action = TriggerAction.Send;
        }
    }
}