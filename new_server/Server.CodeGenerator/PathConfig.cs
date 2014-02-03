using System.IO;
using System.Reflection;

namespace Server.CodeGenerator
{
    internal class PathConfig
    {
        public static string ExecutablePath
        {
            get { return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location); }
        }

        public static string DataPath
        {
            get { return Path.Combine(SolutionPath, @"..\resources\data"); }
        }

        public static string MessageXmlPath
        {
            get { return Path.Combine(DataPath, "msg.xml"); }
        }

        public static string DataXmlPath
        {
            get { return Path.Combine(DataPath, "mmo-data.xml"); }
        }

        public static string SolutionPath
        {
            get { return Path.Combine(ExecutablePath, "..", "..", ".."); }
        }

        public static string MessageProjectPath
        {
            get { return Path.Combine(SolutionPath, "Server.Message"); }
        }

        public static string MessageProjectFilePath
        {
            get { return Path.Combine(MessageProjectPath, "Server.Message.csproj"); }
        }

        public static string DataProjectPath
        {
            get { return Path.Combine(SolutionPath, "Server.Data"); }
        }

        public static string DataProjectFilePath
        {
            get { return Path.Combine(DataProjectPath, "Server.Data.csproj"); }
        }

        public static string ServerProjectPath
        {
            get { return Path.Combine(SolutionPath, "Server"); }
        }
    }
}