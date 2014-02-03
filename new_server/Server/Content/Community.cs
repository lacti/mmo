using System.Text;
using Server.Core.Command;
using Server.Core.Component;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Extension;
using Server.Extension.Handler;
using Server.Message.Game;
using Server.Message.Resource;

namespace Server.Content
{
    public class Community : IHandler
    {
        [MessageHandler]
        internal void OnChat(Entity entity, MessageSession session, ChatMsg msg)
        {
            if (string.IsNullOrWhiteSpace(msg.Message))
                return;

            if (!CheckAndProcessAdminCommand(entity, msg.Message))
                entity.Broadcast(msg);
        }

        private static bool CheckAndProcessAdminCommand(Entity entity, string message)
        {
            if (!CommandMap.Instance.IsAdminCommand(message))
                return false;

            bool success;
            if (!CommandMap.Instance.Dispatch(entity, message, out success))
                return false;

            var report = message + " -> " + (success ? "ok." : "fail.");
            var msg = new ChatMsg {Id = entity.Id, Message = report};

            Logger.Write(report);
            entity.Broadcast(msg);

            return true;
        }

        [CommandHandler("talk", "특정 Entity가 지정된 대사를 하도록 합니다")]
        internal bool TalkToOther(Entity admin,
                                  [CommandArgument("메시지를 출력할 대상")] Entity target,
                                  [CommandArgument("출력할 메시지", CommandArgumentOption.AfterAll)] string message)
        {
            target.Broadcast(new ChatMsg { Id = target.Id, Message = message });
            return true;
        }

        [CommandHandler("speech", "지정된 텍스트를 음성으로 출력합니다")]
        internal bool Speech(Entity admin,
                             [CommandArgument("음성으로 출력할 메시지", CommandArgumentOption.AfterAll)] string message)
        {
            if (string.IsNullOrWhiteSpace(message))
                return false;

            VoiceReader.Read(message).ContinueWith(task =>
            {
                var bytes = task.Result;
                var session = admin.GetSession();
                session.Send(new VoiceMsg { Id = admin.Id, Mp3 = bytes });
            });
            return true;
        }

        [CommandHandler("help", "명령어를 확인합니다")]
        internal bool SendHelpOfCommands(Entity admin)
        {
            var message = new StringBuilder();
            foreach (var info in CommandMap.Instance.HandlerInfos)
            {
                message.AppendLine(string.Format(".{0} -> {1}", info.Command, info.Description));

                // 첫 번째 인자로 들어가는 admin은 생략한다.
                for (var paramIndex = 1; paramIndex < info.Parameters.Length; ++paramIndex)
                {
                    var param = info.Parameters[paramIndex];
                    message.AppendLine(string.Format("  [{0}] {1} ({2})", paramIndex, param.Description, param.ActualType.Name));
                }
            }

            admin.GetSession().Send(new ChatMsg { Id = EntityManager.SystemId, Message = message.ToString()});
            return true;
        }
    }
}