using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal class MessageGroup
    {
        private static int _idSerial;
        private readonly int _id = ++_idSerial;

        public List<Message> Messages = new List<Message>();
        public string Name { get; set; }

        private IEnumerable<Message> SortedMessages
        {
            get
            {
                var sorted = new List<Message>(Messages);
                sorted.Sort((left, right) =>
                    {
                        if (left.HasReference(right)) return 1;
                        if (right.HasReference(left)) return -1;
                        return left.RefCount - right.RefCount;
                    });
                return sorted;
            }
        }

        public Message this[string name]
        {
            get { return Messages.Find(e => e.Name.Equals(name)); }
        }

        public void Generate(string outputDirectory, string directionName)
        {
            var msgFileName = Path.Combine(outputDirectory, Name + "_msg.h");
            var code = new SourceCode();
            code.Append("#pragma once");
            code.NewLine();
            code.Append("#include <msg_def.h>");
            code.Append("#include <msg_reader.h>");
            code.Append("#include <msg_writer.h>");
            code.Append("#include <msg_session_ref.h>");
            code.NewLine();
            code.Append("#pragma warning (disable: 4100)");
            code.NewLine();

            code.Append(GenerateMsgType());

            code.Append("namespace msg {");
            code.NewLine();

            var context = new CodeContext();
            foreach (var msg in SortedMessages)
            {
                code.BracketStart("struct {0}", msg.ClassName);
                code.Append("static const msg_type_t __type = msg_type::{0};", msg.Name);
                code.NewLine();

                context.StartAccessor("msg");
                msg.Fields.ForEach(field => code.Append(field.GenerateField(context)));
                code.NewLine();

                code.Append(msg.GenerateArgumentedConstructor());
                code.Append(msg.Fields.GenerateDefaultConstructor(msg.ClassName));

                code.Append(msg.GenerateHandlerDeclaration());
                context.EndAccessor();

                code.BracketEnd(";");
                code.NewLine();
            }
            code.Append("}");
            code.NewLine();
            code.NewLine();
            code.Append(GenerateReaderWriter(context));
            code.NewLine();
            code.NewLine();
            code.Append("namespace msg {");
            foreach (var msg in SortedMessages)
            {
                context.StartAccessor("msg");
                code.Append(msg.GenerateHandlerBody(context));
                context.EndAccessor();
                code.NewLine();
            }
            code.Append("}");
            code.NewLine();
            if (!String.IsNullOrWhiteSpace(directionName))
            {
                code.Append(GenerateEntityBindCode(directionName));
                code.NewLine();
            }
            code.NewLine();
            code.Append("#pragma warning (default: 4100)");

            code.WriteToFile(msgFileName);
        }

        private SourceCode GenerateEntityBindCode(string directionName)
        {
            var code = new SourceCode();

            if (!Messages.Exists(msg => msg.HasAttributeBindedField))
                return code;

            var directions = Enum.GetValues(typeof (Message.Direction)).OfType<Message.Direction>().ToArray();
            var attributes = new List<string>();
            var bindBodyCodes = new List<SourceCode>();
            code.Append("#include <cbes/entity_def.h>");
            code.NewLine();
            foreach (var msg in SortedMessages.Where(e => e.HasAttributeBindedField))
            {
                foreach (var dir in directions)
                {
                    if (!msg.HasDirection(directionName, dir))
                        continue;

                    code.Append(msg.GenerateEntityBindDeclCode(dir));
                    bindBodyCodes.Add(msg.GenerateEntityBindBodyCode(dir));
                }

                attributes.AddRange(
                    msg.Fields.OfType<SimpleField>().Where(e => e.BindAttribute != null)
                       .Select(e => e.BindAttribute.AttributeName).Distinct());
            }

            code.NewLine();
            code.Append("#include <cbes/entity.h>");
            attributes.ForEach(e => code.Append("#include \"{0}_attribute.h\"", e));
            code.NewLine();
            bindBodyCodes.ForEach(e => code.Append(e));

            return code;
        }

        private SourceCode GenerateMsgType()
        {
            var code = new SourceCode();
            code.BracketStart("namespace msg_type");

            var msgSerial = 0;
            foreach (var msg in Messages)
            {
                code.Append("const msg_type_t {0} = {1};", msg.Name, _id*1000 + msgSerial);
                ++msgSerial;
            }
            code.BracketEnd();
            code.NewLine();
            code.NewLine();
            return code;
        }

        private SourceCode GenerateReaderWriter(CodeContext context)
        {
            var code = new SourceCode();
            code.Append("#pragma region /* reader & writer */");
            code.NewLine();

            foreach (var msg in SortedMessages)
            {
                var accessor = context.StartAccessor("msg");

                code.Append("template <>");
                code.Append("inline msg_writer& msg_writer::operator << (const {0}& {1})", msg.CanonicalName, accessor);
                code.BracketStart();
                code.Append("(*this) << {0};", msg.MessageTypeName);

                msg.Fields.ForEach(field => code.Append(field.GenerateWriter(context)));
                code.Append("return (*this);");
                code.BracketEnd();
                code.NewLine();

                code.Append("template <>");
                code.Append("inline msg_reader& msg_reader::operator >> ({0}& {1})", msg.CanonicalName, accessor);
                code.BracketStart();
                msg.Fields.ForEach(field => code.Append(field.GenerateReader(context)));
                code.Append("return (*this);");
                code.BracketEnd();
                code.NewLine();

                context.EndAccessor();
            }
            code.Append("#pragma endregion");
            return code;
        }
    }
}