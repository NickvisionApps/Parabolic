using Microsoft.Extensions.DependencyInjection;
using Nickvision.Parabolic.WinUI.Controls;
using Nickvision.Parabolic.WinUI.Views;

namespace Nickvision.Parabolic.WinUI.Helpers;

public static class IServiceCollectionExtensions
{
    extension(IServiceCollection collection)
    {
        public IServiceCollection AddControls()
        {
            collection.AddTransient<AddDownloadDialog>();
            collection.AddTransient<HistoryPage>();
            collection.AddTransient<KeyringPage>();
            collection.AddSingleton<MainWindow>();
            collection.AddTransient<SettingsPage>();
            collection.AddSingleton<AboutDialog>();
            collection.AddTransient<DownloadRow>();
            return collection;
        }
    }
}
