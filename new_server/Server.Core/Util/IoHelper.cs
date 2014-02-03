using System.IO;

namespace Server.Core.Util
{
    public class IoHelper
    {
        public static void CreateDirectory(string directory)
        {
            CreateDirectory(new DirectoryInfo(directory));
        }

        public static void CreateDirectory(DirectoryInfo dirInfo)
        {
            if (dirInfo.Parent != null && dirInfo.Parent.Exists) CreateDirectory(dirInfo.Parent);
            if (!dirInfo.Exists) dirInfo.Create();
        }
    }
}