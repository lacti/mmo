#region

using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

#endregion

namespace Server.Core.Util
{
    public class SourceCode : ICode
    {
        private static readonly Regex WhitespaceRemover = new Regex(@"\s+", RegexOptions.Compiled);
        private readonly Block _main = new Block {Parent = null};
        private Block _currentBlock;
        private int _markerIndex = -1;

        public SourceCode()
        {
            _currentBlock = _main;
        }

        public SourceCode(string oneLine)
        {
            _currentBlock = _main;
            Append(oneLine);
        }

        public string Generate()
        {
            return _main.Generate();
        }

        public SourceCode Append(string format, params object[] args)
        {
            if (args == null || args.Length == 0)
                return Append(new Line {Code = format});
            return Append(string.Format(format, args));
        }

        public SourceCode NewLine()
        {
            return Append("");
        }

        public SourceCode Append(SourceCode other)
        {
            other._main.Codes.ForEach(c => Append(c));
            other._main.Parent = _currentBlock;
            return this;
        }

        private SourceCode Append(ICode code)
        {
            if (_markerIndex < 0)
                _currentBlock.Codes.Add(code);
            else
            {
                _currentBlock.Codes.Insert(_markerIndex, code);
                ++_markerIndex;
            }
            return this;
        }

        public void AddMarker(string name)
        {
            var namedBlock = new Block {Parent = _currentBlock, Name = name};
            _currentBlock.Codes.Add(namedBlock);
        }

        public SourceCode IndentRight()
        {
            var newBlock = new Block {Parent = _currentBlock};
            _currentBlock.Codes.Add(newBlock);
            _currentBlock = newBlock;
            return this;
        }

        public SourceCode IndentLeft()
        {
            _currentBlock = _currentBlock.Parent;
            return this;
        }

        public SourceCode GoMarker(string markerName)
        {
            var target = FindMarker(_main, markerName);
            if (target != null)
            {
                _currentBlock = target.Parent;
                _markerIndex = _currentBlock.Codes.IndexOf(target);
                return this;
            }
            return null;
        }

        public SourceCode ResetMarker()
        {
            _markerIndex = -1;
            return this;
        }

        private Block FindMarker(Block block, string markerName)
        {
            var target = (Block) block.Codes.Find(c => c is Block && markerName.Equals(((Block) c).Name));
            if (target != null)
                return target;

            foreach (var each in from e in block.Codes where e is Block select e as Block)
            {
                target = FindMarker(each, markerName);
                if (target != null)
                    return target;
            }
            return null;
        }

        public bool WriteToFile(string filePath, params string[] prefixes)
        {
            var originalContent = "";
            if (File.Exists(filePath))
                originalContent = File.ReadAllText(filePath, Encoding.UTF8);

            var newContent = "/* this code is auto-generated. */\n" + string.Join("\n", prefixes) + "\n" + Generate();
            if (WhitespaceRemover.Replace(originalContent, "").Equals(WhitespaceRemover.Replace(newContent, "")))
                return false;

            File.WriteAllText(filePath, newContent, Encoding.UTF8);
            return true;
        }

        public SourceCode IndentedReturn(string value)
        {
            return IndentRight().Append("return {0};", value).IndentLeft();
        }

        public SourceCode BracketStart()
        {
            return BracketStart(null);
        }

        public SourceCode BracketStart(string format, params object[] args)
        {
            if (args == null || args.Length == 0)
                return Append(format != null ? format + " {" : "{").IndentRight();
            return Append(string.Format(format, args) + " {").IndentRight();
        }

        public SourceCode BracketEnd()
        {
            return BracketEnd("");
        }

        public SourceCode BracketEnd(string add)
        {
            return IndentLeft().Append("}" + add);
        }

        public static SourceCode Parse(string code)
        {
            var source = new SourceCode();
            code = code.Replace("\r", "");
            code = code.Replace("    ", "\t");
            var prevTabCount = 0;
            foreach (var line in code.Split('\n'))
            {
                var tabCount = line.CountTab();
                if (prevTabCount > tabCount)
                {
                    for (var i = 0; i < prevTabCount - tabCount; ++i)
                        source.IndentLeft();
                }
                else if (prevTabCount < tabCount)
                {
                    for (var i = 0; i < tabCount - prevTabCount; ++i)
                        source.IndentRight();
                }
                prevTabCount = tabCount;
                var trimmed = line.Trim();
                if (trimmed.StartsWith("<%") && trimmed.EndsWith("%>"))
                    source.AddMarker(trimmed.Strip("<%", "%>"));
                else source.Append(trimmed);
            }
            return source;
        }

        private class Block : ICode
        {
            public readonly List<ICode> Codes = new List<ICode>();
            public Block Parent;
            public string Name { get; set; }

            public string Generate()
            {
                var builder = new StringBuilder();
                foreach (var code in Codes)
                {
                    if (code is Block && ((Block) code).Name != null)
                        continue;
                    foreach (var line in code.Generate().Split('\n'))
                    {
                        if (Parent != null)
                            builder.Append("    ");
                        builder.Append(line).Append('\n');
                    }
                }
                if (builder.Length > 1)
                    builder.Remove(builder.Length - 1, 1);
                return builder.ToString();
            }
        }

        private class Line : ICode
        {
            public string Code { private get; set; }

            public string Generate()
            {
                return Code;
            }
        }
    }
}