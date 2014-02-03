/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class Sprite : IData
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int AnimStateRef { get; set; }
        public int Umax { get; set; }
        public int Vmax { get; set; }
        public double Scale { get; set; }
        public int Count { get; set; }
        public int TileSize { get; set; }
        public AnimState AnimState { get { return DataCenter.Instance[typeof (AnimState), AnimStateRef] as AnimState; } }
    }
    
}
