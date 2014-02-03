using Server.Attribute;
using Server.Core.Command;
using Server.Core.Component;
using Server.Core.Util;

namespace Server.Extension
{
    public static class CommandArgumentsExtension
    {
        public static Pos GetPos(this CommandArguments args, int index)
        {
            return GetPos(args, index, new Pos());
        }

        public static Pos GetPos(this CommandArguments args, int index, Pos defaultPos)
        {
            return new Pos
                {
                    X = args.Get(index, defaultPos.X),
                    Y = args.Get(index + 1, defaultPos.Y),
                    Z = args.Get(index + 2, defaultPos.Z)
                };
        }

        public static Entity GetEntity(this CommandArguments args, int index = 0)
        {
            return EntityManager.Instance.At(args.Get<int>(index));
        }
    }
}