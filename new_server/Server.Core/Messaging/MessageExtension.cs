using System;
using System.Collections;
using System.Diagnostics;
using System.IO;
using System.Text;
using Server.Core.Util;

namespace Server.Core.Messaging
{
    public static class MessageExtension
    {
        public static int GetTypeId(this IMessage message)
        {
            if (message == null)
                return -1;

            return MessageHelper.GetTypeId(message.GetType());
        }

        #region Reflection

        public static bool HasField(this IMessage message, string name)
        {
            if (message == null)
                return false;

            var property = message.GetType().GetProperty(name);
            return property != null;
        }

        public static T ReadField<T>(this IMessage message, string name)
        {
            if (message == null)
                return default(T);

            var property = message.GetType().GetProperty(name);
            if (property == null)
                return default(T);

            return (T) property.GetValue(message);
        }

        #endregion

        #region Serialize & Deserialize

        public static void WriteTo(this IMessage message, BinaryWriter writer)
        {
            writer.Write(message.GetTypeId());

            foreach (var eachProperty in message.GetType().GetProperties())
            {
                var propertyType = eachProperty.PropertyType;
                var propertyValue = eachProperty.GetValue(message, null);
                if (propertyType.IsPrimitive)
                {
                    writer.Write(propertyType, propertyValue);
                }
                else if (propertyType == typeof (byte[]))
                {
                    var bytesValue = propertyValue as byte[];
                    Debug.Assert(bytesValue != null, "bytesValue != null");

                    writer.Write(bytesValue.Length);
                    writer.Write(bytesValue);
                }
                else if (propertyType == typeof (string))
                {
                    if (propertyValue == null)
                    {
                        writer.Write(0);
                    }
                    else
                    {
                        var stringValue = propertyValue as string;
                        if (stringValue == null)
                        {
                            writer.Write(0);
                        }
                        else
                        {
                            var stringBytes = Encoding.UTF8.GetBytes(stringValue);
                            writer.Write(stringBytes.Length);
                            writer.Write(stringBytes);
                        }
                    }
                }
                else if (typeof (IMessage).IsAssignableFrom(propertyType))
                {
                    var childMessage = propertyValue as IMessage;
                    Debug.Assert(childMessage != null, "childMessage != null");

                    childMessage.WriteTo(writer);
                }
                else if (typeof (IList).IsAssignableFrom(propertyType))
                {
                    var list = propertyValue as IList;
                    Debug.Assert(list != null, "list != null");

                    writer.Write((uint) list.Count);
                    foreach (var each in list)
                    {
                        var childMessage = each as IMessage;
                        Debug.Assert(childMessage != null, "childMessage != null");

                        childMessage.WriteTo(writer);
                    }
                }
                else throw new InvalidOperationException("Invalid Type: " + propertyType);
            }
        }

        public static void Read(this IMessage message, BinaryReader reader)
        {
            foreach (var eachProperty in message.GetType().GetProperties())
            {
                var propertyType = eachProperty.PropertyType;
                if (propertyType.IsPrimitive)
                {
                    eachProperty.SetValue(message, reader.Read(propertyType), null);
                }
                else if (propertyType == typeof (byte[]))
                {
                    var length = reader.ReadInt32();
                    var bytes = reader.ReadBytes(length);

                    eachProperty.SetValue(message, bytes, null);
                }
                else if (propertyType == typeof (string))
                {
                    var length = reader.ReadInt32();
                    var stringBytes = reader.ReadBytes(length);
                    var value = Encoding.UTF8.GetString(stringBytes);

                    eachProperty.SetValue(message, value, null);
                }
                else if (typeof (IMessage).IsAssignableFrom(propertyType))
                {
                    eachProperty.SetValue(message, MessageHelper.ReadMessage(reader), null);
                }
                else if (typeof (IList).IsAssignableFrom(propertyType))
                {
                    var list = Activator.CreateInstance(propertyType) as IList;
                    Debug.Assert(list != null);

                    var count = reader.ReadUInt32();
                    if (count > 0)
                    {
                        while (count-- != 0)
                        {
                            var childMessage = MessageHelper.ReadMessage(reader);
                            if (childMessage != null)
                                list.Add(childMessage);
                        }
                    }

                    eachProperty.SetValue(message, list, null);
                }
                else throw new InvalidOperationException("Invalid Type: " + propertyType);
            }
        }

        #endregion

        #region Execute

        public static bool Execute(this IMessage message)
        {
            return MessageQueue.Global.SendTo(message);
        }

        public static void ExecuteDelayed(this IMessage message, int delayedMilliSec)
        {
            Timer.Execute(() => message.Execute(), delayedMilliSec);
        }

        #endregion
    }
}