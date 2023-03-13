using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Threading.Tasks;
using Windows.Storage.Pickers;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public sealed partial class AddDownloadDialog : ContentDialog
{
    private AddDownloadDialogController _controller;
    private readonly Action<object> _initializeWithWindow;
    private VideoUrlInfo? _videoUrlInfo;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    /// <param name="controller">The AddDownloadDialogController</param>
    /// <param name="initializeWithWindow">The Action<object> callback for InitializeWithWindow</param>
    public AddDownloadDialog(AddDownloadDialogController controller, Action<object> initializeWithWindow)
    {
        InitializeComponent();
        _controller = controller;
        _initializeWithWindow = initializeWithWindow;
        _videoUrlInfo = null;
        //Localize Strings
        Title = _controller.Localizer["AddDownload"];
        CloseButtonText = _controller.Localizer["Cancel"];
        PrimaryButtonText = _controller.Localizer["Download"];
        TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "Field"];
        TxtVideoUrl.PlaceholderText = _controller.Localizer["VideoUrl", "Placeholder"];
        ToolTipService.SetToolTip(BtnSearchUrl, _controller.Localizer["Search"]);
        LblBack.Text = _controller.Localizer["Back"];
        CmbFileType.Header = _controller.Localizer["FileType", "Field"];
        CmbFileType.Items.Add("MP4");
        CmbFileType.Items.Add("WEBM");
        CmbFileType.Items.Add("MP3");
        CmbFileType.Items.Add("OPUS");
        CmbFileType.Items.Add("FLAC");
        CmbFileType.Items.Add("WAV");
        CmbQuality.Header = _controller.Localizer["Quality", "Field"];
        CmbQuality.Items.Add(_controller.Localizer["Quality", "Best"]);
        CmbQuality.Items.Add(_controller.Localizer["Quality", "Good"]);
        CmbQuality.Items.Add(_controller.Localizer["Quality", "Worst"]);
        CmbQuality.SelectedIndex = 0;
        CmbSubtitle.Header = _controller.Localizer["Subtitle", "Field"];
        CmbSubtitle.Items.Add(_controller.Localizer["Subtitle", "None"]);
        CmbSubtitle.Items.Add("VTT");
        CmbSubtitle.Items.Add("SRT");
        CmbSubtitle.SelectedIndex = 0;
        TxtSaveFolder.Header = _controller.Localizer["SaveFolder", "Field"];
        ToolTipService.SetToolTip(BtnSelectSaveFolder, _controller.Localizer["SelectSaveFolder"]);
        TxtErrors.Text = _controller.Localizer["FixErrors", "WinUI"];
        //Load
        ViewStack.ChangePage("Url");
        IsPrimaryButtonEnabled = false;
        CmbFileType.SelectedIndex = (int)_controller.PreviousMediaFileType;
        TxtSaveFolder.Text = _controller.PreviousSaveFolder;
    }

    // <summary>
    /// Shows the AddDownloadDialog
    /// </summary>
    /// <returns>True if the dialog was accepted, else false</returns>
    public new async Task<bool> ShowAsync()
    {
        var result = await base.ShowAsync();
        if (result == ContentDialogResult.None)
        {
            _controller.Accepted = false;
            return false;
        }
        _controller.Accepted = true;
        _controller.PopulateDownloads(_videoUrlInfo!, (MediaFileType)CmbFileType.SelectedIndex, (Quality)CmbQuality.SelectedIndex, (Subtitle)CmbSubtitle.SelectedIndex, TxtSaveFolder.Text);
        return true;
    }

    /// <summary>
    /// Occurs when the search video url button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void SearchUrl(object sender, RoutedEventArgs e)
    {
        LoadingUrl.Visibility = Visibility.Visible;
        IsPrimaryButtonEnabled = false;
        await Task.Delay(25);
        _videoUrlInfo = await _controller.SearchUrlAsync(TxtVideoUrl.Text);
        LoadingUrl.Visibility = Visibility.Collapsed;
        if (_videoUrlInfo == null)
        {
            TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "Invalid"];
            TxtErrors.Visibility = Visibility.Visible;
        }
        else
        {
            TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "Field"];
            TxtErrors.Visibility = Visibility.Collapsed;
            ViewStack.ChangePage("Download");
            IsPrimaryButtonEnabled = !string.IsNullOrEmpty(TxtSaveFolder.Text);
            if (_videoUrlInfo.Videos.Count > 1)
            {
                LblTitle.Text = _videoUrlInfo.PlaylistTitle!;
            }
            else
            {
                LblTitle.Text = _videoUrlInfo.Videos[0].Title;
            }
            ListVideos.ItemsSource = _videoUrlInfo.Videos;
        }
    }

    private void Back(object sender, RoutedEventArgs e)
    {
        ViewStack.ChangePage("Url");
        IsPrimaryButtonEnabled = false;
    }

    private void CmbFileType_SelectionChanged(object sender, SelectionChangedEventArgs e) => CmbSubtitle.IsEnabled = ((MediaFileType)CmbFileType.SelectedIndex).GetIsVideo();

    private async void SelectSaveFolder(object sender, RoutedEventArgs e)
    {
        var folderPicker = new FolderPicker();
        var fileType = (MediaFileType)CmbFileType.SelectedIndex;
        _initializeWithWindow(folderPicker);
        folderPicker.SuggestedStartLocation = fileType.GetIsVideo() ? PickerLocationId.VideosLibrary : PickerLocationId.MusicLibrary;
        var folder = await folderPicker.PickSingleFolderAsync();
        if (folder != null)
        {
            TxtSaveFolder.Text = folder.Path;
            IsPrimaryButtonEnabled = true;
        }
    }
}
