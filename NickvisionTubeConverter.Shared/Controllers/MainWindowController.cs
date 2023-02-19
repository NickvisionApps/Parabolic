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
    private List<IDownloadRowControl> _downloadRows;

    /// <summary>
    /// The localizer to get translated strings from
    /// </summary>
    public Localizer Localizer { get; init; }
    /// <summary>
    /// The UI function for creating a download row
    /// </summary>
    public Func<Download, IDownloadRowControl>? UICreateDownloadRow { get; set; }

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
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;

    /// <summary>
    /// Constructs a MainWindowController
    /// </summary>
    public MainWindowController()
    {
        _disposed = false;
        _downloadRows = new List<IDownloadRowControl>();
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
            var timeNowHours = DateTime.Now.Hour;
            if (timeNowHours >= 0 && timeNowHours < 6)
            {
                return Localizer["Greeting", "Night"];
            }
            else if (timeNowHours >= 6 && timeNowHours < 12)
            {
                return Localizer["Greeting", "Morning"];
            }
            else if (timeNowHours >= 12 && timeNowHours < 18)
            {
                return Localizer["Greeting", "Afternoon"];
            }
            else if (timeNowHours >= 18 && timeNowHours < 24)
            {
                return Localizer["Greeting", "Evening"];
            }
            else
            {
                return Localizer["Greeting", "Generic"];
            }
        }
    }

    /// <summary>
    /// Whether or not downloads are running
    /// </summary>
    public bool AreDownloadsRunning
    {
        get
        {
            foreach(var row in _downloadRows)
            {
                if(!row.IsDone)
                {
                    return true;
                }
            }
            return false;
        }
    }

    /// <summary>
    /// Frees resources used by the Account object
    /// </summary>
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Frees resources used by the Account object
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
        }
        _disposed = true;
    }

    /// <summary>
    /// Creates a new AddDownloadDialogController
    /// </summary>
    /// <returns>The new AddDownloadDialogController</returns>
    public AddDownloadDialogController CreateAddDownloadDialogController() => new AddDownloadDialogController(Localizer);

    /// <summary>
    /// Downloads dependencies for the application
    /// </summary>
    /// <returns>True if successful, else false</returns>
    public async Task<bool> DownloadDependenciesAsync()
    {
        var ytdlpVersion = new Version(2023, 2, 17);
        if(Configuration.Current.YtdlpVersion != ytdlpVersion || !File.Exists(DependencyManager.YtdlpPath) || !File.Exists(DependencyManager.Ffmpeg))
        {
            if(await DependencyManager.DownloadDependenciesAsync())
            {
                Configuration.Current.YtdlpVersion = ytdlpVersion;
                Configuration.Current.Save();
                return true;
            }
            return false;
        }
        return true;
    }

    /// <summary>
    /// Adds a download row to the window
    /// </summary>
    /// <param name="download">The download model for the row</param>
    public async Task AddDownloadAsync(Download download)
    {
        var newRow = UICreateDownloadRow!(download);
        _downloadRows.Add(newRow);
        await newRow.StartAsync(Configuration.Current.EmbedMetadata);
    }

    /// <summary>
    /// Stops all downloads
    /// </summary>
    public void StopDownloads()
    {
        foreach(var row in _downloadRows)
        {
            row.Stop();
        }
    }
}
