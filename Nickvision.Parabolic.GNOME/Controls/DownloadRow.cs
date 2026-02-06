using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.IO;

namespace Nickvision.Parabolic.GNOME.Controls;

public class DownloadRow : Gtk.ListBoxRow
{
    private readonly ITranslationService _translator;
    private readonly Gtk.Window _parent;
    private readonly Gtk.Builder _builder;
    private int _id;
    private string _path;
    private string _log;
    private bool _isPaused;

    [Gtk.Connect("statusIcon")]
    private Gtk.Image? _statusIcon;
    [Gtk.Connect("filenameLabel")]
    private Gtk.Label? _filenameLabel;
    [Gtk.Connect("statusLabel")]
    private Gtk.Label? _statusLabel;
    [Gtk.Connect("spinner")]
    private Adw.Spinner? _spinner;
    [Gtk.Connect("viewLogButton")]
    private Gtk.ToggleButton? _viewLogButton;
    [Gtk.Connect("buttonsViewStack")]
    private Adw.ViewStack? _buttonsViewStack;
    [Gtk.Connect("pauseResumeButton")]
    private Gtk.Button? _pauseResumeButton;
    [Gtk.Connect("stopButton")]
    private Gtk.Button? _stopButton;
    [Gtk.Connect("playButton")]
    private Gtk.Button? _playButton;
    [Gtk.Connect("openButton")]
    private Gtk.Button? _openButton;
    [Gtk.Connect("retryButton")]
    private Gtk.Button? _retryButton;
    [Gtk.Connect("logLabel")]
    private Gtk.Label? _logLabel;
    [Gtk.Connect("logScroll")]
    private Gtk.ScrolledWindow? _logScroll;
    [Gtk.Connect("logToClipboardButton")]
    private Gtk.Button? _logToClipboardButton;

    public event EventHandler<int>? PauseRequested;
    public event EventHandler<int>? ResumeRequested;
    public event EventHandler<int>? StopRequested;
    public event EventHandler<int>? RetryRequested;

    public DownloadStatus Status { get; private set; }

    public DownloadRow(ITranslationService translator, Gtk.Window parent) : this(translator, parent, Gtk.Builder.NewFromBlueprint("DownloadRow", translator))
    {

    }

    private DownloadRow(ITranslationService translator, Gtk.Window parent, Gtk.Builder builder) : base(new Gtk.Internal.ListBoxRowHandle(builder.GetPointer("root"), false))
    {
        _translator = translator;
        _parent = parent;
        _builder = builder;
        _id = -1;
        _path = string.Empty;
        _log = string.Empty;
        _isPaused = false;
        _builder.Connect(this);
        // Events
        _pauseResumeButton!.OnClicked += PauseResumeButton_OnClicked;
        _stopButton!.OnClicked += StopButton_OnClicked;
        _playButton!.OnClicked += PlayButton_OnClicked;
        _openButton!.OnClicked += OpenFolderButton_OnClicked;
        _retryButton!.OnClicked += RetryButton_OnClicked;
        _logToClipboardButton!.OnClicked += LogToClipboardButton_OnClicked;
    }

    public void TriggerAddedState(DownloadAddedEventArgs args)
    {
        _id = args.Id;
        _path = args.Path;
        _statusIcon!.AddCssClass("stopped");
        _statusIcon.SetFromIconName("folder-download-symbolic");
        _filenameLabel!.SetLabel(Path.GetFileName(_path));
        if (args.Status == DownloadStatus.Queued)
        {
            Status = DownloadStatus.Queued;
            _statusLabel!.SetLabel(_translator._("Queued"));
        }
        else if (args.Status == DownloadStatus.Running)
        {
            Status = DownloadStatus.Running;
            _statusLabel!.SetLabel(_translator._("Starting"));
        }
        else
        {
            Status = DownloadStatus.Running;
            _statusLabel!.SetLabel(_translator._("Unknown"));
        }
        _spinner!.Visible = true;
        _buttonsViewStack!.SetVisibleChildName("Downloading");
    }

    public void TriggerCompletedState(DownloadCompletedEventArgs args)
    {
        _path = args.Path;
        if (args.Log.Length > 0)
        {
            _log = args.Log.ToString();
        }
        _filenameLabel!.SetLabel(Path.GetFileName(_path));
        _logLabel!.SetLabel(_log.ToString());
        _statusIcon!.RemoveCssClass("stopped");
        _spinner!.Visible = false;
        if (args.Status == DownloadStatus.Error)
        {
            Status = DownloadStatus.Error;
            _statusIcon.AddCssClass("error");
            _statusIcon.SetFromIconName("process-stop-symbolic");
            _statusLabel!.SetLabel(_translator._("Error"));
            _buttonsViewStack!.SetVisibleChildName("Error");
        }
        else if (args.Status == DownloadStatus.Success)
        {
            Status = DownloadStatus.Success;
            _statusIcon.AddCssClass("success");
            _statusIcon.SetFromIconName("checkmark-small-symbolic");
            _statusLabel!.SetLabel(_translator._("Success"));
            _buttonsViewStack!.SetVisibleChildName("Success");
        }
    }

    public void TriggerPausedState()
    {
        Status = DownloadStatus.Paused;
        _statusIcon!.SetFromIconName("media-playback-pause-symbolic");
        _statusLabel!.SetLabel(_translator._("Paused"));
        _spinner!.Visible = false;
        _pauseResumeButton!.SetIconName("media-playback-start-symbolic");
        _pauseResumeButton.SetTooltipText(_translator._("Resume"));
        _isPaused = true;
    }

    public void TriggerProgressState(DownloadProgressChangedEventArgs args)
    {
        _statusIcon!.SetFromIconName("folder-download-symbolic");
        _spinner!.Visible = true;
        if (args.LogChunk.Length > 0)
        {
            _log += $"{args.LogChunk.ToString()}\n";
        }
        if (double.IsNaN(args.Progress))
        {
            _statusLabel!.SetLabel(_translator._("Processing"));
        }
        else if (double.IsNegativeInfinity(args.Progress))
        {
            _statusLabel!.SetLabel(_translator._("Sleeping for {0}", TimeSpan.FromSeconds(args.Speed).ToString("g")));
        }
        else
        {
            _statusLabel!.SetLabel(_translator._("{0}% Complete • {1} • {2} Remaining",
                    Math.Round(args.Progress * 100, 2),
                    args.Speed > 0 ? args.SpeedString : _translator._("Unknown"),
                    args.Eta > 0 ? args.EtaString : _translator._("Unknown")));
        }
        _logLabel!.SetLabel(_log.ToString());
        var vadjustment = _logScroll!.GetVadjustment();
        vadjustment!.SetValue(vadjustment.GetUpper());
    }

    public void TriggerResumedState()
    {
        Status = DownloadStatus.Running;
        _spinner!.Visible = true;
        _pauseResumeButton!.SetIconName("media-playback-pause-symbolic");
        _pauseResumeButton.SetTooltipText(_translator._("Pause"));
        _isPaused = false;
    }

    public void TriggerStartedFromQueueState()
    {
        Status = DownloadStatus.Running;
        _statusIcon!.AddCssClass("stopped");
        _statusIcon.SetFromIconName("folder-download-symbolic");
        _spinner!.Visible = true;
        _statusLabel!.SetLabel(_translator._("Running"));
    }

    public void TriggerStoppedState()
    {
        Status = DownloadStatus.Stopped;
        _statusIcon!.SetFromIconName("media-playback-stop-symbolic");
        _statusLabel!.SetLabel(_translator._("Stopped"));
        _spinner!.Visible = false;
        _buttonsViewStack!.SetVisibleChildName("Error");
    }

    private void LogToClipboardButton_OnClicked(Gtk.Button sender, EventArgs args)
    {
        var clipboard = Gdk.Display.GetDefault()!.GetClipboard();
        clipboard.SetText(_log.ToString());
    }

    private async void OpenFolderButton_OnClicked(Gtk.Button sender, EventArgs args)
    {
        var launcher = Gtk.FileLauncher.New(Gio.FileHelper.NewForPath(Path.GetDirectoryName(_path)!));
        await launcher.LaunchAsync(_parent);
    }

    private void PauseResumeButton_OnClicked(Gtk.Button sender, EventArgs args)
    {
        if (_isPaused)
        {
            ResumeRequested?.Invoke(this, _id);
        }
        else
        {
            PauseRequested?.Invoke(this, _id);
        }
    }

    private async void PlayButton_OnClicked(Gtk.Button sender, EventArgs args)
    {
        var launcher = Gtk.FileLauncher.New(Gio.FileHelper.NewForPath(_path));
        await launcher.LaunchAsync(GetNative() as Gtk.Window);
    }

    private void RetryButton_OnClicked(Gtk.Button sender, EventArgs args) => RetryRequested?.Invoke(this, _id);

    private void StopButton_OnClicked(Gtk.Button sender, EventArgs args) => StopRequested?.Invoke(this, _id);
}
