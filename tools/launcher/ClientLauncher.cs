using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using Server.Core.Util;
using Server.Message.Pylon;

namespace launcher
{
    internal class ClientLauncher
    {
        public static readonly ClientLauncher Instance = new ClientLauncher();

        private readonly List<ServerMsg> _serverMsgs = new List<ServerMsg>();
        private readonly ReaderWriterLockSlim _serversLock = new ReaderWriterLockSlim();

        public List<ServerMsg> Servers
        {
            get { return _serversLock.DoRead(() => _serverMsgs.ToList()); }
            set
            {
                _serversLock.DoWrite(() =>
                    {
                        _serverMsgs.Clear();
                        _serverMsgs.AddRange(value);
                    });
            }
        }

        public static string ClientPath
        {
            get { return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "mmo"); }
        }

        public static string ClientExecutablePath
        {
            get { return Path.Combine(ClientPath, "clientsdl.exe"); }
        }

        public void Execute(int serverIndex, string userName)
        {
            if (string.IsNullOrWhiteSpace(userName))
                throw new InvalidOperationException("Invalid username");

            if (!File.Exists(ClientExecutablePath))
                throw new InvalidOperationException("Cannot find a client binary.\nPlease restart this launcher.");

            var server =
                _serversLock.DoRead(
                    () => serverIndex >= 0 && serverIndex < _serverMsgs.Count ? _serverMsgs[serverIndex] : null);
            if (server == null)
                throw new InvalidOperationException("No server selected.");

            using (var clientProcess = new Process())
            {
                clientProcess.StartInfo.FileName = ClientExecutablePath;
                clientProcess.StartInfo.Arguments = string.Format(@"--username ""{0}"" --server ""{1}""", userName,
                                                                  server.Host);
                clientProcess.StartInfo.WorkingDirectory = ClientPath;
                clientProcess.StartInfo.WindowStyle = ProcessWindowStyle.Normal;
                clientProcess.Start();
            }
        }
    }
}