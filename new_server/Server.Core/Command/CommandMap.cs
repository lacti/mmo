using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using Server.Core.Component;
using Server.Core.Util;

namespace Server.Core.Command
{
    public class CommandMap
    {
        public const string CommandPrefix = ".";
        public static readonly CommandMap Instance = new CommandMap();

        private readonly MultiDictionary<string, Handler> _commandMap = new MultiDictionary<string, Handler>();

        private CommandMap()
        {
        }

        public IEnumerable<CommandHandlerInfo> HandlerInfos
        {
            get { return from handlers in _commandMap.Values from each in handlers select each.Info; }
        }

        public bool IsAdminCommand(string input)
        {
            if (string.IsNullOrWhiteSpace(input))
                return false;

            return input.StartsWith(CommandPrefix);
        }

        public bool Dispatch(Entity entity, string input, out bool success)
        {
            success = false;
            if (!IsAdminCommand(input))
                return false;

            var args = new CommandArguments(input.Substring(1));    // remove leading . character.
            var command = args.Command.ToLower();
            if (!_commandMap.ContainsKey(command))
                return false;

            success = _commandMap[command].Aggregate(false, (result, handler) => InvokeHandler(entity, args, handler.Object, handler.Info) || result);
            return true;
        }

        public void Add(string command, object handler, MethodInfo method)
        {
            _commandMap.Add(command, new Handler {Object = handler, Info = new CommandHandlerInfo(method)});
        }

        private static bool InvokeHandler(Entity admin, CommandArguments args, object handler, CommandHandlerInfo handlerInfo)
        {
            if (handlerInfo.Parameters.Length == 0)
                return (bool) handlerInfo.Method.Invoke(handler, new object[0]);

            var objects = new List<object>();

            // 첫 번재 인자로 admin을 요청할 경우에는 admin을 넣어준다.
            if (handlerInfo.Parameters[0].ParameterType == typeof (Entity))
                objects.Add(admin);

            for (var index = 1; index < handlerInfo.Parameters.Length; ++index)
            {
                var argsIndex = index - 1;
                var parameter = handlerInfo.Parameters[index];

                var isAfterAll = false;
                object value = null;

                // CommandArgument의 속성에 따라 value를 미리 구해본다.
                if (parameter.IsOptional)
                    value = parameter.Default;

                if (parameter.IsAfterAll)
                {
                    // AfterAll은 꼭 마지막에 와야한다.
                    Debug.Assert(index == handlerInfo.Parameters.Length - 1);
                    value = args.GetAll(argsIndex);
                    isAfterAll = true;
                }

                // Attribute로부터 값을 구하지 못했고, 입력된 args도 없다면 인자 부족으로 종료한다.
                if (args.Count <= argsIndex)
                {
                    if (value == null)
                        return false;
                }
                else
                {
                    // AfterAll이 아니면 값을 가져올 수 있다.
                    if (!isAfterAll)
                    {
                        // Entity 형일 경우 int를 받아서 Entity를 가져온다.
                        if (parameter.ParameterType == typeof (Entity))
                        {
                            if (!args.Is<int>(argsIndex))
                                return false;

                            var targetEntityId = args.Get<int>(argsIndex);
                            var targetEntity = EntityManager.Instance.At(targetEntityId);
                            if (targetEntity == null)
                                return false;

                            value = targetEntity;
                        }
                        else
                        {
                            // 잘못된 인자가 넘어왔다면 인자 오류로 종료한다.
                            if (!args.IsType(argsIndex, parameter.ParameterType))
                                return false;

                            value = args.GetValue(argsIndex, parameter.ParameterType);
                        }
                    }
                }

                objects.Add(value);
            }

            return (bool) handlerInfo.Method.Invoke(handler, objects.ToArray());
        }

        private class Handler
        {
            public object Object { get; set; }
            public CommandHandlerInfo Info { get; set; }
        }
    }
}