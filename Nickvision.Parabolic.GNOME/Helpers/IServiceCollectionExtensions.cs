using Microsoft.Extensions.DependencyInjection;
using Nickvision.Parabolic.GNOME.Controls;
using Nickvision.Parabolic.GNOME.Views;

namespace Nickvision.Parabolic.GNOME.Helpers;

public static class IServiceCollectionExtensions
{
    extension(IServiceCollection collection)
    {
        public IServiceCollection AddControls()
        {
            collection.AddTransient<AddDownloadDialog>();
            collection.AddTransient<HistoryDialog>();
            collection.AddTransient<KeyringDialog>();
            collection.AddTransient<PreferencesDialog>();
            collection.AddTransient<DownloadRow>();
            collection.AddTransient<LoadingDialog>();
            return collection;
        }
    }
}