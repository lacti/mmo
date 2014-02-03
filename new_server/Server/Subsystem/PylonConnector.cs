using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Extension.Handler;
using Server.Message.Pylon;

namespace Server.Subsystem
{
    public class PylonConnector : IHandler
    {
        public static PylonConnector Instance { get; private set; }

        private const int RetryInterval = 10000;

        private MessageSession _session;
        private readonly ManualResetEvent _event;

        public PylonConnector()
        {
            // IHandler에 의해 Handler가 등록되는 과정에서 객체가 만들어지게 되므로, 중복 생성하면 안된다.
            Debug.Assert(Instance == null);
            Instance = this;

            _event = new ManualResetEvent(false);
        }

        public void Start()
        {
            Task.Factory.StartNew(ConnectionLoop);
        }

        private void ConnectionLoop()
        {
            while (true)
            {
                try
                {
                    _event.Reset();

                    Logger.Write("Try to connect Pylon({0}:{1})", Configuration.PylonHost, Configuration.PylonPort);
                    _session = MessageSessionManager.Instance.CreateSession();
                    _session.Connect(Configuration.PylonHost, Configuration.PylonPort);

                    Task.Factory.StartNew(_session.StartReceive);
                    _session.Send(new ServerMsg
                        {
                            Host = Configuration.HostAddress,
                            Port = Configuration.Port,
                            Name = Configuration.ServerName
                        });

                    Logger.Write("Pylon Connected.");

                    _event.WaitOne();
                }
#if DEBUG
                catch (Exception e)
                {
                    Logger.Write("Cannot connect to Pylon");
                    Logger.Write(e);

                    Thread.Sleep(RetryInterval);
                }
#else
                catch
                {
                    Logger.Write("Cannot connect to Pylon");
                    Thread.Sleep(RetryInterval);
                }
#endif
            }
// ReSharper disable FunctionNeverReturns
        }
// ReSharper restore FunctionNeverReturns


        [MessageHandler]
        internal void ResetPeer(MessageSession session, ResetPeerMessage msg)
        {
            if (session != _session)
                return;

            Logger.Write("Pylon Disconnected.");
            _event.Set();
        }

        [MessageHandler]
        internal void ProcessPing(MessageSession session, AlivePingMsg msg)
        {
            if (session != _session)
                return;

            // ping을 받았으니 pong을 보낸다.
            session.Send(new AlivePongMsg());
        }
    }
}
