using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using SyncBlink.Desktop.Views;

namespace SyncBlink.Desktop.ViewModels
{
    public partial class SystemTrayViewModel : ObservableObject
    {
        private readonly SyncBlinkApi _api;
        private readonly LogWindow _logWindow;

        [ObservableProperty]
        private string _infoMessage = "Searching ...";
        private string _hubIp = string.Empty;

        public SystemTrayViewModel(SyncBlinkApi api, LogWindow logWindow)
        {
            _api = api;
            _logWindow = logWindow;

            _api.OnConnectionChange((ip, connected) => {
                _hubIp = ip;
                if (connected)
                    InfoMessage = $"Connected to {_hubIp}";
                else
                    InfoMessage = $"Searching ...";
                //_webBtn.Enabled = connected;
            });
            _api.TryConnect();
        }

        [RelayCommand]
        public void ShowLogWindow()
        {
            _logWindow.Show();
        }

        [RelayCommand]
        public void ExitApplication()
        {
            Application.Current.Shutdown();
        }
    }
}
