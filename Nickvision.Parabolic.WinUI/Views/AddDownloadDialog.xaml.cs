using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.Storage.Pickers;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class AddDownloadDialog : ContentDialog
{
    private enum Pages
    {
        Discover = 0,
        Loading,
        Single,
        Playlist
    }

    private static bool _showTeachDownloadImmediately = true;

    private readonly AddDownloadDialogController _controller;
    private readonly WindowId _windowId;
    private int _discoveryId = 0;

    static AddDownloadDialog()
    {
        _showTeachDownloadImmediately = true;
    }

    public AddDownloadDialog(AddDownloadDialogController controller, WindowId windowId)
    {
        InitializeComponent();
        _controller = controller;
        _windowId = windowId;
        Title = _controller.Translator._("Add Download");
        PrimaryButtonText = _controller.Translator._("Discover");
        CloseButtonText = _controller.Translator._("Cancel");
        DefaultButton = ContentDialogButton.Primary;
        IsPrimaryButtonEnabled = false;
        TxtUrl.Header = _controller.Translator._("Media URL");
        TxtUrl.PlaceholderText = _controller.Translator._("Enter media url here");
        LblSelectBatchFile.Text = _controller.Translator._("Select Batch File");
        TglUseAuthentication.OnContent = _controller.Translator._("Use Authentication");
        TglUseAuthentication.OffContent = _controller.Translator._("Use Authentication");
        CmbCredential.Header = _controller.Translator._("Credential");
        TxtUsername.Header = _controller.Translator._("Username");
        TxtUsername.PlaceholderText = _controller.Translator._("Enter username here");
        TxtPassword.Header = _controller.Translator._("Password");
        TxtPassword.PlaceholderText = _controller.Translator._("Enter password here");
        TglDownloadImmediately.OnContent = _controller.Translator._("Download Immediately");
        TglDownloadImmediately.OffContent = _controller.Translator._("Download Immediately");
        TeachDownloadImmediately.Title = _controller.Translator._("Warning");
        TeachDownloadImmediately.Subtitle = _controller.Translator._("Parabolic will download media based off of previously configured options and sensable defaults. Options including save folder, format, and subtitle selection will not be shown.");
        LblLoading.Text = _controller.Translator._("This may take some time...");
        foreach (var name in _controller.CredentialNames)
        {
            CmbCredential.Items.Add(name);
        }
        CmbCredential.SelectedIndex = 0;
        TxtSingleSaveFilename.Header = _controller.Translator._("File Name");
        ToolTipService.SetToolTip(BtnSingleRevertFilename, _controller.Translator._("Revert to Title"));
        TxtSingleSaveFolder.Header = _controller.Translator._("Save Folder");
        ToolTipService.SetToolTip(BtnSingleSelectSaveFolder, _controller.Translator._("Select Save Folder"));
    }

    public async new Task<ContentDialogResult> ShowAsync()
    {
        ViewStack.SelectedIndex = (int)Pages.Discover;
        TglDownloadImmediately.IsOn = _controller.PreviousDownloadOptions.DownloadImmediately;
        if (Clipboard.GetContent().Contains(StandardDataFormats.Text))
        {
            if (Uri.TryCreate(await Clipboard.GetContent().GetTextAsync(), UriKind.Absolute, out var uri))
            {
                TxtUrl.Text = uri.ToString();
                IsPrimaryButtonEnabled = true;
            }
        }
        var result = await base.ShowAsync();
        if (result != ContentDialogResult.Primary)
        {
            return result;
        }
        var cancellationToken = new CancellationTokenSource();
        Title = _controller.Translator._("Discovering Media");
        PrimaryButtonText = null;
        CloseButtonText = null;
        SecondaryButtonText = _controller.Translator._("Cancel");
        DefaultButton = ContentDialogButton.None;
        ViewStack.SelectedIndex = (int)Pages.Loading;
        DispatcherQueue.TryEnqueue(async () => await DiscoverMediaAsync(cancellationToken.Token));
        result = await base.ShowAsync();
        if (result == ContentDialogResult.Primary)
        {
            // Download
        }
        else if (result == ContentDialogResult.Secondary)
        {
            cancellationToken.Cancel();
        }
        return result;
    }

    private async Task DiscoverMediaAsync(CancellationToken cancellationToken)
    {
        DiscoveryResult? result = null;
        if (CmbCredential.SelectedIndex == 0)
        {
            Credential? credential = null;
            if (!string.IsNullOrEmpty(TxtUsername.Text) && !string.IsNullOrEmpty(TxtPassword.Password))
            {
                credential = new Credential("manual", TxtUsername.Text, TxtPassword.Password);
            }
            result = await _controller.DiscoverAsync(new Uri(TxtUrl.Text), credential, cancellationToken);
        }
        else
        {
            result = await _controller.DiscoverAsync(new Uri(TxtUrl.Text), (CmbCredential.SelectedItem as string)!, cancellationToken);
        }
        if (result is null)
        {
            Hide();
            return;
        }
        _discoveryId = result.Id;
        Title = _controller.Translator._("Add Download");
        PrimaryButtonText = _controller.Translator._("Download");
        CloseButtonText = _controller.Translator._("Cancel");
        SecondaryButtonText = null;
        DefaultButton = ContentDialogButton.Primary;
        if (!result.IsPlaylist)
        {
            var media = result.Media[0];
            ViewStack.SelectedIndex = (int)Pages.Single;
            TxtSingleSaveFilename.Text = media.Title;
            TxtSingleSaveFolder.Text = !string.IsNullOrEmpty(media.SuggestedSaveFolder) ? media.SuggestedSaveFolder : _controller.PreviousDownloadOptions.SaveFolder;
        }
        else
        {
            ViewStack.SelectedIndex = (int)Pages.Playlist;
        }
    }

    private void TxtUrl_TextChanged(object sender, TextChangedEventArgs e) => IsPrimaryButtonEnabled = Uri.TryCreate(TxtUrl.Text, UriKind.Absolute, out var _);

    private async void BtnSelectBatchFile_Click(object sender, RoutedEventArgs e)
    {
        var picker = new FileOpenPicker(_windowId)
        {
            SuggestedStartLocation = PickerLocationId.DocumentsLibrary,
            FileTypeFilter = { ".txt" }
        };
        var file = await picker.PickSingleFileAsync();
        if (file is not null)
        {
            TxtUrl.Text = new Uri($"file://{file.Path}").ToString();
        }
    }

    private void CmbCredential_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        var visibility = CmbCredential.SelectedIndex == 0 ? Visibility.Visible : Visibility.Collapsed;
        TxtUsername.Visibility = visibility;
        TxtPassword.Visibility = visibility;
    }

    private void TglDownloadImmediately_Toggled(object sender, RoutedEventArgs e)
    {
        if(!_controller.PreviousDownloadOptions.DownloadImmediately && _showTeachDownloadImmediately)
        {
            TeachDownloadImmediately.IsOpen = true;
            _showTeachDownloadImmediately = false;
        }
    }

    private void BtnSingleRevertFilename_Click(object sender, RoutedEventArgs e) => TxtSingleSaveFilename.Text = _controller.GetDiscoveredMediaTitle(_discoveryId, 0);

    private async void BtnSingleSelectSaveFolder_Click(object sender, RoutedEventArgs e)
    {
        var picker = new FolderPicker(_windowId)
        {
            SuggestedStartLocation = PickerLocationId.Downloads
        };
        var folder = await picker.PickSingleFolderAsync();
        if (folder is not null)
        {
            TxtSingleSaveFolder.Text = folder.Path;
        }
    }
}
