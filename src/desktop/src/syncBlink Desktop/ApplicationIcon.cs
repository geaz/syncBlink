using System;
using System.Windows.Forms;

namespace SyncBlink.Desktop
{
    internal class ApplicationIcon : IDisposable
    {
        private readonly NotifyIcon _icon = new NotifyIcon();

        public void Display()
        {
            _icon.MouseClick += OnMouseClick;
            _icon.Icon = Resources.ApplicationIcon;
            _icon.Text = "syncBlink Desktop";
            _icon.Visible = true;

            // Attach a context menu.
            _icon.ContextMenuStrip = new ContextMenu().Create();
        }

        public void Dispose()
        {
            _icon.Dispose();
        }

        private void OnMouseClick(object sender, MouseEventArgs e)
        {
        }
    }
}
