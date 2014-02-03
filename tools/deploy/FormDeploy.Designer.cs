namespace deploy
{
    partial class FormDeploy
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
            this.textHistory = new System.Windows.Forms.TextBox();
            this.labelHistoryLabel = new System.Windows.Forms.Label();
            this.buttonDeployClient = new System.Windows.Forms.Button();
            this.labelLogLabel = new System.Windows.Forms.Label();
            this.buttonClear = new System.Windows.Forms.Button();
            this.panelLog = new System.Windows.Forms.Panel();
            this.textLog = new System.Windows.Forms.RichTextBox();
            this.buttonDeployServer = new System.Windows.Forms.Button();
            this.panelLog.SuspendLayout();
            this.SuspendLayout();
            // 
            // textHistory
            // 
            this.textHistory.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textHistory.BackColor = System.Drawing.SystemColors.Info;
            this.textHistory.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textHistory.Location = new System.Drawing.Point(14, 28);
            this.textHistory.Multiline = true;
            this.textHistory.Name = "textHistory";
            this.textHistory.Size = new System.Drawing.Size(516, 129);
            this.textHistory.TabIndex = 1;
            // 
            // labelHistoryLabel
            // 
            this.labelHistoryLabel.AutoSize = true;
            this.labelHistoryLabel.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.labelHistoryLabel.Location = new System.Drawing.Point(12, 10);
            this.labelHistoryLabel.Name = "labelHistoryLabel";
            this.labelHistoryLabel.Size = new System.Drawing.Size(87, 15);
            this.labelHistoryLabel.TabIndex = 0;
            this.labelHistoryLabel.Text = "Update History";
            // 
            // buttonDeployClient
            // 
            this.buttonDeployClient.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonDeployClient.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.buttonDeployClient.Location = new System.Drawing.Point(536, 28);
            this.buttonDeployClient.Name = "buttonDeployClient";
            this.buttonDeployClient.Size = new System.Drawing.Size(75, 48);
            this.buttonDeployClient.TabIndex = 2;
            this.buttonDeployClient.Text = "Client &Deploy";
            this.buttonDeployClient.UseVisualStyleBackColor = true;
            this.buttonDeployClient.Click += new System.EventHandler(this.buttonDeploy_Click);
            // 
            // labelLogLabel
            // 
            this.labelLogLabel.AutoSize = true;
            this.labelLogLabel.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.labelLogLabel.Location = new System.Drawing.Point(12, 186);
            this.labelLogLabel.Name = "labelLogLabel";
            this.labelLogLabel.Size = new System.Drawing.Size(27, 15);
            this.labelLogLabel.TabIndex = 4;
            this.labelLogLabel.Text = "Log";
            // 
            // buttonClear
            // 
            this.buttonClear.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonClear.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.buttonClear.Location = new System.Drawing.Point(536, 175);
            this.buttonClear.Name = "buttonClear";
            this.buttonClear.Size = new System.Drawing.Size(75, 23);
            this.buttonClear.TabIndex = 5;
            this.buttonClear.Text = "&Clear";
            this.buttonClear.UseVisualStyleBackColor = true;
            this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
            // 
            // panelLog
            // 
            this.panelLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelLog.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelLog.Controls.Add(this.textLog);
            this.panelLog.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.panelLog.Location = new System.Drawing.Point(14, 204);
            this.panelLog.Name = "panelLog";
            this.panelLog.Size = new System.Drawing.Size(595, 320);
            this.panelLog.TabIndex = 6;
            // 
            // textLog
            // 
            this.textLog.BackColor = System.Drawing.SystemColors.WindowFrame;
            this.textLog.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textLog.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textLog.Location = new System.Drawing.Point(0, 0);
            this.textLog.Name = "textLog";
            this.textLog.ReadOnly = true;
            this.textLog.Size = new System.Drawing.Size(593, 318);
            this.textLog.TabIndex = 0;
            this.textLog.Text = "";
            // 
            // buttonDeployServer
            // 
            this.buttonDeployServer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonDeployServer.Font = new System.Drawing.Font("Malgun Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.buttonDeployServer.Location = new System.Drawing.Point(536, 82);
            this.buttonDeployServer.Name = "buttonDeployServer";
            this.buttonDeployServer.Size = new System.Drawing.Size(75, 48);
            this.buttonDeployServer.TabIndex = 3;
            this.buttonDeployServer.Text = "&Server Deploy";
            this.buttonDeployServer.UseVisualStyleBackColor = true;
            this.buttonDeployServer.Click += new System.EventHandler(this.buttonDeploy_Click);
            // 
            // FormDeploy
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(621, 536);
            this.Controls.Add(this.panelLog);
            this.Controls.Add(this.buttonClear);
            this.Controls.Add(this.labelLogLabel);
            this.Controls.Add(this.buttonDeployServer);
            this.Controls.Add(this.buttonDeployClient);
            this.Controls.Add(this.labelHistoryLabel);
            this.Controls.Add(this.textHistory);
            this.Name = "FormDeploy";
            this.Text = "Deploy";
            this.panelLog.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textHistory;
        private System.Windows.Forms.Label labelHistoryLabel;
        private System.Windows.Forms.Button buttonDeployClient;
        private System.Windows.Forms.Label labelLogLabel;
        private System.Windows.Forms.Button buttonClear;
        private System.Windows.Forms.Panel panelLog;
        private System.Windows.Forms.RichTextBox textLog;
        private System.Windows.Forms.Button buttonDeployServer;
    }
}