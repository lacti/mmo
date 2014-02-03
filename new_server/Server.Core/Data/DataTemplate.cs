using System.Collections.Generic;

namespace Server.Core.Data
{
    public class DataTemplate
    {
        public DataTemplate()
        {
            Attributes = new Dictionary<string, List<string>>();
            Children = new List<DataTemplate>();
        }

        public string Name { get; set; }
        public DataTemplate Parent { get; set; }
        public Dictionary<string, List<string>> Attributes { get; private set; }
        public List<DataTemplate> Children { get; private set; }

        public bool IsRoot
        {
            get { return Parent == null; }
        }

        public void AddAttribute(string name, string value)
        {
            if (!Attributes.ContainsKey(name))
                Attributes.Add(name, new List<string>());
            Attributes[name].Add(value);
        }

        public void AddChild(DataTemplate child)
        {
            child.Parent = this;
            Children.Add(child);
        }
    }
}