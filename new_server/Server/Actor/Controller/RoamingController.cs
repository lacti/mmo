using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Attribute;

namespace Server.Actor.Controller
{
    public class RoamingController : BaseController
    {
        public RoamingController(BaseActor actor) : base(actor)
        {
        }

        public Pos RoamingStartPos { get; set; }
        public Pos RoamingEndPos { get; set; }

    }
}
