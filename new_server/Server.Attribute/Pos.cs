using System;
using Server.Core.Component;

namespace Server.Attribute
{
    public class Pos : IAttribute
    {
        public static readonly Pos Empty = new Pos();

        public double X { get; set; }
        public double Y { get; set; }
        public double Z { get; set; }

        public static double Distance(Pos from, Pos to)
        {
            return Math.Sqrt(Math.Pow(from.X - to.X, 2) + Math.Pow(from.Y - to.Y, 2) + Math.Pow(from.Z - to.Z, 2));
        }

        public static double Distance(double x1, double y1, double x2, double y2)
        {
            return Math.Sqrt(Math.Pow(x1 - x2, 2) + Math.Pow(y1 - y2, 2));
        }

        public override string ToString()
        {
            return string.Format("{0},{1},{2}", X, Y, Z);
        }
    }
}