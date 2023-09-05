using System;
using System.Windows;
using Hardcodet.Wpf.TaskbarNotification;
using Microsoft.Extensions.DependencyInjection;
using SyncBlink.Desktop.Views;
using SyncBlink.Desktop.ViewModels;

namespace SyncBlink.Desktop
{
    public partial class App : Application
    {
        private TaskbarIcon? _notifyIcon;

        public App()
        {
            Services = ConfigureServices();
            this.InitializeComponent();
        }

        private IServiceProvider ConfigureServices()
        {
            var services = new ServiceCollection();

            services.AddSingleton(new SyncBlinkApi(64, "Desktop", true));

            services.AddTransient<SystemTrayViewModel>();
            services.AddTransient<LogViewModel>();

            services.AddTransient<LogWindow>();

            return services.BuildServiceProvider();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            _notifyIcon = (TaskbarIcon)FindResource("syncBlinkSysTray");
            _notifyIcon.DataContext = Services.GetService<SystemTrayViewModel>();
        }

        protected override void OnExit(ExitEventArgs e)
        {
            _notifyIcon?.Dispose();
            base.OnExit(e);
        }

        public IServiceProvider Services { get; }
    }
}