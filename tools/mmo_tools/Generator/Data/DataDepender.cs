using System.Collections.Generic;

namespace MmoTools.Generator.Data
{
    internal class DataDepender : Dictionary<DataClass, List<DataClass>>
    {
        public static DataDepender Instance = new DataDepender();
    }
}