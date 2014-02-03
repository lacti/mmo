using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace launcher
{
    internal class ServerLauncher
    {
        public static string ServerPath
        {
            get { return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "mmo", "server"); }
        }

        public static string ServerExecutablePath
        {
            get { return Path.Combine(ServerPath, "Server.exe"); }
        }

        public static void Execute()
        {
            if (!File.Exists(ServerExecutablePath))
                throw new InvalidOperationException("Cannot find a client binary.\nPlease restart this launcher.");

            using (var serverProcess = new Process())
            {
                serverProcess.StartInfo.FileName = ServerExecutablePath;
                serverProcess.StartInfo.WorkingDirectory = ServerPath;
                serverProcess.StartInfo.WindowStyle = ProcessWindowStyle.Normal;
                serverProcess.Start();
            }
        }
    }
}
