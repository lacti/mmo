/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class Character : IData
    {
        public int Id { get; set; }
        public int SpriteRef { get; set; }
        public Sprite Sprite { get { return DataCenter.Instance[typeof (Sprite), SpriteRef] as Sprite; } }
    }
    
}
