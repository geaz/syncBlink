using System;
using System.Configuration;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SyncBlink.Desktop
{
    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            using (var appIcon = new ApplicationIcon())
            {
                appIcon.Display();

                var task = Task.Run(() =>
                {
                    var api = new SyncBlinkApi(ConfigurationManager.AppSettings.Get("StationIP"), 100, "Desktop");
                    api.OnFrequencyUpdate((b, d) => Console.WriteLine(d));
                    api.Start();
                });

                Application.Run();
            }
        }
    }
}
