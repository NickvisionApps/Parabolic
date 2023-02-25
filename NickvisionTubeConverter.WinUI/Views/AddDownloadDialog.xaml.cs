using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public sealed partial class AddDownloadDialog : ContentDialog
{
    private AddDownloadDialogController _controller;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    public AddDownloadDialog(AddDownloadDialogController controller)
    {
        InitializeComponent();
        _controller = controller;
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

    private void SelectSavePath(object sender, RoutedEventArgs e)
    {

    }

    private async void TxtVideoUrl_TextChanged(object sender, TextChangedEventArgs e)
    {
        ViewStack.ChangePage("Loading");
        await Task.Delay(50);
        await ValidateAsync();
        ViewStack.ChangePage("Download");
    }

    private async void CmbFileType_SelectionChanged(object sender, SelectionChangedEventArgs e) => await ValidateAsync();

    private async void CmbQuality_SelectionChanged(object sender, SelectionChangedEventArgs e) => await ValidateAsync();

    private async void CmbSubtitle_SelectionChanged(object sender, SelectionChangedEventArgs e) => await ValidateAsync();
}
