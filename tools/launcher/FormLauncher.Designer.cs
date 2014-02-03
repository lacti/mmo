namespace launcher
{
    partial class FormLauncher
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.webBrowser = new System.Windows.Forms.WebBrowser();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.buttonInterChat = new System.Windows.Forms.Button();
            this.listViewServers = new System.Windows.Forms.ListView();
            this.columnServer = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.imageList = new System.Windows.Forms.ImageList(this.components);
            this.buttonStartGame = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.textAccountName = new System.Windows.Forms.TextBox();
            this.buttonStartServer = new System.Windows.Forms.Button();
            this.timerRefresh = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // webBrowser
            // 
            this.webBrowser.Location = new System.Drawing.Point(12, 12);
            this.webBrowser.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowser.Name = "webBrowser";
            this.webBrowser.ScrollBarsEnabled = false;
            this.webBrowser.Size = new System.Drawing.Size(677, 448);
            this.webBrowser.TabIndex = 5;
            this.webBrowser.Url = new System.Uri("http://launcher.mmo.pe.kr/", System.UriKind.Absolute);
            // 
            // progressBar
            // 
            this.progressBar.Location = new System.Drawing.Point(12, 466);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(677, 22);
            this.progressBar.TabIndex = 6;
            // 
            // buttonInterChat
            // 
            this.buttonInterChat.Location = new System.Drawing.Point(704, 451);
            this.buttonInterChat.Name = "buttonInterChat";
            this.buttonInterChat.Size = new System.Drawing.Size(164, 37);
            this.buttonInterChat.TabIndex = 4;
            this.buttonInterChat.Text = "Inter &Chat";
            this.buttonInterChat.UseVisualStyleBackColor = true;
            this.buttonInterChat.Click += new System.EventHandler(this.buttonInterChat_Click);
            // 
            // listViewServers
            // 
            this.listViewServers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnServer});
            this.listViewServers.HideSelection = false;
            this.listViewServers.Location = new System.Drawing.Point(704, 114);
            this.listViewServers.MultiSelect = false;
            this.listViewServers.Name = "listViewServers";
            this.listViewServers.Size = new System.Drawing.Size(164, 245);
            this.listViewServers.SmallImageList = this.imageList;
            this.listViewServers.TabIndex = 1;
            this.listViewServers.UseCompatibleStateImageBehavior = false;
            this.listViewServers.View = System.Windows.Forms.View.Details;
            // 
            // columnServer
            // 
            this.columnServer.Text = "서버 목록";
            this.columnServer.Width = 159;
            // 
            // imageList
            // 
            this.imageList.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.imageList.ImageSize = new System.Drawing.Size(1, 24);
            this.imageList.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // buttonStartGame
            // 
            this.buttonStartGame.Enabled = false;
            this.buttonStartGame.Location = new System.Drawing.Point(704, 365);
            this.buttonStartGame.Name = "buttonStartGame";
            this.buttonStartGame.Size = new System.Drawing.Size(164, 37);
            this.buttonStartGame.TabIndex = 2;
            this.buttonStartGame.Text = "&Start Game";
            this.buttonStartGame.UseVisualStyleBackColor = true;
            this.buttonStartGame.Click += new System.EventHandler(this.buttonStartGame_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.textAccountName);
            this.groupBox1.Location = new System.Drawing.Point(704, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(164, 96);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Account Name";
            // 
            // textAccountName
            // 
            this.textAccountName.Location = new System.Drawing.Point(18, 42);
            this.textAccountName.Name = "textAccountName";
            this.textAccountName.Size = new System.Drawing.Size(130, 21);
            this.textAccountName.TabIndex = 0;
            // 
            // buttonStartServer
            // 
            this.buttonStartServer.Enabled = false;
            this.buttonStartServer.Location = new System.Drawing.Point(704, 408);
            this.buttonStartServer.Name = "buttonStartServer";
            this.buttonStartServer.Size = new System.Drawing.Size(164, 37);
            this.buttonStartServer.TabIndex = 3;
            this.buttonStartServer.Text = "Start Ser&ver";
            this.buttonStartServer.UseVisualStyleBackColor = true;
            this.buttonStartServer.Click += new System.EventHandler(this.buttonStartServer_Click);
            // 
            // timerRefresh
            // 
            this.timerRefresh.Interval = 5000;
            this.timerRefresh.Tick += new System.EventHandler(this.timerRefresh_Tick);
            // 
            // FormLauncher
            // 
            this.AcceptButton = this.buttonStartGame;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(880, 500);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.listViewServers);
            this.Controls.Add(this.buttonStartServer);
            this.Controls.Add(this.buttonStartGame);
            this.Controls.Add(this.buttonInterChat);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.webBrowser);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "FormLauncher";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "mmo";
            this.Load += new System.EventHandler(this.FormLauncher_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.WebBrowser webBrowser;
        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.Button buttonInterChat;
        private System.Windows.Forms.ListView listViewServers;
        private System.Windows.Forms.ColumnHeader columnServer;
        private System.Windows.Forms.ImageList imageList;
        private System.Windows.Forms.Button buttonStartGame;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox textAccountName;
        private System.Windows.Forms.Button buttonStartServer;
        private System.Windows.Forms.Timer timerRefresh;
    }
}

