using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Xml;

namespace Server.Core.Util
{
    public class Configuration
    {
        public static string ExecutablePath
        {
            get { return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location); }
        }

        #region Logging

        [XPath(XPath = "/Config/Logging/@directory", Default = "Log")]
        public static string LogDirectory;

        [XPath(XPath = "/Config/Logging/@console", Default = true)]
        public static bool UseLogConsole;

        #endregion

        #region Network

        [XPath(XPath = "/Config/Network/@port", Default = 40000)]
        public static int Port;

        [XPath(XPath = "/Config/Pylon/@host", Default = "mmo.pe.kr")]
        public static string PylonHost;

        [XPath(XPath = "/Config/Pylon/@port", Default = 40001)]
        public static int PylonPort;

        [XPath(XPath = "/Config/Pylon/@port", Default = 40002)]
        public static int PylonWebPort;

        #endregion

        #region Data

        [XPath(XPath = "/Config/Data/@directory", Default = @"../../../../resources/data")]
        public static string DatasheetFileDirectory;

        [XPath(XPath = "/Config/Data/@pattern", Default = "mmo-data.xml")]
        public static string DatasheetFilePattern;

        #endregion

        #region Database

        [XPath(XPath = "/Config/Database/@mongo", Default = true)]
        public static bool UseMongoDatabase;

        [XPath(XPath = "/Config/Database/@xml", Default = false)]
        public static bool UseXmlDatabase;

        [XPath(XPath = "/Config/Database/@do-sync", Default = false)]
        public static bool UseDatabaseSync;

        [XPath(XPath = "/Config/Database/@sync-from", Default = "Server.Core.Component.MongoBind")]
        public static string SyncFromDatabaseType;

        [XPath(XPath = "/Config/Database/@sync-to", Default = "Server.Core.Component.XmlBind")]
        public static string SyncToDatabaseType;


        [XPath(XPath = "/Config/MongoDatabase/@host", Default = "mmo.pe.kr")]
        public static string MongoDatabaseHost;

        [XPath(XPath = "/Config/MongoDatabase/@port", Default = 27017)]
        public static int MongoDatabasePort;

        [XPath(XPath = "/Config/MongoDatabase/@database", Default = "mmo")]
        public static string MongoDatabaseName;

        [XPath(XPath = "/Config/MongoDatabase/@collection", Default = "entity")]
        public static string MongoDatabaseCollectionName;


        [XPath(XPath = "/Config/XmlDatabase/@directory", Default = "XmlDb")]
        public static string XmlDatabaseDirectory;

        #endregion

        #region Assemblies

        [XPath(XPath = "/Config/Assembly/@message", Default = "Server.Message.dll")]
        public static string MessageAssemblyName;

        [XPath(XPath = "/Config/Assembly/@attribute", Default = "Server.Attribute.dll")]
        public static string AttributeAssemblyName;

        [XPath(XPath = "/Config/Assembly/@data", Default = "Server.Data.dll")]
        public static string DataAssemblyName;

        public static Assembly MessageAssembly
        {
            get { return GetAssembly(MessageAssemblyName); }
        }

        public static Assembly AttributeAssembly
        {
            get { return GetAssembly(AttributeAssemblyName); }
        }

        public static Assembly DataAssembly
        {
            get { return GetAssembly(DataAssemblyName); }
        }

        public static string ServerName
        {
            get { return Environment.UserName; }
        }

        public static string HostAddress
        {
            get { return SocketHelper.GetHostIpAddress(); }
        }

        #endregion

        #region AssemblyCache

        private static readonly Dictionary<string, Assembly> AssemblyCache = new Dictionary<string, Assembly>();

        private static Assembly GetAssembly(string dllName)
        {
            if (!AssemblyCache.ContainsKey(dllName))
            {
                try
                {
                    AssemblyCache.Add(dllName, Assembly.LoadFile(Path.Combine(ExecutablePath, dllName)));
                }
                catch (Exception e)
                {
                    Logger.Write("Cannot load assembly: " + dllName);
                    Logger.Write(e);
                    Debug.Assert(false);
                }
            }

            return AssemblyCache[dllName];
        }

        #endregion

        #region Init & Load

        static Configuration()
        {
            foreach (var field in typeof(Configuration).GetFields(BindingFlags.Public | BindingFlags.Static))
            {
                var attribute = field.GetCustomAttribute<XPathAttribute>();
                if (attribute != null && attribute.Default != null)
                    field.SetValue(null, attribute.Default);
            }
        }

        public static void Load(string configPath)
        {
            if (!File.Exists(configPath))
                return;

            var doc = new XmlDocument();
            doc.Load(configPath);

            foreach (var field in typeof(Configuration).GetFields(BindingFlags.Public | BindingFlags.Static))
            {
                var attribute = field.GetCustomAttribute<XPathAttribute>();
                if (attribute == null)
                    continue;

                var node = doc.SelectSingleNode(attribute.XPath);
                if (node == null)
                    continue;

                field.SetValue(null, Convert.ChangeType(node.Value, field.FieldType));
            }
        }

        #endregion
    }

    [AttributeUsage(AttributeTargets.Field, Inherited = false, AllowMultiple = false)]
    internal class XPathAttribute : Attribute
    {
        public string XPath;
        public object Default;
    }
}