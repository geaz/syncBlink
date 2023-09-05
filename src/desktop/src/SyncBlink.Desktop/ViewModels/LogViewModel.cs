using CommunityToolkit.Mvvm.ComponentModel;
using System;

namespace SyncBlink.Desktop.ViewModels
{
    public partial class LogViewModel : ObservableObject
    {
        [ObservableProperty]
        private string _log = string.Empty;

        private readonly SyncBlinkApi _api;

        public LogViewModel(SyncBlinkApi api)
        {
            _api = api;
            _api.OnMessage((msg, isErr) => {
                Log += isErr ? "[ERROR]" : "[INFO]";
                Log += $" {msg}{Environment.NewLine}";

                if (Log.Length > 50000) Log = Log.Substring(Log.Length - 50000);
            });
        }
    }
}