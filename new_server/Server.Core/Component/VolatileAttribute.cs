using System;

namespace Server.Core.Component
{
    /// <summary>
    ///     EntityBinder에서 Persistence를 고려할 때, 값을 저장하지 않을 것임을 명시하기 위해 사용되는 Attribute
    /// </summary>
    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Class, Inherited = false)]
    public class VolatileAttribute : Attribute
    {
    }
}