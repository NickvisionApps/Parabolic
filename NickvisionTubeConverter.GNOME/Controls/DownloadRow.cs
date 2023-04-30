using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public partial class DownloadRow : Adw.Bin, IDownloadRowControl
{
    private delegate bool GSourceFunc(nint data);

    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_main_context_invoke(nint context, GSourceFunc function, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_idle_add(GSourceFunc function, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_timeout_add(uint interval, GSourceFunc function, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_launcher_new(nint file);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_launcher_launch(nint fileLauncher, nint parent, nint cancellable, GAsyncReadyCallback callback, nint data);

    private readonly Localizer _localizer;
    private readonly Download _download;
    private readonly GSourceFunc _runStartCallback;
    private readonly GSourceFunc _runEndCallback;
    private readonly GSourceFunc _stopCallback;
    private readonly GSourceFunc _updateLogCallback;
    private readonly GSourceFunc _processingCallback;
    private readonly GSourceFunc _downloadingCallback;
    private bool? _previousEmbedMetadata;
    private bool _wasStopped;
    private DownloadProgressStatus _progressStatus;
    private string _logMessage;
    private bool _processingCallbackRunning;
    private Action<NotificationSentEventArgs> _sendNotificationCallback;

    [Gtk.Connect] private readonly Gtk.Image _statusIcon;
    [Gtk.Connect] private readonly Gtk.Label _filenameLabel;
    [Gtk.Connect] private readonly Gtk.Label _progressLabel;
    [Gtk.Connect] private readonly Adw.ViewStack _stateViewStack;
    [Gtk.Connect] private readonly Gtk.ProgressBar _progressBar;
    [Gtk.Connect] private readonly Gtk.LevelBar _levelBar;
    [Gtk.Connect] private readonly Adw.ViewStack _actionViewStack;
    [Gtk.Connect] private readonly Gtk.Button _stopButton;
    [Gtk.Connect] private readonly Gtk.Button _openFileButton;
    [Gtk.Connect] private readonly Gtk.Button _openFolderButton;
    [Gtk.Connect] private readonly Gtk.Button _retryButton;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _scrollLog;
    [Gtk.Connect] private readonly Gtk.Label _lblLog;
    [Gtk.Connect] private readonly Gtk.Button _btnLogToClipboard;

    /// <summary>
    /// The callback function to run when the download is completed
    /// </summary>
    public Func<IDownloadRowControl, Task>? DownloadCompletedAsyncCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is stopped
    /// </summary>
    public Action<IDownloadRowControl>? DownloadStoppedCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is retried
    /// </summary>
    public Func<IDownloadRowControl, Task>? DownloadRetriedAsyncCallback { get; set; }

    /// <summary>
    /// The filename of the download
    /// </summary>
    public string Filename => _download.Filename;
    /// <summary>
    /// Whether or not the download is done
    /// </summary>
    public bool IsDone => _download.IsDone;
    /// <summary>
    /// Download progress
    /// </summary>
    public double Progress { get; set; }
    /// <summary>
    /// Download speed (in bytes per second)
    /// </summary>
    public double Speed { get; set; }
    /// <summary>
    /// Whether or not download was finished with error
    /// </summary>
    public bool FinishedWithError { get; set; }

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="builder">The Gtk builder for the row</param>
    /// <param name="download">The download model</param>
    /// <param name="localizer">The string localizer</param>
    /// <param name="sendNoticiationCallback">The callback for sending a notification</param>
    private DownloadRow(Gtk.Builder builder, Download download, Localizer localizer, Action<NotificationSentEventArgs> sendNoticiationCallback) : base(builder.GetPointer("_root"), false)
    {
        _localizer = localizer;
        _download = download;
        _previousEmbedMetadata = null;
        _wasStopped = false;
        _logMessage = "";
        _sendNotificationCallback = sendNoticiationCallback;
        _processingCallbackRunning = false;
        Progress = 0.0;
        Speed = 0.0;
        FinishedWithError = false;
        //Build UI
        builder.Connect(this);
        _filenameLabel.SetLabel(download.Filename);
        _stopButton.OnClicked += (sender, e) => Stop();
        _openFileButton.OnClicked += (sender, e) =>
        {
            var file = Gio.FileHelper.NewForPath($"{_download.SaveFolder}{Path.DirectorySeparatorChar}{_download.Filename}");
            var fileLauncher = gtk_file_launcher_new(file.Handle);
            gtk_file_launcher_launch(fileLauncher, 0, 0, (source, res, data) => { }, 0);
        };
        _openFolderButton.OnClicked += (sender, e) =>
        {
            var file = Gio.FileHelper.NewForPath(_download.SaveFolder);
            var fileLauncher = gtk_file_launcher_new(file.Handle);
            gtk_file_launcher_launch(fileLauncher, 0, 0, (source, res, data) => { }, 0);
        };
        _retryButton.OnClicked += async (sender, e) =>
        {
            if (DownloadRetriedAsyncCallback != null)
            {
                await DownloadRetriedAsyncCallback(this);
            }
        };
        _btnLogToClipboard.OnClicked += (sender, e) =>
        {
            _lblLog.GetClipboard().SetText(_lblLog.GetText());
            _sendNotificationCallback(new NotificationSentEventArgs(_localizer["LogCopied"], NotificationSeverity.Informational));
        };
        //Callbacks
        _runStartCallback = (x) =>
        {
            _statusIcon.AddCssClass("accent");
            _statusIcon.RemoveCssClass("error");
            _statusIcon.SetFromIconName("folder-download-symbolic");
            _stateViewStack.SetVisibleChildName("downloading");
            _progressLabel.SetText(_localizer["DownloadState", "Preparing"]);
            _filenameLabel.SetText(_download.Filename);
            _actionViewStack.SetVisibleChildName("cancel");
            _progressBar.SetFraction(0);
            return false;
        };
        _runEndCallback = (x) =>
        {
            _statusIcon.RemoveCssClass("accent");
            _statusIcon.AddCssClass(!FinishedWithError ? "success" : "error");
            _statusIcon.SetFromIconName(!FinishedWithError ? "emblem-ok-symbolic" : "process-stop-symbolic");
            _stateViewStack.SetVisibleChildName("done");
            _levelBar.SetValue(!FinishedWithError ? 1 : 0);
            _progressLabel.SetText(!FinishedWithError ? _localizer["Success"] : _localizer["Error"]);
            _actionViewStack.SetVisibleChildName(!FinishedWithError ? "open" : "retry");
            return false;
        };
        _stopCallback = (x) =>
        {
            _progressBar.SetFraction(1.0);
            _statusIcon.RemoveCssClass("accent");
            _statusIcon.AddCssClass("error");
            _statusIcon.SetFromIconName("process-stop-symbolic");
            _stateViewStack.SetVisibleChildName("done");
            _levelBar.SetValue(0);
            _progressLabel.SetText(_localizer["Stopped"]);
            _actionViewStack.SetVisibleChildName("retry");
            return false;
        };
        _updateLogCallback = (x) =>
        {
            _lblLog.SetLabel(_logMessage);
            var vadjustment = _scrollLog.GetVadjustment();
            vadjustment.SetValue(vadjustment.GetUpper() - vadjustment.GetPageSize());
            return false;
        };
        _downloadingCallback = (stateHandle) =>
        {
            var state = (DownloadProgressState)(GCHandle.FromIntPtr(stateHandle).Target!);
            if (!_processingCallbackRunning)
            {
                Progress = state.Progress;
                _progressBar.SetFraction(state.Progress);
                Speed = state.Speed;
                var speedString = state.Speed.GetSpeedString(_localizer);
                _progressLabel.SetText(string.Format(_localizer["DownloadState", "Downloading"], state.Progress * 100, speedString));
            }
            state.Dispose();
            return false;
        };
        _processingCallback = (x) =>
        {
            _progressBar.Pulse();
            if ((_progressStatus != DownloadProgressStatus.Processing && _progressStatus != DownloadProgressStatus.DownloadingAria) || IsDone)
            {
                _processingCallbackRunning = false;
            }
            return _processingCallbackRunning;
        };
    }

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="download">The download model</param>
    /// <param name="localizer">The string localizer</param>
    /// <param name="sendNoticiationCallback">The callback for sending a notification</param>
    public DownloadRow(Download download, Localizer localizer, Action<NotificationSentEventArgs> sendNoticiationCallback) : this(Builder.FromFile("download_row.ui", localizer), download, localizer, sendNoticiationCallback)
    {

    }

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public async Task RunAsync(bool embedMetadata)
    {
        if (_previousEmbedMetadata == null)
        {
            _previousEmbedMetadata = embedMetadata;
        }
        _wasStopped = false;
        FinishedWithError = false;
        g_main_context_invoke(0, _runStartCallback, 0);
        var success = await _download.RunAsync(embedMetadata, _localizer, (state) =>
        {
            _progressStatus = state.Status;
            _logMessage = state.Log + "\n";
            g_idle_add(_updateLogCallback, 0);
            switch (state.Status)
            {
                case DownloadProgressStatus.Downloading:
                    g_idle_add(_downloadingCallback, (IntPtr)state.Handle!);
                    break;
                case DownloadProgressStatus.DownloadingAria:
                    _progressLabel.SetText(_localizer["Downloading"]);
                    Progress = 0.0;
                    Speed = 0.0;
                    if (!_processingCallbackRunning)
                    {
                        _processingCallbackRunning = true;
                        g_timeout_add(30, _processingCallback, 0);
                    }
                    break;
                case DownloadProgressStatus.Processing:
                    _progressLabel.SetText(_localizer["DownloadState", "Processing"]);
                    Progress = 1.0;
                    Speed = 0.0;
                    if (!_processingCallbackRunning)
                    {
                        _processingCallbackRunning = true;
                        g_timeout_add(30, _processingCallback, 0);
                    }
                    break;
            }
        });
        if (!_wasStopped)
        {
            FinishedWithError = !success;
            g_main_context_invoke(0, _runEndCallback, 0);
        }
        if (DownloadCompletedAsyncCallback != null)
        {
            await DownloadCompletedAsyncCallback(this);
        }
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop()
    {
        _wasStopped = true;
        _download.Stop();
        g_main_context_invoke(0, _stopCallback, 0);
        if (DownloadStoppedCallback != null)
        {
            DownloadStoppedCallback(this);
        }
    }

    public async Task RetryAsync()
    {
        if(_wasStopped || FinishedWithError)
        {
            if (DownloadRetriedAsyncCallback != null)
            {
                await DownloadRetriedAsyncCallback(this);
            }
        }
    }
}