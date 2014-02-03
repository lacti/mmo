using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Actor.Controller
{
    public class DisplayController : BaseController
    {
        public DisplayController(BaseActor actor) : base(actor)
        {
        }

        // nameplate
        public string Title { get; set; }
        public string Name { get; set; }

        // displayable
        public bool Visible { get; set; }
        public int ResourceId { get; set; }
    }
}
