using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Actor.Controller;
using Server.Attribute;
using Server.Core.Util;

namespace Server.Actor
{
    public class Npc : BaseActor
    {
        public bool Interactive { get; set; }

        protected override void InitializeControllers()
        {
            Attach<MoveController>();
            Attach<FightController>();
            Attach<DisplayController>();
            Attach<RoamingController>();
        }
    }
}
