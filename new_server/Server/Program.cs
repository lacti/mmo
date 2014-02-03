using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using Server.Attribute;
using Server.Core.Component;
using Server.Core.Data;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Data;
using Server.Extension;
using Server.Extension.Handler;
using Server.Subsystem;

namespace Server
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var options = new CommandOptions(args);

            // Configuration 읽기
            Configuration.Load(options.Get("--config", @"Config.xml"));

            // 설정 덮어쓰기
            options.Override("--port", ref Configuration.Port);
            options.Override("--pylonHost", ref  Configuration.PylonHost);
            options.Override("--pylonPort", ref  Configuration.PylonPort);
            options.Override("--mongodb", ref Configuration.UseMongoDatabase);
            options.Override("--xmldb", ref Configuration.UseXmlDatabase);
            options.Override("--sync", ref Configuration.UseDatabaseSync);

            // Sync를 사용할 경우에는 모든 EntityBind가 등록되어야 한다.
            if (Configuration.UseDatabaseSync)
            {
                Configuration.UseMongoDatabase = true;
                Configuration.UseXmlDatabase = true;
            }

            // Datasheet 읽기
            DataCenter.Instance.Load(Configuration.DatasheetFileDirectory, Configuration.DatasheetFilePattern);

            // 모든 handler 등록 (Sync 쓸 때는 Handler 실행 안되도록 한다.)
            if (!Configuration.UseDatabaseSync)
                HandlerHelper.Register(Assembly.GetExecutingAssembly());

            // System Entity 생성
            EntityManager.Instance.Create(EntityManager.SystemId, DataCenter.Instance.GetCreateInfo(EntityTemplate.Ids.System));

            // Database 연결
            if (Configuration.UseMongoDatabase)
            {
                EntityBinder.Instance.AddBinder(new MongoBind(
                                                    Configuration.MongoDatabaseHost, Configuration.MongoDatabasePort,
                                                    Configuration.MongoDatabaseName, Configuration.MongoDatabaseCollectionName));
            }

            if (Configuration.UseXmlDatabase)
                EntityBinder.Instance.AddBinder(new XmlBind(Configuration.XmlDatabaseDirectory));

            if (Configuration.UseDatabaseSync)
            {
                EntityBinder.Instance.Sync(Configuration.SyncFromDatabaseType, Configuration.SyncToDatabaseType);

                Logger.Write("Sync Completed.");
                Console.ReadLine();
                return;
            }

            // EntityManager caching 및 준비
            EntityBinder.Instance.ForEach(each =>
                {
                    if (each.Has<Player>())
                    {
                        each.Get<Player>().Logged = false;

                        if (each.Has<Nameplate>())
                        {
                            var playerName = each.Get<Nameplate>().Name;
                            NameCache.Instance.Add(playerName, each.Id);
                        }
                    }

                    if (!each.Has<Player>() && each.Has<Pos>())
                        PositionSystem.Global.Appear(each);

                    EntityManager.Instance.Add(each);
                });
            EntityManager.Instance.TrimIdSerial();
            Logger.Write("Player [{0}] loaded.", NameCache.Instance.Count);

            // Network 시작
            var listener = MessageSessionManager.Instance.CreateListener();
            Task.Factory.StartNew(() => listener.Start(Configuration.Port), TaskCreationOptions.LongRunning);

            // Pylon 등록
            PylonConnector.Instance.Start();

            Logger.Write("Server is ready.");
            Console.ReadLine();
        }
    }
}