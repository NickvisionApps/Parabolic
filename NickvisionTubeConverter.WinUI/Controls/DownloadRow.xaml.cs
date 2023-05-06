using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using Windows.System;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A row for managing a download
/// </summary>
public sealed partial class DownloadRow : UserControl, IDownloadRowControl
{
    private readonly Localizer _localizer;
    private Guid _id;
    private string _filename;
    private string _saveFolder;

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
    /// <param name="id">The Guid of the download</param>
    /// <param name="filename">The filename of the download</param>
    /// <param name="saveFolder">The save folder of the download</param>
    /// <param name="localizer">The Localizer of strings</param>
    public DownloadRow(Guid id, string filename, string saveFolder, Localizer localizer)
    {
        InitializeComponent();
        _localizer = localizer;
        _id = id;
        _filename = filename;
        _saveFolder = saveFolder;
        //Default
        SetWaitingState();
        //Localize Strings
        ToolTipService.SetToolTip(BtnViewLog, _localizer["ViewLog"]);
        ToolTipService.SetToolTip(BtnStop, _localizer["StopDownload"]);
        ToolTipService.SetToolTip(BtnRetry, _localizer["RetryDownload"]);
        ToolTipService.SetToolTip(BtnOpenFile, _localizer["OpenFile"]);
        ToolTipService.SetToolTip(BtnOpenSaveFolder, _localizer["OpenSaveFolder"]);
    }

    /// <summary>
    /// Sets the row to the waiting state
    /// </summary>
    public void SetWaitingState()
    {
        Icon.Glyph = "\uE118";
        Icon.Foreground = (SolidColorBrush)Application.Current.Resources["ToolTipForegroundThemeBrush"];
        LblFilename.Text = _filename;
        LblStatus.Text = _localizer["DownloadState", "Waiting"];
        BtnStop.Visibility = Visibility.Visible;
        BtnRetry.Visibility = Visibility.Collapsed;
        BtnOpenFile.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        ProgBar.Value = 0;
    }

    /// <summary>
    /// Sets the row to the preparing state
    /// </summary>
    public void SetPreparingState()
    {
        Icon.Glyph = "\uE118";
        Icon.Foreground = (SolidColorBrush)Application.Current.Resources["ToolTipForegroundThemeBrush"];
        LblFilename.Text = _filename;
        LblStatus.Text = _localizer["DownloadState", "Preparing"];
        BtnStop.Visibility = Visibility.Visible;
        BtnRetry.Visibility = Visibility.Collapsed;
        BtnOpenFile.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        ProgBar.Value = 0;
    }

    /// <summary>
    /// Sets the row to the progress state
    /// </summary>
    /// <param name="state">The DownloadProgressState</param>
    public void SetProgressState(DownloadProgressState state)
    {
        Icon.Foreground = (SolidColorBrush)Application.Current.Resources["AccentAAFillColorDefaultBrush"];
        ProgBar.Foreground = (SolidColorBrush)Application.Current.Resources["AccentAAFillColorDefaultBrush"];
        LblLog.Text = state.Log;
        ScrollLog.UpdateLayout();
        ScrollLog.ScrollToVerticalOffset(ScrollLog.ScrollableHeight);
        switch (state.Status)
        {
            case DownloadProgressStatus.Downloading:
                ProgBar.IsIndeterminate = false;
                ProgBar.Value = state.Progress;
                LblStatus.Text = string.Format(_localizer["DownloadState", "Downloading"], state.Progress * 100, state.Speed.GetSpeedString(_localizer));
                break;
            case DownloadProgressStatus.DownloadingAria:
                LblStatus.Text = _localizer["Downloading"];
                ProgBar.IsIndeterminate = true;
                break;
            case DownloadProgressStatus.Processing:
                LblStatus.Text = _localizer["DownloadState", "Processing"];
                ProgBar.IsIndeterminate = true;
                break;
        }
    }

    /// <summary>
    /// Sets the row to the completed state
    /// </summary>
    /// <param name="success">Whether or not the download was successful</param>
    public void SetCompletedState(bool success)
    {
        Icon.Glyph = success ? "\uE10B" : "\uE10A";
        Icon.Foreground = new SolidColorBrush(success ? Colors.ForestGreen : Colors.Red);
        ProgBar.IsIndeterminate = false;
        ProgBar.Value = 1;
        ProgBar.Foreground = new SolidColorBrush(success ? Colors.ForestGreen : Colors.Red);
        LblStatus.Text = success ? _localizer["Success"] : _localizer["Error"];
        BtnStop.Visibility = Visibility.Collapsed;
        BtnRetry.Visibility = !success ? Visibility.Visible : Visibility.Collapsed;
        BtnOpenFile.Visibility = success ? Visibility.Visible : Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = success ? Visibility.Visible : Visibility.Collapsed;
    }

    /// <summary>
    /// Sets the row to the stop state
    /// </summary>
    public void SetStopState()
    {
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
    }

    /// <summary>
    /// Sets the row to the retry state
    /// </summary>
    public void SetRetryState()
    {
        Icon.Glyph = "\uE118";
        Icon.Foreground = (SolidColorBrush)Application.Current.Resources["ToolTipForegroundThemeBrush"];
        LblFilename.Text = _filename;
        LblStatus.Text = _localizer["DownloadState", "Waiting"];
        BtnStop.Visibility = Visibility.Visible;
        BtnRetry.Visibility = Visibility.Collapsed;
        BtnOpenFile.Visibility = Visibility.Collapsed;
        BtnOpenSaveFolder.Visibility = Visibility.Collapsed;
        ProgBar.Value = 0;
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
    private void BtnStop_Click(object sender, RoutedEventArgs e) => StopRequested?.Invoke(this, _id);

    /// <summary>
    /// Occurs when the retry button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void BtnRetry_Click(object sender, RoutedEventArgs e) => RetryRequested?.Invoke(this, _id);

    /// <summary>
    /// Occurs when the open file button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void BtnOpenFile_Click(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri($"{_saveFolder}{Path.DirectorySeparatorChar}{_filename}"));

    /// <summary>
    /// Occurs when the open save folder button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void BtnOpenSaveFolder_Click(object sender, RoutedEventArgs e) => await Launcher.LaunchFolderPathAsync(_saveFolder);
}
