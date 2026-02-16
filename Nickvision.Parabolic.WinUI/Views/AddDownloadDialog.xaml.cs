using CommunityToolkit.WinUI;
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.Storage.Pickers;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.WinUI.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.WinUI.Helpers;
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

    private enum PlaylistPages
    {
        General = 0,
        Items,
        Subtitles,
        Advanced
    }

    private readonly AddDownloadDialogController _controller;
    private readonly WindowId _windowId;
    private DiscoveryContext? _discoveryContext;
    private bool _isUpdatingSubtitleSelection;

    public AddDownloadDialog(AddDownloadDialogController controller, WindowId windowId)
    {
        InitializeComponent();
        _controller = controller;
        _windowId = windowId;
        _discoveryContext = null;
        _isUpdatingSubtitleSelection = false;
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
        TeachDownloadImmediately.Subtitle = _controller.Translator._("Parabolic will download media based off of previously configured options and sensible defaults. Options including save folder, format, and subtitle selection will not be shown.");
        LblLoading.Text = _controller.Translator._("This may take some time...");
        CmbCredential.ItemsSource = _controller.AvailableCredentials;
        CmbCredential.SelectSelectionItem();
        NavViewItemSingleGeneral.Text = _controller.Translator._("General");
        NavViewItemSingleSubtitles.Text = _controller.Translator._("Subtitles");
        NavViewItemSingleAdvanced.Text = _controller.Translator._("Advanced");
        TxtSingleSaveFilename.Header = _controller.Translator._("File Name");
        ToolTipService.SetToolTip(BtnSingleRevertFilename, _controller.Translator._("Revert to Title"));
        TxtSingleSaveFolder.Header = _controller.Translator._("Save Folder");
        ToolTipService.SetToolTip(BtnSingleSelectSaveFolder, _controller.Translator._("Select Save Folder"));
        CmbSingleFileType.Header = _controller.Translator._("File Type");
        TeachSingleFileType.Title = _controller.Translator._("Warning");
        TeachSingleFileType.Subtitle = _controller.Translator._("Generic types do not support embedding thumbnails and subtitles. If necessary, please select a specific type that is known to support embedding to prevent separate files from being written.");
        CmbSingleVideoFormat.Header = _controller.Translator._("Video Format");
        CmbSingleAudioFormat.Header = _controller.Translator._("Audio Format");
        StatusSingleSubtitles.Title = _controller.Translator._("No Subtitles");
        StatusSingleSubtitles.Description = _controller.Translator._("No subtitles were found for this media.");
        LblSingleSelectAllSubtitles.Text = _controller.Translator._("Select All");
        LblSingleDeselectAllSubtitles.Text = _controller.Translator._("Deselect All");
        TxtSingleSubtitlesSearch.PlaceholderText = _controller.Translator._("Search subtitles");
        TglSingleSplitChapters.OnContent = _controller.Translator._("Split into Files by Chapters");
        TglSingleSplitChapters.OffContent = _controller.Translator._("Split into Files by Chapters");
        TglSingleExportDescription.OnContent = _controller.Translator._("Export Description to File");
        TglSingleExportDescription.OffContent = _controller.Translator._("Export Description to File");
        TglSingleExcludeFromHistory.OnContent = _controller.Translator._("Exclude from History");
        TglSingleExcludeFromHistory.OffContent = _controller.Translator._("Exclude from History");
        CmbSinglePostProcessorArgument.Header = _controller.Translator._("Post Processor Argument");
        TxtSingleStartTime.Header = _controller.Translator._("Start Time");
        TxtSingleEndTime.Header = _controller.Translator._("End Time");
        NavViewItemPlaylistGeneral.Text = _controller.Translator._("General");
        NavViewItemPlaylistItems.Text = _controller.Translator._("Items");
        NavViewItemPlaylistSubtitles.Text = _controller.Translator._("Subtitles");
        NavViewItemPlaylistAdvanced.Text = _controller.Translator._("Advanced");
        TxtPlaylistSaveFolder.Header = _controller.Translator._("Save Folder");
        ToolTipService.SetToolTip(BtnPlaylistSelectSaveFolder, _controller.Translator._("Select Save Folder"));
        CmbPlaylistFileType.Header = _controller.Translator._("File Type");
        TeachPlaylistFileType.Title = _controller.Translator._("Warning");
        TeachPlaylistFileType.Subtitle = _controller.Translator._("Generic types do not support embedding thumbnails and subtitles. If necessary, please select a specific type that is known to support embedding to prevent separate files from being written.");
        CmbPlaylistSuggestedVideoResolution.Header = _controller.Translator._("Suggested Video Resolution");
        CmbPlaylistSuggestedAudioBitrate.Header = _controller.Translator._("Suggested Audio Bitrate");
        LblPlaylistSelectAllItems.Text = _controller.Translator._("Select All");
        LblPlaylistDeselectAllItems.Text = _controller.Translator._("Deselect All");
        TglPlaylistReverseDownloadOrder.OnContent = _controller.Translator._("Reverse Download Order");
        TglPlaylistReverseDownloadOrder.OffContent = _controller.Translator._("Reverse Download Order");
        TglPlaylistNumberTitles.OnContent = _controller.Translator._("Number Titles");
        TglPlaylistNumberTitles.OffContent = _controller.Translator._("Number Titles");
        TeachPlaylistNumberTitles.Title = _controller.Translator._("Warning");
        TeachPlaylistNumberTitles.Subtitle = _controller.Translator._("Numbering will be applied to titles of selected items in succession on download.");
        StatusPlaylistSubtitles.Title = _controller.Translator._("No Subtitles");
        StatusPlaylistSubtitles.Description = _controller.Translator._("No subtitles were found in this playlist.");
        LblPlaylistSelectAllSubtitles.Text = _controller.Translator._("Select All");
        LblPlaylistDeselectAllSubtitles.Text = _controller.Translator._("Deselect All");
        LblPlaylistSubtitleNote.Text = _controller.Translator._("Note: Some playlist items may not contain subtitles for a selected language.");
        TxtPlaylistSubtitlesSearch.PlaceholderText = _controller.Translator._("Search subtitles");
        TglPlaylistExportM3U.OnContent = _controller.Translator._("Export M3U Playlist File");
        TglPlaylistExportM3U.OffContent = _controller.Translator._("Export M3U Playlist File");
        TglPlaylistSplitChapters.OnContent = _controller.Translator._("Split into Files by Chapters");
        TglPlaylistSplitChapters.OffContent = _controller.Translator._("Split into Files by Chapters");
        TglPlaylistExportDescription.OnContent = _controller.Translator._("Export Description to File");
        TglPlaylistExportDescription.OffContent = _controller.Translator._("Export Description to File");
        TglPlaylistExcludeFromHistory.OnContent = _controller.Translator._("Exclude from History");
        TglPlaylistExcludeFromHistory.OffContent = _controller.Translator._("Exclude from History");
        CmbPlaylistPostProcessorArgument.Header = _controller.Translator._("Post Processor Argument");
    }

    public async new Task<ContentDialogResult> ShowAsync()
    {
        ViewStack.SelectedIndex = (int)Pages.Discover;
        TglDownloadImmediately.IsOn = _controller.PreviousDownloadOptions.DownloadImmediately;
        if (string.IsNullOrEmpty(TxtUrl.Text))
        {
            if (Clipboard.GetContent().Contains(StandardDataFormats.Text))
            {
                if (Uri.TryCreate(await Clipboard.GetContent().GetTextAsync(), UriKind.Absolute, out var uri))
                {
                    TxtUrl.Text = uri.ToString();
                    IsPrimaryButtonEnabled = true;
                }
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
        CloseButtonText = _controller.Translator._("Cancel");
        DefaultButton = ContentDialogButton.None;
        ViewStack.SelectedIndex = (int)Pages.Loading;
        DispatcherQueue.TryEnqueue(async () => await DiscoverMediaAsync(cancellationToken.Token));
        result = await base.ShowAsync();
        if (result == ContentDialogResult.Primary)
        {
            if (_discoveryContext is not null && _discoveryContext.Items.Count > 1)
            {
                await DownloadPlaylistAsync();
            }
            else
            {
                await DownloadSingleAsync();
            }
        }
        else
        {
            cancellationToken.Cancel();
        }
        cancellationToken.Dispose();
        return result;
    }

    public async Task<ContentDialogResult> ShowAsync(Uri url)
    {
        TxtUrl.Text = url.ToString();
        IsPrimaryButtonEnabled = true;
        return await ShowAsync();
    }

    private async Task DiscoverMediaAsync(CancellationToken cancellationToken)
    {
        Credential? credential = null;
        if (!string.IsNullOrEmpty(TxtUsername.Text) || !string.IsNullOrEmpty(TxtPassword.Password))
        {
            credential = new Credential("manual", TxtUsername.Text, TxtPassword.Password);
        }
        else
        {
            credential = (CmbCredential.SelectedItem as SelectionItem<Credential?>)!.Value;
        }
        _discoveryContext = await _controller.DiscoverAsync(new Uri(TxtUrl.Text), credential, cancellationToken);
        if (_discoveryContext is null)
        {
            Hide();
            return;
        }
        Title = _controller.Translator._("Configure Download");
        PrimaryButtonText = _controller.Translator._("Download");
        CloseButtonText = _controller.Translator._("Cancel");
        SecondaryButtonText = null;
        DefaultButton = ContentDialogButton.Primary;
        _controller.PreviousDownloadOptions.DownloadImmediately = TglDownloadImmediately.IsOn;
        if (_discoveryContext.Items.Count == 1)
        {
            ViewStack.SelectedIndex = (int)Pages.Single;
            ViewStackSingle.SelectedIndex = (int)SinglePages.General;
            ViewStackSingleSubtitles.SelectedIndex = _discoveryContext.SubtitleLanguages.Any() ? 1 : 0;
            TxtSingleSaveFilename.Text = _discoveryContext.Items[0].Label;
            TxtSingleSaveFolder.Text = _controller.PreviousDownloadOptions.SaveFolder;
            CmbSingleVideoFormat.ItemsSource = _discoveryContext.VideoFormats;
            CmbSingleAudioFormat.ItemsSource = _discoveryContext.AudioFormats;
            CmbSingleFileType.ItemsSource = _discoveryContext.FileTypes;
            CmbSingleFileType.SelectSelectionItem();
            ListSingleSubtitles.ItemsSource = _discoveryContext.SubtitleLanguages;
            ListSingleSubtitles.SelectSelectionItems();
            TxtSingleSubtitlesSearch.Text = string.Empty;
            TglSingleSplitChapters.IsOn = _controller.PreviousDownloadOptions.SplitChapters;
            TglSingleExportDescription.IsOn = _controller.PreviousDownloadOptions.ExportDescription;
            CmbSinglePostProcessorArgument.ItemsSource = _controller.AvailablePostProcessorArguments;
            CmbSinglePostProcessorArgument.SelectSelectionItem();
            TxtSingleStartTime.PlaceholderText = _discoveryContext.Items[0].StartTime;
            TxtSingleStartTime.Text = _discoveryContext.Items[0].StartTime;
            TxtSingleEndTime.PlaceholderText = _discoveryContext.Items[0].EndTime;
            TxtSingleEndTime.Text = _discoveryContext.Items[0].EndTime;
            if (TglDownloadImmediately.IsOn)
            {
                await DownloadSingleAsync();
                Hide();
            }
        }
        else
        {
            ViewStack.SelectedIndex = (int)Pages.Playlist;
            ViewStackPlaylist.SelectedIndex = (int)PlaylistPages.General;
            ViewStackPlaylistSubtitles.SelectedIndex = _discoveryContext.SubtitleLanguages.Any() ? 1 : 0;
            TxtPlaylistSaveFolder.Text = _controller.PreviousDownloadOptions.SaveFolder;
            CmbPlaylistFileType.ItemsSource = _discoveryContext.FileTypes;
            CmbPlaylistFileType.SelectSelectionItem();
            CmbPlaylistSuggestedVideoResolution.ItemsSource = _discoveryContext.VideoResolutions;
            CmbPlaylistSuggestedVideoResolution.SelectSelectionItem();
            CmbPlaylistSuggestedAudioBitrate.ItemsSource = _discoveryContext.AudioBitrates;
            CmbPlaylistSuggestedAudioBitrate.SelectSelectionItem();
            TglPlaylistReverseDownloadOrder.IsOn = _controller.PreviousDownloadOptions.ReverseDownloadOrder;
            TglPlaylistNumberTitles.IsOn = _controller.PreviousDownloadOptions.NumberTitles;
            ListPlaylistItems.ItemsSource = _discoveryContext.Items;
            ListPlaylistItems.SelectSelectionItems();
            ListPlaylistSubtitles.ItemsSource = _discoveryContext.SubtitleLanguages;
            ListPlaylistSubtitles.SelectSelectionItems();
            TxtPlaylistSubtitlesSearch.Text = string.Empty;
            TglPlaylistExportM3U.IsOn = _controller.PreviousDownloadOptions.ExportM3U;
            TglPlaylistSplitChapters.IsOn = _controller.PreviousDownloadOptions.SplitChapters;
            TglPlaylistExportDescription.IsOn = _controller.PreviousDownloadOptions.ExportDescription;
            CmbPlaylistPostProcessorArgument.ItemsSource = _controller.AvailablePostProcessorArguments;
            CmbPlaylistPostProcessorArgument.SelectSelectionItem();
            if (TglDownloadImmediately.IsOn)
            {
                await DownloadPlaylistAsync();
                Hide();
            }
        }
    }

    private async Task DownloadSingleAsync() => await _controller.AddSingleDownloadAsync(_discoveryContext!,
        TxtSingleSaveFilename.Text,
        TxtSingleSaveFolder.Text,
        (CmbSingleFileType.SelectedItem as SelectionItem<MediaFileType>)!,
        (CmbSingleVideoFormat.SelectedItem as SelectionItem<Format>)!,
        (CmbSingleAudioFormat.SelectedItem as SelectionItem<Format>)!,
        _discoveryContext!.SubtitleLanguages.Where(x => x.ShouldSelect),
        TglSingleSplitChapters.IsOn,
        TglSingleExportDescription.IsOn,
        TglSingleExcludeFromHistory.IsOn,
        (CmbSinglePostProcessorArgument.SelectedItem as SelectionItem<PostProcessorArgument?>)!,
        TxtSingleStartTime.Text,
        TxtSingleEndTime.Text
    );

    private async Task DownloadPlaylistAsync() => await _controller.AddPlaylistDownloadsAsync(_discoveryContext!,
        ListPlaylistItems.SelectedItems.Cast<MediaSelectionItem>(),
        TxtPlaylistSaveFolder.Text,
        (CmbPlaylistFileType.SelectedItem as SelectionItem<MediaFileType>)!,
        (CmbPlaylistSuggestedVideoResolution.SelectedItem as SelectionItem<VideoResolution>)!,
        (CmbPlaylistSuggestedAudioBitrate.SelectedItem as SelectionItem<double>)!,
        TglPlaylistReverseDownloadOrder.IsOn,
        TglPlaylistNumberTitles.IsOn,
        _discoveryContext!.SubtitleLanguages.Where(x => x.ShouldSelect),
        TglPlaylistExportM3U.IsOn,
        TglPlaylistSplitChapters.IsOn,
        TglPlaylistExportDescription.IsOn,
        TglPlaylistExcludeFromHistory.IsOn,
        (CmbPlaylistPostProcessorArgument.SelectedItem as SelectionItem<PostProcessorArgument?>)!
    );

    private void TxtUrl_TextChanged(object? sender, TextChangedEventArgs e) => IsPrimaryButtonEnabled = !TxtUrl.Text.StartsWith("//") && Uri.TryCreate(TxtUrl.Text, UriKind.Absolute, out var _);

    private async void BtnSelectBatchFile_Click(object? sender, RoutedEventArgs e)
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

    private void CmbCredential_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        var visibility = (CmbCredential.SelectedItem as SelectionItem<Credential?>)!.Value is null ? Visibility.Visible : Visibility.Collapsed;
        TxtUsername.Visibility = visibility;
        TxtPassword.Visibility = visibility;
    }

    private void TglDownloadImmediately_Toggled(object? sender, RoutedEventArgs e) => TeachDownloadImmediately.IsOpen = _controller.GetShouldShowDownloadImmediatelyTeach();

    private void NavViewSingle_SelectionChanged(SelectorBar sender, SelectorBarSelectionChangedEventArgs args)
    {
        ViewStackSingle.SelectedIndex = (NavViewSingle.SelectedItem.Tag as string)! switch
        {
            "Subtitles" => (int)SinglePages.Subtitles,
            "Advanced" => (int)SinglePages.Advanced,
            _ => (int)SinglePages.General
        };
    }

    private void BtnSingleRevertFilename_Click(object? sender, RoutedEventArgs e) => TxtSingleSaveFilename.Text = _discoveryContext!.Items[0].Label;

    private async void BtnSingleSelectSaveFolder_Click(object? sender, RoutedEventArgs e)
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

    private void CmbSingleFileType_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        var selectedFileType = (CmbSingleFileType.SelectedItem as SelectionItem<MediaFileType>)!;
        TeachSingleFileType.IsOpen = _controller.GetShouldShowFileTypeTeach(_discoveryContext!, selectedFileType);
        CmbSingleVideoFormat.SelectSelectionItemByFormatId(_controller.PreviousDownloadOptions.VideoFormatIds[selectedFileType.Value]);
        CmbSingleAudioFormat.SelectSelectionItemByFormatId(_controller.PreviousDownloadOptions.AudioFormatIds[selectedFileType.Value]);
    }

    private void BtnSingleSelectAllSubtitles_Click(object? sender, RoutedEventArgs e) => ListSingleSubtitles.SelectAll();

    private void BtnSingleDeselectAllSubtitles_Click(object? sender, RoutedEventArgs e) => ListSingleSubtitles.DeselectAll();

    private void NavViewPlaylist_SelectionChanged(SelectorBar sender, SelectorBarSelectionChangedEventArgs args)
    {
        ViewStackPlaylist.SelectedIndex = (NavViewPlaylist.SelectedItem.Tag as string)! switch
        {
            "Items" => (int)PlaylistPages.Items,
            "Subtitles" => (int)PlaylistPages.Subtitles,
            "Advanced" => (int)PlaylistPages.Advanced,
            _ => (int)PlaylistPages.General
        };
    }

    private async void BtnPlaylistSelectSaveFolder_Click(object? sender, RoutedEventArgs e)
    {
        var picker = new FolderPicker(_windowId)
        {
            SuggestedStartLocation = PickerLocationId.Downloads
        };
        var folder = await picker.PickSingleFolderAsync();
        if (folder is not null)
        {
            TxtPlaylistSaveFolder.Text = folder.Path;
        }
    }

    private void CmbPlaylistFileType_SelectionChanged(object? sender, SelectionChangedEventArgs e) => TeachPlaylistFileType.IsOpen = _controller.GetShouldShowFileTypeTeach(_discoveryContext!, (CmbPlaylistFileType.SelectedItem as SelectionItem<MediaFileType>)!);

    private void BtnPlaylistSelectAllItems_Click(object? sender, RoutedEventArgs e) => ListPlaylistItems.SelectAll();

    private void BtnPlaylistDeselectAllItems_Click(object? sender, RoutedEventArgs e) => ListPlaylistItems.DeselectAll();

    private void TglPlaylistNumberTitles_Toggled(object? sender, RoutedEventArgs e) => TeachPlaylistNumberTitles.IsOpen = _controller.GetShouldShowNumberTitlesTeach();

    private void BtnPlaylistRevertFilename_Click(object? sender, RoutedEventArgs e)
    {
        var index = (int)(sender as Button)!.Tag;
        if (ListPlaylistItems.ItemsSource is IReadOnlyList<MediaSelectionItem> items)
        {
            items[index].Filename = items[index].Label;
        }
    }

    private void BtnPlaylistSelectAllSubtitles_Click(object? sender, RoutedEventArgs e) => ListPlaylistSubtitles.SelectAll();

    private void BtnPlaylistDeselectAllSubtitles_Click(object? sender, RoutedEventArgs e) => ListPlaylistSubtitles.DeselectAll();

    private void TxtSingleSubtitlesSearch_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs e)
    {
        if (_discoveryContext is null)
        {
            return;
        }
        var searchText = TxtSingleSubtitlesSearch.Text.Trim().ToLower() ?? string.Empty;
        _isUpdatingSubtitleSelection = true;
        ListSingleSubtitles.ItemsSource = string.IsNullOrEmpty(searchText) ? _discoveryContext.SubtitleLanguages : _discoveryContext.SubtitleLanguages.Where(x => x.Value.Language.ToLower().Contains(searchText));
        ListSingleSubtitles.SelectSelectionItems();
        _isUpdatingSubtitleSelection = false;
    }

    private void TxtPlaylistSubtitlesSearch_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs e)
    {
        if (_discoveryContext is null)
        {
            return;
        }
        var searchText = TxtPlaylistSubtitlesSearch.Text.Trim().ToLower() ?? string.Empty;
        _isUpdatingSubtitleSelection = true;
        ListPlaylistSubtitles.ItemsSource = string.IsNullOrEmpty(searchText) ? _discoveryContext.SubtitleLanguages : _discoveryContext.SubtitleLanguages.Where(x => x.Value.Language.ToLower().Contains(searchText));
        ListPlaylistSubtitles.SelectSelectionItems();
        _isUpdatingSubtitleSelection = false;
    }

    private void ListSubtitles_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (_isUpdatingSubtitleSelection)
        {
            return;
        }
        foreach(var item in e.AddedItems)
        {
            (item as SelectionItem<SubtitleLanguage>)!.ShouldSelect = true;
        }
        foreach(var item in e.RemovedItems)
        {
            (item as SelectionItem<SubtitleLanguage>)!.ShouldSelect = false;
        }
    }
}
