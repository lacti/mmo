using System.Globalization;

namespace MmoTools.Util
{
    public class NameStack
    {
        public string Name { get; set; }
        public int Level { get; set; }

        public string Current
        {
            get { return Name + (Level == 0 ? "" : Level.ToString(CultureInfo.InvariantCulture)); }
        }
    }
}