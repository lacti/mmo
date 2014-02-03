using System;
using System.Threading;

namespace Server.Core.Util
{
    public class ExclusiveRun : IDisposable
    {
        private const int None = 0;
        private const int Processed = 1;

        private readonly Flag _flag;

        public ExclusiveRun(Flag flag)
        {
            _flag = flag;
            IsAcquired = Interlocked.CompareExchange(ref _flag.Value, Processed, None) == None;
        }

        public bool IsAcquired { get; private set; }

        public void Dispose()
        {
            Interlocked.Exchange(ref _flag.Value, None);
        }

        public class Flag
        {
            public int Value = None;
        }
    }
}