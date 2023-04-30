using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Threading.Tasks;
using Windows.System;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A row for managing a download
/// </summary>
public sealed partial class DownloadRow : UserControl, IDownloadRowControl
{
    private readonly Localizer _localizer;
    private readonly Download _download;
    private bool? _previousEmbedMetadata;
    private bool _wasStopped;

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
    /// <param name="localizer">Localizer</param>
    /// <param name="download">Download</param>
    public DownloadRow(Localizer localizer, Download download)
    {
        InitializeComponent();
        _localizer = localizer;
        _download = download;
        _previousEmbedMetadata = null;
        _wasStopped = false;
        Progress = 0.0;
        Speed = 0.0;
        FinishedWithError = false;
        //Default
        Icon.Glyph = "\uE118";
        Icon.Foreground = (SolidColorBrush)Application.Current.Resources["ToolTipForegroundThemeBrush"];
        LblFilename.Text = _download.Filename;
        LblStatus.Text = _localizer["DownloadState", "Waiting"];
        BtnStop.Visibility = Visibility.Visible;
        BtnRetry.Visibility = Visibility.Collapsed;
        BtnOpenFile.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        ProgBar.Value = 0;
        //Localize Strings
        ToolTipService.SetToolTip(BtnViewLog, _localizer["ViewLog"]);
        ToolTipService.SetToolTip(BtnStop, _localizer["StopDownload"]);
        ToolTipService.SetToolTip(BtnRetry, _localizer["RetryDownload"]);
        ToolTipService.SetToolTip(BtnOpenFile, _localizer["OpenFile"]);
        ToolTipService.SetToolTip(BtnOpenSaveFolder, _localizer["OpenSaveFolder"]);
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
        Icon.Glyph = "\uE118";
        Icon.Foreground = (SolidColorBrush)Application.Current.Resources["ToolTipForegroundThemeBrush"];
        LblFilename.Text = _download.Filename;
        LblStatus.Text = _localizer["DownloadState", "Preparing"];
        BtnStop.Visibility = Visibility.Visible;
        BtnRetry.Visibility = Visibility.Collapsed;
        BtnOpenFile.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        ProgBar.Value = 0;
        var success = await _download.RunAsync(embedMetadata, _localizer, (state) =>
        {
            App.MainWindow!.DispatcherQueue.TryEnqueue(() =>
            {
                Icon.Foreground = (SolidColorBrush)Application.Current.Resources["AccentAAFillColorDefaultBrush"];
                ProgBar.Foreground = (SolidColorBrush)Application.Current.Resources["AccentAAFillColorDefaultBrush"];
                LblLog.Text = state.Log;
                ScrollLog.UpdateLayout();
                ScrollLog.ScrollToVerticalOffset(ScrollLog.ScrollableHeight);
            });
            switch (state.Status)
            {
                case DownloadProgressStatus.Downloading:
                    Progress = state.Progress;
                    Speed = state.Speed;
                    App.MainWindow!.DispatcherQueue.TryEnqueue(() =>
                    {
                        ProgBar.IsIndeterminate = false;
                        ProgBar.Value = state.Progress;
                        LblStatus.Text = string.Format(_localizer["DownloadState", "Downloading"], state.Progress * 100, state.Speed.GetSpeedString(_localizer));
                    });
                    break;
                case DownloadProgressStatus.DownloadingAria:
                    Progress = 1.0;
                    Speed = 0.0;
                    App.MainWindow!.DispatcherQueue.TryEnqueue(() =>
                    {
                        LblStatus.Text = _localizer["Downloading"];
                        ProgBar.IsIndeterminate = true;
                    });
                    break;
                case DownloadProgressStatus.Processing:
                    Progress = 1.0;
                    Speed = 0.0;
                    App.MainWindow!.DispatcherQueue.TryEnqueue(() =>
                    {
                        LblStatus.Text = _localizer["DownloadState", "Processing"];
                        ProgBar.IsIndeterminate = true;
                    });
                    break;
            }
        });
        FinishedWithError = !success;
        Icon.Foreground = new SolidColorBrush(success ? Colors.ForestGreen : Colors.Red);
        Icon.Glyph = success ? "\uE10B" : "\uE10A";
        ProgBar.IsIndeterminate = false;
        ProgBar.Value = 1;
        ProgBar.Foreground = new SolidColorBrush(success ? Colors.ForestGreen : Colors.Red);
        LblStatus.Text = success ? _localizer["Success"] : (_wasStopped ? _localizer["Stopped"] : _localizer["Error"]);
        BtnStop.Visibility = Visibility.Collapsed;
        BtnRetry.Visibility = !success ? Visibility.Visible : Visibility.Collapsed;
        BtnOpenFile.Visibility = success ? Visibility.Visible : Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = success ? Visibility.Visible : Visibility.Collapsed;
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
        Icon.Foreground = new SolidColorBrush(Colors.Red);
        Icon.Glyph = "\uE10A";
        ProgBar.IsIndeterminate = false;
        ProgBar.Value = 1;
        ProgBar.Foreground = new SolidColorBrush(Colors.Red);
        LblStatus.Text = _localizer["Stopped"];
        BtnStop.Visibility = Visibility.Collapsed;
        BtnRetry.Visibility = Visibility.Visible;
        BtnOpenFile.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        if (DownloadStoppedCallback != null)
        {
            DownloadStoppedCallback(this);
        }
    }

    /// <summary>
    /// Retries the download if needed
    /// </summary>
    public async Task RetryAsync()
    {
        if (_wasStopped || FinishedWithError)
        {
            _wasStopped = false;
            FinishedWithError = false;
            Icon.Glyph = "\uE118";
            Icon.Foreground = (SolidColorBrush)Application.Current.Resources["ToolTipForegroundThemeBrush"];
            LblFilename.Text = _download.Filename;
            LblStatus.Text = _localizer["DownloadState", "Waiting"];
            BtnStop.Visibility = Visibility.Visible;
            BtnRetry.Visibility = Visibility.Collapsed;
            BtnOpenFile.Visibility = Visibility.Collapsed;
            BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
            ProgBar.Value = 0;
            if (DownloadRetriedAsyncCallback != null)
            {
                await DownloadRetriedAsyncCallback(this);
            }
        }
    }

    /// <summary>
    /// Occurs when the view log button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void BtnViewLog_Click(object sender, RoutedEventArgs e) => SectionLog.Visibility = BtnViewLog.IsChecked ?? false ? Visibility.Visible : Visibility.Collapsed;

    /// <summary>
    /// Occurs when the stop button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void BtnStop_Click(object sender, RoutedEventArgs e) => Stop();

    /// <summary>
    /// Occurs when the retry button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void BtnRetry_Click(object sender, RoutedEventArgs e) => await RetryAsync();

    /// <summary>
    /// Occurs when the open file button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void BtnOpenFile_Click(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri($"{_download.SaveFolder}{Path.DirectorySeparatorChar}{_download.Filename}"));

    /// <summary>
    /// Occurs when the open save folder button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void BtnOpenSaveFolder_Click(object sender, RoutedEventArgs e) => await Launcher.LaunchFolderPathAsync(_download.SaveFolder);
}
