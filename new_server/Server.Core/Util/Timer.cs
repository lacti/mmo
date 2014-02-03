using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public class Timer
    {
        private const int BucketCount = 65536;
        private const int Resolution = 32;
        private static readonly Timer Instance = new Timer();
        private static long _futureActionStatistics;
        private readonly ActionBucket[] _buckets = new ActionBucket[BucketCount];

        private readonly Stopwatch _delayWatch = new Stopwatch();
        private readonly Stopwatch _processWatch = new Stopwatch();
        private int _currentBucketIndex;
        private int _delayCorrection;

        static Timer()
        {
            Instance.StartLoop();
        }

        private Timer()
        {
            for (var index = 0; index < BucketCount; ++index)
                _buckets[index] = new ActionBucket();
        }

        private void StartLoop()
        {
            Task.Factory.StartNew(TimerLoop, TaskCreationOptions.LongRunning);
        }

        public void TimerLoop()
        {
            while (true)
            {
                _buckets[_currentBucketIndex].ExecuteAndClear();
                _currentBucketIndex = (_currentBucketIndex + 1)%BucketCount;

                var waitingMilliSec = _processWatch.IsRunning ? (int) _processWatch.ElapsedMilliseconds : 0;
                var nextMilliSec = (Resolution*2) - waitingMilliSec - _delayCorrection;

                _processWatch.Restart();
                if (nextMilliSec > 0)
                {
                    _delayWatch.Start();
                    Thread.Sleep(nextMilliSec);

                    _delayCorrection = Math.Min(0, (int) _delayWatch.ElapsedMilliseconds - nextMilliSec);
                }
            }
// ReSharper disable FunctionNeverReturns
        }

// ReSharper restore FunctionNeverReturns

        public static void Execute(Action action, int delayedMilliSec)
        {
            Instance.ExecuteInternal(action, delayedMilliSec);
        }

        internal void ExecuteInternal(Action action, int delayedMilliSec)
        {
            if (action == null)
                return;

            // 먼 미래에 실행
            if (delayedMilliSec >= BucketCount*Resolution)
            {
                StartFutureAction(action, delayedMilliSec);
                return;
            }

            // Bucket에 넣어두면 TiemrLoop에서 실행해준다.
            var bucketIndex = GetBucketIndex(delayedMilliSec);
            _buckets[bucketIndex].Add(action);
        }

        private static void StartFutureAction(Action action, int delayedMilliSec)
        {
            // 사용량을 파악하기 위한 통계 변수
            Interlocked.Increment(ref _futureActionStatistics);

            // 먼 미래에 실행하는 작업은 그냥 Task로 실행해버린다. (많지 않을 것으로 가정)
            Task.Delay(delayedMilliSec).ContinueWith(task => Task.Factory.StartNew(action));
        }

        private int GetBucketIndex(int delayedMilliSec)
        {
            // 32ms (1 Resolution) 미만의 작업들은 1번 slot에 들어가도록 한다.
            var relativeIndex = Math.Max(1, delayedMilliSec/Resolution);
            Debug.Assert(relativeIndex < BucketCount);

            var absoluteIndex = (_currentBucketIndex + relativeIndex)%BucketCount;
            return absoluteIndex;
        }

        private class ActionBucket
        {
            private readonly List<Action> _actions = new List<Action>();
            private SpinLock _lock = new SpinLock();

            public void Add(Action action)
            {
                _lock.DoLock(() => _actions.Add(action));
            }

            public void ExecuteAndClear()
            {
                _lock.DoLock(() =>
                    {
                        _actions.ForEach(each => each());
                        _actions.Clear();
                    });
            }
        }
    }
}