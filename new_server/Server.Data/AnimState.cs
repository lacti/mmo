/* this code is auto-generated. */

using Server.Core.Data;
using System.Collections.Generic;

namespace Server.Data
{
    public class AnimState : IData
    {
        public class State
        {
            public string Name { get; set; }
        }
        
        public int Id { get; set; }
        public List<State> States { get; private set; }
        
        public AnimState()
        {
            States = new List<State>();
        }
    }
    
}
