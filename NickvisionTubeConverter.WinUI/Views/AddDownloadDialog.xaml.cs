using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
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
        ToolTipService.SetToolTip(BtnPasteFromClipboard, _controller.Localizer["PasteFromClipboard"]);
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
        ChkOverwriteFiles.Content = _controller.Localizer["OverwriteExistingFiles"];
        LblNumberVideos.Text = _controller.Localizer["NumberVideos"];
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
            return false;
        }
        _controller.PopulateDownloads(_videoUrlInfo!, (MediaFileType)CmbFileType.SelectedIndex, (Quality)CmbQuality.SelectedIndex, (Subtitle)CmbSubtitle.SelectedIndex, TxtSaveFolder.Text, ChkOverwriteFiles.IsChecked ?? false);
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
            LblTitle.Text = _videoUrlInfo.Videos.Count > 1 ? _videoUrlInfo.PlaylistTitle! : _videoUrlInfo.Videos[0].Title;
            BtnNumberVideos.Visibility = _videoUrlInfo.Videos.Count > 1 ? Visibility.Visible : Visibility.Collapsed;
            ListVideos.Items.Clear();
            foreach (var videoInfo in _videoUrlInfo.Videos)
            {
                ListVideos.Items.Add(new VideoRow(videoInfo, _controller.Localizer));
            }
        }
    }

    /// <summary>
    /// Occurs when the paste from clipboard is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void PasteFromClipboard(object sender, RoutedEventArgs e)
    {
        if (Clipboard.GetContent().Contains(StandardDataFormats.Text))
        {
            TxtVideoUrl.Text = (await Clipboard.GetContent().GetTextAsync()).ToString();
            SearchUrl(sender, e);
        }
    }

    /// <summary>
    /// Occurs when the back button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Back(object sender, RoutedEventArgs e)
    {
        ViewStack.ChangePage("Url");
        TxtVideoUrl.Text = "";
        IsPrimaryButtonEnabled = false;
    }

    /// <summary>
    /// Occurs when the file type combobox is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private void CmbFileType_SelectionChanged(object sender, SelectionChangedEventArgs e) => CmbSubtitle.IsEnabled = ((MediaFileType)CmbFileType.SelectedIndex).GetIsVideo();

    /// <summary>
    /// Occurs when the select save folder button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void SelectSaveFolder(object sender, RoutedEventArgs e)
    {
        var folderPicker = new FolderPicker();
        var fileType = (MediaFileType)CmbFileType.SelectedIndex;
        _initializeWithWindow(folderPicker);
        folderPicker.SuggestedStartLocation = fileType.GetIsVideo() ? PickerLocationId.VideosLibrary : PickerLocationId.MusicLibrary;
        folderPicker.FileTypeFilter.Add("*");
        var folder = await folderPicker.PickSingleFolderAsync();
        if (folder != null)
        {
            TxtSaveFolder.Text = folder.Path;
            IsPrimaryButtonEnabled = true;
        }
    }

    /// <summary>
    /// Occurs when the number videos toggle button is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ToggleNumberVideos(object sender, RoutedEventArgs e) => _controller.ToggleNumberVideos(_videoUrlInfo!, BtnNumberVideos.IsChecked ?? false);
}
