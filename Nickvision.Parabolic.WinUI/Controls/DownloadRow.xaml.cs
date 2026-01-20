using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.IO;
using Windows.Storage;
using Windows.System;

namespace Nickvision.Parabolic.WinUI.Controls;

public sealed partial class DownloadRow : UserControl
{
    private enum ButtonsPage
    {
        Running = 0,
        Done,
        Error
    }

    private readonly ITranslationService _translator;
    private int _id;
    private string _path;
    private string _log;
    private bool _isPaused;

    public event EventHandler<int>? PauseRequested;
    public event EventHandler<int>? ResumeRequested;
    public event EventHandler<int>? StopRequested;
    public event EventHandler<int>? RetryRequested;

    public DownloadStatus Status { get; private set; }

    public DownloadRow(ITranslationService translator)
    {
        InitializeComponent();
        _translator = translator;
        _id = -1;
        _path = string.Empty;
        _log = string.Empty;
        _isPaused = false;
        LblShowLog.Text = _translator._("Log");
        LblPauseResume.Text = _translator._("Pause");
        LblStop.Text = _translator._("Stop");
        LblPlay.Text = _translator._("Play");
        LblOpenFolder.Text = _translator._("Open");
        LblRetry.Text = _translator._("Retry");
    }

    public void TriggerAddedState(DownloadAddedEventArgs args)
    {
        _id = args.Id;
        _path = args.Path;
        IcnStatus.Glyph = "\uE896";
        LblTitle.Text = Path.GetFileName(_path);
        ProgBar.Value = 0.0;
        if (args.Status == DownloadStatus.Queued)
        {
            Status = DownloadStatus.Queued;
            LblStatus.Text = _translator._("Queued");
            ProgBar.IsIndeterminate = false;
        }
        else if (args.Status == DownloadStatus.Running)
        {
            Status = DownloadStatus.Running;
            LblStatus.Text = _translator._("Running");
            ProgBar.IsIndeterminate = true;
        }
        else
        {
            Status = DownloadStatus.Running;
            LblStatus.Text = _translator._("Processing");
            ProgBar.IsIndeterminate = true;
        }
        ViewStackButtons.SelectedIndex = (int)ButtonsPage.Running;
    }

    public void TriggerCompletedState(DownloadCompletedEventArgs args)
    {
        _path = args.Path;
        if (args.Log.Length > 0)
        {
            _log = args.Log.ToString();
        }
        LblTitle.Text = Path.GetFileName(_path);
        LblLog.Text = _log;
        ScrollLog.ChangeView(null, ScrollLog.ScrollableHeight, null);
        ProgBar.Value = 1.0;
        ProgBar.IsIndeterminate = false;
        if (args.Status == DownloadStatus.Error)
        {
            Status = DownloadStatus.Error;
            IcnStatus.Glyph = "\uEA39";
            LblStatus.Text = _translator._("Error");
            ViewStackButtons.SelectedIndex = (int)ButtonsPage.Error;
        }
        else if (args.Status == DownloadStatus.Success)
        {
            Status = DownloadStatus.Success;
            IcnStatus.Glyph = "\uE930";
            LblStatus.Text = _translator._("Success");
            ViewStackButtons.SelectedIndex = (int)ButtonsPage.Done;
        }
    }

    public void TriggerPausedState()
    {
        Status = DownloadStatus.Paused;
        IcnStatus.Glyph = "\uE769";
        LblStatus.Text = _translator._("Paused");
        ProgBar.IsIndeterminate = false;
        FntPauseResume.Glyph = "\uE768";
        LblPauseResume.Text = _translator._("Resume");
        _isPaused = true;
    }

    public void TriggerProgressState(DownloadProgressChangedEventArgs args)
    {
        IcnStatus.Glyph = "\uE896";
        if (args.LogChunk.Length > 0)
        {
            _log += $"{args.LogChunk.ToString()}\n";
        }
        if (double.IsNaN(args.Progress))
        {
            LblStatus.Text = _translator._("Processing");
            ProgBar.Value = 0.0;
            ProgBar.IsIndeterminate = true;
        }
        else
        {
            var speedStr = args.Speed > 0 ? FormatSpeed(args.Speed) : _translator._("Unknown");
            var etaStr = args.Eta > 0 ? FormatEta(args.Eta) : _translator._("Unknown");
            LblStatus.Text = _translator._("{0} • {1}", speedStr, etaStr);
            ProgBar.Value = args.Progress;
            ProgBar.IsIndeterminate = false;
        }
        LblLog.Text = _log;
        ScrollLog.ChangeView(null, ScrollLog.ScrollableHeight, null);
    }

    public void TriggerResumedState()
    {
        Status = DownloadStatus.Running;
        FntPauseResume.Glyph = "\uE769";
        LblPauseResume.Text = _translator._("Pause");
        _isPaused = false;
    }

    public void TriggerStartedFromQueueState()
    {
        Status = DownloadStatus.Running;
        LblStatus.Text = _translator._("Running");
        ProgBar.Value = 0.0;
        ProgBar.IsIndeterminate = true;
    }

    public void TriggerStoppedState()
    {
        Status = DownloadStatus.Stopped;
        IcnStatus.Glyph = "\uE783";
        LblStatus.Text = _translator._("Stopped");
        ProgBar.Value = 1.0;
        ProgBar.IsIndeterminate = false;
        ViewStackButtons.SelectedIndex = (int)ButtonsPage.Error;
    }

    private string FormatEta(int seconds)
    {
        if (seconds < 0)
        {
            return _translator._("Unknown");
        }
        var timeSpan = TimeSpan.FromSeconds(seconds);
        if (timeSpan.TotalHours >= 1)
        {
            return $"{(int)timeSpan.TotalHours}:{timeSpan.Minutes:D2}:{timeSpan.Seconds:D2}";
        }
        return $"{timeSpan.Minutes}:{timeSpan.Seconds:D2}";
    }

    private string FormatSpeed(double bytesPerSecond)
    {
        var units = new[] { "B/s", "KB/s", "MB/s", "GB/s" };
        var unitIndex = 0;
        while (bytesPerSecond >= 1024 && unitIndex < units.Length - 1)
        {
            bytesPerSecond /= 1024;
            unitIndex++;
        }
        return $"{bytesPerSecond:F2} {units[unitIndex]}";
    }

    private async void OpenFolder(object sender, RoutedEventArgs e) => await Launcher.LaunchFolderAsync(await StorageFolder.GetFolderFromPathAsync(Path.GetDirectoryName(_path)!));

    private void PauseResume(object sender, RoutedEventArgs e)
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

    private async void Play(object sender, RoutedEventArgs e) => await Launcher.LaunchFileAsync(await StorageFile.GetFileFromPathAsync(_path));

    private void Retry(object sender, RoutedEventArgs e) => RetryRequested?.Invoke(this, _id);

    private void ShowLog(object sender, RoutedEventArgs e) => GridLog.Visibility = BtnShowLog.IsChecked ?? false ? Visibility.Visible : Visibility.Collapsed;

    private void Stop(object sender, RoutedEventArgs e) => StopRequested?.Invoke(this, _id);
}
