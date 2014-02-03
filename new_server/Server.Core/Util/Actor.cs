using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public abstract class Actor<TMessage>
    {
        private readonly SinglyAccessQueue<TMessage> _queue;

        protected Actor()
        {
            _queue = new SinglyAccessQueue<TMessage>(this, ProcessMessage);
        }

        protected abstract void ProcessMessage(TMessage message);

        public void Post(TMessage message)
        {
            _queue.EnqueueOrProcess(message);
        }
    }
}
