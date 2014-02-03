using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using Redmine.Net.Api;
using Redmine.Net.Api.Types;

namespace deploy
{
    public class ServerDeploy : Deploy
    {
        protected override void ExecuteInternal(string history)
        {
            Log(Color.Yellow, " * Build Start.");
            var buildResult = BuildProject(@"..\new_server\Server.sln", "Release|Any CPU");

            Log(Color.Cyan, 4, buildResult.Stdout.Trim());
            Log(Color.Yellow, " * Build Ok.");

            foreach (var each in File.ReadAllLines("deploy_server.list").Where(e => !string.IsNullOrWhiteSpace(e)))
            {
                var pair = each.Split(',');
                CopyFiles(pair[0], pair[1]);
            }
            Log(Color.Yellow, " * Copy Ok.");

            ModifyXml(@"server\Config.xml", "/Config/Data/@directory", "data");
            Log(Color.Yellow, " * Edit Config.");

            MakeHash("server");

            Log(Color.Yellow, " * Upload Start.");
            var uploadResult = UploadFile("server");

            Log(Color.Cyan, 4, uploadResult.Stdout.Trim());
            Log(Color.Yellow, " * Upload Ok.");

            Log(Color.Yellow, " * Write History.");
            WriteDeployHistory("서버", history);
            Log(Color.Yellow, " * Write History Ok.");
        }
    }
}
