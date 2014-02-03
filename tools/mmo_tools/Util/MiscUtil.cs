using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace MmoTools.Util
{
    public class MiscUtil
    {
        public static string Md5(string str)
        {
            var builder = new StringBuilder();
            var byteArr = Encoding.ASCII.GetBytes(str);
            var resultArr = (new MD5CryptoServiceProvider()).ComputeHash(byteArr);

            for (var cnti = 1; cnti < resultArr.Length; cnti++)
            {
                builder.Append(resultArr[cnti].ToString("X2"));
            }
            return builder.ToString();
        }

        public static void CreateDirectory(string directory)
        {
            CreateDirectory(new DirectoryInfo((directory)));
        }

        private static void CreateDirectory(DirectoryInfo directory)
        {
            if (directory.Parent != null) CreateDirectory(directory.Parent);
            if (!directory.Exists) directory.Create();
        }
    }
}