using System;
using System.Threading;

namespace Server.Core.Util
{
    public static class LockHelper
    {
        public static void DoRead(this ReaderWriterLockSlim locker, Action action)
        {
            locker.EnterReadLock();
            try
            {
                action();
            }
            finally
            {
                locker.ExitReadLock();
            }
        }

        public static T DoRead<T>(this ReaderWriterLockSlim locker, Func<T> action)
        {
            locker.EnterReadLock();
            try
            {
                return action();
            }
            finally
            {
                locker.ExitReadLock();
            }
        }

        public static void DoWrite(this ReaderWriterLockSlim locker, Action action)
        {
            locker.EnterWriteLock();
            try
            {
                action();
            }
            finally
            {
                locker.ExitWriteLock();
            }
        }

        public static T DoWrite<T>(this ReaderWriterLockSlim locker, Func<T> action)
        {
            locker.EnterWriteLock();
            try
            {
                return action();
            }
            finally
            {
                locker.ExitWriteLock();
            }
        }

        public static void DoLock(this SpinLock locker, Action action, int maximumRetryCount = 4096)
        {
            var retryCount = 0;
            var acquired = false;
            while (!acquired)
            {
                locker.Enter(ref acquired);
                if (++retryCount > maximumRetryCount)
                    throw new InvalidOperationException("lock timeout");
            }

            action();
            locker.Exit();
        }
    }
}