using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Services;
using NReco.Logging.File;
using System;
using System.IO;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class HostApplicationBuilderExtensions
{
    extension(IHostApplicationBuilder builder)
    {
        public IHostApplicationBuilder ConfigureParabolic(string[] args)
        {
            var appInfo = new AppInfo("org.nickvision.tubeconverter", "Nickvision Parabolic", "Parabolic")
            {
                Version = new AppVersion("2026.3.0-next"),
                Changelog = """
                - Added macOS app for the GNOME version of Parabolic
                - Added Windows portable version of Parabolic
                - Added the ability to specify a preferred frame rate for video downloads in Parabolic's settings
                - Added the ability to automatically translate embedded metadata and chapters to the app's language on supported sites. This can be turned off in Converter settings
                - Added failed filter to downloads view
                - Improved selection of playlist video formats when resolutions are specified
                - Improved selection of playlist audio formats on Windows when bitrates are specified
                - Improved cropping of audio thumbnails
                - Improved handling of long file names, they will now be truncated if too long
                - Removed unsupported cookie browsers on Windows. Manual txt files should be used instead
                - Updated yt-dlp
                """,
                SourceRepository = new Uri("https://github.com/NickvisionApps/Parabolic"),
                IssueTracker = new Uri("https://github.com/NickvisionApps/Parabolic/issues/new"),
                DiscussionsForum = new Uri("https://github.com/NickvisionApps/Parabolic/discussions"),
                IsPortable = OperatingSystem.IsWindows() && args.Contains("--portable")
            };
            var loggingPath = appInfo.IsPortable ? "app.log" : Path.Combine(UserDirectories.LocalData, appInfo.Name, "app.log");
            builder.Properties.Add("AppInfo", appInfo);
            builder.Services.AddSingleton(appInfo);
            builder.ConfigureNickvision(args);
            builder.Services.AddSingleton<IEventsService, EventsService>();
            builder.Services.AddSingleton<IDiscoveryService, DiscoveryService>();
            builder.Services.AddSingleton<IDownloadService, DownloadService>();
            builder.Services.AddSingleton<IHistoryService, HistoryService>();
            builder.Services.AddSingleton<IRecoveryService, RecoveryService>();
            builder.Services.AddSingleton<IYtdlpExecutableService, YtdlpExecutableService>();
            builder.Services.AddHttpClient<IYtdlpExecutableService, YtdlpExecutableService>();
            builder.Services.AddTransient<AddDownloadDialogController>();
            builder.Services.AddTransient<HistoryViewController>();
            builder.Services.AddTransient<KeyringViewController>();
            builder.Services.AddSingleton<MainWindowController>();
            builder.Services.AddTransient<PreferencesViewController>();
            builder.Logging.ClearProviders();
            builder.Logging.SetMinimumLevel(LogLevel.Information);
            builder.Logging.AddConsole();
            builder.Logging.AddFile(loggingPath, false);
            return builder;
        }
    }
}
