using CommunityToolkit.WinUI;
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.Storage.Pickers;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Linq;
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

    private enum SinglePages
    {
        General = 0,
        Subtitles,
        Advanced
    }

    private readonly AddDownloadDialogController _controller;
    private readonly WindowId _windowId;
    private int _discoveryId = 0;

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
        CmbCredential.ItemsSource = _controller.GetAvailableCredentials();
        CmbCredential.SelectedItem = (CmbCredential.ItemsSource as IEnumerable<SelectionItem>)?.FirstOrDefault(x => x.ShouldSelect);
        NavViewItemSingleGeneral.Text = _controller.Translator._("General");
        NavViewItemSingleSubtitles.Text = _controller.Translator._("Subtitles");
        NavViewItemSingleAdvanced.Text = _controller.Translator._("Advanced");
        TxtSingleSaveFilename.Header = _controller.Translator._("File Name");
        ToolTipService.SetToolTip(BtnSingleRevertFilename, _controller.Translator._("Revert to Title"));
        TxtSingleSaveFolder.Header = _controller.Translator._("Save Folder");
        ToolTipService.SetToolTip(BtnSingleSelectSaveFolder, _controller.Translator._("Select Save Folder"));
        CmbSingleFileType.Header = _controller.Translator._("File Type");
        TeachSingleFileType.Title = _controller.Translator._("Warning");
        TeachSingleFileType.Subtitle = _controller.Translator._("Generic file types do not fully support embedding thumbnails and subtitles. If neccessary, please select a specific file type that is known to support embedding to prevent separate files from being written.");
        CmbSingleVideoFormat.Header = _controller.Translator._("Video Format");
        CmbSingleAudioFormat.Header = _controller.Translator._("Audio Format");
        StatusSingleSubtitles.Title = _controller.Translator._("No Subtitles");
        StatusSingleSubtitles.Description = _controller.Translator._("No subtitles were found for this media.");
        LblSingleSelectAllSubtitles.Text = _controller.Translator._("Select All");
        LblSingleDeselectAllSubtitles.Text = _controller.Translator._("Deselect All");
        TglSingleSplitChapters.OnContent = _controller.Translator._("Split into Files by Chapters");
        TglSingleSplitChapters.OffContent = _controller.Translator._("Split into Files by Chapters");
        TglSingleExportDescription.OnContent = _controller.Translator._("Export Description to File");
        TglSingleExportDescription.OffContent = _controller.Translator._("Export Description to File");
        TglSingleExcludeFromHistory.OnContent = _controller.Translator._("Exclude from History");
        TglSingleExcludeFromHistory.OffContent = _controller.Translator._("Exclude from History");
        CmbSinglePostProcessorArgument.Header = _controller.Translator._("Post Processor Argument");
        TxtSingleStartTime.Header = _controller.Translator._("Start Time");
        TxtSingleEndTime.Header = _controller.Translator._("End Time");
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
        if (result == ContentDialogResult.Secondary)
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
        _controller.PreviousDownloadOptions.DownloadImmediately = TglDownloadImmediately.IsOn;
        if (!result.IsPlaylist)
        {
            var media = result.Media[0];
            var subtitles = _controller.GetAvailableSubtitleLanguages(_discoveryId, 0);
            PrimaryButtonClick += async (_, _) => await DownloadSingleAsync();
            ViewStack.SelectedIndex = (int)Pages.Single;
            ViewStackSingle.SelectedIndex = (int)SinglePages.General;
            ViewStackSingleSubtitles.SelectedIndex = subtitles.Any() ? 1 : 0;
            TxtSingleSaveFilename.Text = media.Title;
            TxtSingleSaveFolder.Text = !string.IsNullOrEmpty(media.SuggestedSaveFolder) ? media.SuggestedSaveFolder : _controller.PreviousDownloadOptions.SaveFolder;
            CmbSingleFileType.ItemsSource = _controller.GetAvailableFileTypes(_discoveryId, 0);
            CmbSingleFileType.SelectedItem = (CmbSingleFileType.ItemsSource as IEnumerable<SelectionItem>)?.FirstOrDefault(x => x.ShouldSelect);
            ListSingleSubtitles.ItemsSource = subtitles;
            foreach (var item in subtitles.Where(x => x.ShouldSelect))
            {
                ListSingleSubtitles.SelectedItems.Add(item);
            }
            TglSingleSplitChapters.IsOn = _controller.PreviousDownloadOptions.SplitChapters;
            TglSingleExportDescription.IsOn = _controller.PreviousDownloadOptions.ExportDescription;
            CmbSinglePostProcessorArgument.ItemsSource = await _controller.GetAvailablePostProcessorArgumentsAsync();
            CmbSinglePostProcessorArgument.SelectedItem = (CmbSinglePostProcessorArgument.ItemsSource as IEnumerable<SelectionItem>)?.FirstOrDefault(x => x.ShouldSelect);
            TxtSingleStartTime.PlaceholderText = media.TimeFrame.StartString;
            TxtSingleStartTime.Text = media.TimeFrame.StartString;
            TxtSingleEndTime.PlaceholderText = media.TimeFrame.EndString;
            TxtSingleEndTime.Text = media.TimeFrame.EndString;
            if (TglDownloadImmediately.IsOn)
            {
                await DownloadSingleAsync();
                Hide();
            }
        }
        else
        {
            PrimaryButtonClick += async (_, _) => await DownloadPlaylistAsync();
            ViewStack.SelectedIndex = (int)Pages.Playlist;
            // Playlist
            if (TglDownloadImmediately.IsOn)
            {
                await DownloadPlaylistAsync();
                Hide();
            }
        }
    }

    private async Task DownloadSingleAsync() => await _controller.AddSingleDownloadAsync(_discoveryId, 0,
        TxtSingleSaveFilename.Text,
        TxtSingleSaveFolder.Text,
        (CmbSingleFileType.SelectedItem as SelectionItem)!,
        (CmbSingleVideoFormat.SelectedItem as SelectionItem)!,
        (CmbSingleAudioFormat.SelectedItem as SelectionItem)!,
        ListSingleSubtitles.SelectedItems.Cast<SelectionItem>(),
        TglSingleSplitChapters.IsOn,
        TglSingleExportDescription.IsOn,
        TglSingleExcludeFromHistory.IsOn,
        (CmbSinglePostProcessorArgument.SelectedItem as SelectionItem)!,
        TxtSingleStartTime.Text,
        TxtSingleEndTime.Text
    );

    private async Task DownloadPlaylistAsync()
    {
        // Download
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

    private void TglDownloadImmediately_Toggled(object sender, RoutedEventArgs e) => TeachDownloadImmediately.IsOpen = _controller.GetShouldShowDownloadImmediatelyTeach();

    private void NavViewSingle_SelectionChanged(SelectorBar sender, SelectorBarSelectionChangedEventArgs args)
    {
        ViewStackSingle.SelectedIndex = (NavViewSingle.SelectedItem.Tag as string)! switch
        {
            "Subtitles" => (int)SinglePages.Subtitles,
            "Advanced" => (int)SinglePages.Advanced,
            _ => (int)SinglePages.General
        };
    }

    private void BtnSingleRevertFilename_Click(object sender, RoutedEventArgs e) => TxtSingleSaveFilename.Text = _controller.GetMediaTitle(_discoveryId, 0);

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

    private void CmbSingleFileType_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        TeachSingleFileType.IsOpen = _controller.GetShouldShowFileTypeTeach(_discoveryId, 0, (CmbSingleFileType.SelectedItem as SelectionItem)!);
        CmbSingleVideoFormat.ItemsSource = _controller.GetAvailableVideoFormats(_discoveryId, 0, (CmbSingleFileType.SelectedItem as SelectionItem)!);
        CmbSingleVideoFormat.SelectedItem = (CmbSingleVideoFormat.ItemsSource as IEnumerable<SelectionItem>)?.FirstOrDefault(x => x.ShouldSelect);
        CmbSingleAudioFormat.ItemsSource = _controller.GetAvailableAudioFormats(_discoveryId, 0, (CmbSingleFileType.SelectedItem as SelectionItem)!);
        CmbSingleAudioFormat.SelectedItem = (CmbSingleAudioFormat.ItemsSource as IEnumerable<SelectionItem>)?.FirstOrDefault(x => x.ShouldSelect);

    }

    private void BtnSingleSelectAllSubtitles_Click(object sender, RoutedEventArgs e) => ListSingleSubtitles.SelectAll();

    private void BtnSingleDeselectAllSubtitles_Click(object sender, RoutedEventArgs e) => ListSingleSubtitles.DeselectAll();
}
