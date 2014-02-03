using System;

namespace Server.Core.Command
{
    [AttributeUsage(AttributeTargets.Method, Inherited = false, AllowMultiple = false)]
    public class CommandHandlerAttribute : Attribute
    {
        public CommandHandlerAttribute(string command, string description)
        {
            Command = command;
            Description = description;
        }

        public string Command { get; set; }
        public string Description { get; set; }
    }
}