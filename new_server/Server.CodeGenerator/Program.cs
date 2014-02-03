using Server.CodeGenerator.Generator;

namespace Server.CodeGenerator
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var generators = new IGenerator[]
                {
                    new MessageGenerator(),
                    new DataGenerator(),
                };

            foreach (var each in generators)
                each.Generate();
        }
    }
}