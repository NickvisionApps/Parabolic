using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Threading.Tasks;
using Windows.System;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A row for managing a download
/// </summary>
public sealed partial class DownloadRow : UserControl, IDownloadRowControl
{
    private bool _disposed;
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
    /// Whether or not the download is done
    /// </summary>
    public bool IsDone => _download.IsDone;

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="localizer">Localizer</param>
    /// <param name="download">Download</param>
    public DownloadRow(Localizer localizer, Download download)
    {
        InitializeComponent();
        _disposed = false;
        _localizer = localizer;
        _download = download;
        _previousEmbedMetadata = null;
        _wasStopped = false;
        //Default
        Icon.Glyph = "\uE118";
        LblFilename.Text = _download.Filename;
        LblStatus.Text = _localizer["DownloadState", "Waiting"];
        BtnStop.Visibility = Visibility.Visible;
        BtnRetry.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        ProgBar.Value = 0;
        //Localize Strings
        ToolTipService.SetToolTip(BtnViewLog, _localizer["ViewLog"]);
        ToolTipService.SetToolTip(BtnStop, _localizer["StopDownload"]);
        ToolTipService.SetToolTip(BtnRetry, _localizer["RetryDownload"]);
        ToolTipService.SetToolTip(BtnOpenSaveFolder, _localizer["OpenSaveFolder"]);
        //Load
        LblUrl.Text = _download.VideoUrl;
    }

    /// <summary>
    /// Frees resources used by the DownloadRow object
    /// </summary>
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Frees resources used by the DownloadRow object
    /// </summary>
    private void Dispose(bool disposing)
    {
        if (_disposed)
        {
            return;
        }
        if (disposing)
        {
            _download.Dispose();
        }
        _disposed = true;
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
        Icon.Glyph = "\uE118";
        LblFilename.Text = _download.Filename;
        LblStatus.Text = _localizer["DownloadState", "Preparing"];
        BtnStop.Visibility = Visibility.Visible;
        BtnRetry.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        ProgBar.Value = 0;
        var success = await _download.RunAsync(embedMetadata, (state) =>
        {
            App.MainWindow!.DispatcherQueue.TryEnqueue(() =>
            {
                LblLog.Text = state.Log;
                ScrollLog.UpdateLayout();
                ScrollLog.ScrollToVerticalOffset(ScrollLog.ScrollableHeight);
            });
            switch (state.Status)
            {
                case DownloadProgressStatus.Downloading:
                    App.MainWindow!.DispatcherQueue.TryEnqueue(() =>
                    {
                        ProgBar.IsIndeterminate = false;
                        ProgBar.Value = state.Progress;
                        LblStatus.Text = string.Format(_localizer["DownloadState", "Downloading"], state.Progress * 100, state.Speed);
                    });
                    break;
                case DownloadProgressStatus.Processing:
                    App.MainWindow!.DispatcherQueue.TryEnqueue(() =>
                    {
                        LblStatus.Text = _localizer["DownloadState", "Processing"];
                        ProgBar.IsIndeterminate = true;
                    });
                    break;
            }
        });
        if (!_wasStopped)
        {
            Icon.Glyph = success ? "\uE10B" : "\uE10A";
            ProgBar.IsIndeterminate = false;
            ProgBar.Value = 1;
            LblStatus.Text = success ? _localizer["Success"] : _localizer["Error"];
            BtnStop.Visibility = Visibility.Collapsed;
            BtnRetry.Visibility = !success ? Visibility.Visible : Visibility.Collapsed;
            BtnOpenSaveFolder.Visibility = success ? Visibility.Visible : Visibility.Collapsed;
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
        Icon.Glyph = "\uE10A";
        ProgBar.IsIndeterminate = false;
        ProgBar.Value = 1;
        LblStatus.Text = _localizer["Stopped"];
        BtnStop.Visibility = Visibility.Collapsed;
        BtnRetry.Visibility = Visibility.Visible;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        if (DownloadStoppedCallback != null)
        {
            DownloadStoppedCallback(this);
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
    private async void BtnRetry_Click(object sender, RoutedEventArgs e)
    {
        if (DownloadRetriedAsyncCallback != null)
        {
            await DownloadRetriedAsyncCallback(this);
        }
    }

    /// <summary>
    /// Occurs when the open save folder button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void BtnOpenSaveFolder_Click(object sender, RoutedEventArgs e) => await Launcher.LaunchFolderPathAsync(_download.SaveFolder);
}
