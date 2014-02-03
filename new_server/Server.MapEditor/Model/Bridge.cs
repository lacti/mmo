#region

using System;
using System.Collections.Generic;
using System.Drawing;

#endregion

namespace Server.MapEditor.Model
{
    internal class Bridge : IDrawable
    {
        private static readonly Brush BridgeBrush = new SolidBrush(Color.FromArgb(150, 202, 183, 128));
        private static readonly Pen BridgePen = new Pen(BridgeBrush, 5);

        private static readonly Random random = new Random();

        public Bridge(City from, City to)
        {
            From = from;
            To = to;

            var points = new List<Point>();
            points.Add(new Point(From.X, From.Y));

            var count = 5 + random.Next(10);
            for (var index = 0; index < count; ++index)
            {
                points.Add(new Point(
                               From.X + (To.X - From.X)*index/count + random.Next(10),
                               From.Y + (To.Y - From.Y)*index/count + random.Next(10)
                               ));
            }

            points.Add(new Point(To.X, To.Y));
            Points = points.ToArray();
        }

        public City From { get; private set; }
        public City To { get; private set; }
        private Point[] Points { get; set; }

        public void Draw(Graphics g)
        {
            g.DrawCurve(BridgePen, Points);
        }
    }
}