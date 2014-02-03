using System;

namespace MmoTools.Util
{
    public class TypeUtil
    {
        public static TypeEnum Parse(string name)
        {
            TypeEnum enumValue;
            if (Enum.TryParse(name.ToUpper(), out enumValue))
                return enumValue;

            switch (name.ToLower())
            {
                case "__int64":
                    return TypeEnum.INT64;
                case "std::string":
                    return TypeEnum.STRING;
                case "msg::bin_t":
                    return TypeEnum.BIN;
                case "entity_id_t":
                    return TypeEnum.ENTITY_ID;
                case "data::id_t":
                    return TypeEnum.DATA_ID;
                case "data::xyz_t":
                    return TypeEnum.DATA_POS;
            }
            throw new InvalidOperationException("등록되지 않은 Type입니다 (" + name + ")");
        }

        public static string ToDeclareTypeName(TypeEnum type)
        {
            switch (type)
            {
                case TypeEnum.INT64:
                    return "__int64";
                case TypeEnum.STRING:
                    return "std::string";
                case TypeEnum.BIN:
                    return "msg::bin_t";
                case TypeEnum.ENTITY_ID:
                    return "entity_id_t";
                case TypeEnum.DATA_ID:
                    return "data::id_t";
                case TypeEnum.DATA_POS:
                    return "data::xyz_t";
                case TypeEnum.DATA_EXPRESSION:
                    return "double";
            }
            return type.ToString().ToLower();
        }

        public static string ToArgumentTypeName(TypeEnum type)
        {
            return ToDeclareTypeName(type);
        }

        public static string ToDefaultValue(TypeEnum type)
        {
            switch (type)
            {
                case TypeEnum.BOOL:
                    return "false";
                case TypeEnum.CHAR:
                case TypeEnum.SHORT:
                case TypeEnum.INT:
                case TypeEnum.LONG:
                case TypeEnum.INT64:
                case TypeEnum.FLOAT:
                case TypeEnum.DOUBLE:
                    return "0";
                case TypeEnum.STRING:
                    return "\"\"";
                case TypeEnum.BIN:
                    return "";
                case TypeEnum.ENTITY_ID:
                    return "INVALID_ENTITY_ID";
                case TypeEnum.DATA_ID:
                    return "0";
                case TypeEnum.DATA_POS:
                    return "";
            }
            throw new InvalidOperationException("기본값이 정의되지 않은 Type입니다 (" + type + ")");
        }

        public static string ToDefaultValueInInitializer(TypeEnum type)
        {
            switch (type)
            {
                case TypeEnum.STRING:
                    return "";
            }
            return ToDefaultValue(type);
        }

        public static bool IsPrimitiveType(TypeEnum type)
        {
            switch (type)
            {
                case TypeEnum.BOOL:
                case TypeEnum.CHAR:
                case TypeEnum.SHORT:
                case TypeEnum.INT:
                case TypeEnum.LONG:
                case TypeEnum.INT64:
                case TypeEnum.FLOAT:
                case TypeEnum.DOUBLE:
                case TypeEnum.ENTITY_ID:
                case TypeEnum.DATA_ID:
                    return true;
            }
            return false;
        }
    }
}