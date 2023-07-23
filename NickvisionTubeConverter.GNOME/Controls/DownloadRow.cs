using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using static Nickvision.GirExt.GtkExt;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public partial class DownloadRow : Adw.Bin, IDownloadRowControl
{
    private bool _runPulsingBar;
    private string _oldLog;

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
    /// <param name="parent">Parent window</param>
    /// <param name="id">The Guid of the download</param>
    /// <param name="filename">The filename of the download</param>
    /// <param name="saveFolder">The save folder of the download</param>
    /// <param name="sendNotificationCallback">The callback for sending a notification</param>
    private DownloadRow(Gtk.Builder builder, Gtk.Window parent, Guid id, string filename, string saveFolder, Action<NotificationSentEventArgs> sendNotificationCallback) : base(builder.GetPointer("_root"), false)
    {
        Id = id;
        Filename = filename;
        _oldLog = "";
        //Build UI
        builder.Connect(this);
        _filenameLabel.SetLabel(Filename);
        _stopButton.OnClicked += (sender, e) => StopRequested?.Invoke(this, Id);
        _retryButton.OnClicked += (sender, e) => RetryRequested?.Invoke(this, Id);
        _openFileButton.OnClicked += async (sender, e) =>
        {
            var fileLauncher = Gtk.FileLauncher.New(Gio.FileHelper.NewForPath($"{saveFolder}{Path.DirectorySeparatorChar}{Filename}"));
            try
            {
                await fileLauncher.LaunchAsync(parent);
            }
            catch { }
        };
        _openFolderButton.OnClicked += async (sender, e) =>
        {
            var fileLauncher = Gtk.FileLauncher.New(Gio.FileHelper.NewForPath($"{saveFolder}{Path.DirectorySeparatorChar}{Filename}"));
            try
            {
                await fileLauncher.OpenContainingFolderAsync(parent);
            }
            catch { }
        };
        _btnLogToClipboard.OnClicked += (sender, e) =>
        {
            _lblLog.GetClipboard().SetText(_lblLog.GetBuffer().Text ?? "");
            sendNotificationCallback(new NotificationSentEventArgs(_("Download log was copied to clipboard."), NotificationSeverity.Informational));
        };
        _runPulsingBar = false;
    }

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="parent">Parent window</param>
    /// <param name="id">The Guid of the download</param>
    /// <param name="filename">The filename of the download</param>
    /// <param name="saveFolder">The save folder of the download</param>
    /// <param name="sendNotificationCallback">The callback for sending a notification</param>
    public DownloadRow(Gtk.Window parent, Guid id, string filename, string saveFolder, Action<NotificationSentEventArgs> sendNotificationCallback) : this(Builder.FromFile("download_row.ui"), parent, id, filename, saveFolder, sendNotificationCallback)
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
                    GLib.Functions.TimeoutAdd(0, 30, () =>
                    {
                        _pulsingBar.Pulse();
                        return _runPulsingBar;
                    });
                }
                _stateViewStack.SetVisibleChildName("processing");
                _progressLabel.SetText(_("Downloading"));
                break;
            case DownloadProgressStatus.Processing:
                if (!_runPulsingBar)
                {
                    _runPulsingBar = true;
                    GLib.Functions.TimeoutAdd(0, 30, () =>
                    {
                        _pulsingBar.Pulse();
                        return _runPulsingBar;
                    });
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