using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Net.Http;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class MainWindowController : IDisposable
{
    private readonly string[] _args;
    private readonly HttpClient _httpClient;
    private readonly ServiceCollection _services;
    private AppVersion _latestVersion;

    public AppInfo AppInfo { get; init; }

    public MainWindowController(string[] args)
    {
        _args = args;
        _services = new ServiceCollection();
        _httpClient = new HttpClient();
        _latestVersion = new AppVersion("2025.12.0-next");
        AppInfo = new AppInfo("org.nickvision.tubeconverter", "Nickvision Parabolic", "Parabolic")
        {
            Version = _latestVersion,
            Changelog = "- Initial release",
            SourceRepository = new Uri("https://github.com/NickvisionApps/Parabolic"),
            IssueTracker = new Uri("https://github.com/NickvisionApps/Parabolic/issues/new"),
            DiscussionsForum = new Uri("https://github.com/NickvisionApps/Parabolic/discussions")
        };
        // Register services
        var jsonFileService = _services.Add<IJsonFileService>(new JsonFileService(AppInfo));
        var updaterService = _services.Add<IUpdaterService>(new GitHubUpdaterService(AppInfo, _httpClient));
        var translationService = _services.Add<ITranslationService>(new GettextTranslationService(AppInfo, jsonFileService!.Load<Configuration>(Configuration.Key).TranslationLanguage));
        var notificationService = _services.Add<INotificationService>(new NotificationService(AppInfo, translationService!._("Open")));
        _services.Add<IDiscoveryService>(new DiscoveryService());
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

    public bool CanShutdown => true;

    public PreferencesViewController PreferencesViewController => new PreferencesViewController(_services.Get<IJsonFileService>()!, _services.Get<ITranslationService>()!);

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

    public async Task CheckForUpdatesAsync(bool showNotificationForNoUpdates)
    {
        var config = _services.Get<IJsonFileService>()!.Load<Configuration>(Configuration.Key);
        var notificationService = _services.Get<INotificationService>()!;
        var translationService = _services.Get<ITranslationService>()!;
        var updaterService = _services.Get<IUpdaterService>()!;
        var stableVersion = await updaterService.GetLatestStableVersionAsync();
        if (stableVersion is not null)
        {
            _latestVersion = stableVersion;
        }
        if (config.AllowPreviewUpdates)
        {
            var previewVersion = await updaterService.GetLatestPreviewVersionAsync();
            if (previewVersion is not null && previewVersion > stableVersion)
            {
                _latestVersion = previewVersion;
            }
        }
        if (_latestVersion > AppInfo.Version!)
        {
            notificationService.Send(new AppNotification(translationService._("New {0} update available: {1}", AppInfo.ShortName!, _latestVersion.ToString()), NotificationSeverity.Success)
            {
                Action = "update"
            });
        }
        else if (showNotificationForNoUpdates)
        {
            notificationService.Send(new AppNotification(translationService._("No update available"), NotificationSeverity.Warning));
        }
    }

    public string GetDebugInformation(string extraInformation = "") => Desktop.System.Environment.GetDebugInformation(AppInfo, extraInformation);

#if OS_WINDOWS
    public async Task WindowsUpdateAsync(IProgress<DownloadProgress> progress)
    {
        var res = await _services.Get<IUpdaterService>()!.WindowsUpdate(_latestVersion, progress);
        if (!res)
        {
            _services.Get<INotificationService>()!.Send(new AppNotification(_services.Get<ITranslationService>()!._("Unable to download and install the update"), NotificationSeverity.Error));
        }
    }
#endif

    private void Dispose(bool disposing)
    {
        if (!disposing)
        {
            return;
        }
        _services.Dispose();
        _httpClient.Dispose();
    }
}
