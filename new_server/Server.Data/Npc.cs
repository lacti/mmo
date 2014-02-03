/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class Npc : IData
    {
        public int Id { get; set; }
        public int CharacterRef { get; set; }
        public int NpcNameRef { get; set; }
        public Character Character { get { return DataCenter.Instance[typeof (Character), CharacterRef] as Character; } }
        public NpcName NpcName { get { return DataCenter.Instance[typeof (NpcName), NpcNameRef] as NpcName; } }
    }
    
}
