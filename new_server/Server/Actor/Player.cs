using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Actor.Controller;

namespace Server.Actor
{
    public class Player : BaseActor
    {
        public bool Logged { get; set; }
        public int ViewRange { get; set; }

        protected override void InitializeControllers()
        {
            Attach<MoveController>();
            Attach<FightController>();
            Attach<DisplayController>();
        }
    }
}
