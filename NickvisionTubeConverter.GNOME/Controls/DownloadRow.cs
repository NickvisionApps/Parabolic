using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public partial class DownloadRow : Adw.Bin, IDownloadRowControl
{
    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);
    private delegate bool GSourceFunc(nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_launcher_new(nint file);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_launcher_launch(nint fileLauncher, nint parent, nint cancellable, GAsyncReadyCallback callback, nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_launcher_open_containing_folder(nint fileLauncher, nint parent, nint cancellable, GAsyncReadyCallback callback, nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_timeout_add(uint interval, GSourceFunc func, nint data);

    private readonly GSourceFunc _pulsingBarCallback;
    private bool _runPulsingBar;
    private string _saveFolder;
    private string _oldLog;
    private Action<NotificationSentEventArgs> _sendNotificationCallback;

    [Gtk.Connect] private readonly Gtk.Image _statusIcon;
    [Gtk.Connect] private readonly Gtk.Label _filenameLabel;
    [Gtk.Connect] private readonly Gtk.Label _progressLabel;
    [Gtk.Connect] private readonly Adw.ViewStack _stateViewStack;
    [Gtk.Connect] private readonly Gtk.ProgressBar _progressBar;
    [Gtk.Connect] private readonly Gtk.ProgressBar _pulsingBar;
    [Gtk.Connect] private readonly Gtk.LevelBar _levelBar;
    [Gtk.Connect] private readonly Adw.ViewStack _actionViewStack;
    [Gtk.Connect] private readonly Gtk.Button _stopButton;
    [Gtk.Connect] private readonly Gtk.Button _openFileButton;
    [Gtk.Connect] private readonly Gtk.Button _openFolderButton;
    [Gtk.Connect] private readonly Gtk.Button _retryButton;
    [Gtk.Connect] private readonly Gtk.TextView _lblLog;
    [Gtk.Connect] private readonly Gtk.Button _btnLogToClipboard;

    /// <summary>
    /// The Id of the download
    /// </summary>
    public Guid Id { get; init; }
    /// <summary>
    /// The filename of the download
    /// </summary>
    public string Filename { get; private set; }

    /// <summary>
    /// Occurs when the download is requested to stop
    /// </summary>
    public event EventHandler<Guid>? StopRequested;
    /// <summary>
    /// Occurs when the download is requested to be retried
    /// </summary>
    public event EventHandler<Guid>? RetryRequested;

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="builder">The Gtk builder for the row</param>
    /// <param name="id">The Guid of the download</param>
    /// <param name="filename">The filename of the download</param>
    /// <param name="saveFolder">The save folder of the download</param>
    /// <param name="sendNotificationCallback">The callback for sending a notification</param>
    private DownloadRow(Gtk.Builder builder, Guid id, string filename, string saveFolder, Action<NotificationSentEventArgs> sendNotificationCallback) : base(builder.GetPointer("_root"), false)
    {
        _saveFolder = saveFolder;
        Id = id;
        Filename = filename;
        _oldLog = "";
        _sendNotificationCallback = sendNotificationCallback;
        //Build UI
        builder.Connect(this);
        _filenameLabel.SetLabel(Filename);
        _stopButton.OnClicked += (sender, e) => StopRequested?.Invoke(this, Id);
        _retryButton.OnClicked += (sender, e) => RetryRequested?.Invoke(this, Id);
        _openFileButton.OnClicked += (sender, e) =>
        {
            var file = Gio.FileHelper.NewForPath($"{_saveFolder}{Path.DirectorySeparatorChar}{Filename}");
            var fileLauncher = gtk_file_launcher_new(file.Handle);
            gtk_file_launcher_launch(fileLauncher, 0, 0, (source, res, data) => { }, 0);
        };
        _openFolderButton.OnClicked += (sender, e) =>
        {
            var file = Gio.FileHelper.NewForPath($"{_saveFolder}{Path.DirectorySeparatorChar}{Filename}");
            var fileLauncher = gtk_file_launcher_new(file.Handle);
            gtk_file_launcher_open_containing_folder(fileLauncher, 0, 0, (source, res, data) => { }, 0);
        };
        _btnLogToClipboard.OnClicked += (sender, e) =>
        {
            _lblLog.GetClipboard().SetText(_lblLog.GetBuffer().Text ?? "");
            _sendNotificationCallback(new NotificationSentEventArgs(_("Download log was copied to clipboard."), NotificationSeverity.Informational));
        };
        _runPulsingBar = false;
        _pulsingBarCallback = (data) =>
        {
            _pulsingBar.Pulse();
            return _runPulsingBar;
        };
    }

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="id">The Guid of the download</param>
    /// <param name="filename">The filename of the download</param>
    /// <param name="saveFolder">The save folder of the download</param>
    /// <param name="sendNotificationCallback">The callback for sending a notification</param>
    public DownloadRow(Guid id, string filename, string saveFolder, Action<NotificationSentEventArgs> sendNotificationCallback) : this(Builder.FromFile("download_row.ui"), id, filename, saveFolder, sendNotificationCallback)
    {

    }

    /// <summary>
    /// Sets the row to the waiting state
    /// </summary>
    public void SetWaitingState()
    {
        _statusIcon.RemoveCssClass("error");
        _statusIcon.AddCssClass("stopped");
        _statusIcon.SetFromIconName("folder-download-symbolic");
        _stateViewStack.SetVisibleChildName("downloading");
        _progressLabel.SetText(_("Waiting..."));
        _actionViewStack.SetVisibleChildName("cancel");
        _progressBar.SetFraction(0);
    }

    /// <summary>
    /// Sets the row to the preparing state
    /// </summary>
    public void SetPreparingState()
    {
        _statusIcon.RemoveCssClass("error");
        _statusIcon.RemoveCssClass("stopped");
        _statusIcon.SetFromIconName("folder-download-symbolic");
        _stateViewStack.SetVisibleChildName("downloading");
        _progressLabel.SetText(_("Preparing..."));
        _actionViewStack.SetVisibleChildName("cancel");
        _progressBar.SetFraction(0);
    }

    /// <summary>
    /// Sets the row to the progress state
    /// </summary>
    /// <param name="state">The DownloadProgressState</param>
    public void SetProgressState(DownloadProgressState state)
    {
        if (_oldLog.Length != state.Log.Length)
        {
            _lblLog.GetBuffer().SetText(state.Log, state.Log.Length);
            _oldLog = state.Log;
            var vadjustment = _lblLog.GetVadjustment()!;
            vadjustment.SetValue(vadjustment.GetUpper() - vadjustment.GetPageSize());
        }
        switch (state.Status)
        {
            case DownloadProgressStatus.Downloading:
                _stateViewStack.SetVisibleChildName("downloading");
                _progressBar.SetFraction(state.Progress);
                _progressLabel.SetText(_("Downloading {0:f2}% ({1})", state.Progress * 100, state.Speed.GetSpeedString()));
                break;
            case DownloadProgressStatus.DownloadingAria:
            case DownloadProgressStatus.DownloadingFfmpeg:
                if (!_runPulsingBar)
                {
                    _runPulsingBar = true;
                    g_timeout_add(30, _pulsingBarCallback, 0);
                }
                _stateViewStack.SetVisibleChildName("processing");
                _progressLabel.SetText(_("Downloading"));
                break;
            case DownloadProgressStatus.Processing:
                if (!_runPulsingBar)
                {
                    _runPulsingBar = true;
                    g_timeout_add(30, _pulsingBarCallback, 0);
                }
                _stateViewStack.SetVisibleChildName("processing");
                _progressLabel.SetText(_("Processing..."));
                break;
        }
    }

    /// <summary>
    /// Sets the row to the completed state
    /// </summary>
    /// <param name="success">Whether or not the download was successful</param>
    /// <param name="filename">The filename of the download</param>
    public void SetCompletedState(bool success, string filename)
    {
        _runPulsingBar = false;
        _statusIcon.AddCssClass(success ? "success" : "error");
        _statusIcon.SetFromIconName(success ? "emblem-ok-symbolic" : "process-stop-symbolic");
        _stateViewStack.SetVisibleChildName("done");
        _levelBar.SetValue(success ? 1 : 0);
        _progressLabel.SetText(success ? _("Success") : _("Error"));
        _actionViewStack.SetVisibleChildName(success ? "open" : "retry");
        Filename = filename;
        _filenameLabel.SetLabel(Filename);
    }

    /// <summary>
    /// Sets the row to the stop state
    /// </summary>
    public void SetStopState()
    {
        _runPulsingBar = false;
        _progressBar.SetFraction(1.0);
        _statusIcon.AddCssClass("stopped");
        _statusIcon.SetFromIconName("process-stop-symbolic");
        _stateViewStack.SetVisibleChildName("done");
        _levelBar.SetValue(0);
        _progressLabel.SetText(_("Stopped"));
        _actionViewStack.SetVisibleChildName("retry");
    }
}