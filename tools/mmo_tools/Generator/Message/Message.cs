using System;
using System.Collections.Generic;
using System.Linq;
using MmoTools.Util;

namespace MmoTools.Generator.Message
{
    internal class Message
    {
        public enum Direction
        {
            Both,
            From,
            To
        }

        private string _canonicalName;
        private string _className;
        private string _messageTypeName;

        public Message()
        {
            Fields = new List<Field>();
        }

        public List<Field> Fields { get; private set; }
        public MessageGroup Group { get; set; }
        public string Name { get; set; }
        public string From { get; set; }
        public string To { get; set; }

        public string Namespace { get; set; }
        public string Postfix { get; set; }

        public string MessageTypeName
        {
            get { return _messageTypeName ?? (_messageTypeName = String.Format("msg_type::{0}", Name)); }
        }

        public string ClassName
        {
            get { return _className ?? (_className = Name + "_" + Postfix); }
        }

        public string CanonicalName
        {
            get { return _canonicalName ?? (_canonicalName = CodeUtil.GetCanonicalName(Namespace, Name + "_" + Postfix)); }
        }

        public int RefCount
        {
            get { return Fields.Count(e => e is RefField); }
        }

        public bool HasAttributeBindedField
        {
            get { return Fields.OfType<SimpleField>().Any(e => e.BindAttribute != null) || Fields.Any(e => e.Name.Equals("id")); }
        }

        public bool HasReference(Message message)
        {
            return Fields.OfType<RefField>().Any(e => e.Name.Equals(message.Name));
        }

        public bool HasDirection(string name, Direction direction)
        {
            switch (direction)
            {
                case Direction.From:
                    return From.Equals("both", StringComparison.OrdinalIgnoreCase) || From.Equals(name, StringComparison.OrdinalIgnoreCase);
                case Direction.To:
                    return To.Equals("both", StringComparison.OrdinalIgnoreCase) || To.Equals(name, StringComparison.OrdinalIgnoreCase);
                case Direction.Both:
                    return From.Equals("both", StringComparison.OrdinalIgnoreCase) || To.Equals("both", StringComparison.OrdinalIgnoreCase);
            }
            return false;
        }

        public SourceCode GenerateArgumentedConstructor()
        {
            // msg argumented constructor
            var paramArgs = new List<string>();
            var initializeArgs = new List<string>();
            foreach (var field in Fields.OfType<SimpleField>())
            {
                paramArgs.Add(string.Format("{0} _{1}", TypeUtil.ToArgumentTypeName(field.Type), field.Name));
                initializeArgs.Add(string.Format("{0}(_{0})", field.Name));
            }
            var code = new SourceCode();
            if (initializeArgs.Count == 0)
                return code;

            code.Append("{0}({1})", ClassName, string.Join(", ", paramArgs));
            code.IndentRight();
            code.Append(": {0} {{}}", string.Join(", ", initializeArgs));
            code.IndentLeft();
            code.NewLine();
            return code;
        }

        public string GenerateEntityBindDeclCode(Direction dir)
        {
            return String.Format("void operator {1} (const {0}& msg, entity_ref entity);", CanonicalName,
                                 dir == Direction.From ? "<<" : ">>");
        }

        public SourceCode GenerateEntityBindBodyCode(Direction dir)
        {
            var operatorDir = dir == Direction.From ? "<<" : ">>";

            var code = new SourceCode();
            code.Append("inline void operator {2} ({0}{1}& msg, entity_ref entity)", dir == Direction.To ? "const " : "", CanonicalName, operatorDir);
            code.BracketStart();

            if (dir == Direction.From && Fields.Exists(e => e.Name.Equals("id")))
                code.Append("msg.id = entity->id;");

            foreach (var field in Fields.OfType<SimpleField>().Where(e => e.BindAttribute != null))
            {
                var msgVar = String.Format("msg.{0}", field.Name);
                var entityVar = String.Format("entity->get<{0}_t>()->{1}", field.BindAttribute.AttributeName, field.BindAttribute.FieldName);
                var format = String.Format("if (entity->has<{0}_t>()) ", field.BindAttribute.AttributeName) +
                             (dir == Direction.From ? "{0} = {1};" : "{1} = {0};");
                code.Append(format, msgVar, entityVar);
            }

            var bindedRefMessageFields = Fields.OfType<RefField>().Where(e => e.Target != null && e.Target.HasAttributeBindedField && !(e is RefListField));
            foreach (var field in bindedRefMessageFields)
                code.Append("msg.{0} {1} entity;", field.TargetName, operatorDir);

            code.BracketEnd();
            code.NewLine();
            return code;
        }

        public SourceCode GenerateHandlerDeclaration()
        {
            var code = new SourceCode();
            code.NewLine();
            code.Append("typedef {0} type;", ClassName);
            code.Append("typedef std::function<void (msg_session_ref, const type&)> handler_t;");
            code.Append("typedef void (*listener_t)(const type&);");
            code.Append("typedef std::vector<listener_t> listener_list;");
            code.NewLine();
            code.Append("static handler_t _handler;");
            code.Append("static listener_list _listeners;");
            code.Append("static void handle(msg_session_ref session, const type& msg);");
            code.Append("static void handle(msg_session_ref session, msg_reader reader);");
            code.Append("static void broadcast(const type& msg);");
            return code;
        }

        public SourceCode GenerateHandlerBody(CodeContext context)
        {
            var code = new SourceCode();
            code.Append("inline void {0}::handle(msg_session_ref session, const type& msg)", CanonicalName);
            code.BracketStart();
            code.Append("_ASSERT(_handler != NULL);");
            code.Append("_handler(session, msg);");
            Fields.ForEach(field => code.Append(field.GenerateHandler(context)));
            code.BracketEnd();
            code.NewLine();

            code.Append("inline void {0}::handle(msg_session_ref session, msg_reader reader)", CanonicalName);
            code.BracketStart();
            var accessor = context.StartAccessor("msg");
            {
                code.Append("{0} {1};", ClassName, accessor);
                code.Append("reader >> {0};", accessor);
                code.Append("handle(session, {0});", accessor);
            }
            context.EndAccessor();
            code.BracketEnd();
            code.NewLine();

            code.Append("inline void {0}::broadcast(const type& msg)", CanonicalName);
            code.BracketStart();
            context.StartAccessor("msg");
            {
                code.Append(CodeUtil.ForEachCode("listener_list", "_listeners.", "iter", false));
                code.IndentRight();
                // not-recursive ref
                code.Append("(*iter)(msg);");
                code.BracketEnd();
            }
            context.EndAccessor();
            code.BracketEnd();
            return code;
        }
    }
}