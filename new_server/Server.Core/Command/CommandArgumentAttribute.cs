using System;

namespace Server.Core.Command
{
    [AttributeUsage(AttributeTargets.Parameter, Inherited = false, AllowMultiple = false)]
    public class CommandArgumentAttribute : Attribute
    {
        public CommandArgumentAttribute(string description)
            : this(description, CommandArgumentOption.None)
        {
        }

        public CommandArgumentAttribute(string description, object defaultValue)
            : this(description, CommandArgumentOption.Optional, defaultValue)
        {
        }

        public CommandArgumentAttribute(string description, CommandArgumentOption options)
            : this(description, options, null)
        {
        }


        public CommandArgumentAttribute(string description, CommandArgumentOption options, object defaultValue)
        {
            Description = description;
            Options = options;
            Default = defaultValue;
        }

        public string Description { get; set; }
        public CommandArgumentOption Options { get; set; }
        public object Default { get; set; }
    }
}