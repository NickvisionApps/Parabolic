using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// A controller for a MainWindow
/// </summary>
public class MainWindowController : IDisposable
{
    private bool _disposed;
    private nint _pythonThreadState;
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
            foreach (var row in _downloadRows)
            {
                if (!row.IsDone)
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
            Python.Runtime.PythonEngine.EndAllowThreads(_pythonThreadState);
            Python.Runtime.PythonEngine.Shutdown();
        }
        _disposed = true;
    }

    /// <summary>
    /// Starts the application
    /// </summary>
    public async Task StartupAsync()
    {
        if (!await DependencyManager.SetupDependenciesAsync())
        {
            NotificationSent?.Invoke(this, new NotificationSentEventArgs(Localizer["DependencyError"], NotificationSeverity.Error));
        }
        else
        {
            Python.Runtime.PythonEngine.Initialize();
            _pythonThreadState = Python.Runtime.PythonEngine.BeginAllowThreads();
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                using (Python.Runtime.Py.GIL())
                {
                    dynamic sys = Python.Runtime.Py.Import("sys");
                    var pathToOutput = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}{Path.DirectorySeparatorChar}output.txt";
                    sys.stdout = Python.Runtime.PythonEngine.Eval($"open(\"{Regex.Replace(pathToOutput, @"\\", @"\\")}\", \"w\")");
                }
            }
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
        foreach (var row in _downloadRows)
        {
            row.Stop();
        }
    }
}
