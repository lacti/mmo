using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Server.Core.Command;
using Server.Core.Component;
using Server.Core.Messaging;

namespace Server.Extension.Handler
{
    public static class HandlerHelper
    {
        public static void Register(Assembly targetAssembly)
        {
            foreach (var eachType in targetAssembly.GetTypes())
            {
                if (!typeof (IHandler).IsAssignableFrom(eachType))
                    continue;

                // bind handler static methods
                RegisterMethods(eachType.GetMethods(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic),
                                null);

                // bind handler instance methods
                var instanceMethods =
                    eachType.GetMethods(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
                if (instanceMethods.Any())
                {
                    RegisterMethods(instanceMethods,
                                    Activator.CreateInstance(eachType));
                }
            }
        }

        private static void RegisterMethods(IEnumerable<MethodInfo> methods, object handler)
        {
            foreach (var method in methods)
            {
                var messageHandler = method.GetCustomAttribute<MessageHandlerAttribute>();
                if (messageHandler != null)
                    RegisterMessageHandler(handler, method, messageHandler.MessageId);

                var commandHandler = method.GetCustomAttribute<CommandHandlerAttribute>();
                if (commandHandler != null)
                    CommandMap.Instance.Add(commandHandler.Command, handler, method);
            }
        }

        private static void RegisterMessageHandler(object handler, MethodInfo method, int messageId)
        {
            var parameters = method.GetParameters();

            // MessageId가 있으면 그것으로 MessageType을 찾지만, 없다면 가장 마지막 인자가 MessageType이라고 추측한다.
            var messageType = messageId != MessageHandlerAttribute.DeduceFromParameter
                ? MessageHelper.TypeMap[messageId] 
                : parameters.Last().ParameterType;

            // bind entity handler
            if (CheckIfParametersEquals(parameters, typeof(Entity), typeof(MessageSession),
                                        messageType))
            {
                var entitySessionHandler = new DispatchHandler.EntitySessionHandler(
                    (entity, session, message) => method.Invoke(handler, new object[] {entity, session, message}));

                DispatchHandler.Instance.AddHandler(messageType, entitySessionHandler);
            }

            // bind non-entity handler
            if (CheckIfParametersEquals(parameters, typeof(MessageSession), messageType))
            {
                var sessionHandler = new DispatchHandler.SessionHandler(
                    (session, message) => method.Invoke(handler, new object[] {session, message}));

                DispatchHandler.Instance.AddHandler(messageType, sessionHandler);
            }

            // bind non-session handler
            if (CheckIfParametersEquals(parameters, typeof(Entity), messageType))
            {
                var entityHandler = new DispatchHandler.EntityHandler(
                    (entity, message) => method.Invoke(handler, new object[] {entity, message}));

                DispatchHandler.Instance.AddHandler(messageType, entityHandler);
            }

            // bind message-only handler
            if (CheckIfParametersEquals(parameters, messageType))
            {
                var messageHandler = new DispatchHandler.Handler(
                    message => method.Invoke(handler, new object[] {message}));

                DispatchHandler.Instance.AddHandler(messageType, messageHandler);
            }
        }

        private static bool CheckIfParametersEquals(ParameterInfo[] parameters, params Type[] targets)
        {
            if (parameters.Length != targets.Length)
                return false;

            return !parameters.Where((each, index) => each.ParameterType != targets[index]).Any();
        }
    }
}