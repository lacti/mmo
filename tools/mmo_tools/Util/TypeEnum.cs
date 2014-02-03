namespace MmoTools.Util
{
    public enum TypeEnum
    {
        #region Defaults

        BOOL,
        CHAR,
        SHORT,
        INT,
        LONG,
        INT64,
        FLOAT,
        DOUBLE,
        STRING,

        #endregion

        #region Message

        BIN,

        #endregion

        #region CBES

        ENTITY_ID,

        #endregion

        #region mmo-Database

        DATA_ID,
        DATA_POS,
        DATA_REFERENCE,
        DATA_EXPRESSION,
        DATA_ENUM,

        #endregion
    }
}