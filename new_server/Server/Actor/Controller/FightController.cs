using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Actor.Controller
{
    public class FightController : BaseController
    {
        public FightController(BaseActor actor) : base(actor)
        {
        }

        public int Hp { get; set; }
        public int Recovery { get; set; }
        public int RecoveryTickCount { get; set; }
        public int RecoveryCurrentTick { get; set; }

        public bool IsAlive { get { return Hp > 0; } }
    }
}
