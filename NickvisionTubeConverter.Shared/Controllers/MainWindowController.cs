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

    /// <summary>
    /// The manager for downloads
    /// </summary>
    public DownloadManager DownloadManager { get; init; }
    /// <summary>
    /// A function for getting a password for the Keyring
    /// </summary>
    public Func<string, Task<string?>>? KeyringLoginAsync { get; set; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;
    /// <summary>
    /// Whether or not the version is a development version or not
    /// </summary>
    public bool IsDevVersion => AppInfo.Current.Version.IndexOf('-') != -1;
    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public Theme Theme => Configuration.Current.Theme;
    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public NotificationPreference CompletedNotificationPreference => Configuration.Current.CompletedNotificationPreference;
    /// <summary>
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground => Configuration.Current.RunInBackground;
    /// <summary>
    /// The DownloadOptions for a download
    /// </summary>
    public DownloadOptions DownloadOptions => new DownloadOptions(Configuration.Current.OverwriteExistingFiles, Configuration.Current.UseAria, Configuration.Current.CookiesPath, Configuration.Current.AriaMaxConnectionsPerServer, Configuration.Current.AriaMinSplitSize, Configuration.Current.SubtitleLangs, Configuration.Current.EmbedMetadata, Configuration.Current.EmbedChapters);
    /// <summary>
    /// Gets the DownloadHistory object
    /// </summary>
    public DownloadHistory DownloadHistory => DownloadHistory.Current;

    /// <summary>
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;
    /// <summary>
    /// Invoked to check if RunInBackground changed after settings saved
    /// </summary>
    public event EventHandler? RunInBackgroundChanged;

    /// <summary>
    /// Constructs a MainWindowController
    /// </summary>
    public MainWindowController()
    {
        _disposed = false;
        _pythonThreadState = IntPtr.Zero;
        DownloadManager = new DownloadManager(5);
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
        _disposed = true;
    }

    /// <summary>
    /// Creates a new KeyringDialogController
    /// </summary>
    /// <returns>The KeyringDialogController</returns>
    public KeyringDialogController CreateKeyringDialogController() => new KeyringDialogController(AppInfo.Current.ID, _keyring);

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
        Configuration.Current.Saved += ConfigurationSaved;
        DownloadManager.MaxNumberOfActiveDownloads = Configuration.Current.MaxNumberOfActiveDownloads;
        if (Directory.Exists(Configuration.TempDir))
        {
            Directory.Delete(Configuration.TempDir, true);
        }
        Directory.CreateDirectory(Configuration.TempDir);
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
        if(Keyring.Exists(AppInfo.Current.ID))
        {
            var attempts = 0;
            while(_keyring == null && attempts < 3)
            {
                var password = await KeyringLoginAsync!(_("Unlock Keyring"));
                _keyring = Keyring.Access(AppInfo.Current.ID, password);
                attempts++;
            }
            if(_keyring == null)
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Unable to unlock keyring. Restart the app to try again."), NotificationSeverity.Error));
            }
        }
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
        RunInBackgroundChanged?.Invoke(this, EventArgs.Empty);
        DownloadManager.MaxNumberOfActiveDownloads = Configuration.Current.MaxNumberOfActiveDownloads;
    }
}
