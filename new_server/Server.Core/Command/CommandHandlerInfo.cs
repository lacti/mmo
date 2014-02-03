using System.Diagnostics;
using System.Linq;
using System.Reflection;

namespace Server.Core.Command
{
    public class CommandHandlerInfo
    {
        internal CommandHandlerInfo(MethodInfo method)
        {
            Method = method;
            Attribute = method.GetCustomAttribute<CommandHandlerAttribute>();
            Debug.Assert(Attribute != null);

            Parameters = (from e in method.GetParameters() select new CommandParameterInfo(e)).ToArray();
        }

        public string Command
        {
            get { return Attribute.Command; }
        }

        public string Description
        {
            get { return Attribute.Description; }
        }

        internal MethodInfo Method { get; private set; }
        internal CommandHandlerAttribute Attribute { get; private set; }

        public CommandParameterInfo[] Parameters { get; private set; }
    }
}