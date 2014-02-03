#region

using System;
using System.Drawing;
using System.Windows.Forms;
using Server.MapEditor.Model;
using Server.MapEditor.Properties;
using Server.MapEditor.Util;

#endregion

namespace Server.MapEditor
{
    public partial class FormEditor : Form
    {
        public FormEditor()
        {
            InitializeComponent();
            World_ = new World();

            var drawPanel = new SelectablePanel();

            drawPanel.BackgroundImage = Resources.Tile2;
            drawPanel.Location = new Point(0, 0);
            drawPanel.Name = "drawPanel";
            drawPanel.TabIndex = 2;
            drawPanel.Paint += drawPanel_Paint;
            drawPanel.MouseClick += drawPanel_MouseClick;
            drawPanel.ClientSize = new Size(World_.Width, World_.Height);
            splitContainer1.Panel1.Controls.Add(drawPanel);

            propertyGrid1.SelectedObject = World_;
        }

        private World World_ { get; set; }

        private void drawPanel_Paint(object sender, PaintEventArgs e)
        {
            World_.Draw(e.Graphics);
        }

        private void drawPanel_MouseClick(object sender, MouseEventArgs e)
        {
            foreach (var city in World_.Cities)
            {
                if (Math.Abs(e.X - city.X) < 16 && Math.Abs(e.Y - city.Y) < 16)
                {
                    propertyGrid1.SelectedObject = city;
                    return;
                }
            }
            propertyGrid1.SelectedObject = World_;
        }

        private void FormEditor_Load(object sender, EventArgs e)
        {
        }
    }
}