using System.Collections.Generic;

namespace MmoTools.Generator.Data
{
    internal class DataCenter : Dictionary<string, DataClass>
    {
        public static DataCenter Instance = new DataCenter();
    }
}