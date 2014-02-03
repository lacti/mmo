using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Win32;
using Server.Core.Messaging;
using Server.Core.Util;
using Server.Extension;
using Server.Extension.Handler;
using Server.Message.Pylon;

namespace launcher
{
    public partial class FormLauncher : Form
    {
        private readonly MessageSession _session = MessageSessionManager.Instance.CreateSession();

        private FormInterChat _interChatForm;

        public FormLauncher()
        {
            InitializeComponent();
        }

        private void FormLauncher_Load(object sender, EventArgs e)
        {
            Task.Factory.StartNew(ConnectAndUpdateServerList);
            Task.Factory.StartNew(PatchClient).ContinueWith(_ => PatchServer());

            LoadAccountNameFromRegistry();
        }

        private void ConnectAndUpdateServerList()
        {
            DispatchHandler.Instance.AddHandler<ServersMsg>(ProcessServersMsg);
            DispatchHandler.Instance.AddHandler<InterChatCommandMsg>(ProcessInterChatCommandMsg);

            try
            {
#if DEBUG
                _session.Connect("127.0.0.1", Configuration.PylonPort);
#else
                _session.Connect(Configuration.PylonHost, Configuration.PylonPort);
#endif
                _session.StartReceive();

                _session.Send(new RequestServerMsg());
                UpdateUi(() => timerRefresh.Enabled = true);
            }
            catch (Exception exception)
            {
                UpdateUi(() =>
                    {
                        ShowError("Cannot connect to xelina: {0}", exception.Message);

                        textAccountName.Enabled = false;
                        listViewServers.Enabled = false;
                        buttonInterChat.Enabled = false;
                        buttonStartGame.Enabled = false;
                    });
            }
        }

        private void ProcessServersMsg(ServersMsg msg)
        {
            ClientLauncher.Instance.Servers = msg.ServerList;
            UpdateServerList();
        }

        private void UpdateServerList()
        {
            if (InvokeRequired)
                Invoke(new MethodInvoker(UpdateServerList));
            else
            {
                var servers = ClientLauncher.Instance.Servers;
                listViewServers.Items.Clear();
                foreach (var each in servers)
                    listViewServers.Items.Add(each.Name);

                if (listViewServers.Items.Count > 0)
                    listViewServers.Items[0].Selected = true;
            }
        }

        private void PatchClient()
        {
            try
            {
                PatchBinary("client/", ClientLauncher.ClientPath);
                UpdateUi(() =>
                {
                    buttonStartGame.Enabled = true;
                    textAccountName.Focus();
                });
            }
            catch (Exception exception)
            {
                UpdateUi(() =>
                {
                    ShowError("Cannot update client: {0}", exception.Message);

                    textAccountName.Enabled = false;
                    listViewServers.Enabled = false;
                    buttonInterChat.Enabled = false;
                    buttonStartGame.Enabled = false;
                    timerRefresh.Enabled = false;
                });
            }
        }

        private void PatchServer()
        {
            try
            {
                PatchBinary("server/", ServerLauncher.ServerPath);
                UpdateUi(() =>
                {
                    buttonStartServer.Enabled = true;
                    textAccountName.Focus();
                });
            }
            catch (Exception exception)
            {
                UpdateUi(() =>
                {
                    ShowError("Cannot update server: {0}", exception.Message);
                    buttonStartServer.Enabled = false;
                });
            }
        }

        private void PatchBinary(string patchPath, string localPath)
        {
            var baseUrl = "http://dist.mmo.pe.kr/patch/" + patchPath;
            CreateDirectory(new DirectoryInfo(localPath));

            using (var webClient = new WebClient())
            {
                var files = webClient.DownloadString(baseUrl + "files");
                var downloadMap = new Dictionary<string, string>();
                foreach (var each in files.Split('\r', '\n').Where(e => !string.IsNullOrWhiteSpace(e)))
                {
                    var triple = each.Split(',');
                    var filePath = triple[0];
                    var length = long.Parse(triple[1]);
                    var hash = triple[2];

                    var destPath = Path.Combine(localPath, filePath.Replace("/", Path.DirectorySeparatorChar.ToString(CultureInfo.InvariantCulture)));
                    var destFileInfo = new FileInfo(destPath);
                    if (destFileInfo.Exists && destFileInfo.Length == length && hash == Hash(destPath))
                        continue;

                    var fileUrl = baseUrl + filePath;
                    downloadMap.Add(fileUrl, destPath);
                }

                UpdateUi(() =>
                    {
                        progressBar.Maximum = downloadMap.Count;
                        progressBar.Value = 0;
                    });

                foreach (var pair in downloadMap)
                {
                    var parentPath = Path.GetDirectoryName(pair.Value);
                    if (parentPath != null)
                        CreateDirectory(new DirectoryInfo(parentPath));

                    webClient.DownloadFile(pair.Key, pair.Value);

                    UpdateUi(() => ++progressBar.Value);
                }
            }
        }

        private void UpdateUi(Action action)
        {
            if (InvokeRequired)
                Invoke(new MethodInvoker(action));
            else action();
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

        private static string Hash(string filePath)
        {
            var result = new StringBuilder();
            var fileBytes = File.ReadAllBytes(filePath);
            var hashBytes = (new MD5CryptoServiceProvider()).ComputeHash(fileBytes);

            foreach (var each in hashBytes)
                result.Append(each.ToString("X2"));

            return result.ToString();
        }

        private static void CreateDirectory(DirectoryInfo directory)
        {
            if (directory.Parent != null) CreateDirectory(directory.Parent);
            if (!directory.Exists) directory.Create();
        }

        private void buttonStartGame_Click(object sender, EventArgs e)
        {
            var userName = textAccountName.Text.Trim();
            if (string.IsNullOrWhiteSpace(userName))
            {
                ShowError("Please input an account name.");
                textAccountName.SelectAll();
                textAccountName.Focus();
                return;
            }

            var serverIndices = listViewServers.SelectedIndices;
            if (serverIndices.Count == 0 || serverIndices.Count > 1)
            {
                ShowError("Please select one server.");
                listViewServers.Select();
                listViewServers.Focus();
                return;
            }

            var serverIndex = serverIndices[0];
            try
            {
                timerRefresh.Enabled = false;

                ClientLauncher.Instance.Execute(serverIndex, userName);

                SaveAccountNameToRegistry();
                Close();
            }
            catch (Exception exception)
            {
                ShowError(exception.Message);
            }
        }

        private void buttonStartServer_Click(object sender, EventArgs e)
        {
            try
            {
                ServerLauncher.Execute();
            }
            catch (Exception exception)
            {
                ShowError(exception.Message);
            }
        }

        private void buttonInterChat_Click(object sender, EventArgs e)
        {
            var userName = textAccountName.Text.Trim();
            if (string.IsNullOrWhiteSpace(userName))
            {
                ShowError("Invalid username");
                textAccountName.SelectAll();
                textAccountName.Focus();
                return;
            }

            if (_interChatForm != null)
            {
                _interChatForm.Dispose();
                _interChatForm = null;
                GC.Collect();
            }

            // 첫 패킷의 유실을 막기 위해 일단 패킷 핸들러부터 먼저 만든다.
            _interChatForm = new FormInterChat(_session, userName);
            _interChatForm.Closed += (obj, args) => Close();

            _session.Send(new InterChatLoginMsg { Name = userName });
        }

        private void ProcessInterChatCommandMsg(InterChatCommandMsg msg)
        {
            switch ((InterChatCommandType)msg.TypeCode)
            {
                case InterChatCommandType.CheckUserName:
                    if (msg.Content.As<bool>())
                    {
                        StartInterChat();
                    }
                    else
                    {
                        ShowError("Duplicated name: " + textAccountName.Text);
                        UpdateUi(() =>
                            {
                                textAccountName.SelectAll();
                                textAccountName.Focus();
                            });
                    }
                    break;
            }
        }

        private void StartInterChat()
        {
            UpdateUi(() =>
                {
                    _interChatForm.Show();
                    Hide();

                    timerRefresh.Enabled = false;
                });

            SaveAccountNameToRegistry();
        }

        private void LoadAccountNameFromRegistry()
        {
            using (var regKey = Registry.CurrentUser.CreateSubKey("InterChat"))
            {
                if (regKey != null)
                {
                    var userName = Convert.ToString(regKey.GetValue("userName"));
                    if (!string.IsNullOrWhiteSpace(userName))
                    {
                        UpdateUi(() => textAccountName.Text = userName);
                    }
                }
            }
        }

        private void SaveAccountNameToRegistry()
        {
            try
            {
                using (var regKey = Registry.CurrentUser.CreateSubKey("InterChat"))
                {
                    if (regKey != null)
                        regKey.SetValue("userName", textAccountName.Text);
                }
            }
            // ReSharper disable EmptyGeneralCatchClause
            catch
            // ReSharper restore EmptyGeneralCatchClause
            {
            }
        }

        private void timerRefresh_Tick(object sender, EventArgs e)
        {
            try
            {
                _session.Send(new RequestServerMsg());
            }
// ReSharper disable EmptyGeneralCatchClause
            catch (Exception)
// ReSharper restore EmptyGeneralCatchClause
            {
            }
        }
    }
}