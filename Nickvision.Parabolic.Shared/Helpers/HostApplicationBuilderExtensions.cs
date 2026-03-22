using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Services;
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
                Version = new AppVersion("2026.3.0-beta3"),
                Changelog = """
                - Added macOS app for the GNOME version of Parabolic
                - Added Windows portable version of Parabolic
                - Added the ability to toggle super resolution formats in Parabolic's settings
                - Added the ability to specify a preferred frame rate for video downloads in Parabolic's settings
                - Added the ability to automatically translate embedded metadata and chapters to the app's language on supported sites. This can be turned off in Converter settings
                - Added the ability to update deno from within the app
                - Added thumbnail image preview to add download dialog and downloads view
                - Added failed filter to downloads view
                - Added total time label to playlist items view
                - Improved selection of playlist video formats when resolutions are specified
                - Improved selection of playlist audio formats on Windows when bitrates are specified
                - Improved cropping of audio thumbnails
                - Improved handling of long file names, they will now be truncated if too long
                - Removed unsupported cookie browsers on Windows. Manual txt files should be used instead
                - Fixed an issue where download progress did not show correctly
                - Fixed an issue where the preferred video codec was ignored when a preferred frame rate was also set
                - Fixed an issue where credentials would not save on Linux
                - Fixed an issue where batch files were unusable on Linux and macOS
                - Fixed an issue where uploading a cookies file did not work on Windows
                - Fixed an issue where time frame downloads would not complete on Windows
                - Updated yt-dlp
                """,
                SourceRepository = new Uri("https://github.com/NickvisionApps/Parabolic"),
                IssueTracker = new Uri("https://github.com/NickvisionApps/Parabolic/issues/new"),
                DiscussionsForum = new Uri("https://github.com/NickvisionApps/Parabolic/discussions"),
                IsPortable = OperatingSystem.IsWindows() && args.Contains("--portable")
            };
            builder.Properties.Add("AppInfo", appInfo);
            builder.Services.AddSingleton(appInfo);
            builder.ConfigureNickvision(args, appInfo.IsPortable ? "app.log" : Path.Combine(UserDirectories.LocalData, appInfo.Name, "app.log"));
            builder.Services.AddSingleton<IEventsService, EventsService>();
            builder.Services.AddSingleton<IDenoExecutableService, DenoExecutableService>();
            builder.Services.AddSingleton<IDiscoveryService, DiscoveryService>();
            builder.Services.AddSingleton<IDownloadService, DownloadService>();
            builder.Services.AddSingleton<IHistoryService, HistoryService>();
            builder.Services.AddSingleton<IRecoveryService, RecoveryService>();
            builder.Services.AddSingleton<IThumbnailService, ThumbnailService>();
            builder.Services.AddSingleton<IYtdlpExecutableService, YtdlpExecutableService>();
            builder.Services.AddTransient<AddDownloadDialogController>();
            builder.Services.AddTransient<HistoryViewController>();
            builder.Services.AddTransient<KeyringViewController>();
            builder.Services.AddSingleton<MainWindowController>();
            builder.Services.AddTransient<PreferencesViewController>();
            return builder;
        }
    }
}
