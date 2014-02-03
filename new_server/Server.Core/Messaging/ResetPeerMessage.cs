namespace Server.Core.Messaging
{
    public class ResetPeerMessage : IMessage
    {
        public const int TypeId = 65535;

        public int Id { get; set; }
        public int ErrorCode { get; set; }
    }
}