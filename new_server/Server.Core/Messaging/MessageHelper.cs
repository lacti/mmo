using System;
using System.IO;
using System.Reflection;
using Server.Core.Util;

namespace Server.Core.Messaging
{
    public static class MessageHelper
    {
        public static readonly TypeMap<int> TypeMap =
            new TypeMap<int>(new[] {Configuration.MessageAssembly, Assembly.GetAssembly(typeof (IMessage))},
                             typeof (IMessage), GetTypeId, id => id > 0);

        public static int GetTypeId(Type messageType)
        {
            var typeIdField = messageType.GetField("TypeId", BindingFlags.Public | BindingFlags.Static);
            if (typeIdField == null)
                return -1;

            return (int) typeIdField.GetValue(null);
        }

        public static IMessage ReadMessage(byte[] bytes)
        {
            using (var reader = new BinaryReader(new MemoryStream(bytes)))
            {
                return ReadMessage(reader);
            }
        }

        public static IMessage ReadMessage(BinaryReader reader)
        {
            var typeId = reader.ReadInt32();
            if (!TypeMap.ContainsKey(typeId))
                return null;

            var messageType = TypeMap[typeId];
            var message = Activator.CreateInstance(messageType) as IMessage;

            message.Read(reader);
            return message;
        }
    }
}