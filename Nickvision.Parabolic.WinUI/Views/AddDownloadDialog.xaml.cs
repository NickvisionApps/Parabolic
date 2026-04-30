using CommunityToolkit.WinUI;
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.Windows.Storage.Pickers;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.WinUI.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.WinUI.Helpers;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Graphics.Imaging;

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
    private readonly ITranslationService _translationService;
    private DiscoveryContext? _discoveryContext;
    private bool _isUpdatingSubtitleSelection;

    public WindowId? WindowId { get; set; }

    public AddDownloadDialog(AddDownloadDialogController controller, ITranslationService translationService)
    {
        InitializeComponent();
        _controller = controller;
        _translationService = translationService;
        _discoveryContext = null;
        _isUpdatingSubtitleSelection = false;
        Title = _translationService._("Add Download");
        PrimaryButtonText = _translationService._("Discover");
        CloseButtonText = _translationService._("Cancel");
        DefaultButton = ContentDialogButton.Primary;
        IsPrimaryButtonEnabled = false;
        TxtUrl.Header = _translationService._("Media URL");
        TxtUrl.PlaceholderText = _translationService._("Enter media url here");
        LblSelectBatchFile.Text = _translationService._("Select Batch File");
        TglUseAuthentication.OnContent = _translationService._("Use Authentication");
        TglUseAuthentication.OffContent = _translationService._("Use Authentication");
        CmbCredential.Header = _translationService._("Credential");
        TxtUsername.Header = _translationService._("Username");
        TxtUsername.PlaceholderText = _translationService._("Enter username here");
        TxtPassword.Header = _translationService._("Password");
        TxtPassword.PlaceholderText = _translationService._("Enter password here");
        TglDownloadImmediatelyAsVideo.OnContent = _translationService._("Download Immediately as Video");
        TglDownloadImmediatelyAsVideo.OffContent = _translationService._("Download Immediately as Video");
        TglDownloadImmediatelyAsAudio.OnContent = _translationService._("Download Immediately as Audio");
        TglDownloadImmediatelyAsAudio.OffContent = _translationService._("Download Immediately as Audio");
        TeachDownloadImmediately.Title = _translationService._("Warning");
        TeachDownloadImmediately.Subtitle = _translationService._("Parabolic will download media based off of previously configured options and sensible defaults. Options including save folder, format, and subtitle selection will not be shown.");
        LblLoading.Text = _translationService._("This may take some time...");
        NavViewItemSingleGeneral.Text = _translationService._("General");
        NavViewItemSingleSubtitles.Text = _translationService._("Subtitles");
        NavViewItemSingleAdvanced.Text = _translationService._("Advanced");
        TxtSingleSaveFilename.Header = _translationService._("File Name");
        ToolTipService.SetToolTip(BtnSingleRevertFilename, _translationService._("Revert to Title"));
        TxtSingleSaveFolder.Header = _translationService._("Save Folder");
        ToolTipService.SetToolTip(BtnSingleSelectSaveFolder, _translationService._("Select Save Folder"));
        CmbSingleFileType.Header = _translationService._("File Type");
        TeachSingleFileType.Title = _translationService._("Warning");
        TeachSingleFileType.Subtitle = _translationService._("Generic types do not support embedding thumbnails and subtitles. If necessary, please select a specific type that is known to support embedding to prevent separate files from being written.");
        CmbSingleVideoFormat.Header = _translationService._("Video Format");
        CmbSingleAudioFormat.Header = _translationService._("Audio Format");
        StatusSingleSubtitles.Title = _translationService._("No Subtitles");
        StatusSingleSubtitles.Description = _translationService._("No subtitles were found for this media.");
        LblSingleSelectAllSubtitles.Text = _translationService._("Select All");
        LblSingleDeselectAllSubtitles.Text = _translationService._("Deselect All");
        TxtSingleSubtitlesSearch.PlaceholderText = _translationService._("Search subtitles");
        TglSingleSplitChapters.OnContent = _translationService._("Split into Files by Chapters");
        TglSingleSplitChapters.OffContent = _translationService._("Split into Files by Chapters");
        TglSingleExportDescription.OnContent = _translationService._("Export Description to File");
        TglSingleExportDescription.OffContent = _translationService._("Export Description to File");
        TglSingleExcludeFromHistory.OnContent = _translationService._("Exclude from History");
        TglSingleExcludeFromHistory.OffContent = _translationService._("Exclude from History");
        CmbSinglePostProcessorArgument.Header = _translationService._("Post Processor Argument");
        TxtSingleStartTime.Header = _translationService._("Start Time");
        TxtSingleEndTime.Header = _translationService._("End Time");
        NavViewItemPlaylistGeneral.Text = _translationService._("General");
        NavViewItemPlaylistItems.Text = _translationService._("Items");
        NavViewItemPlaylistSubtitles.Text = _translationService._("Subtitles");
        NavViewItemPlaylistAdvanced.Text = _translationService._("Advanced");
        TxtPlaylistSaveFolder.Header = _translationService._("Save Folder");
        ToolTipService.SetToolTip(BtnPlaylistSelectSaveFolder, _translationService._("Select Save Folder"));
        CmbPlaylistFileType.Header = _translationService._("File Type");
        TeachPlaylistFileType.Title = _translationService._("Warning");
        TeachPlaylistFileType.Subtitle = _translationService._("Generic types do not support embedding thumbnails and subtitles. If necessary, please select a specific type that is known to support embedding to prevent separate files from being written.");
        CmbPlaylistSuggestedVideoResolution.Header = _translationService._("Suggested Video Resolution");
        CmbPlaylistSuggestedAudioBitrate.Header = _translationService._("Suggested Audio Bitrate");
        LblPlaylistSelectAllItems.Text = _translationService._("Select All");
        LblPlaylistDeselectAllItems.Text = _translationService._("Deselect All");
        TglPlaylistReverseDownloadOrder.OnContent = _translationService._("Reverse Download Order");
        TglPlaylistReverseDownloadOrder.OffContent = _translationService._("Reverse Download Order");
        TglPlaylistNumberTitles.OnContent = _translationService._("Number Titles");
        TglPlaylistNumberTitles.OffContent = _translationService._("Number Titles");
        TeachPlaylistNumberTitles.Title = _translationService._("Warning");
        TeachPlaylistNumberTitles.Subtitle = _translationService._("Numbering will be applied to titles of selected items in succession on download.");
        StatusPlaylistSubtitles.Title = _translationService._("No Subtitles");
        StatusPlaylistSubtitles.Description = _translationService._("No subtitles were found in this playlist.");
        LblPlaylistSelectAllSubtitles.Text = _translationService._("Select All");
        LblPlaylistDeselectAllSubtitles.Text = _translationService._("Deselect All");
        LblPlaylistSubtitleNote.Text = _translationService._("Note: Some playlist items may not contain subtitles for a selected language.");
        TxtPlaylistSubtitlesSearch.PlaceholderText = _translationService._("Search subtitles");
        TglPlaylistExportM3U.OnContent = _translationService._("Export M3U Playlist File");
        TglPlaylistExportM3U.OffContent = _translationService._("Export M3U Playlist File");
        TglPlaylistSplitChapters.OnContent = _translationService._("Split into Files by Chapters");
        TglPlaylistSplitChapters.OffContent = _translationService._("Split into Files by Chapters");
        TglPlaylistExportDescription.OnContent = _translationService._("Export Description to File");
        TglPlaylistExportDescription.OffContent = _translationService._("Export Description to File");
        TglPlaylistExcludeFromHistory.OnContent = _translationService._("Exclude from History");
        TglPlaylistExcludeFromHistory.OffContent = _translationService._("Exclude from History");
        CmbPlaylistPostProcessorArgument.Header = _translationService._("Post Processor Argument");
    }

    public async new Task<ContentDialogResult> ShowAsync()
    {
        CmbCredential.ItemsSource = (await _controller.GetAvailableCredentialsAsync()).ToBindableSelectonItems();
        CmbCredential.SelectSelectionItem();
        ViewStack.SelectedIndex = (int)Pages.Discover;
        TglDownloadImmediatelyAsVideo.IsOn = _controller.PreviousDownloadImmediatelyAsVideo;
        TglDownloadImmediatelyAsAudio.IsOn = _controller.PreviousDownloadImmediatelyAsAudio;
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
        Title = _translationService._("Discovering Media");
        PrimaryButtonText = null;
        CloseButtonText = _translationService._("Cancel");
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
            credential = (CmbCredential.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Credential?>()!.Value;
        }
        _controller.PreviousDownloadImmediatelyAsVideo = TglDownloadImmediatelyAsVideo.IsOn;
        _controller.PreviousDownloadImmediatelyAsAudio = TglDownloadImmediatelyAsAudio.IsOn;
        _discoveryContext = await _controller.DiscoverAsync(new Uri(TxtUrl.Text), credential, cancellationToken);
        if (_discoveryContext is null)
        {
            Hide();
            return;
        }
        using var thumbnailMemoryStream = await _controller.GetThumbnailImageStreamAsync(_discoveryContext);
        using var thumbnailStream = thumbnailMemoryStream.AsRandomAccessStream();
        var thumbnailDecoder = await BitmapDecoder.CreateAsync(thumbnailStream);
        var thumbnailBitmap = await thumbnailDecoder.GetSoftwareBitmapAsync(BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
        var thumbnailSource = new SoftwareBitmapSource();
        await thumbnailSource.SetBitmapAsync(thumbnailBitmap);
        Title = _translationService._("Configure Download");
        PrimaryButtonText = _translationService._("Download");
        CloseButtonText = _translationService._("Cancel");
        SecondaryButtonText = null;
        DefaultButton = ContentDialogButton.Primary;
        if (_discoveryContext.Items.Count == 1)
        {
            ViewStack.SelectedIndex = (int)Pages.Single;
            ViewStackSingle.SelectedIndex = (int)SinglePages.General;
            ViewStackSingleSubtitles.SelectedIndex = _discoveryContext.SubtitleLanguages.Any() ? 1 : 0;
            ImgSingleThumbnail.Source = thumbnailSource;
            LblSingleTitle.Text = _discoveryContext.Title;
            LblSingleUrl.Text = _discoveryContext.Url.ToString();
            TxtSingleSaveFilename.Text = _discoveryContext.Items[0].Label;
            TxtSingleSaveFolder.Text = _controller.PreviousSaveFolder;
            CmbSingleVideoFormat.ItemsSource = _discoveryContext.VideoFormats.ToBindableSelectonItems();
            CmbSingleAudioFormat.ItemsSource = _discoveryContext.AudioFormats.ToBindableSelectonItems();
            CmbSingleFileType.ItemsSource = _discoveryContext.FileTypes.ToBindableSelectonItems();
            CmbSingleFileType.SelectSelectionItem();
            ListSingleSubtitles.ItemsSource = _discoveryContext.SubtitleLanguages.ToBindableSelectonItems();
            ListSingleSubtitles.SelectSelectionItems();
            TxtSingleSubtitlesSearch.Text = string.Empty;
            TglSingleSplitChapters.IsOn = _controller.PreviousSplitChapters;
            TglSingleExportDescription.IsOn = _controller.PreviousExportDescription;
            CmbSinglePostProcessorArgument.ItemsSource = _controller.GetAvailablePostProcessorArguments().ToBindableSelectonItems();
            CmbSinglePostProcessorArgument.SelectSelectionItem();
            TxtSingleStartTime.PlaceholderText = _discoveryContext.Items[0].StartTime;
            TxtSingleStartTime.Text = _discoveryContext.Items[0].StartTime;
            TxtSingleEndTime.PlaceholderText = _discoveryContext.Items[0].EndTime;
            TxtSingleEndTime.Text = _discoveryContext.Items[0].EndTime;
            if (TglDownloadImmediatelyAsVideo.IsOn || TglDownloadImmediatelyAsAudio.IsOn)
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
            ImgPlaylistThumbnail.Source = thumbnailSource;
            LblPlaylistTitle.Text = _discoveryContext.Title;
            LblPlaylistUrl.Text = _discoveryContext.Url.ToString();
            TxtPlaylistSaveFolder.Text = _controller.PreviousSaveFolder;
            CmbPlaylistFileType.ItemsSource = _discoveryContext.FileTypes.ToBindableSelectonItems();
            CmbPlaylistFileType.SelectSelectionItem();
            CmbPlaylistSuggestedVideoResolution.ItemsSource = _discoveryContext.VideoResolutions.ToBindableSelectonItems();
            CmbPlaylistSuggestedVideoResolution.SelectSelectionItem();
            CmbPlaylistSuggestedAudioBitrate.ItemsSource = _discoveryContext.AudioBitrates.ToBindableSelectonItems();
            CmbPlaylistSuggestedAudioBitrate.SelectSelectionItem();
            LblPlaylistItemsTime.Text = _translationService._("Total Duration: {0}", _discoveryContext.TotalDuration);
            TglPlaylistReverseDownloadOrder.IsOn = _controller.PreviousReverseDownloadOrder;
            TglPlaylistNumberTitles.IsOn = _controller.PreviousNumberTitles;
            ListPlaylistItems.ItemsSource = _discoveryContext.Items.ToBindableMediaSelectionItems();
            ListPlaylistItems.SelectMediaSelectionItems();
            ListPlaylistSubtitles.ItemsSource = _discoveryContext.SubtitleLanguages.ToBindableSelectonItems();
            ListPlaylistSubtitles.SelectSelectionItems();
            TxtPlaylistSubtitlesSearch.Text = string.Empty;
            TglPlaylistExportM3U.IsOn = _controller.PreviousExportM3U;
            TglPlaylistSplitChapters.IsOn = _controller.PreviousSplitChapters;
            TglPlaylistExportDescription.IsOn = _controller.PreviousExportDescription;
            CmbPlaylistPostProcessorArgument.ItemsSource = _controller.GetAvailablePostProcessorArguments().ToBindableSelectonItems();
            CmbPlaylistPostProcessorArgument.SelectSelectionItem();
            if (TglDownloadImmediatelyAsVideo.IsOn || TglDownloadImmediatelyAsAudio.IsOn)
            {
                await DownloadPlaylistAsync();
                Hide();
            }
        }
    }

    private Task DownloadSingleAsync() => _controller.AddSingleDownloadAsync(_discoveryContext!,
        TxtSingleSaveFilename.Text,
        TxtSingleSaveFolder.Text,
        (CmbSingleFileType.SelectedItem as BindableSelectionItem)!.ToSelectionItem<MediaFileType>()!,
        (CmbSingleVideoFormat.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Format>()!,
        (CmbSingleAudioFormat.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Format>()!,
        _discoveryContext!.SubtitleLanguages.Where(x => x.ShouldSelect),
        TglSingleSplitChapters.IsOn,
        TglSingleExportDescription.IsOn,
        TglSingleExcludeFromHistory.IsOn,
        (CmbSinglePostProcessorArgument.SelectedItem as BindableSelectionItem)!.ToSelectionItem<PostProcessorArgument?>()!,
        TxtSingleStartTime.Text,
        TxtSingleEndTime.Text
    );

    private async Task DownloadPlaylistAsync()
    {
        var selectedPlaylistItems = new List<MediaSelectionItem>();
        foreach (var item in ListPlaylistItems.SelectedItems)
        {
            selectedPlaylistItems.Add((item as BindableMediaSelectionItem)!.SelectionItem);
        }
        await _controller.AddPlaylistDownloadsAsync(_discoveryContext!,
            selectedPlaylistItems,
            TxtPlaylistSaveFolder.Text,
            (CmbPlaylistFileType.SelectedItem as BindableSelectionItem)!.ToSelectionItem<MediaFileType>()!,
            (CmbPlaylistSuggestedVideoResolution.SelectedItem as BindableSelectionItem)!.ToSelectionItem<VideoResolution>()!,
            (CmbPlaylistSuggestedAudioBitrate.SelectedItem as BindableSelectionItem)!.ToSelectionItem<double>()!,
            TglPlaylistReverseDownloadOrder.IsOn,
            TglPlaylistNumberTitles.IsOn,
            _discoveryContext!.SubtitleLanguages.Where(x => x.ShouldSelect),
            TglPlaylistExportM3U.IsOn,
            TglPlaylistSplitChapters.IsOn,
            TglPlaylistExportDescription.IsOn,
            TglPlaylistExcludeFromHistory.IsOn,
            (CmbPlaylistPostProcessorArgument.SelectedItem as BindableSelectionItem)!.ToSelectionItem<PostProcessorArgument?>()!);
    }

    private void TxtUrl_TextChanged(object? sender, TextChangedEventArgs e) => IsPrimaryButtonEnabled = !TxtUrl.Text.StartsWith("//") && Uri.TryCreate(TxtUrl.Text, UriKind.Absolute, out var _);

    private async void BtnSelectBatchFile_Click(object? sender, RoutedEventArgs e)
    {
        var picker = new FileOpenPicker(WindowId!.Value)
        {
            SuggestedStartLocation = PickerLocationId.DocumentsLibrary,
            FileTypeFilter = { ".txt" }
        };
        var file = await picker.PickSingleFileAsync();
        if (file is not null)
        {
            TxtUrl.Text = new Uri(file.Path).ToString();
        }
    }

    private void CmbCredential_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        var visibility = (CmbCredential.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Credential?>()!.Value is null ? Visibility.Visible : Visibility.Collapsed;
        TxtUsername.Visibility = visibility;
        TxtPassword.Visibility = visibility;
    }

    private void TglDownloadImmediatelyAsVideo_Toggled(object? sender, RoutedEventArgs e)
    {
        if (TglDownloadImmediatelyAsVideo.IsOn)
        {
            TglDownloadImmediatelyAsAudio.IsOn = false;
            TeachDownloadImmediately.Target = TglDownloadImmediatelyAsVideo;
            TeachDownloadImmediately.IsOpen = _controller.GetShouldShowDownloadImmediatelyTeach();
        }
    }

    private void TglDownloadImmediatelyAsAudio_Toggled(object? sender, RoutedEventArgs e)
    {
        if (TglDownloadImmediatelyAsAudio.IsOn)
        {
            TglDownloadImmediatelyAsVideo.IsOn = false;
            TeachDownloadImmediately.Target = TglDownloadImmediatelyAsAudio;
            TeachDownloadImmediately.IsOpen = _controller.GetShouldShowDownloadImmediatelyTeach();
        }
    }

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
        var picker = new FolderPicker(WindowId!.Value)
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
        var selectedFileType = (CmbSingleFileType.SelectedItem as BindableSelectionItem)!.ToSelectionItem<MediaFileType>()!;
        TeachSingleFileType.IsOpen = _controller.GetShouldShowFileTypeTeach(_discoveryContext!, selectedFileType);
        CmbSingleVideoFormat.SelectSelectionItemByFormatId(_controller.PreviousVideoFormatIds[selectedFileType.Value]);
        CmbSingleAudioFormat.SelectSelectionItemByFormatId(_controller.PreviousAudioFormatIds[selectedFileType.Value]);
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
        var picker = new FolderPicker(WindowId!.Value)
        {
            SuggestedStartLocation = PickerLocationId.Downloads
        };
        var folder = await picker.PickSingleFolderAsync();
        if (folder is not null)
        {
            TxtPlaylistSaveFolder.Text = folder.Path;
        }
    }

    private void CmbPlaylistFileType_SelectionChanged(object? sender, SelectionChangedEventArgs e) => TeachPlaylistFileType.IsOpen = _controller.GetShouldShowFileTypeTeach(_discoveryContext!, (CmbPlaylistFileType.SelectedItem as BindableSelectionItem)!.ToSelectionItem<MediaFileType>()!);

    private void BtnPlaylistSelectAllItems_Click(object? sender, RoutedEventArgs e) => ListPlaylistItems.SelectAll();

    private void BtnPlaylistDeselectAllItems_Click(object? sender, RoutedEventArgs e) => ListPlaylistItems.DeselectAll();

    private void TglPlaylistNumberTitles_Toggled(object? sender, RoutedEventArgs e) => TeachPlaylistNumberTitles.IsOpen = _controller.GetShouldShowNumberTitlesTeach();

    private void BtnPlaylistRevertFilename_Click(object? sender, RoutedEventArgs e)
    {
        var index = (int)(sender as Button)!.Tag;
        if (ListPlaylistItems.ItemsSource is IReadOnlyList<BindableMediaSelectionItem> items)
        {
            items[index].Filename = items[index].Label;
        }
    }

    private void ListPlaylistItems_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        var totalDuration = TimeSpan.Zero;
        foreach (var selectedItem in ListPlaylistItems.SelectedItems)
        {
            totalDuration += ((BindableMediaSelectionItem)selectedItem).Duration;
        }
        LblPlaylistItemsTime.Text = _translationService._("Total Duration: {0}", totalDuration);
    }

    private void BtnPlaylistSelectAllSubtitles_Click(object? sender, RoutedEventArgs e) => ListPlaylistSubtitles.SelectAll();

    private void BtnPlaylistDeselectAllSubtitles_Click(object? sender, RoutedEventArgs e) => ListPlaylistSubtitles.DeselectAll();

    private void TxtSingleSubtitlesSearch_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs e)
    {
        if (_discoveryContext is null)
        {
            return;
        }
        var searchText = TxtSingleSubtitlesSearch.Text.Trim();
        _isUpdatingSubtitleSelection = true;
        ListSingleSubtitles.ItemsSource = string.IsNullOrEmpty(searchText) ? _discoveryContext.SubtitleLanguages.ToBindableSelectonItems() : _discoveryContext.SubtitleLanguages.Where(x => x.Value.Language.Contains(searchText, StringComparison.OrdinalIgnoreCase)).ToBindableSelectonItems();
        ListSingleSubtitles.SelectSelectionItems();
        _isUpdatingSubtitleSelection = false;
    }

    private void TxtPlaylistSubtitlesSearch_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs e)
    {
        if (_discoveryContext is null)
        {
            return;
        }
        var searchText = TxtPlaylistSubtitlesSearch.Text.Trim();
        _isUpdatingSubtitleSelection = true;
        ListPlaylistSubtitles.ItemsSource = string.IsNullOrEmpty(searchText) ? _discoveryContext.SubtitleLanguages.ToBindableSelectonItems() : _discoveryContext.SubtitleLanguages.Where(x => x.Value.Language.Contains(searchText, StringComparison.OrdinalIgnoreCase)).ToBindableSelectonItems();
        ListPlaylistSubtitles.SelectSelectionItems();
        _isUpdatingSubtitleSelection = false;
    }

    private void ListSubtitles_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (_isUpdatingSubtitleSelection)
        {
            return;
        }
        foreach (var item in e.AddedItems)
        {
            (item as BindableSelectionItem)!.ShouldSelect = true;
        }
        foreach (var item in e.RemovedItems)
        {
            (item as BindableSelectionItem)!.ShouldSelect = false;
        }
    }
}
