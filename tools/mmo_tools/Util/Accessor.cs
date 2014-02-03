namespace MmoTools.Util
{

    #region Accessor

    public class Accessor
    {
        public bool Pointer { get; set; }
        public string Name { get; set; }

        public override string ToString()
        {
            return Pointer ? (Name + "->") : (Name + ".");
        }
    }

    #endregion

    #region SourceCode

    #endregion
}