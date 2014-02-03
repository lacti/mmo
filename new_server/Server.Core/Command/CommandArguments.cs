using System;
using System.Text.RegularExpressions;

namespace Server.Core.Command
{
    public class CommandArguments
    {
        private static readonly Regex WhitespaceRegex = new Regex(@"\s+", RegexOptions.Compiled);
        private readonly string[] _arguments;

        public CommandArguments(string input)
        {
            if (input.Contains(" "))
            {
                Command = input.Substring(0, input.IndexOf(' '));
                Argument = input.Substring(input.IndexOf(' ')).TrimStart(' ');
                _arguments = WhitespaceRegex.Split(Argument.Trim());
            }
            else
            {
                Command = input;
                Argument = "";
                _arguments = new string[0];
            }
        }

        public int Count
        {
            get { return _arguments.Length; }
        }

        public string Command { get; private set; }

        public string Argument { get; private set; }

        public T Get<T>(int index, T defaultValue = default(T))
        {
            return (T) GetValue(index, typeof (T), defaultValue);
        }

        public bool Is<T>(int index)
        {
            return IsType(index, typeof (T));
        }

        public object GetValue(int index, Type valueType, object defaultValue = null)
        {
            if (!IsType(index, valueType))
                return valueType.IsValueType ? Activator.CreateInstance(valueType) : defaultValue;

            return Convert.ChangeType(_arguments[index], valueType);
        }

        public bool IsType(int index, Type valueType)
        {
            if (index < 0 || index >= Count)
                return false;

            try
            {
                // ReSharper disable ReturnValueOfPureMethodIsNotUsed
                Convert.ChangeType(_arguments[index], valueType);
                // ReSharper restore ReturnValueOfPureMethodIsNotUsed
            }
                // ReSharper disable EmptyGeneralCatchClause
            catch (Exception)
                // ReSharper restore EmptyGeneralCatchClause
            {
                return false;
            }
            return true;
        }

        public string GetAll(int index)
        {
            var startPos = 0;
            while (index-- > 0)
            {
                startPos = Argument.IndexOf(" ", startPos, StringComparison.Ordinal);
                if (startPos < 0)
                    return "";

                ++startPos;
            }
            return Argument.Substring(startPos);
        }
    }
}