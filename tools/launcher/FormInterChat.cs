using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Media;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Server.Core.Command;
using Server.Core.Messaging;
using Server.Extension;
using Server.Extension.Handler;
using Server.Message.Pylon;
using launcher.Properties;
using Server.Core.Util;
using System.Threading.Tasks;

namespace launcher
{
    public partial class FormInterChat : Form
    {
        private readonly Dictionary<string, Color> _userMap = new Dictionary<string, Color>();
        private SoundPlayer _player;
        private bool _shiftDowned;

        private MessageSession _session;
        private readonly string _userName;

        public FormInterChat(MessageSession session, string userName)
        {
            InitializeComponent();

            _session = session;
            _userName = userName;


            // ReSharper disable ConvertToLambdaExpression
            DispatchHandler.Instance
                           .AddHandler<InterChatMsg>(msg => AddMessage(msg.Name, msg.Message, msg.Ticks))
                           .AddHandler<ResetPeerMessage>(OnDisconnected)
                           .AddHandler<InterChatCommandMsg>(DispatchCommand)
                           .AddHandler<AlivePingMsg>(OnPing);
        }

        private void OnPing(AlivePingMsg msg)
        {
            _session.Send(new AlivePongMsg());
        }

        private void OnDisconnected(ResetPeerMessage msg)
        {
            AddMessage("system", "Gone, server, gone.", DateTime.Now.Ticks);
            Reconnect();
        }

        private void Reconnect()
        {
            try
            {
                _session = MessageSessionManager.Instance.CreateSession();
#if DEBUG
                _session.Connect("127.0.0.1", Configuration.PylonPort);
#else
                _session.Connect(Configuration.PylonHost, Configuration.PylonPort);
#endif
                _session.StartReceive();

                _session.Send(new InterChatLoginMsg { Name = _userName });
                return;
            }
            catch
            {
                Task.Delay(5000).ContinueWith(_ => Reconnect());
            }
        }

        private void DisableControls()
        {
            if (InvokeRequired)
                Invoke(new MethodInvoker(DisableControls));
            else
            {
                textLog.Enabled = false;
                textMessage.Enabled = false;
            }
        }

        private void EnableControls()
        {
            if (InvokeRequired)
                Invoke(new MethodInvoker(EnableControls));
            else
            {
                textLog.Enabled = true;
                textMessage.Enabled = true;
            }
        }

        private void ShowError(string format, params object[] args)
        {
            if (InvokeRequired)
                Invoke(new MethodInvoker(() => ShowError(format, args)));
            else
            {
                MessageBox.Show(this, string.Format(format, args), Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void FormInterChat_Load(object sender, EventArgs e)
        {
            DisableControls();

            try
            {
                _player = new SoundPlayer();

                EnableControls();
            }
            catch (Exception exception)
            {
                ShowError("Cannot connect to mmo.pe.kr: {0}", exception.Message);
                textMessage.Enabled = false;
            }
        }

        private void DispatchCommand(InterChatCommandMsg msg)
        {
            switch ((InterChatCommandType) msg.TypeCode)
            {
                case InterChatCommandType.InformLoggedUsers:
                    _userMap.Clear();
                    if (!string.IsNullOrWhiteSpace(msg.Content))
                    {
                        foreach (var each in msg.Content.Split('|'))
                        {
                            KeyValuePair<string, Color> userInfo;
                            if (ParseUser(each, out userInfo))
                                _userMap.Add(userInfo.Key, userInfo.Value);
                        }
                    }
                    break;

                case InterChatCommandType.InformLoggedUser:
                    {
                        KeyValuePair<string, Color> userInfo;
                        if (ParseUser(msg.Content, out userInfo))
                            _userMap.Add(userInfo.Key, userInfo.Value);
                    }
                    break;

                case InterChatCommandType.InformLogoutUser:
                    _userMap.Remove(msg.Content);
                    break;

                case InterChatCommandType.ChangeColor:
                    {
                        KeyValuePair<string, Color> userInfo;
                        if (ParseUser(msg.Content, out userInfo) && _userMap.ContainsKey(userInfo.Key))
                            _userMap[userInfo.Key] = userInfo.Value;
                    }
                    break;

                case InterChatCommandType.Speech:
                    {
                        var mp3Bytes = Convert.FromBase64String(msg.Content);
                        var tempFile = Path.GetTempFileName() + ".mp3";
                        File.WriteAllBytes(tempFile, mp3Bytes);
                        var player = new WMPLib.WindowsMediaPlayer {URL = tempFile};
                        player.controls.play();
                    }
                    break;
            }

            if (InvokeRequired)
                Invoke(new MethodInvoker(UpdateUsers));
            else UpdateUsers();
        }

        public static bool ParseUser(string content, out KeyValuePair<string, Color> userInfo)
        {
            userInfo = default(KeyValuePair<string, Color>);

            var argbIndex = content.LastIndexOf("#", StringComparison.Ordinal);
            if (argbIndex < 0)
                return false;

            int argbValue;
            var argbString = content.Substring(argbIndex + 1);
            if (string.IsNullOrWhiteSpace(argbString) || !int.TryParse(argbString, out argbValue))
                return false;

            var userName = content.Substring(0, argbIndex);
            var textColor = Color.FromArgb(argbValue);
            userInfo = new KeyValuePair<string, Color>(userName, textColor);
            return true;
        }

        private void UpdateUsers()
        {
            listUsers.Visible = true;
            listUsers.Items.Clear();
            foreach (var each in _userMap.Keys)
                listUsers.Items.Add(each);
        }

        private static readonly string[][] ReplaceStringPairs = new[]
            {
                new[] { @"/^", @"↗" },
                new[] { @"^\", @"↖" },
                new[] { @"v/", @"↙" },
                new[] { @"\v", @"↘" },
                new[] { @"^|", @"↑" },
                new[] { @"|v", @"↓" },
            };

        private void AddMessage(string sender, string message, long tick)
        {
            foreach (var pair in ReplaceStringPairs)
            {
                if (message.Contains(pair[0]))
                    message = message.Replace(pair[0], pair[1]);
            }

            var time = new DateTime(tick);
            AppendLog(time.ToString("HH:mm:ss"), sender, message);

            if (checkBoxFlash.Checked == false)
            {
                DoFlashWindow();
            }

            if (checkBoxMute.Checked == false)
            {
                _player.Stream = Resources.CHAT_SOUND_1;
                _player.Play();
            }
        }

        private void DoFlashWindow()
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(DoFlashWindow));
                return;
            }

            if (!ApplicationIsActivated())
            {
                FlashWindow.Flash(this, 3);
            }
        }

        private void AppendLog(string time, string sender, string message)
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(() => AppendLog(time, sender, message)));
                return;
            }

            var textColor = _userMap.ContainsKey(sender) ? _userMap[sender] : Color.Black;

            textLog.AppendText("(", Color.Black)
                   .AppendText(time, Color.DarkBlue)
                   .AppendText(") ", Color.Black)
                   .AppendText("<", Color.Black)
                   .AppendText(sender, Color.Blue)
                   .AppendText("> ", Color.Black)
                   .AppendText(message + "\r\n", textColor);

            textLog.SelectionStart = textLog.Text.Length;
            textLog.ScrollToCaret();
        }

        private void AppendInternalLog(string message)
        {
            AppendLog(DateTime.Now.ToString("HH:mm:ss"), "system", message);
        }

        public static bool ApplicationIsActivated()
        {
            var activatedHandle = GetForegroundWindow();
            if (activatedHandle == IntPtr.Zero)
            {
                return false; // No window is currently activated
            }

            var procId = Process.GetCurrentProcess().Id;
            int activeProcId;
            GetWindowThreadProcessId(activatedHandle, out activeProcId);

            return activeProcId == procId;
        }


        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        private static extern IntPtr GetForegroundWindow();

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern int GetWindowThreadProcessId(IntPtr handle, out int processId);


        private void textMessage_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Shift)
                _shiftDowned = true;
        }

        private void textMessage_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == '\r' && !_shiftDowned)
            {
                e.Handled = true;
                buttonSend_Click(sender, e);
            }
        }

        private void textMessage_KeyUp(object sender, KeyEventArgs e)
        {
            _shiftDowned = false;
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            var message = textMessage.Text.TrimEnd();
            if (string.IsNullOrWhiteSpace(message))
                return;

            try
            {
                var captured = false;
                if (message.StartsWith("."))
                {
                    if (HookCommand(message))
                        captured = true;
                }

                if (!captured)
                {
                    var msg = new InterChatMsg {Message = message};
                    _session.Send(msg);
                }

                textMessage.Text = "";
                textMessage.Focus();
            }
            catch (Exception exception)
            {
                ShowError("Cannot connect to mmo.pe.kr: {0}", exception.Message);
                textMessage.Enabled = false;
            }
        }

        private bool HookCommand(string message)
        {
            try
            {
                var args = new CommandArguments(message.Substring(1));
                switch (args.Command.ToLower())
                {
                    case "color":
                        {
                            var newColor = Color.FromName(args.Get<string>(0));
                            if (newColor.IsKnownColor)
                                SendCommand(InterChatCommandType.ChangeColor, newColor.ToArgb());
                        }
                        break;

                    case "speech":
                        {
                            var content = args.GetAll(0);
                            if (!string.IsNullOrWhiteSpace(content))
                                SendCommand(InterChatCommandType.Speech, content);
                        }
                        break;

                    case "go":
                        {
                            var serverIndex = args.Get<int>(0);
                            var userName = args.Get<string>(1, _userName);
                            ClientLauncher.Instance.Execute(serverIndex, userName);
                        }
                        break;

                    case "server":
                        {
                            var servers = ClientLauncher.Instance.Servers;
                            for (var serverIndex = 0; serverIndex < servers.Count; ++serverIndex)
                            {
                                var server = servers[serverIndex];
                                AppendInternalLog(string.Format("[{0}] {1} ({2}:{3})", serverIndex, server.Name,
                                                                server.Host, server.Port));
                            }

                        }
                        break;

                    default:
                        return false;
                }
                return true;
            }
            catch (Exception exception)
            {
                ShowError("Invalid command: {0}", exception.Message);
            }
            return false;
        }

        private void SendCommand(InterChatCommandType type, object value)
        {
            var msg = new InterChatCommandMsg {TypeCode = (int) type, Content = Convert.ToString(value)};
            _session.Send(msg);
        }

        private void textLog_LinkClicked(object sender, LinkClickedEventArgs e)
        {
            try
            {
                Process.Start(e.LinkText);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, Text);
            }
        }
    }


    public static class RichTextBoxExtensions
    {
        public static RichTextBox AppendText(this RichTextBox box, string text, Color color)
        {
            box.SelectionStart = box.TextLength;
            box.SelectionLength = 0;

            box.SelectionColor = color;
            box.AppendText(text);
            box.SelectionColor = box.ForeColor;
            return box;
        }
    }
}