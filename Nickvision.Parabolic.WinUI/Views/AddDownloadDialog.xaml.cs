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

    private readonly AddDownloadDialogController _controller;
    private readonly WindowId _windowId;

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
        ChkUseAuthentication.Content = _controller.Translator._("Use Authentication");
        CmbCredential.Header = _controller.Translator._("Credential");
        TxtUsername.Header = _controller.Translator._("Username");
        TxtUsername.PlaceholderText = _controller.Translator._("Enter username here");
        TxtPassword.Header = _controller.Translator._("Password");
        TxtPassword.PlaceholderText = _controller.Translator._("Enter password here");
        LblLoading.Text = _controller.Translator._("This may take some time...");
        foreach(var name in _controller.CredentialNames)
        {
            CmbCredential.Items.Add(name);
        }
        CmbCredential.SelectedIndex = 0;
    }

    public async new Task<ContentDialogResult> ShowAsync()
    {
        ViewStack.SelectedIndex = (int)Pages.Discover;
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
        ViewStack.SelectedIndex = (int)Pages.Loading;
        DispatcherQueue.TryEnqueue(async () =>
        {
            DiscoveryResult? result = null;
            if(CmbCredential.SelectedIndex == 0)
            {
                Credential? credential = null;
                if (!string.IsNullOrEmpty(TxtUsername.Text) && !string.IsNullOrEmpty(TxtPassword.Password))
                {
                    credential = new Credential("manual", TxtUsername.Text, TxtPassword.Password);
                }
                result = await _controller.DiscoverAsync(new Uri(TxtUrl.Text), credential, cancellationToken.Token);
            }
            else
            {
                result = await _controller.DiscoverAsync(new Uri(TxtUrl.Text), (CmbCredential.SelectedItem as string)!, cancellationToken.Token);
            }
            if (result is not null)
            {
                Title = _controller.Translator._("Add Download");
                PrimaryButtonText = _controller.Translator._("Download");
                CloseButtonText = _controller.Translator._("Cancel");
                SecondaryButtonText = null;
                DefaultButton = ContentDialogButton.Primary;
                ViewStack.SelectedIndex = (int)Pages.Single;
                // Load single
            }
            else
            {
                // Show error
            }
        });
        result = await base.ShowAsync();
        if(result == ContentDialogResult.Primary)
        {
            // Download
        }
        else if(result == ContentDialogResult.Secondary)
        {
            cancellationToken.Cancel();
        }
        return result;
    }

    private void TxtUrl_TextChanged(object sender, TextChangedEventArgs e) => IsPrimaryButtonEnabled = Uri.TryCreate(TxtUrl.Text, UriKind.Absolute, out var _);

    private async void BtnSelectSaveFolder_Click(object sender, RoutedEventArgs e)
    {
        var picker = new FileOpenPicker(_windowId)
        {
            SuggestedStartLocation = PickerLocationId.DocumentsLibrary,
            FileTypeFilter = { ".txt" }
        };
        var file = await picker.PickSingleFileAsync();
        if(file is not null)
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
}
