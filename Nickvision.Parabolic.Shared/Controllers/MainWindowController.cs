using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.Notifications;
using Nickvision.Desktop.System;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net.Http;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class MainWindowController : IDisposable
{
    private readonly string[] _args;
    private readonly HttpClient _httpClient;
    private readonly ServiceCollection _services;
    private AppVersion _latestAppVersion;
    private AppVersion _latestYtdlpVersion;

    public AppInfo AppInfo { get; }

    public event EventHandler<DownloadRequestedEventArgs>? DownloadRequested;

    public MainWindowController(string[] args)
    {
        _args = args;
        _services = new ServiceCollection();
        _httpClient = new HttpClient();
        _latestAppVersion = new AppVersion("2026.1.0-next");
        AppInfo = new AppInfo("org.nickvision.tubeconverter", "Nickvision Parabolic", "Parabolic")
        {
            Version = _latestAppVersion,
            Changelog = "- Parabolic has been rewritten in C# from C++\n- Added support for playlist quality options\n- Added support for playlist subtitle options\n- Added support for reversing the download order of a playlist\n- Added support for remembering the previous Download Immediately selection in the add download dialog\n- Improved the design and usability of the Windows version of Parabolic\n- Fixed an issue where Parabolic crashed when adding large amounts of downloads from a playlist\n- Fixed an issue where Parabolic crashed when validating certain URLs\n- Fixed an issue where Parabolic refused to start\n- Updated yt-dlp",
            SourceRepository = new Uri("https://github.com/NickvisionApps/Parabolic"),
            IssueTracker = new Uri("https://github.com/NickvisionApps/Parabolic/issues/new"),
            DiscussionsForum = new Uri("https://github.com/NickvisionApps/Parabolic/discussions")
        };
        // Register services
        var jsonFileService = _services.Add<IJsonFileService>(new JsonFileService(AppInfo))!;
        var updaterService = _services.Add<IUpdaterService>(new GitHubUpdaterService(AppInfo, _httpClient))!;
        var translationService = _services.Add<ITranslationService>(new GettextTranslationService(AppInfo, jsonFileService.Load<Configuration>(Configuration.Key).TranslationLanguage))!;
        var notificationService = _services.Add<INotificationService>(new NotificationService(AppInfo, translationService._("Open")))!;
        var secretService = _services.Add<ISecretService>(new SystemSecretService())!;
        var keyringService = _services.Add<IKeyringService>(new DatabaseKeyringService(AppInfo, secretService))!;
        var ytdlpExecutableService = _services.Add<IYtdlpExecutableService>(new YtdlpExecutableService(jsonFileService, _httpClient))!;
        var historyService = _services.Add<IHistoryService>(new HistoryService(AppInfo))!;
        var recoveryService = _services.Add<IRecoveryService>(new RecoveryService(AppInfo))!;
        _services.Add<IDiscoveryService>(new DiscoveryService(jsonFileService, translationService, ytdlpExecutableService));
        _services.Add<IDownloadService>(new DownloadService(jsonFileService, translationService, ytdlpExecutableService, historyService, recoveryService));
        _latestYtdlpVersion = ytdlpExecutableService!.BundledVersion;
        // Translate strings
        AppInfo.ShortName = translationService._("Parabolic");
        AppInfo.Description = translationService._("Download web video and audio.");
        AppInfo.DocumentationStore = new Uri(AppInfo.Version.IsPreview ? "https://github.com/NickvisionApps/Parabolic/blob/main/docs/html" : $"https://github.com/NickvisionApps/Parabolic/blob/{AppInfo.Version}/docs/html");
        AppInfo.ExtraLinks.Add(translationService._("Matrix Chat"), new Uri("https://matrix.to/#/#nickvision:matrix.org"));
        AppInfo.Developers.Add("Nicholas Logozzo", "https://github.com/nlogozzo");
        AppInfo.Developers.Add(translationService._("Contributors on GitHub ❤️"), "https://github.com/NickvisionApps/Parabolic/graphs/contributors");
        AppInfo.Designers.Add("Nicholas Logozzo", "https://github.com/nlogozzo");
        AppInfo.Designers.Add(translationService._("Fyodor Sobolev"), "https://github.com/fsobolev");
        AppInfo.Designers.Add("DaPigGuy", "https://github.com/DaPigGuy");
        AppInfo.Artists.Add(translationService._("David Lapshin"), "https://github.com/daudix");
        AppInfo.TranslationCredits = translationService._("translation-credits");
    }

    ~MainWindowController()
    {
        Dispose(false);
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    public event EventHandler<AppNotificationSentEventArgs>? AppNotificationSent
    {
        add => _services.Get<INotificationService>()!.AppNotificationSent += value;

        remove => _services.Get<INotificationService>()!.AppNotificationSent -= value;
    }

    public event EventHandler<JsonFileSavedEventArgs>? JsonFileSaved
    {
        add => _services.Get<IJsonFileService>()!.Saved += value;

        remove => _services.Get<IJsonFileService>()!.Saved -= value;
    }

    public event EventHandler<HistoryChangedEventArgs> HistoryChanged
    {
        add => _services.Get<IHistoryService>()!.Changed += value;

        remove => _services.Get<IHistoryService>()!.Changed -= value;
    }

    public event EventHandler<DownloadAddedEventArgs> DownloadAdded
    {
        add => _services.Get<IDownloadService>()!.DownloadAdded += value;

        remove => _services.Get<IDownloadService>()!.DownloadAdded -= value;
    }

    public event EventHandler<DownloadCompletedEventArgs> DownloadCompleted
    {
        add => _services.Get<IDownloadService>()!.DownloadCompleted += value;

        remove => _services.Get<IDownloadService>()!.DownloadCompleted -= value;
    }

    public event EventHandler<DownloadProgressChangedEventArgs> DownloadProgressChanged
    {
        add => _services.Get<IDownloadService>()!.DownloadProgressChanged += value;

        remove => _services.Get<IDownloadService>()!.DownloadProgressChanged -= value;
    }

    public event EventHandler<DownloadEventArgs> DownloadRetired
    {
        add => _services.Get<IDownloadService>()!.DownloadRetired += value;

        remove => _services.Get<IDownloadService>()!.DownloadRetired -= value;
    }

    public event EventHandler<DownloadEventArgs> DownloadStartedFromQueue
    {
        add => _services.Get<IDownloadService>()!.DownloadStartedFromQueue += value;

        remove => _services.Get<IDownloadService>()!.DownloadStartedFromQueue -= value;
    }

    public event EventHandler<DownloadEventArgs> DownloadStopped
    {
        add => _services.Get<IDownloadService>()!.DownloadStopped += value;

        remove => _services.Get<IDownloadService>()!.DownloadStopped -= value;
    }

    public bool CanShutdown => _services.Get<IDownloadService>()!.RemainingCount == 0;

    public AddDownloadDialogController AddDownloadDialogController => new AddDownloadDialogController(_services.Get<IJsonFileService>()!, _services.Get<ITranslationService>()!, _services.Get<IKeyringService>()!, _services.Get<INotificationService>()!, _services.Get<IDiscoveryService>()!, _services.Get<IDownloadService>()!);

    public HistoryPageController HistoryPageController
    {
        get
        {
            var controller = new HistoryPageController(_services.Get<ITranslationService>()!, _services.Get<IHistoryService>()!);
            controller.DownloadRequested += (sender, e) => DownloadRequested?.Invoke(this, e);
            return controller;
        }
    }

    public KeyringPageController KeyringPageController => new KeyringPageController(_services.Get<ITranslationService>()!, _services.Get<INotificationService>()!, _services.Get<IKeyringService>()!);

    public PreferencesViewController PreferencesViewController => new PreferencesViewController(_services.Get<IJsonFileService>()!, _services.Get<ITranslationService>()!, _services.Get<INotificationService>()!, _services.Get<IHistoryService>()!);

    public int RemainingDownloadsCount => _services.Get<IDownloadService>()!.RemainingCount;

    public bool ShowDisclaimerOnStartup
    {
        get => _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key).ShowDislcaimerOnStartup;

        set
        {
            var config = _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key);
            config.ShowDislcaimerOnStartup = value;
            _services.Get<IJsonFileService>()!.Save(config, Configuration.Key);
        }
    }

    public Theme Theme => _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key).Theme;

    public ITranslationService Translator => _services.Get<ITranslationService>()!;

    public WindowGeometry WindowGeometry
    {
        get => _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key).WindowGeometry;

        set
        {
            var config = _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key);
            config.WindowGeometry = value;
            _services.Get<IJsonFileService>()!.Save(config, Configuration.Key);
        }
    }

    public bool ShowDislcaimerOnStartup
    {
        get => _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key).ShowDislcaimerOnStartup;

        set
        {
            var config = _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key);
            config.ShowDislcaimerOnStartup = value;
            _services.Get<IJsonFileService>()!.Save(config, Configuration.Key);
        }
    }

    public async Task CheckForUpdatesAsync(bool showNotificationForNoUpdates)
    {
        var config = _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key);
        var notificationService = _services.Get<INotificationService>()!;
        var translationService = _services.Get<ITranslationService>()!;
        var updaterService = _services.Get<IUpdaterService>()!;
        var ytdlpService = _services.Get<IYtdlpExecutableService>()!;
        var stableAppVersion = await updaterService.GetLatestStableVersionAsync();
        var stableYtdlpVersion = await ytdlpService.GetLatestStableVersionAsync();
        if (stableAppVersion is not null)
        {
            _latestAppVersion = stableAppVersion;
        }
        if (stableYtdlpVersion is not null)
        {
            _latestYtdlpVersion = stableYtdlpVersion;
        }
        if (config.AllowPreviewUpdates)
        {
            var previewAppVersion = await updaterService.GetLatestPreviewVersionAsync();
            var previewYtdlpVersion = await ytdlpService.GetLatestPreviewVersionAsync();
            if (previewAppVersion is not null && previewAppVersion > stableAppVersion)
            {
                _latestAppVersion = previewAppVersion;
            }
            if (previewYtdlpVersion is not null && previewYtdlpVersion > stableYtdlpVersion)
            {
                _latestYtdlpVersion = previewYtdlpVersion;
            }
        }
        if (_latestAppVersion > AppInfo.Version!)
        {
            notificationService.Send(new AppNotification(translationService._("New {0} update available: {1}", AppInfo.ShortName!, _latestAppVersion.ToString()), NotificationSeverity.Success)
            {
                Action = "update"
            });
        }
        else if (_latestYtdlpVersion > ytdlpService.BundledVersion && _latestYtdlpVersion > config.InstalledYtdlpAppVersion)
        {
            notificationService.Send(new AppNotification(translationService._("New yt-dlp update available: {0}", _latestYtdlpVersion.ToString()), NotificationSeverity.Success)
            {
                Action = "update-ytdlp"
            });
        }
        else if (showNotificationForNoUpdates)
        {
            notificationService.Send(new AppNotification(translationService._("No update available"), NotificationSeverity.Warning));
        }
    }

    public IEnumerable<int> ClearCompletedDownloads() => _services.Get<IDownloadService>()!.ClearCompleted();

    public IEnumerable<int> ClearQueuedDownloads() => _services.Get<IDownloadService>()!.ClearQueued();

    public async Task<string> GetDebugInformationAsync(string extraInformation = "")
    {
        var ytdlpVersion = await _services.Get<IYtdlpExecutableService>()!.GetExecutableVersionAsync();
        var denoVersion = await ExecuteAsync("deno", "--version");
        var ffmpegVersion = await ExecuteAsync("ffmpeg", "-version");
        var ariaVersion = await ExecuteAsync("aria2c", "--version");
        extraInformation += string.IsNullOrEmpty(extraInformation) ? string.Empty : "\n";
        extraInformation += $"yt-dlp: {(ytdlpVersion is not null ? ytdlpVersion.ToString() : "not found")}";
        extraInformation += $"\ndeno: {(!string.IsNullOrEmpty(denoVersion) ? denoVersion.Substring(denoVersion.IndexOf("deno ") + 5, denoVersion.IndexOf('\n') - 5) : "not found")}";
        extraInformation += $"\nffmpeg: {(!string.IsNullOrEmpty(ffmpegVersion) ? ffmpegVersion.Substring(ffmpegVersion.IndexOf("ffmpeg version") + 15, ffmpegVersion.IndexOf("Copyright") - 15) : "not found")}";
        extraInformation += $"\naria2: {(!string.IsNullOrEmpty(ariaVersion) ? ariaVersion.Substring(ariaVersion.IndexOf("aria2 version") + 14, ariaVersion.IndexOf('\n') - 14) : "not found")}";
        extraInformation += $"\n\n{await _services.Get<IJsonFileService>()!.LoadAsync<Configuration>()}";
        return Desktop.System.Environment.GetDebugInformation(AppInfo, extraInformation);
    }

    public bool PauseDownload(int id) => _services.Get<IDownloadService>()!.Pause(id);

    public bool ResumeDownload(int id) => _services.Get<IDownloadService>()!.Resume(id);

    public async Task RetryFailedDownloadsAsync() => await _services.Get<IDownloadService>()!.RetryFailedAsync();

    public async Task<bool> RetryDownloadAsync(int id) => await _services.Get<IDownloadService>()!.RetryAsync(id);

    public async Task StopAllDownloadsAsync() => await _services.Get<IDownloadService>()!.StopAllAsync();

    public async Task<bool> StopDownloadAsync(int id) => await _services.Get<IDownloadService>()!.StopAsync(id);

#if OS_WINDOWS
    public async Task WindowsUpdateAsync(IProgress<DownloadProgress> progress)
    {
        var res = await _services.Get<IUpdaterService>()!.WindowsUpdate(_latestAppVersion, progress);
        if (!res)
        {
            _services.Get<INotificationService>()!.Send(new AppNotification(_services.Get<ITranslationService>()!._("Unable to download and install the update"), NotificationSeverity.Error));
        }
    }
#endif

    public async Task YtdlpUpdateAsync(IProgress<DownloadProgress> progress)
    {
        var res = await _services.Get<IYtdlpExecutableService>()!.DownloadUpdateAsync(_latestYtdlpVersion, progress);
        if (!res)
        {
            _services.Get<INotificationService>()!.Send(new AppNotification(_services.Get<ITranslationService>()!._("Unable to download and install the yt-dlp update"), NotificationSeverity.Error));
        }
    }

    private void Dispose(bool disposing)
    {
        if (!disposing)
        {
            return;
        }
        _services.Dispose();
        _httpClient.Dispose();
    }

    private async Task<string> ExecuteAsync(string executable, string arguments)
    {
        using var process = new Process()
        {
            StartInfo = new ProcessStartInfo(executable, arguments)
            {
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            }
        };
        process.Start();
        var outputTask = process.StandardOutput.ReadToEndAsync();
        await process.WaitForExitAsync();
        var output = await outputTask;
        return process.ExitCode == 0 ? output : string.Empty;
    }
}
