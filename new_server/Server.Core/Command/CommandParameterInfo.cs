using System;
using System.Reflection;
using Server.Core.Component;
using Server.Core.Util;

namespace Server.Core.Command
{
    public class CommandParameterInfo
    {
        private readonly object _default;
        private readonly string _description;
        private readonly CommandArgumentOption _option = CommandArgumentOption.None;

        internal CommandParameterInfo(ParameterInfo parameter)
        {
            ParameterType = parameter.ParameterType;

            var attribute = parameter.GetCustomAttribute<CommandArgumentAttribute>();
            if (attribute != null)
            {
                _description = attribute.Description;
                _default = attribute.Default;
                _option = attribute.Options;
            }

            Parameter = parameter;
            Attribute = attribute;
        }

        public Type ParameterType { get; private set; }

        public string Description
        {
            get { return _description ?? ""; }
        }

        public bool IsOptional
        {
            get { return _option == CommandArgumentOption.Optional; }
        }

        public bool IsAfterAll
        {
            get { return _option == CommandArgumentOption.AfterAll; }
        }

        public object Default
        {
            get
            {
                var result = _default ?? (ParameterType.IsValueType ? Activator.CreateInstance(ParameterType) : null);
                return result != null ? result.As(ParameterType) : null;
            }
        }

        public bool IsEntityType
        {
            get { return ParameterType == typeof (Entity); }
        }

        public Type ActualType
        {
            get { return IsEntityType ? typeof (int) : ParameterType; }
        }

        internal ParameterInfo Parameter { get; private set; }
        internal CommandArgumentAttribute Attribute { get; private set; }
    }
}