using System;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace Server.Core.Util
{
    public static class AsyncSocketExtension
    {
        public static Task<Socket> AcceptAsync(this Socket socket)
        {
            var source = new TaskCompletionSource<Socket>(socket);
            socket.BeginAccept(state =>
                {
                    try
                    {
                        source.SetResult(socket.EndAccept(state));
                    }
                    catch (Exception e)
                    {
                        source.SetException(e);
                    }
                }, source);
            return source.Task;
        }

        public static Task<int> ReceiveAsync(this Socket socket, byte[] buffer, int offset = 0, int size = -1,
                                             SocketFlags flags = SocketFlags.None)
        {
            if (size < 0)
                size = buffer.Length;

            var source = new TaskCompletionSource<int>(socket);
            socket.BeginReceive(buffer, offset, size, flags, state =>
                {
                    try
                    {
                        source.SetResult(socket.EndReceive(state));
                    }
                    catch (Exception e)
                    {
                        source.SetException(e);
                    }
                }, source);
            return source.Task;
        }

        public static async Task<byte[]> ReceiveAsync(this Socket socket, int count)
        {
            if (count < 0)
                throw new ArgumentOutOfRangeException("count", "count cannot be negative.");

            if (count == 0)
                return new byte[0];

            var buffer = new byte[count];
            var length = 0;
            do
            {
                var num = await ReceiveAsync(socket, buffer, length, count);
                if (num == 0)
                    break;

                length += num;
                count -= num;
            } while (count > 0);

            if (length != buffer.Length)
                throw new IOException("packet is truncated.");

            return buffer;
        }

        public static Task<int> SendAsync(this Socket socket, byte[] buffer, int offset = 0, int size = -1,
                                          SocketFlags flags = SocketFlags.None)
        {
            if (size < 0)
                size = buffer.Length;

            var source = new TaskCompletionSource<int>(socket);
            socket.BeginSend(buffer, offset, size, flags, state =>
                {
                    try
                    {
                        source.SetResult(socket.EndSend(state));
                    }
                    catch (Exception e)
                    {
                        source.SetException(e);
                    }
                }, source);
            return source.Task;
        }
    }
}