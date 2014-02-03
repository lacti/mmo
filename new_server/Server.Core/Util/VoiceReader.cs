using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using SwfDotNet.IO;
using SwfDotNet.IO.Tags;

namespace Server.Core.Util
{
    public static class VoiceReader
    {
        public static async Task<byte[]> Read(string text)
        {
            const string baseUrl = "http://www.neospeech.com/GetAudio1.ashx?speaker=10&content=";
            var url = baseUrl + text;
            using (var client = new WebClient())
            {
                var bytes = await client.DownloadDataTaskAsync(url);
                return bytes;
            }
        }

        [Obsolete]
        public static async Task<byte[]> ReadOldApi(string text)
        {
            var cookies = new CookieContainer();
            var handler = new HttpClientHandler
                {
                    CookieContainer = cookies,
                    UseCookies = true,
                    UseDefaultCredentials = false,
                };
            var client = new HttpClient(handler);
            await client.GetAsync("http://text-to-speech.imtranslator.net/speech.asp?dir=ko");

            var req = new HttpRequestMessage(HttpMethod.Post,
                                             "http://text-to-speech.imtranslator.net/sockets/box.asp");
            req.Headers.Referrer = new Uri("http://text-to-speech.imtranslator.net/speech.asp?dir=ko", UriKind.Absolute);
            req.Content = new FormUrlEncodedContent(new[]
                {
                    new KeyValuePair<string, string>("text", text),
                });
            await client.SendAsync(req);
            var response = await client.GetAsync(
                "http://text-to-speech.imtranslator.net/sockets/tts.asp?speed=0&url=F_TR&dir=ko&B=1&ID=735231370&chr=MiaHead&vc=VW%20Yumi");
            var content = await response.Content.ReadAsStringAsync();
            var target = content.Substring("<OBJECT");
            var url = target.Extract("VALUE=\"", "\"");
            Logger.Write(url);

            var bytes = await client.GetByteArrayAsync(url);

            var tempSwfFile = Path.GetTempPath() + ".swf";
            File.WriteAllBytes(tempSwfFile, bytes);

            var swfReader = new SwfReader(tempSwfFile);
            var swf = swfReader.ReadSwf();

            using (var outStream = new MemoryStream())
            {
                foreach (var tag in swf.Tags)
                {
                    var blockTag = tag as SoundStreamBlockTag;
                    if (blockTag == null)
                        continue;

                    var soundTag = blockTag;
                    var data = soundTag.SoundData;
                    outStream.Write(data, 4, data.Length - 4);
                }
                return outStream.ToArray();
            }
        }
    }
}