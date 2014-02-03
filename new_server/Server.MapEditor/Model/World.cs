#region

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;

#endregion

namespace Server.MapEditor.Model
{
    internal class World : IDrawable
    {
        public World()
        {
            Players = new List<Player>();
            Cities = new List<City>();
            Bridges = new List<Bridge>();

            Generate();
        }

        public List<Player> Players { get; private set; }
        public List<City> Cities { get; private set; }
        public List<Bridge> Bridges { get; private set; }

        public int Index { get; private set; }
        public string Name { get; private set; }

        public int Width { get; private set; }
        public int Height { get; private set; }

        public int RegionInterval { get; private set; }
        public int XCount { get; private set; }
        public int YCount { get; private set; }

        public void Draw(Graphics g)
        {
            g.SmoothingMode = SmoothingMode.HighQuality;
            Bridges.ForEach(e => e.Draw(g));
            Cities.ForEach(e => e.Draw(g));
            Players.ForEach(e => e.Draw(g));
        }

        private void Generate()
        {
            var random = new Random();
            Width = random.Next(1000) + 4000;
            Height = random.Next(1000) + 3000;

            RegionInterval = (int) Math.Sqrt(Width*Height/(double) (random.Next(100) + 200));
            XCount = Width/RegionInterval;
            YCount = Height/RegionInterval;
            Width = XCount*RegionInterval;
            Height = YCount*RegionInterval;

            var margin = RegionInterval/8;
            var realInterval = RegionInterval - margin*2;
            for (var x = 0; x < XCount; x++)
            {
                for (var y = 0; y < YCount; y++)
                {
                    var cityX = x*RegionInterval + random.Next(realInterval) + margin;
                    var cityY = y*RegionInterval + random.Next(realInterval) + margin;
                    Cities.Add(new City(cityX, cityY));
                }
            }

            var isolatedCities = new HashSet<City>(Cities);

            while (isolatedCities.Count > 0)
            {
                foreach (var fromCity in Cities)
                {
                    if (!isolatedCities.Contains(fromCity)) continue;

                    var cityBridges = random.Next((int) Math.Pow(2, 8));
                    for (var bridgeType = 0; bridgeType < 8; bridgeType++)
                    {
                        if ((cityBridges & (1 << bridgeType)) != 0)
                        {
                            var toCityIndex = GetAdjancyCity(Cities.IndexOf(fromCity), bridgeType);
                            if (toCityIndex > 0)
                            {
                                var toCity = Cities[toCityIndex];
                                Bridges.Add(new Bridge(fromCity, toCity));

                                isolatedCities.Remove(fromCity);
                                isolatedCities.Remove(toCity);
                            }
                        }
                    }
                }
            }
        }

        public int GetAdjancyCity(int index, int type)
        {
            var adjIndex = -1;
            switch (type)
            {
                case 0:
                    adjIndex = index - YCount - 1;
                    break;
                case 1:
                    adjIndex = index - 1;
                    break;
                case 2:
                    adjIndex = index + YCount - 1;
                    break;
                case 3:
                    adjIndex = index + YCount;
                    break;
                case 4:
                    adjIndex = index + YCount + 1;
                    break;
                case 5:
                    adjIndex = index + 1;
                    break;
                case 6:
                    adjIndex = index - YCount + 1;
                    break;
                case 7:
                    adjIndex = index - YCount;
                    break;
            }
            /* TOP    */
            if (index%YCount == 0 && (type == 0 || type == 1 || type == 2)) return -1;
            /* BOTTOM */
            if ((index + 1)%YCount == 0 && (type == 4 || type == 5 || type == 6)) return -1;
            /* LEFT   */
            if (index < YCount && (type == 0 || type == 6 || type == 7)) return -1;
            /* RIGHT  */
            if (index > (Cities.Count - YCount) && (type == 2 || type == 3 || type == 4)) return -1;
            return adjIndex < 0 ? -1 : (adjIndex >= Cities.Count ? -1 : adjIndex);
        }
    }
}