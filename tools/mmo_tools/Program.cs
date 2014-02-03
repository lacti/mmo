using System.IO;
using System.Threading.Tasks;
using System.Xml;
using MmoTools.Base;
using MmoTools.Generator.Attribute;
using MmoTools.Generator.Data;
using MmoTools.Generator.Message;
using MmoTools.Util;

namespace MmoTools
{
    internal class Program
    {
        public static void Main(string[] args)
        {
            var targetDirectory = Path.GetFullPath(args.Length > 0 ? args[0] : ".");
            var outputDirectory = Path.GetFullPath(args.Length > 1 ? args[1] : ".");
            var options = args.Length > 2 ? args[2] : "";

            if (!Directory.Exists(targetDirectory))
                return;

            if (!Directory.Exists(outputDirectory))
                MiscUtil.CreateDirectory(outputDirectory);

            var generators = new IGenerator[]
                {
                    new MessageCodeGenerator(outputDirectory, options),
                    new DataCodeGenerator(outputDirectory),
                    new AttributeCodeGenerator(outputDirectory)
                };

            var targetFiles = Directory.GetFiles(targetDirectory, "*.xml", SearchOption.AllDirectories);
            Parallel.ForEach(targetFiles, eachFile =>
                {
                    var document = new XmlDocument();
                    document.Load(eachFile);

                    Parallel.ForEach(generators, e => e.Load(eachFile, document));
                });

            Parallel.ForEach(generators, e => e.Generate());
        }
    }
}