using System;
using System.IO;

namespace Server.Core.Util
{
    public static class BinaryIoExtension
    {
        public static void Write(this BinaryWriter writer, Type type, object value)
        {
            if (type == typeof (Boolean)) writer.Write((bool) value);
            else if (type == typeof (Int16)) writer.Write((short) value);
            else if (type == typeof (Int32)) writer.Write((int) value);
            else if (type == typeof (UInt32)) writer.Write((uint) value);
            else if (type == typeof (Int64)) writer.Write((long) value);
            else if (type == typeof (UInt64)) writer.Write((ulong) value);
            else if (type == typeof (Single)) writer.Write((float) value);
            else if (type == typeof (Double)) writer.Write((double) value);
            else throw new InvalidOperationException("Invalid Type: " + type);
        }

        public static object Read(this BinaryReader reader, Type type)
        {
            if (type == typeof (Boolean)) return reader.ReadBoolean();
            if (type == typeof (Int16)) return reader.ReadInt16();
            if (type == typeof (Int32)) return reader.ReadInt32();
            if (type == typeof (UInt32)) return reader.ReadUInt32();
            if (type == typeof (Int64)) return reader.ReadInt64();
            if (type == typeof (UInt64)) return reader.ReadUInt64();
            if (type == typeof (Single)) return reader.ReadSingle();
            if (type == typeof (Double)) return reader.ReadDouble();
            throw new InvalidOperationException("Invalid Type: " + type);
        }
    }
}