using System.Xml;

namespace MmoTools.Base
{
    public interface IGenerator
    {
        string DocumentElementName { get; }

        void Load(string fileName, XmlDocument document);
        void Generate();
    }
}