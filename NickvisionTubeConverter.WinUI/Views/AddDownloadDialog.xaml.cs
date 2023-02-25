using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
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
        //Localize Strings
        Title = _controller.Localizer["AddDownload"];
        CloseButtonText = _controller.Localizer["Cancel"];
        PrimaryButtonText = _controller.Localizer["Download"];
        TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "Field"];
        TxtVideoUrl.PlaceholderText = _controller.Localizer["VideoUrl", "Placeholder"];
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
        TxtSavePath.Header = _controller.Localizer["SavePath", "Field"];
        ToolTipService.SetToolTip(BtnSelectSavePath, _controller.Localizer["SelectSaveFolder"]);
        TxtErrors.Text = _controller.Localizer["FixErrors", "WinUI"];
        //Load
        ViewStack.ChangePage("Download");
        CmbFileType.SelectedIndex = (int)_controller.PreviousMediaFileType;
    }

    // <summary>
    /// Shows the AddDownloadDialog
    /// </summary>
    /// <returns>True if the dialog was accepted, else false</returns>
    public async Task<bool> ShowAsync()
    {
        await ValidateAsync();
        var result = await base.ShowAsync();
        if (result == ContentDialogResult.None)
        {
            _controller.Accepted = false;
            return false;
        }
        _controller.Accepted = true;
        return true;
    }

    /// <summary>
    /// Validates the dialog's input
    /// </summary>
    private async Task ValidateAsync()
    {
        var checkStatus = await _controller.UpdateDownloadAsync(TxtVideoUrl.Text, (MediaFileType)CmbFileType.SelectedIndex, TxtSavePath.Text, (Quality)CmbQuality.SelectedIndex, (Subtitle)CmbSubtitle.SelectedIndex);
        TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "Field"];
        CmbFileType.IsEnabled = false;
        CmbQuality.IsEnabled = false;
        CmbSubtitle.IsEnabled = false;
        TxtSavePath.Header = _controller.Localizer["SavePath", "Field"];
        TxtSavePath.IsEnabled = false;
        TxtSavePath.Text = _controller.SavePath;
        BtnSelectSavePath.IsEnabled = false;
        if (checkStatus == DownloadCheckStatus.Valid)
        {
            CmbFileType.IsEnabled = true;
            CmbQuality.IsEnabled = true;
            CmbSubtitle.IsEnabled = ((MediaFileType)CmbFileType.SelectedIndex).GetIsVideo();
            TxtSavePath.IsEnabled = true;
            BtnSelectSavePath.IsEnabled = true;
            TxtErrors.Visibility = Visibility.Collapsed;
            IsPrimaryButtonEnabled = true;
        }
        else
        {
            if (checkStatus.HasFlag(DownloadCheckStatus.EmptyVideoUrl))
            {
                TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "Empty"];
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.InvalidVideoUrl))
            {
                TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "Invalid"];
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.PlaylistNotSupported))
            {
                TxtVideoUrl.Header = _controller.Localizer["VideoUrl", "PlaylistNotSupported"];
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.InvalidSaveFolder))
            {
                TxtSavePath.Header = _controller.Localizer["SavePath", "Invalid"];
                CmbFileType.IsEnabled = true;
                CmbQuality.IsEnabled = true;
                CmbSubtitle.IsEnabled = ((MediaFileType)CmbFileType.SelectedIndex).GetIsVideo();
                TxtSavePath.IsEnabled = true;
                BtnSelectSavePath.IsEnabled = true;

            }
            TxtErrors.Visibility = Visibility.Visible;
            IsPrimaryButtonEnabled = false;
        }
    }

    /// <summary>
    /// Occurs when the select save path button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void SelectSavePath(object sender, RoutedEventArgs e)
    {
        var fileSavePicker = new FileSavePicker();
        var fileType = (MediaFileType)CmbFileType.SelectedIndex;
        _initializeWithWindow(fileSavePicker);
        fileSavePicker.FileTypeChoices.Add(fileType.ToString(), new List<string>() { fileType.GetDotExtension() });
        fileSavePicker.SuggestedStartLocation = fileType.GetIsVideo() ? PickerLocationId.VideosLibrary : PickerLocationId.MusicLibrary;
        var file = await fileSavePicker.PickSaveFileAsync();
        if (file != null)
        {
            TxtSavePath.Text = file.Path;
            await ValidateAsync();
        }
    }

    /// <summary>
    /// Occurs when the video url is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">TextChangedEventArgs</param>
    private async void TxtVideoUrl_TextChanged(object sender, TextChangedEventArgs e)
    {
        ViewStack.ChangePage("Loading");
        await Task.Delay(50);
        await ValidateAsync();
        ViewStack.ChangePage("Download");
    }

    /// <summary>
    /// Occurs when the file type is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private async void CmbFileType_SelectionChanged(object sender, SelectionChangedEventArgs e) => await ValidateAsync();

    /// <summary>
    /// Occurs when the quality is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private async void CmbQuality_SelectionChanged(object sender, SelectionChangedEventArgs e) => await ValidateAsync();

    /// <summary>
    /// Occurs when the subtitle is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private async void CmbSubtitle_SelectionChanged(object sender, SelectionChangedEventArgs e) => await ValidateAsync();
}
