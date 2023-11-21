using Nickvision.Aura;
using Nickvision.Aura.Events;
using Nickvision.Aura.Keyring;
using Nickvision.Aura.Network;
using Nickvision.Aura.Taskbar;
using Nickvision.Aura.Update;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using Python.Runtime;
using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// A controller for a MainWindow
/// </summary>
public class MainWindowController : IDisposable
{
    private bool _disposed;
    private bool _started;
    private string? _urlToLaunch;
    private nint _pythonThreadState;
    private Keyring? _keyring;
    private NetworkMonitor? _netmon;
    private TaskbarItem? _taskbarItem;
    private Updater? _updater;
    private readonly Stopwatch _taskbarStopwatch;

    private const int TASKBAR_STOPWATCH_THRESHOLD = 500;

    /// <summary>
    /// The manager for downloads
    /// </summary>
    public DownloadManager DownloadManager { get; init; }
    /// <summary>
    /// A function for getting a password for the Keyring
    /// </summary>
    public Func<string, Task<(bool WasSkipped, string Password)>>? KeyringLoginAsync { get; set; }
    /// <summary>
    /// Whether or not the window is active
    /// </summary>
    public bool IsWindowActive { get; set; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => Aura.Active.AppInfo;
    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public Theme Theme => Configuration.Current.Theme;
    /// <summary>
    /// Whether or not to prevent suspend when downloads are in progress
    /// </summary>
    public bool PreventSuspendWhenDownloading => Configuration.Current.PreventSuspendWhenDownloading;
    /// <summary>
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground => Configuration.Current.RunInBackground;

    /// <summary>
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;
    /// <summary>
    /// Occurs when a shell notification is sent
    /// </summary>
    public event EventHandler<ShellNotificationSentEventArgs>? ShellNotificationSent;
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
    public MainWindowController(string[] args)
    {
        _disposed = false;
        _started = false;
        if (args.Length > 0)
        {
            UrlToLaunch = args[0];
        }
        _pythonThreadState = IntPtr.Zero;
        _taskbarStopwatch = new Stopwatch();
        DownloadManager = new DownloadManager(5);
        IsWindowActive = false;
        Aura.Init("org.nickvision.tubeconverter", "Nickvision Tube Converter");
        AppInfo.EnglishShortName = "Parabolic";
        if (Directory.Exists($"{UserDirectories.Config}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Name}"))
        {
            // Move config files from older versions and delete old directory
            try
            {
                foreach (var file in Directory.GetFiles($"{UserDirectories.Config}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Name}"))
                {
                    File.Move(file, $"{UserDirectories.ApplicationConfig}{Path.DirectorySeparatorChar}{Path.GetFileName(file)}");
                }
            }
            catch (IOException) { }
            Directory.Delete($"{UserDirectories.Config}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Name}", true);
        }
        Aura.Active.SetConfig<Configuration>("config");
        Configuration.Current.Saved += ConfigurationSaved;
        Aura.Active.SetConfig<DownloadHistory>("downloadHistory");
        AppInfo.Version = "2023.11.1-next";
        AppInfo.ShortName = _("Parabolic");
        AppInfo.Description = _("Download web video and audio");
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
        DownloadManager.DownloadCompleted += DownloadCompleted;
    }

    /// <summary>
    /// Finalizes the MainWindowController
    /// </summary>
    ~MainWindowController() => Dispose(false);

    /// <summary>
    /// Whether or not to show the disclaimer on startup
    /// </summary>
    public bool ShowDisclaimerOnStartup
    {
        get => Configuration.Current.ShowDisclaimerOnStartup;

        set => Configuration.Current.ShowDisclaimerOnStartup = value;
    }

    /// <summary>
    /// A url to set to launch on startup
    /// </summary>
    public string? UrlToLaunch
    {
        get => _urlToLaunch;

        set
        {
            var result = Uri.TryCreate(value, UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
            if (result)
            {
                _urlToLaunch = value;
            }
            else
            {
                _urlToLaunch = null;
            }
        }
    }

    /// <summary>
    /// The TaskbarItem to show progress
    /// </summary>
    public TaskbarItem? TaskbarItem
    {
        set
        {
            if (value == null)
            {
                return;
            }
            _taskbarItem = value;
            DownloadManager.DownloadProgressUpdated += (_, _) => UpdateTaskbar();
            DownloadManager.DownloadCompleted += (_, _) => UpdateTaskbar(true);
            DownloadManager.DownloadStopped += (_, _) => UpdateTaskbar(true);
        }
    }

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
        DownloadManager.StopAllDownloads(false);
        _taskbarItem?.Dispose();
        PythonEngine.EndAllowThreads(_pythonThreadState);
        PythonEngine.Shutdown();
        if (Directory.Exists(UserDirectories.ApplicationCache))
        {
            try
            {
                Directory.Delete(UserDirectories.ApplicationCache, true);
            }
            catch { }
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
    /// <returns>A URL to validate on startup</returns>
    public async Task<string?> StartupAsync()
    {
        if(_started)
        {
            return UrlToLaunch;
        }
        //Update app
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows) && Configuration.Current.AutomaticallyCheckForUpdates)
        {
            await CheckForUpdatesAsync();
        }
        //Setup folders
        DownloadManager.MaxNumberOfActiveDownloads = Configuration.Current.MaxNumberOfActiveDownloads;
        try
        {
            if (Directory.Exists(UserDirectories.ApplicationCache))
            {
                Directory.Delete(UserDirectories.ApplicationCache, true);
            }
            Directory.CreateDirectory(UserDirectories.ApplicationCache);
        }
        catch { }
        //Setup Dependencies
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            Runtime.PythonDLL = DependencyLocator.Find("python311.dll")!;
        }
        else
        {
            using var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = DependencyLocator.Find("python3"),
                    Arguments = "-c \"import sysconfig; import os; print(('/snap/tube-converter/current/gnome-platform' if os.environ.get('SNAP') else '') + ('/'.join(sysconfig.get_config_vars('LIBDIR', 'INSTSONAME'))))\"",
                    UseShellExecute = false,
                    RedirectStandardOutput = true
                }
            };
            process.Start();
            Runtime.PythonDLL = process.StandardOutput.ReadToEnd().Trim();
            await process.WaitForExitAsync();
        }
        // Install yt-dlp plugin
        var pluginPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}yt-dlp{Path.DirectorySeparatorChar}plugins{Path.DirectorySeparatorChar}tubeconverter{Path.DirectorySeparatorChar}yt_dlp_plugins{Path.DirectorySeparatorChar}postprocessor{Path.DirectorySeparatorChar}tubeconverter.py";
        Directory.CreateDirectory(pluginPath.Substring(0, pluginPath.LastIndexOf(Path.DirectorySeparatorChar)));
        using var pluginResource = Assembly.GetExecutingAssembly().GetManifestResourceStream("NickvisionTubeConverter.Shared.Resources.tubeconverter.py")!;
        using var pluginFile = new FileStream(pluginPath, FileMode.Create, FileAccess.Write);
        pluginResource.CopyTo(pluginFile);
        RuntimeData.FormatterType = typeof(NoopFormatter);
        PythonEngine.Initialize();
        _pythonThreadState = PythonEngine.BeginAllowThreads();
        //Setup Keyring
        if (Keyring.Exists(AppInfo.ID))
        {
            _keyring = await Keyring.AccessAsync(AppInfo.ID);
            while (_keyring == null)
            {
                var res = await KeyringLoginAsync!(_("Unlock Keyring"));
                if (res.WasSkipped)
                {
                    break;
                }
                _keyring = await Keyring.AccessAsync(AppInfo.ID, res.Password);
            }
        }
        //Check Network
        _netmon = await NetworkMonitor.NewAsync();
        _netmon.StateChanged += (sender, state) =>
        {
            if (state)
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs("", NotificationSeverity.Success, "network-restored"));
            }
            else
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("No active internet connection"), NotificationSeverity.Error, "no-network"));
            }
        };
        //Fix Aria Max Connections Per Server
        if (Configuration.Current.AriaMaxConnectionsPerServer > 16)
        {
            Configuration.Current.AriaMaxConnectionsPerServer = 16;
            Aura.Active.SaveConfig("config");
        }
        _started = true;
        return UrlToLaunch;
    }

    /// <summary>
    /// Saves the app's configuration file to disk
    /// </summary>
    public void SaveConfig() => Aura.Active.SaveConfig("config");

    /// <summary>
    /// Checks for an application update and notifies the user if one is available
    /// </summary>
    public async Task CheckForUpdatesAsync()
    {
        if (_updater == null)
        {
            _updater = await Updater.NewAsync();
        }
        var version = await _updater!.GetCurrentStableVersionAsync();
        if (version != null && (!AppInfo.IsDevVersion ? version > new Version(AppInfo.Version) : version >= new Version(AppInfo.Version.Remove(AppInfo.Version.IndexOf('-')))))
        {
            NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("New update available."), NotificationSeverity.Success, "update"));
        }
    }

    /// <summary>
    /// Downloads and installs the latest application update for Windows systems
    /// </summary>
    /// <returns>True if successful, else false</returns>
    /// <remarks>CheckForUpdatesAsync must be called before this method</remarks>
    public async Task<bool> WindowsUpdateAsync()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows) && _updater != null)
        {
            var res = await _updater.WindowsUpdateAsync(VersionType.Stable);
            if (!res)
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Unable to download and install update."), NotificationSeverity.Error));
            }
            return res;
        }
        return false;
    }

    /// <summary>
    /// Updates the Keyring object
    /// </summary>
    /// <param name="controller">The KeyringDialogController</param>
    /// <exception cref="ArgumentException">Thrown if the Keyring does not belong</exception>
    public void UpdateKeyring(KeyringDialogController controller)
    {
        if (controller.Keyring != null && _keyring != null)
        {
            if (controller.Keyring.Name != _keyring.Name)
            {
                throw new ArgumentException($"Keyring is not {_keyring.Name}");
            }
        }
        _keyring = controller.Keyring;
    }

    /// <summary>
    /// Adds downloads to the download manager
    /// </summary>
    /// <param name="controller">AddDownloadDialogController</param>
    public void AddDownloads(AddDownloadDialogController controller)
    {
        foreach (var download in controller.Downloads)
        {
            DownloadManager.AddDownload(download, DownloadOptions.Current);
        }
    }

    /// <summary>
    /// Updates taskbar item to show current total progress
    /// </summary>
    /// <param name="ignoreStopwatch">Whether to ignore stopwatch that limits update frequency</param>
    private void UpdateTaskbar(bool ignoreStopwatch = false)
    {
        if (_taskbarStopwatch.IsRunning && _taskbarStopwatch.Elapsed.TotalMilliseconds < TASKBAR_STOPWATCH_THRESHOLD && !ignoreStopwatch)
        {
            return;
        }
        _taskbarStopwatch.Restart();
        var progress = DownloadManager.TotalProgress;
        if (progress > 0 && progress < 1)
        {
            _taskbarItem.Progress = progress;
            _taskbarItem.Count = DownloadManager.RemainingDownloadsCount;
        }
        else
        {
            _taskbarItem.ProgressState = ProgressFlags.NoProgress;
            _taskbarItem.Count = -1;
            _taskbarStopwatch.Stop();
        }
    }

    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">(Guid Id, bool Successful, string Filename, bool ShowNotification)</param>
    private void DownloadCompleted(object? sender, (Guid Id, bool Successful, string Filename, bool ShowNotification) e)
    {
        if(e.ShowNotification && !IsWindowActive)
        {
            if (Configuration.Current.CompletedNotificationPreference == NotificationPreference.ForEach)
            {
                ShellNotificationSent?.Invoke(this, new ShellNotificationSentEventArgs(!e.Successful ? _("Download Finished With Error") : _("Download Finished"), !e.Successful ? _("\"{0}\" has finished with an error!", e.Filename) : _("\"{0}\" has finished downloading.", e.Filename), !e.Successful ? NotificationSeverity.Error : NotificationSeverity.Success, "open-file", DownloadManager.GetCompletedDownloadPath(e.Id)!));
            }
            else if (Configuration.Current.CompletedNotificationPreference == NotificationPreference.AllCompleted && !DownloadManager.AreDownloadsRunning && !DownloadManager.AreDownloadsQueued)
            {
                ShellNotificationSent?.Invoke(this, new ShellNotificationSentEventArgs(_("Downloads Finished"), _("All downloads have finished."), NotificationSeverity.Informational));
            }
        }
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
