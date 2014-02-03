#region

using System;
using System.Drawing;
using Server.MapEditor.Properties;

#endregion

namespace Server.MapEditor.Model
{
    internal class City : IDrawable
    {
        private static readonly Image[] Images = new Image[]
            {
                Resources.Town__1_, Resources.Town__2_, Resources.Town__3_, Resources.Town__4_, Resources.Town__5_,
                Resources.Town__6_, Resources.Town__7_, Resources.Town__8_, Resources.Town__9_, Resources.Town__10_,
                Resources.Town__11_, Resources.Town__12_, Resources.Town__13_, Resources.Town__14_, Resources.Town__15_,
                Resources.Town__16_, Resources.Town__17_, Resources.Town__18_, Resources.Town__19_, Resources.Town__20_,
                Resources.Town__21_, Resources.Town__22_, Resources.Town__23_, Resources.Town__24_,
            };

        private static readonly Random random = new Random();

        public City(int x, int y)
        {
            X = x;
            Y = y;

            Generate();
        }

        public int X { get; private set; }
        public int Y { get; private set; }
        public string Name { get; private set; }
        public int TownType { get; private set; }

        public int Money { get; private set; }
        public int People { get; private set; }
        public int Defense { get; private set; }
        public int Commerce { get; private set; }
        public int Tech { get; private set; }
        public int Support { get; private set; }

        public int Owner { get; private set; }
        public Brush OwnerColor { get; private set; }

        public Image TownImage
        {
            get { return Images[TownType]; }
        }

        public void Draw(Graphics g)
        {
            g.DrawImage(TownImage, X - TownImage.Width/2, Y - TownImage.Height/2);
        }

        private void Generate()
        {
            TownType = random.Next(Images.Length);

            People = random.Next(5000) + 2000;
            Defense = random.Next(40);
            Commerce = random.Next(30);
            Tech = random.Next(20);
            Support = random.Next(10) + 5;
            Money = random.Next(100);

            Owner = -1;
            OwnerColor = Brushes.White;

            Name = "noname";
        }

        public void Conquest(int owner, Brush ownerColor)
        {
            Owner = owner;
            OwnerColor = ownerColor;
        }

        public void Rename(string name)
        {
            Name = name;
        }
    }
}