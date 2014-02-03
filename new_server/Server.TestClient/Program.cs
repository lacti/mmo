using System;
using Server.Core.Data;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Data;
using Server.Extension.Handler;
using Server.Message.Game;
using System.Diagnostics;
using Server.Message.Resource;

namespace Server.TestClient
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            // Configuration 읽기
            Configuration.Load(@"Config.xml");

            // Datasheet 읽기
            DataCenter.Instance.Load(Configuration.DatasheetFileDirectory, Configuration.DatasheetFilePattern);

            // ReSharper disable ConvertToLambdaExpression
            DispatchHandler.Instance
                .AddHandler<WorldInfoMsg>(m =>
                    {
                        Logger.Write(m.WorldId);
                        Logger.Write(m.SpawnList.Count);
                    })
                .AddHandler<SpawnMsg>(m =>
                    {
                        Logger.Write(m.Name);
                        Logger.Write(m.UpdatePosition);
                    })
                .AddHandler<ChatMsg>(m =>
                    {
                        Logger.Write(m.Message);
                    })
                .AddHandler<UpdateHpMsg>(m =>
                    {
                        Logger.Write("{0} - {1}/{2}", m.Id, m.Hp, m.MaxHp);
                    })
                .AddHandler<VoiceMsg>(m =>
                    {
                        Logger.Write("{0} - {1}", m.Id, m.Mp3.Length);
                    });

            // ReSharper restore ConvertToLambdaExpression

            var session = MessageSessionManager.Instance.CreateSession();
            session.Connect("127.0.0.1", Configuration.Port);
            session.StartReceive();

            session.Send(new EnterWorldMsg { Name = "한글 테스트3" });
            session.Send(new ChatMsg { Id = 0, Message = ".spawn" });

            var heal = DataCenter.Instance.GetDatum<InteractTemplate>((int)InteractTemplate.Ids.Heal);
            Debug.Assert(heal != null);

            session.Send(new InteractMsg {InteractId = heal.Id});

            // session.Send(new ChatMsg {Id = 0, Message = ".speech 한글 음성을 테스트합니다."});
            session.Send(new ChatMsg { Id = 0, Message = ".help" });

            Console.WriteLine("wait.");
            Console.ReadKey();
        }
    }
}