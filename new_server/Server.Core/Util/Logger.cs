using System;
using System.Collections.Concurrent;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public class Logger
    {
        private static readonly Logger Instance = new Logger();
        private readonly BlockingCollection<string> _logQueue = new BlockingCollection<string>();

        private Logger()
        {
            IoHelper.CreateDirectory(Configuration.LogDirectory);
            Task.Factory.StartNew(WriteLoop, TaskCreationOptions.LongRunning);
        }

        public static void Write(object value)
        {
            Instance.EnqueueMessage(Convert.ToString(value));
        }

        public static void Write(string format, params object[] args)
        {
            Instance.EnqueueMessage(string.Format(format, args));
        }

        public static void Write(Exception e)
        {
            Write(e.GetType() + ": " + e.Message);
            Write(e.StackTrace);
            if (e.InnerException != null)
            {
                Write(e.InnerException);
            }
        }

        private void EnqueueMessage(string message)
        {
            var log = string.Format("[{0}] {1}\r\n",
                                    DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"), message);
            _logQueue.Add(log);
        }

        private void WriteLoop()
        {
            while (true)
            {
                var log = _logQueue.Take();
                WriteInternal(log);
            }
// ReSharper disable FunctionNeverReturns
        }

// ReSharper restore FunctionNeverReturns

        private static void WriteInternal(string log)
        {
            var logFile = Path.Combine(Configuration.LogDirectory,
                                       string.Format("mmo-server-{0}.log", DateTime.Now.ToString("yyMMdd")));

            if (File.Exists(logFile))
            {
                File.AppendAllText(logFile, log, Encoding.UTF8);
            }
            else
            {
                File.WriteAllText(logFile, log, Encoding.UTF8);
            }

            if (Configuration.UseLogConsole)
            {
                Console.Write(log);
            }
        }
    }
}