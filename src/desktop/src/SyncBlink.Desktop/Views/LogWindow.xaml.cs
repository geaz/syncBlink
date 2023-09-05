using System.ComponentModel;
using System.Windows;
using SyncBlink.Desktop.ViewModels;

namespace SyncBlink.Desktop.Views
{
    public partial class LogWindow : Window
    {
        public LogWindow(LogViewModel viewModel)
        {
            DataContext = viewModel;
            InitializeComponent();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            Hide();
            e.Cancel = true;
            base.OnClosing(e);
        }
    }
}
