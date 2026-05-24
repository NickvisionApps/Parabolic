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
                Version = new AppVersion("2026.5.0-beta1"),
                Changelog = """
                - Improved time frame downloads to cut with ffmpeg instead of using yt-dlp's broken --download-sections option
                - Fixed an issue where some playlists throw a requested format not available error
                - Fixed an issue where the macOS configuration directory was incorrect
                - Fixed an issue where None post processor argument would not be saved
                - Fixed an issue where format strings were not translated correctly
                - Fixed an issue where the application would not start on KDE
                - Fixed an issue where the application would not start if the database file was invalid
                - Fixed an issue where dependencies were missing from the macOS bundle
                - Updated bundled deno
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
