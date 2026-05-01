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
                Version = new AppVersion("2026.4.1"),
                Changelog = """
                - Fixed an issue where some settings would not save correctly
                - Fixed an issue where playlist downloads with a resolution limit had no audio
                - Fixed an issue where portrait/vertical videos in playlists downloaded at incorrect resolutions
                - Fixed an issue where downloads from sites with muxed-only streams would fail
                - Fixed an issue where downloading a time frame clip from a long video produced an incomplete result
                - Fixed an issue where downloading a time frame clip from a long video could hang indefinitely with aria2c enabled
                - Fixed an issue where X/Twitter quoted downloads could produce the same video twice
                - Fixed an issue where deno was unable to be updated in-app on Linux
                - Fixed an issue where browser cookies could not be found when running via Flatpak on Linux
                - Fixed an issue where Parabolic would not start on KDE desktops
                - Fixed an issue where Parabolic did not open links from browser extension on Windows
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
            builder.Services.AddHostedService<ConfigurationMigrationService>();
            return builder;
        }
    }
}
