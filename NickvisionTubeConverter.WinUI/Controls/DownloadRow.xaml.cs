using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage;
using Windows.System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public sealed partial class DownloadRow : UserControl, IDownloadRowControl
{
    private readonly string _saveFolder;
    private readonly Action<NotificationSentEventArgs> _sendNotificationCallback;
    private readonly XamlRoot _xamlRoot;
    private string _log;

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
    /// Construts a DownloadRow
    /// </summary>
    /// <param name="id">The Guid of the download</param>
    /// <param name="filename">The filename of the download</param>
    /// <param name="saveFolder">The save folder of the download</param>
    /// <param name="sendNotificationCallback">The callback for sending a notification</param>
    /// <param name="xamlRoot">The XamlRoot object for showing ContentDialogs</param>
    public DownloadRow(Guid id, string filename, string saveFolder, Action<NotificationSentEventArgs> sendNotificationCallback, XamlRoot xamlRoot)
    {
        InitializeComponent();
        Id = id;
        Filename = filename;
        _saveFolder = saveFolder;
        _sendNotificationCallback = sendNotificationCallback;
        _xamlRoot = xamlRoot;
        _log = "";
        //Localize strings
        LblProgress.Text = _("Waiting...");
        ToolTipService.SetToolTip(BtnStop, _("Stop Download"));
        ToolTipService.SetToolTip(BtnViewLog, _("View Log"));
        ToolTipService.SetToolTip(BtnOpenFile, _("Open File"));
        ToolTipService.SetToolTip(BtnOpenFolder, _("Open Folder"));
        ToolTipService.SetToolTip(BtnViewLog2, _("View Log"));
        ToolTipService.SetToolTip(BtnRetry, _("Retry Download"));
        //Load
        LblFilename.Text = Filename;
        _xamlRoot = xamlRoot;

    }

    /// <summary>
    /// Sets the row to the waiting state
    /// </summary>
    public void SetWaitingState()
    {
        StatusIcon.Glyph = "\uE118";
        StateViewStack.CurrentPageName = "Downloading";
        LblProgress.Text = _("Waiting...");
        ActionViewStack.CurrentPageName = "Cancel";
        ProgressBar.Value = 0;
        _log = "";
    }

    /// <summary>
    /// Sets the row to the preparing state
    /// </summary>
    public void SetPreparingState()
    {
        StatusIcon.Glyph = "\uE118";
        StateViewStack.CurrentPageName = "Downloading";
        LblProgress.Text = _("Preparing...");
        ActionViewStack.CurrentPageName = "Cancel";
        ProgressBar.Value = 0;
        _log = "";
    }

    /// <summary>
    /// Sets the row to the progress state
    /// </summary>
    /// <param name="state">The DownloadProgressState</param>
    public void SetProgressState(DownloadProgressState state)
    {
        StatusIcon.Glyph = "\uE118";
        ActionViewStack.CurrentPageName = "Cancel";
        _log = state.Log;
        switch (state.Status)
        {
            case DownloadProgressStatus.Downloading:
                StateViewStack.CurrentPageName = "Downloading";
                ProgressBar.Value = state.Progress;
                LblProgress.Text = _("Downloading {0:f2}% ({1})", state.Progress * 100, state.Speed.GetSpeedString());
                break;
            case DownloadProgressStatus.DownloadingAria:
            case DownloadProgressStatus.DownloadingFfmpeg:
                StateViewStack.CurrentPageName = "Processing";
                LblProgress.Text = _("Downloading...");
                break;
            case DownloadProgressStatus.Processing:
                StateViewStack.CurrentPageName = "Processing";
                LblProgress.Text = _("Processing...");
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
        StatusIcon.Glyph = success ? "\uE73E" : "\uE894";
        StateViewStack.CurrentPageName = "Downloading";
        ProgressBar.Value = success ? 1 : 0;
        LblProgress.Text = success ? _("Success") : _("Error");
        ActionViewStack.CurrentPageName = success ? "Open" : "Retry";
        Filename = filename;
        LblFilename.Text = Filename;
    }

    /// <summary>
    /// Sets the row to the stop state
    /// </summary>
    public void SetStopState()
    {
        StatusIcon.Glyph = "\uE71A";
        StateViewStack.CurrentPageName = "Downloading";
        ProgressBar.Value = 1;
        LblProgress.Text = _("Stopped");
        ActionViewStack.CurrentPageName = "Retry";
    }

    /// <summary>
    /// Occurs when the stop button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Stop(object sender, RoutedEventArgs e) => StopRequested?.Invoke(this, Id);

    /// <summary>
    /// Occurs when the open file button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void OpenFile(object sender, RoutedEventArgs e) => await Launcher.LaunchFileAsync(await StorageFile.GetFileFromPathAsync($"{_saveFolder}{Path.DirectorySeparatorChar}{Filename}"));

    /// <summary>
    /// Occurs when the open folder button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void OpenFolder(object sender, RoutedEventArgs e) => await Launcher.LaunchFolderPathAsync(_saveFolder);

    /// <summary>
    /// Occurs when the retry button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Retry(object sender, RoutedEventArgs e) => RetryRequested?.Invoke(this, Id);

    /// <summary>
    /// Occurs when the view log button is clicked
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void ViewLog(object sender, RoutedEventArgs e)
    {
        var logDialog = new ContentDialog()
        {
            Title = _("Log"),
            Content = new ScrollViewer()
            {
                MaxHeight = 300,
                Content = new TextBlock()
                {
                    Margin = new Thickness(0, 0, 14, 0),
                    Text = _log,
                    TextWrapping = TextWrapping.WrapWholeWords
                }
            },
            PrimaryButtonText = _("Copy to Clipboard"),
            CloseButtonText = _("OK"),
            DefaultButton = ContentDialogButton.Close,
            XamlRoot = _xamlRoot
        };
        var res = await logDialog.ShowAsync();
        if(res == ContentDialogResult.Primary)
        {
            var package = new DataPackage();
            package.SetText(_log);
            Clipboard.SetContent(package);
            _sendNotificationCallback(new NotificationSentEventArgs(_("Download log was copied to clipboard."), NotificationSeverity.Success));
        }
    }
}
