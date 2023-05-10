using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage.Pickers;
using Windows.System;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public sealed partial class AddDownloadDialog : ContentDialog
{
    private AddDownloadDialogController _controller;
    private readonly Action<object> _initializeWithWindow;
    private MediaUrlInfo? _mediaUrlInfo;

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
        _mediaUrlInfo = null;
        //Localize Strings
        Title = _controller.Localizer["AddDownload"];
        CloseButtonText = _controller.Localizer["Cancel"];
        PrimaryButtonText = _controller.Localizer["Download"];
        TxtMediaUrl.Header = _controller.Localizer["MediaUrl", "Field"];
        TxtMediaUrl.PlaceholderText = _controller.Localizer["MediaUrl", "Placeholder"];
        ToolTipService.SetToolTip(BtnPasteFromClipboard, _controller.Localizer["PasteFromClipboard"]);
        BtnValidateUrl.Content = _controller.Localizer["ValidateUrl"];
        CardFileType.Header = _controller.Localizer["FileType", "Field"];
        CmbFileType.Items.Add("MP4");
        CmbFileType.Items.Add("WEBM");
        CmbFileType.Items.Add("MP3");
        CmbFileType.Items.Add("OPUS");
        CmbFileType.Items.Add("FLAC");
        CmbFileType.Items.Add("WAV");
        CardQuality.Header = _controller.Localizer["Quality", "Field"];
        CmbQuality.Items.Add(_controller.Localizer["Quality", "Best"]);
        CmbQuality.Items.Add(_controller.Localizer["Quality", "Good"]);
        CmbQuality.Items.Add(_controller.Localizer["Quality", "Worst"]);
        CmbQuality.SelectedIndex = 0;
        CardSubtitle.Header = _controller.Localizer["Subtitle", "Field"];
        CmbSubtitle.Items.Add(_controller.Localizer["Subtitle", "None"]);
        CmbSubtitle.Items.Add("VTT");
        CmbSubtitle.Items.Add("SRT");
        CmbSubtitle.SelectedIndex = 0;
        CardSaveFolder.Header = _controller.Localizer["SaveFolder", "Field"];
        ToolTipService.SetToolTip(BtnSelectSaveFolder, _controller.Localizer["SelectSaveFolder"]);
        CardOverwriteFiles.Header = _controller.Localizer["OverwriteExistingFiles"];
        CardSpeedLimit.Header = _controller.Localizer["SpeedLimit"];
        CardSpeedLimit.Description = $"{string.Format(_controller.Localizer["Speed", "KiBps"], _controller.CurrentSpeedLimit)} ({_controller.Localizer["Configurable", "WinUI"]})";
        LblDownloads.Text = _controller.Localizer["Downloads"];
        LblNumberTitles.Text = _controller.Localizer["NumberTitles"];
        TxtErrors.Text = _controller.Localizer["FixErrors", "WinUI"];
        //Load
        ViewStack.ChangePage("Url");
        IsPrimaryButtonEnabled = false;
        CmbFileType.SelectedIndex = (int)_controller.PreviousMediaFileType;
        LblSaveFolder.Text = _controller.PreviousSaveFolder;
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
        _controller.PopulateDownloads(_mediaUrlInfo!, (MediaFileType)CmbFileType.SelectedIndex, (Quality)CmbQuality.SelectedIndex, (Subtitle)CmbSubtitle.SelectedIndex, LblSaveFolder.Text, TglOverwriteFiles.IsOn, TglSpeedLimit.IsOn);
        return true;
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => StackPanel.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the validate media url button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void ValidateUrl(object sender, RoutedEventArgs e)
    {
        BtnValidateUrl.IsEnabled = false;
        LoadingUrl.Visibility = Visibility.Visible;
        IsPrimaryButtonEnabled = false;
        await Task.Delay(25);
        _mediaUrlInfo = await _controller.SearchUrlAsync(TxtMediaUrl.Text);
        BtnValidateUrl.IsEnabled = true;
        LoadingUrl.Visibility = Visibility.Collapsed;
        if (_mediaUrlInfo == null)
        {
            TxtMediaUrl.Header = _controller.Localizer["MediaUrl", "Invalid"];
            TxtErrors.Visibility = Visibility.Visible;
        }
        else
        {
            TxtMediaUrl.Header = _controller.Localizer["MediaUrl", "Field"];
            TxtErrors.Visibility = Visibility.Collapsed;
            ViewStack.ChangePage("Download");
            IsPrimaryButtonEnabled = !string.IsNullOrEmpty(LblSaveFolder.Text);
            BtnNumberTitles.Visibility = _mediaUrlInfo.MediaList.Count > 1 ? Visibility.Visible : Visibility.Collapsed;
            ListMediaList.Children.Clear();
            foreach (var mediaInfo in _mediaUrlInfo.MediaList)
            {
                ListMediaList.Children.Add(new MediaRow(mediaInfo, _controller.Localizer));
            }
        }
    }

    /// <summary>
    /// Occurs when a key is pressed on the TxtMediaUrl control
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">KeyRoutedEventArgs</param>
    private void TxtMediaUrl_KeyDown(object sender, KeyRoutedEventArgs e)
    {
        if (e.Key == VirtualKey.Enter)
        {
            ValidateUrl(sender, e);
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
            TxtMediaUrl.Text = (await Clipboard.GetContent().GetTextAsync()).ToString();
            ValidateUrl(sender, e);
        }
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
        folderPicker.SuggestedStartLocation = fileType.GetIsVideo() ? PickerLocationId.MediaListLibrary : PickerLocationId.MusicLibrary;
        folderPicker.FileTypeFilter.Add("*");
        var folder = await folderPicker.PickSingleFolderAsync();
        if (folder != null)
        {
            LblSaveFolder.Text = folder.Path;
            IsPrimaryButtonEnabled = true;
        }
    }

    /// <summary>
    /// Occurs when the number medias toggle button is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ToggleNumberTitles(object sender, RoutedEventArgs e) => _controller.ToggleNumberTitles(_mediaUrlInfo!, BtnNumberTitles.IsChecked ?? false);
}
