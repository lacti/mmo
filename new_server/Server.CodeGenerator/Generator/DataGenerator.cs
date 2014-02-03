using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Server.Core.Data;
using Server.Core.Util;

namespace Server.CodeGenerator.Generator
{
    internal class DataGenerator : IGenerator
    {
        private readonly DataTemplateDefine _define = new DataTemplateDefine();

        public void Generate()
        {
            _define.Load(PathConfig.DataXmlPath);

            var generatedFiles = new HashSet<string>();
            foreach (var template in _define.Templates)
            {
                var code = new SourceCode();
                code.Append("using Server.Core.Data;");
                code.Append("using System.Collections.Generic;");
                code.NewLine();

                code.Append("namespace Server.Data");
                code.BracketStart();

                WriteTemplateToCode(template, code);

                code.BracketEnd();
                code.NewLine();

                var dataFileName = template.Name + ".cs";
                var dataFilePath = Path.Combine(PathConfig.DataProjectPath, dataFileName);
                code.WriteToFile(dataFilePath);
                generatedFiles.Add(dataFileName);
            }

            GeneratorHelper.UpdateProjectFile(PathConfig.DataProjectFilePath, generatedFiles);
        }

        private void WriteTemplateToCode(DataTemplate dataTemplate, SourceCode code)
        {
            var templateCanonicalName = _define.GetCanonicalName(dataTemplate);
            // root만 IData를 상속 받는다.
            code.Append(dataTemplate.Parent.IsRoot ? "public class {0} : IData" : "public class {0}",
                        templateCanonicalName);

            code.BracketStart();

            foreach (var child in dataTemplate.Children)
                WriteTemplateToCode(child, code);

            foreach (var attribute in dataTemplate.Attributes)
            {
                var valueType = DataTemplateHelper.InferenceType(attribute.Value);
                if (valueType == "enum")
                {
                    var enumName = attribute.Key + (attribute.Key.EndsWith("s") || attribute.Key.EndsWith("x") ? "es" : "s");
                    if (attribute.Key.Equals("id", StringComparison.OrdinalIgnoreCase)) 
                        code.Append("public int {0} {{ get; set; }}", attribute.Key);
                    else
                        code.Append("public {0} {1} {{ get; set; }}", enumName, attribute.Key);

                    code.Append("public enum {0}", enumName);
                    code.BracketStart();
                    foreach (var value in attribute.Value.Distinct())
                        code.Append("{0},", value.Substring(1).ToCamelCase());  // remove leading _ character.
                    code.BracketEnd();
                }
                else code.Append("public {0} {1} {{ get; set; }}", valueType, attribute.Key);
            }

            foreach (var attributeName in from e in dataTemplate.Attributes where e.Key.EndsWith("Ref") select e.Key)
            {
                // Ref를 제거하고 남은 이름
                var typeName = attributeName.Substring(0, attributeName.Length - 3);
                code.Append("public {0} {0} {{ get {{ return DataCenter.Instance[typeof ({0}), {1}] as {0}; }} }}",
                            typeName, attributeName);
            }

            foreach (var child in dataTemplate.Children)
                code.Append("public List<{0}> {0}s {{ get; private set; }}", _define.GetCanonicalName(child));

            if (dataTemplate.Children.Count > 0)
            {
                code.NewLine();
                code.Append("public {0}()", templateCanonicalName);
                code.BracketStart();
                foreach (var child in dataTemplate.Children)
                    code.Append("{0}s = new List<{0}>();", _define.GetCanonicalName(child));
                code.BracketEnd();
            }

            code.BracketEnd();
            code.NewLine();
        }
    }
}