using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// A controller for a MainWindow
/// </summary>
public class MainWindowController : IDisposable
{
    private bool _disposed;
    private nint _pythonThreadState;
    private List<IDownloadRowControl> _downloadingRows;
    private List<IDownloadRowControl> _completedRows;
    private List<IDownloadRowControl> _queuedRows;

    /// <summary>
    /// The localizer to get translated strings from
    /// </summary>
    public Localizer Localizer { get; init; }
    /// <summary>
    /// The UI function for creating a download row
    /// </summary>
    public Func<Download, IDownloadRowControl>? UICreateDownloadRow { get; set; }
    /// <summary>
    /// The UI function for moving a download row
    /// </summary>
    public Action<IDownloadRowControl, DownloadStage>? UIMoveDownloadRow { get; set; }
    /// <summary>
    /// The UI function for deleting a download row from the queue
    /// </summary>
    public Action<IDownloadRowControl>? UIDeleteDownloadRowFromQueue { get; set; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;
    /// <summary>
    /// A PreferencesViewController
    /// </summary>
    public PreferencesViewController PreferencesViewController => new PreferencesViewController(Localizer);
    /// <summary>
    /// Whether or not the version is a development version or not
    /// </summary>
    public bool IsDevVersion => AppInfo.Current.Version.IndexOf('-') != -1;
    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public Theme Theme => Configuration.Current.Theme;
    /// <summary>
    /// Whether or not downloads are running
    /// </summary>
    public bool AreDownloadsRunning => _downloadingRows.Count > 0;
    /// <summary>
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground => Configuration.Current.RunInBackground;

    /// <summary>
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;

    /// <summary>
    /// Constructs a MainWindowController
    /// </summary>
    public MainWindowController()
    {
        _disposed = false;
        _pythonThreadState = IntPtr.Zero;
        _downloadingRows = new List<IDownloadRowControl>();
        _completedRows = new List<IDownloadRowControl>();
        _queuedRows = new List<IDownloadRowControl>();
        Localizer = new Localizer();
    }

    /// <summary>
    /// Whether or not to show a sun icon on the home page
    /// </summary>
    public bool ShowSun
    {
        get
        {
            var timeNowHours = DateTime.Now.Hour;
            return timeNowHours >= 6 && timeNowHours < 18;
        }
    }

    /// <summary>
    /// The string for greeting on the home page
    /// </summary>
    public string Greeting
    {
        get
        {
            var greeting = DateTime.Now.Hour switch
            {
                >= 0 and < 6 => "Night",
                < 12 => "Morning",
                < 18 => "Afternoon",
                < 24 => "Evening",
                _ => "Generic"
            };
            return Localizer["Greeting", greeting];
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
        if (disposing)
        {
            Localizer.Dispose();
            Python.Runtime.PythonEngine.EndAllowThreads(_pythonThreadState);
            Python.Runtime.PythonEngine.Shutdown();
            if (Directory.Exists(Configuration.TempDir))
            {
                Directory.Delete(Configuration.TempDir, true);
            }
        }
        _disposed = true;
    }

    /// <summary>
    /// Starts the application
    /// </summary>
    public async Task StartupAsync()
    {
        Configuration.Current.Saved += ConfigurationSaved;
        if (Directory.Exists(Configuration.TempDir))
        {
            Directory.Delete(Configuration.TempDir, true);
        }
        Directory.CreateDirectory(Configuration.TempDir);
        if (!await DependencyManager.SetupDependenciesAsync())
        {
            NotificationSent?.Invoke(this, new NotificationSentEventArgs(Localizer["DependencyError"], NotificationSeverity.Error));
        }
        else
        {
            Python.Runtime.RuntimeData.FormatterType = typeof(NoopFormatter);
            Python.Runtime.PythonEngine.Initialize();
            _pythonThreadState = Python.Runtime.PythonEngine.BeginAllowThreads();
        }
    }

    /// <summary>
    /// Creates a new AddDownloadDialogController
    /// </summary>
    /// <returns>The new AddDownloadDialogController</returns>
    public AddDownloadDialogController CreateAddDownloadDialogController() => new AddDownloadDialogController(Localizer);

    /// <summary>
    /// Adds a download row to the window
    /// </summary>
    /// <param name="download">The download model for the row</param>
    public void AddDownload(Download download)
    {
        var newRow = UICreateDownloadRow!(download);
        newRow.DownloadCompletedAsyncCallback = DownloadCompletedAsync;
        newRow.DownloadStoppedCallback = DownloadStopped;
        newRow.DownloadRetriedAsyncCallback = DownloadRetried;
        if (_downloadingRows.Count < Configuration.Current.MaxNumberOfActiveDownloads)
        {
            _downloadingRows.Add(newRow);
            UIMoveDownloadRow!(newRow, DownloadStage.Downloading);
            newRow.RunAsync(Configuration.Current.EmbedMetadata).FireAndForget();
        }
        else
        {
            _queuedRows.Add(newRow);
            UIMoveDownloadRow!(newRow, DownloadStage.InQueue);
        }
    }

    /// <summary>
    /// Stops all downloads
    /// </summary>
    public void StopAllDownloads()
    {
        foreach (var row in _downloadingRows)
        {
            row.Stop();
        }
        foreach (var row in _queuedRows)
        {
            row.Stop();
        }
    }

    /// <summary>
    /// Occurs when the configuration is saved
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void ConfigurationSaved(object? sender, EventArgs e)
    {
        while (_downloadingRows.Count < Configuration.Current.MaxNumberOfActiveDownloads && _queuedRows.Count > 0)
        {
            var queuedRow = _queuedRows[0];
            _downloadingRows.Add(queuedRow);
            _queuedRows.RemoveAt(0);
            UIMoveDownloadRow!(queuedRow, DownloadStage.Downloading);
            queuedRow.RunAsync(Configuration.Current.EmbedMetadata).FireAndForget();
        }
    }

    /// <summary>
    /// Occurs when a row's download is completed
    /// </summary>
    /// <param name="row">The completed row</param>
    private async Task DownloadCompletedAsync(IDownloadRowControl row)
    {
        _completedRows.Add(row);
        _downloadingRows.Remove(row);
        UIMoveDownloadRow!(row, DownloadStage.Completed);
        if (_downloadingRows.Count < Configuration.Current.MaxNumberOfActiveDownloads && _queuedRows.Count > 0)
        {
            var queuedRow = _queuedRows[0];
            _downloadingRows.Add(queuedRow);
            _queuedRows.RemoveAt(0);
            UIMoveDownloadRow!(queuedRow, DownloadStage.Downloading);
            await queuedRow.RunAsync(Configuration.Current.EmbedMetadata);
        }
    }

    /// <summary>
    /// Occurs when a row's download is stopped
    /// </summary>
    /// <param name="row">The stopped row</param>
    private void DownloadStopped(IDownloadRowControl row)
    {
        if (_queuedRows.Contains(row))
        {
            _queuedRows.Remove(row);
            UIDeleteDownloadRowFromQueue!(row);
        }
    }

    /// <summary>
    /// Occurs when a row's download is retried
    /// </summary>
    /// <param name="row">The retried row</param>
    private async Task DownloadRetried(IDownloadRowControl row)
    {
        if (_downloadingRows.Count < Configuration.Current.MaxNumberOfActiveDownloads)
        {
            _downloadingRows.Add(row);
            UIMoveDownloadRow!(row, DownloadStage.Downloading);
            await row.RunAsync(Configuration.Current.EmbedMetadata);
        }
        else
        {
            _queuedRows.Add(row);
            UIMoveDownloadRow!(row, DownloadStage.InQueue);
        }
    }
}
