using System.Drawing;
using System.IO;
using System.Linq;

namespace deploy
{
    public class ClientDeploy : Deploy
    {
        protected override void ExecuteInternal(string history)
        {
            Log(Color.Yellow, " * Build Start.");
            var buildResult = BuildProject(@"..\client.sln", "Release|Win32");

            Log(Color.Cyan, 4, buildResult.Stdout.Trim());
            Log(Color.Yellow, " * Build Ok.");

            foreach (var each in File.ReadAllLines("deploy_client.list").Where(e => !string.IsNullOrWhiteSpace(e)))
            {
                var pair = each.Split(',');
                CopyFiles(pair[0], pair[1]);
            }
            Log(Color.Yellow, " * Copy Ok.");

            MakeHash("client");

            Log(Color.Yellow, " * Upload Start.");
            var uploadResult = UploadFile("client");

            Log(Color.Cyan, 4, uploadResult.Stdout.Trim());
            Log(Color.Yellow, " * Upload Ok.");

            Log(Color.Yellow, " * Write History.");
            WriteDeployHistory("클라이언트", history);
            Log(Color.Yellow, " * Write History Ok.");
        }
    }
}
