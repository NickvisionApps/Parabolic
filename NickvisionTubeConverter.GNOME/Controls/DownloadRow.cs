using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public partial class DownloadRow : Adw.Bin, IDownloadRowControl
{
    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_launcher_new(nint file);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_launcher_launch(nint fileLauncher, nint parent, nint cancellable, GAsyncReadyCallback callback, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_launcher_open_containing_folder(nint fileLauncher, nint parent, nint cancellable, GAsyncReadyCallback callback, nint data);

    private readonly Localizer _localizer;
    private string _saveFolder;
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
    /// The Id of the download
    /// </summary>
    public Guid Id { get; private set; }
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
    /// <param name="localizer">The Localizer of strings</param>
    /// <param name="sendNoticiationCallback">The callback for sending a notification</param>
    private DownloadRow(Gtk.Builder builder, Guid id, string filename, string saveFolder, Localizer localizer, Action<NotificationSentEventArgs> sendNoticiationCallback) : base(builder.GetPointer("_root"), false)
    {
        _localizer = localizer;
        _localizer = localizer;
        _saveFolder = saveFolder;
        Id = id;
        Filename = filename;
        _sendNotificationCallback = sendNoticiationCallback;
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
            _lblLog.GetClipboard().SetText(_lblLog.GetText());
            _sendNotificationCallback(new NotificationSentEventArgs(_localizer["LogCopied"], NotificationSeverity.Informational));
        };
    }

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="id">The Guid of the download</param>
    /// <param name="filename">The filename of the download</param>
    /// <param name="saveFolder">The save folder of the download</param>
    /// <param name="localizer">The Localizer of strings</param>
    /// <param name="sendNoticiationCallback">The callback for sending a notification</param>
    public DownloadRow(Guid id, string filename, string saveFolder, Localizer localizer, Action<NotificationSentEventArgs> sendNoticiationCallback) : this(Builder.FromFile("download_row.ui", localizer), id, filename, saveFolder, localizer, sendNoticiationCallback)
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
        _progressLabel.SetText(_localizer["DownloadState", "Waiting"]);
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
        _progressLabel.SetText(_localizer["DownloadState", "Preparing"]);
        _actionViewStack.SetVisibleChildName("cancel");
        _progressBar.SetFraction(0);
    }

    /// <summary>
    /// Sets the row to the progress state
    /// </summary>
    /// <param name="state">The DownloadProgressState</param>
    public void SetProgressState(DownloadProgressState state)
    {
        _lblLog.SetLabel(state.Log);
        var vadjustment = _scrollLog.GetVadjustment();
        vadjustment.SetValue(vadjustment.GetUpper() - vadjustment.GetPageSize());
        switch(state.Status)
        {
            case DownloadProgressStatus.Downloading:
                _progressBar.SetFraction(state.Progress);
                _progressLabel.SetText(string.Format(_localizer["DownloadState", "Downloading"], state.Progress * 100, state.Speed.GetSpeedString(_localizer)));
                break;
            case DownloadProgressStatus.DownloadingAria:
                _progressBar.Pulse();
                _progressLabel.SetText(_localizer["Downloading"]);
                break;
            case DownloadProgressStatus.Processing:
                _progressBar.Pulse();
                _progressLabel.SetText(_localizer["DownloadState", "Processing"]);
                break;
        }
    }

    /// <summary>
    /// Sets the row to the completed state
    /// </summary>
    /// <param name="success">Whether or not the download was successful</param>
    public void SetCompletedState(bool success)
    {
        _statusIcon.AddCssClass(success ? "success" : "error");
        _statusIcon.SetFromIconName(success ? "emblem-ok-symbolic" : "process-stop-symbolic");
        _stateViewStack.SetVisibleChildName("done");
        _levelBar.SetValue(success ? 1 : 0);
        _progressLabel.SetText(success ? _localizer["Success"] : _localizer["Error"]);
        _actionViewStack.SetVisibleChildName(success ? "open" : "retry");
    }

    /// <summary>
    /// Sets the row to the stop state
    /// </summary>
    public void SetStopState()
    {
        _progressBar.SetFraction(1.0);
        _statusIcon.AddCssClass("stopped");
        _statusIcon.SetFromIconName("process-stop-symbolic");
        _stateViewStack.SetVisibleChildName("done");
        _levelBar.SetValue(0);
        _progressLabel.SetText(_localizer["Stopped"]);
        _actionViewStack.SetVisibleChildName("retry");
    }

    /// <summary>
    /// Sets the row to the retry state
    /// </summary>
    public void SetRetryState() => SetWaitingState();
}