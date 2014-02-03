using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Attribute;
using Server.Core.Component;

namespace Server.Actor.Controller
{
    public class MoveController : BaseController
    {
        public MoveController(BaseActor actor) : base(actor)
        {
        }

        public double Dir { get; set; }
        public double Speed { get; set; }

        [Volatile]
        public DateTime MoveStartTime { get; set; }

        public Pos RealPos
        {
            get
            {
                const double zeroSpeed = 0.01;
                if (Math.Abs(Speed - 0) < zeroSpeed)
                    return CurrentPos;

                const int maxSeconds = 120;
                var seconds = Math.Min((DateTime.Now - MoveStartTime).Seconds, maxSeconds);
                return new Pos
                {
                    X = CurrentPos.X + Speed * seconds * Math.Cos(Dir),
                    Y = CurrentPos.Y + Speed * seconds * Math.Sin(Dir),
                    Z = CurrentPos.Z
                };
            }
        }

        public Pos CurrentPos { get; set; }

        public override IEnumerable<int> OnTick()
        {
            const int baseTick = 1000;
            while (true)
            {
                var fight = Actor.Get<FightController>();
                if (fight == null || fight.IsAlive)
                    break;

                if (MoveStartTime != DateTime.MinValue)
                {
                    CurrentPos = RealPos;
                    MoveStartTime = DateTime.Now;
                }
                yield return baseTick;
            }
        }

        
    }
}
