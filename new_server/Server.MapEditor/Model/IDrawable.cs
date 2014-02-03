#region

using System.Drawing;

#endregion

namespace Server.MapEditor.Model
{
    internal interface IDrawable
    {
        void Draw(Graphics g);
    }
}