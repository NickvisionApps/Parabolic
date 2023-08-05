using Nickvision.Aura;
using Nickvision.Aura.Network;
using Nickvision.Keyring.Models;
using Nickvision.Keyring.Controllers;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using Python.Runtime;
using System;
using System.IO;
using System.Threading.Tasks;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// A controller for a MainWindow
/// </summary>
public class MainWindowController : IDisposable
{
    private bool _disposed;
    private nint _pythonThreadState;
    private Keyring? _keyring;
    private NetworkMonitor? _netmon;

    /// <summary>
    /// Application's Aura
    /// </summary>
    public Aura Aura { get; init; }
    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => Aura.Active.AppInfo;
    /// <summary>
    /// The manager for downloads
    /// </summary>
    public DownloadManager DownloadManager { get; init; }
    /// <summary>
    /// A function for getting a password for the Keyring
    /// </summary>
    public Func<string, Task<string?>>? KeyringLoginAsync { get; set; }
    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public Theme Theme => Configuration.Current.Theme;
    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public NotificationPreference CompletedNotificationPreference => Configuration.Current.CompletedNotificationPreference;
    /// <summary>
    /// Whether or not to prevent suspend when downloads are in progress
    /// </summary>
    public bool PreventSuspendWhenDownloading => Configuration.Current.PreventSuspendWhenDownloading;
    /// <summary>
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground => Configuration.Current.RunInBackground;
    /// <summary>
    /// The DownloadOptions for a download
    /// </summary>
    public DownloadOptions DownloadOptions => new DownloadOptions(Configuration.Current.OverwriteExistingFiles, Configuration.Current.UseAria, Configuration.Current.AriaMaxConnectionsPerServer, Configuration.Current.AriaMinSplitSize, Configuration.Current.SubtitleLangs, Configuration.Current.ProxyUrl, Configuration.Current.CookiesPath, Configuration.Current.EmbedMetadata, Configuration.Current.EmbedChapters);
    /// <summary>
    /// Gets the DownloadHistory object
    /// </summary>
    public DownloadHistory DownloadHistory => (DownloadHistory)Aura.Active.ConfigFiles["downloadHistory"];

    /// <summary>
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;
    /// <summary>
    /// Occurs when the configuration is saved
    /// </summary>
    public event EventHandler<EventArgs>? PreventSuspendWhenDownloadingChanged;
    /// <summary>
    /// Occurs when the configuration is saved
    /// </summary>
    public event EventHandler<EventArgs>? RunInBackgroundChanged;

    /// <summary>
    /// Constructs a MainWindowController
    /// </summary>
    public MainWindowController()
    {
        _disposed = false;
        _pythonThreadState = IntPtr.Zero;
        DownloadManager = new DownloadManager(5);
        Aura = new Aura("org.nickvision.tubeconverter", "Nickvision Tube Converter", _("Parabolic"), _("Download web video and audio"));
        if (Directory.Exists($"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Name}"))
        {
            // Move or delete config files from older versions
            try
            {
                Directory.Move($"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Name}", ConfigurationLoader.ConfigDir);
            }
            catch (IOException)
            {
                Directory.Delete($"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Name}", true);
            }
        }
        Aura.Active.SetConfig<Configuration>("config");
        Configuration.Current.Saved += ConfigurationSaved;
        Aura.Active.SetConfig<DownloadHistory>("downloadHistory");
        AppInfo.Version = "2023.8.1-beta1";
        AppInfo.SourceRepo = new Uri("https://github.com/NickvisionApps/Parabolic");
        AppInfo.IssueTracker = new Uri("https://github.com/NickvisionApps/Parabolic/issues/new");
        AppInfo.SupportUrl = new Uri("https://github.com/NickvisionApps/Parabolic/discussions");
        AppInfo.ExtraLinks[_("List of supported sites")] = new Uri("https://github.com/yt-dlp/yt-dlp/blob/master/supportedsites.md");
        AppInfo.ExtraLinks[_("Matrix Chat")] = new Uri("https://matrix.to/#/#nickvision:matrix.org");
        AppInfo.Developers[_("Nicholas Logozzo")] = new Uri("https://github.com/nlogozzo");
        AppInfo.Developers[_("Contributors on GitHub ❤️")] = new Uri("https://github.com/NickvisionApps/Parabolic/graphs/contributors");
        AppInfo.Designers[_("Nicholas Logozzo")] = new Uri("https://github.com/nlogozzo");
        AppInfo.Designers[_("Fyodor Sobolev")] = new Uri("https://github.com/fsobolev");
        AppInfo.Designers[_("DaPigGuy")] = new Uri("https://github.com/DaPigGuy");
        AppInfo.Artists[_("David Lapshin")] = new Uri("https://github.com/daudix-UFO");
        AppInfo.TranslatorCredits = _("translator-credits");
    }

    /// <summary>
    /// Finalizes the MainWindowController
    /// </summary>
    ~MainWindowController() => Dispose(false);

    /// <summary>
    /// Frees resources used by the MainWindowController object
    /// </summary>
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Frees resources used by the MainWindowController object
    /// </summary>
    protected virtual void Dispose(bool disposing)
    {
        if (_disposed)
        {
            return;
        }
        PythonEngine.EndAllowThreads(_pythonThreadState);
        PythonEngine.Shutdown();
        if (Directory.Exists(Configuration.TempDir))
        {
            Directory.Delete(Configuration.TempDir, true);
        }
        _keyring?.Dispose();
        _netmon?.Dispose();
        _disposed = true;
    }

    /// <summary>
    /// Creates a new KeyringDialogController
    /// </summary>
    /// <returns>The KeyringDialogController</returns>
    public KeyringDialogController CreateKeyringDialogController() => new KeyringDialogController(AppInfo.ID, _keyring);

    /// <summary>
    /// Creates a new PreferencesViewController
    /// </summary>
    /// <returns>The PreferencesViewController</returns>
    public PreferencesViewController CreatePreferencesViewController() => new PreferencesViewController();

    /// <summary>
    /// Creates a new AddDownloadDialogController
    /// </summary>
    /// <returns>The new AddDownloadDialogController</returns>
    public AddDownloadDialogController CreateAddDownloadDialogController() => new AddDownloadDialogController(_keyring);

    /// <summary>
    /// Starts the application
    /// </summary>
    public async Task StartupAsync()
    {
        //Setup Folders
        DownloadManager.MaxNumberOfActiveDownloads = Configuration.Current.MaxNumberOfActiveDownloads;
        if (Directory.Exists(Configuration.TempDir))
        {
            Directory.Delete(Configuration.TempDir, true);
        }
        Directory.CreateDirectory(Configuration.TempDir);
        //Setup Dependencies
        try
        {
            var success = DependencyManager.SetupDependencies();
            if (!success)
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Unable to setup dependencies. Please restart the app and try again."), NotificationSeverity.Error));
            }
            else
            {
                RuntimeData.FormatterType = typeof(NoopFormatter);
                PythonEngine.Initialize();
                _pythonThreadState = PythonEngine.BeginAllowThreads();
            }
        }
        catch (Exception e)
        {
            NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Unable to setup dependencies. Please restart the app and try again."), NotificationSeverity.Error, "error", $"{e.Message}\n\n{e.StackTrace}"));
        }
        //Setup Keyring
        if(Keyring.Exists(AppInfo.ID))
        {
            var attempts = 0;
            while(_keyring == null && attempts < 3)
            {
                var password = await KeyringLoginAsync!(_("Unlock Keyring"));
                _keyring = Keyring.Access(AppInfo.ID, password);
                attempts++;
            }
            if(_keyring == null)
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Unable to unlock keyring. Restart the app to try again."), NotificationSeverity.Error));
            }
        }
        //Check Network
        _netmon = await NetworkMonitor.NewAsync();
        _netmon.StateChanged += (sender, state) =>
        {
            if (state == NetworkState.ConnectedGlobal)
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs("", NotificationSeverity.Success, "network-restored"));
            }
            else
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("No active internet connection"), NotificationSeverity.Error, "no-network"));
            }
        };
    }

    /// <summary>
    /// Updates the Keyring object
    /// </summary>
    /// <param name="controller">The KeyringDialogController</param>
    /// <exception cref="ArgumentException">Thrown if the Keyring does not belong</exception>
    public void UpdateKeyring(KeyringDialogController controller)
    {
        if(controller.Keyring != null && _keyring != null)
        {
            if(controller.Keyring.Name != _keyring.Name)
            {
                throw new ArgumentException($"Keyring is not {_keyring.Name}");
            }
        }
        _keyring = controller.Keyring;
    }

    /// <summary>
    /// Occurs when the configuration is saved
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void ConfigurationSaved(object? sender, EventArgs e)
    {
        PreventSuspendWhenDownloadingChanged?.Invoke(this, EventArgs.Empty);
        RunInBackgroundChanged?.Invoke(this, EventArgs.Empty);
        DownloadManager.MaxNumberOfActiveDownloads = Configuration.Current.MaxNumberOfActiveDownloads;
    }
}
