using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.Storage.Pickers;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.WinUI.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.WinUI.Helpers;
using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class SettingsPage : Page
{
    private readonly PreferencesViewController _controller;
    private readonly ITranslationService _translationService;
    private bool _constructing;

    public WindowId? WindowId { get; set; }

    public SettingsPage(PreferencesViewController controller, ITranslationService translationService)
    {
        InitializeComponent();
        _controller = controller;
        _translationService = translationService;
        _constructing = true;
        // Translations
        LblSettings.Text = _translationService._("Settings");
        SelectorUI.Text = _translationService._("User Interface");
        SelectorDownloads.Text = _translationService._("Downloads");
        SelectorDownloader.Text = _translationService._("Downloader");
        SelectorConverter.Text = _translationService._("Converter");
        RowTheme.Header = _translationService._("Theme");
        CmbTheme.ItemsSource = _controller.Themes.ToBindableSelectonItems();
        RowTranslationLanguage.Header = _translationService._("Translation Language");
        RowTranslationLanguage.Description = _translationService._("An application restart is required for a change to take effect");
        CmbTranslationLanguage.ItemsSource = _controller.AvailableTranslationLanguages.ToBindableSelectonItems();
        RowPreviewUpdates.Header = _translationService._("Receive Preview Updates");
        RowPreviewUpdates.Description = _translationService._("Update Parabolic and dependencies, such as yt-dlp, to beta versions");
        TglPreviewUpdates.OnContent = _translationService._("On");
        TglPreviewUpdates.OffContent = _translationService._("Off");
        RowPreventSuspend.Header = _translationService._("Prevent Suspend");
        RowPreventSuspend.Description = _translationService._("Prevent the computer from sleeping while downloads are running");
        TglPreventSuspend.OnContent = _translationService._("On");
        TglPreventSuspend.OffContent = _translationService._("Off");
        RowHistoryLength.Header = _translationService._("Download History Length");
        RowHistoryLength.Description = _translationService._("The amount of time to keep past downloads in the app's history");
        CmbHistoryLength.ItemsSource = _controller.HistoryLengths.ToBindableSelectonItems();
        RowActiveDownloads.Header = _translationService._("Max Number of Active Downloads");
        RowOverwriteFiles.Header = _translationService._("Overwrite Existing Files");
        TglOverwriteFiles.OnContent = _translationService._("On");
        TglOverwriteFiles.OffContent = _translationService._("Off");
        RowIncludeMediaId.Header = _translationService._("Include Media Id in Title");
        RowIncludeMediaId.Description = _translationService._("Add the media's id to its default title");
        TglIncludeMediaId.OnContent = _translationService._("On");
        TglIncludeMediaId.OffContent = _translationService._("Off");
        RowIncludeAutoSubtitles.Header = _translationService._("Include Auto-Generated Subtitles");
        RowIncludeAutoSubtitles.Description = _translationService._("Show auto-generated subtitles to download in addition to available subtitles");
        TglIncludeAutoSubtitles.OnContent = _translationService._("On");
        TglIncludeAutoSubtitles.OffContent = _translationService._("Off");
        RowIncludeSuperResolutions.Header = _translationService._("Include Super Resolution Formats");
        RowIncludeSuperResolutions.Description = _translationService._("Show super (AI-scaled) resolution formats to download in addition to regular formats");
        TglIncludeSuperResolutions.OnContent = _translationService._("On");
        TglIncludeSuperResolutions.OffContent = _translationService._("Off");
        RowPreferredVideoCodec.Header = _translationService._("Preferred Video Codec");
        RowPreferredVideoCodec.Description = _translationService._("Prefer this codec when parsing video formats to show available to download");
        CmbPreferredVideoCodec.ItemsSource = _controller.VideoCodecs.ToBindableSelectonItems();
        RowPreferredAudioCodec.Header = _translationService._("Preferred Audio Codec");
        RowPreferredAudioCodec.Description = _translationService._("Prefer this codec when parsing audio formats to show available to download");
        CmbPreferredAudioCodec.ItemsSource = _controller.AudioCodecs.ToBindableSelectonItems();
        RowPreferredSubtitleFormat.Header = _translationService._("Preferred Subtitle Format");
        RowPreferredSubtitleFormat.Description = _translationService._("Prefer this subtitle file format when downloading");
        CmbPreferredSubtitleFormat.ItemsSource = _controller.SubtitleFormats.ToBindableSelectonItems();
        RowPreferredFrameRate.Header = _translationService._("Preferred Frame Rate");
        RowPreferredFrameRate.Description = _translationService._("Prefer this frame rate when parsing video formats to show available to download");
        CmbPreferredFrameRate.ItemsSource = _controller.FrameRates.ToBindableSelectonItems();
        RowUsePartFiles.Header = _translationService._("Use Part Files");
        RowUsePartFiles.Description = _translationService._("Download media in separate .part files instead of directly into the output file");
        TglUsePartFiles.OnContent = _translationService._("On");
        TglUsePartFiles.OffContent = _translationService._("Off");
        RowUseSponsorBlock.Header = _translationService._("Use SponsorBlock for YouTube");
        RowUseSponsorBlock.Description = _translationService._("Try to remove sponsored segments from videos");
        TglUseSponsorBlock.OnContent = _translationService._("On");
        TglUseSponsorBlock.OffContent = _translationService._("Off");
        RowLimitSpeed.Header = _translationService._("Limit Download Speed");
        TglLimitSpeed.OnContent = _translationService._("On");
        TglLimitSpeed.OffContent = _translationService._("Off");
        RowSpeedLimit.Header = _translationService._("Speed Limit");
        RowProxyUrl.Header = _translationService._("Proxy URL");
        TxtProxyUrl.PlaceholderText = _translationService._("Enter proxy url here");
        RowCookiesFile.Header = _translationService._("Cookies from File");
        RowCookiesFile.Description = _translationService._("Upload a txt cookies file from unlisted browsers");
        LblCookiesFile.Text = _translationService._("No file selected");
        ToolTipService.SetToolTip(BtnClearCookiesFile, _translationService._("Clear Cookies File"));
        ToolTipService.SetToolTip(BtnSelectCookiesFile, _translationService._("Select Cookies File"));
        RowCookiesBrowser.Header = _translationService._("Cookies from Browser");
        CmbCookiesBrowser.ItemsSource = _controller.Browsers.ToBindableSelectonItems();
        LblAria.Text = _translationService._("aria2c");
        RowUseAria.Header = _translationService._("Use aria2c");
        RowUseAria.Description = _translationService._("An alternative downloader that may be faster in some regions compared to yt-dlp's native downloader");
        TglUseAria.OnContent = _translationService._("On");
        TglUseAria.OffContent = _translationService._("Off");
        RowMaxConnectionsPerServer.Header = _translationService._("Max Connections Per Server");
        RowMaxConnectionsPerServer.Description = _translationService._("Corresponds to -x option");
        RowMinimumSplitSize.Header = _translationService._("Minimum Split Size (MiB)");
        RowMinimumSplitSize.Description = _translationService._("Corresponds to -k option");
        LblAdvanced.Text = _translationService._("Advanced");
        RowYtdlpDiscoveryArgs.Header = _translationService._("yt-dlp Discovery Arguments");
        RowYtdlpDiscoveryArgs.Description = _translationService._("Extra arguments to pass to yt-dlp when discovering media");
        TxtYtdlpDiscoveryArgs.PlaceholderText = _translationService._("Enter args here");
        RowYtdlpDownloadArgs.Header = _translationService._("yt-dlp Download Arguments");
        RowYtdlpDownloadArgs.Description = _translationService._("Extra arguments to pass to yt-dlp when downloading media");
        TxtYtdlpDownloadArgs.PlaceholderText = _translationService._("Enter args here");
        RowTranslateMetadataAndChapters.Header = _translationService._("Translate Metadata and Chapters");
        RowTranslateMetadataAndChapters.Description = _translationService._("Automatically translate embedded metadata and chapters to the app's language on supported sites and media");
        TglTranslateMetadataAndChapters.OnContent = _translationService._("On");
        TglTranslateMetadataAndChapters.OffContent = _translationService._("Off");
        RowEmbedMetadata.Header = _translationService._("Embed Metadata");
        TglEmbedMetadata.OnContent = _translationService._("On");
        TglEmbedMetadata.OffContent = _translationService._("Off");
        RowRemoveSourceData.Header = _translationService._("Remove Source Data");
        RowRemoveSourceData.Description = _translationService._("Clear metadata fields containing identifying download information");
        TglRemoveSourceData.OnContent = _translationService._("On");
        TglRemoveSourceData.OffContent = _translationService._("Off");
        RowEmbedThumbnails.Header = _translationService._("Embed Thumbnails");
        RowEmbedThumbnails.Description = _translationService._("If the file type does not support embedding, the thumbnail will be written to a separate image file");
        TglEmbedThumbnails.OnContent = _translationService._("On");
        TglEmbedThumbnails.OffContent = _translationService._("Off");
        RowCropAudioThumbnails.Header = _translationService._("Crop Audio Thumbnails");
        RowCropAudioThumbnails.Description = _translationService._("Crop thumbnails of audio files to squares");
        TglCropAudioThumbnails.OnContent = _translationService._("On");
        TglCropAudioThumbnails.OffContent = _translationService._("Off");
        RowEmbedChapters.Header = _translationService._("Embed Chapters");
        TglEmbedChapters.OnContent = _translationService._("On");
        TglEmbedChapters.OffContent = _translationService._("Off");
        RowEmbedSubtitles.Header = _translationService._("Embed Subtitles");
        RowEmbedSubtitles.Description = _translationService._("If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files");
        TglEmbedSubtitles.OnContent = _translationService._("On");
        TglEmbedSubtitles.OffContent = _translationService._("Off");
        RowFfmpegThreads.Header = _translationService._("FFmpeg Threads");
        RowFfmpegThreads.Description = _translationService._("Limit the number of threads used by ffmpeg");
        NumFfmpegThreads.Maximum = Environment.ProcessorCount;
        RowPostProcessorArguments.Header = _translationService._("Post-Processor Arguments");
        RowPostProcessorArguments.Description = _translationService._("Arguments will be shown for selection in the add download dialog");
        LblAddPostProcessorArgument.Text = _translationService._("Add");
        DlgPostprocessingArgument.Title = _translationService._("Argument");
        DlgPostprocessingArgument.CloseButtonText = _translationService._("Cancel");
        TxtPostprocessingArgumentName.Header = _translationService._("Name");
        TxtPostprocessingArgumentName.PlaceholderText = _translationService._("Enter name here");
        CmbPostprocessingArgumentPostProcessor.Header = _translationService._("Post-Processor");
        CmbPostprocessingArgumentExecutable.Header = _translationService._("Executable");
        TxtPostprocessingArgumentArgs.Header = _translationService._("Args");
        TxtPostprocessingArgumentArgs.PlaceholderText = _translationService._("Enter args here");
    }

    private void Page_Loaded(object sender, RoutedEventArgs e)
    {
        CmbTheme.SelectSelectionItem();
        CmbTranslationLanguage.SelectSelectionItem();
        TglPreviewUpdates.IsOn = _controller.AllowPreviewUpdates;
        TglPreventSuspend.IsOn = _controller.PreventSuspend;
        CmbHistoryLength.SelectSelectionItem();
        NumActiveDownloads.Value = _controller.MaxNumberOfActiveDownloads;
        TglOverwriteFiles.IsOn = _controller.OverwriteExistingFiles;
        TglIncludeMediaId.IsOn = _controller.IncludeMediaIdInTitle;
        TglIncludeAutoSubtitles.IsOn = _controller.IncludeAutoGeneratedSubtitles;
        TglIncludeSuperResolutions.IsOn = _controller.IncludeSuperResolutions;
        CmbPreferredVideoCodec.SelectSelectionItem();
        CmbPreferredAudioCodec.SelectSelectionItem();
        CmbPreferredSubtitleFormat.SelectSelectionItem();
        CmbPreferredFrameRate.SelectSelectionItem();
        TglUsePartFiles.IsOn = _controller.UsePartFiles;
        TglUseSponsorBlock.IsOn = _controller.YouTubeSponsorBlock;
        TglLimitSpeed.IsOn = _controller.SpeedLimit.HasValue;
        NumSpeedLimit.Value = _controller.SpeedLimit ?? 1024;
        TxtProxyUrl.Text = _controller.ProxyUrl;
        LblCookiesFile.Text = !File.Exists(_controller.CookiesPath) ? _translationService._("No file selected") : _controller.CookiesPath;
        CmbCookiesBrowser.SelectSelectionItem();
        TglUseAria.IsOn = _controller.UseAria;
        NumMaxConnectionsPerServer.Value = _controller.AriaMaxConnectionsPerServer;
        NumMinimumSplitSize.Value = _controller.AriaMinSplitSize;
        TxtYtdlpDiscoveryArgs.Text = _controller.YtdlpDiscoveryArgs;
        TxtYtdlpDownloadArgs.Text = _controller.YtdlpDownloadArgs;
        TglTranslateMetadataAndChapters.IsOn = _controller.TranslateMetadataAndChapters;
        TglEmbedMetadata.IsOn = _controller.EmbedMetadata;
        TglRemoveSourceData.IsOn = _controller.RemoveSourceData;
        TglEmbedThumbnails.IsOn = _controller.EmbedThumbnails;
        TglCropAudioThumbnails.IsOn = _controller.CropAudioThumbnails;
        TglEmbedChapters.IsOn = _controller.EmbedChapters;
        TglEmbedSubtitles.IsOn = _controller.EmbedSubtitles;
        NumFfmpegThreads.Value = _controller.PostprocessingThreads;
        RowPostProcessorArguments.ItemsSource = _controller.PostprocessingArguments.ToBindablePostProcessorArguments();
        CmbPostprocessingArgumentPostProcessor.ItemsSource = _controller.PostProcessors.ToBindableSelectonItems();
        CmbPostprocessingArgumentExecutable.ItemsSource = _controller.Executables.ToBindableSelectonItems();
        _constructing = false;
    }

    private async void AddPostprocessingArgument(object? sender, RoutedEventArgs e)
    {
        TxtPostprocessingArgumentName.IsReadOnly = false;
        TxtPostprocessingArgumentName.Text = string.Empty;
        CmbPostprocessingArgumentPostProcessor.SelectSelectionItem();
        CmbPostprocessingArgumentExecutable.SelectSelectionItem();
        TxtPostprocessingArgumentArgs.Text = string.Empty;
        DlgPostprocessingArgument.PrimaryButtonText = _translationService._("Add");
        DlgPostprocessingArgument.XamlRoot = XamlRoot;
        DlgPostprocessingArgument.RequestedTheme = ActualTheme;
        string? error = null;
        do
        {
            if ((await DlgPostprocessingArgument.ShowAsync()) == ContentDialogResult.Primary)
            {
                error = await _controller.AddPostprocessingArgumentAsync(TxtPostprocessingArgumentName.Text,
                    (CmbPostprocessingArgumentPostProcessor.SelectedItem as BindableSelectionItem)!.ToSelectionItem<PostProcessor>()!,
                    (CmbPostprocessingArgumentExecutable.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Executable>()!,
                    TxtPostprocessingArgumentArgs.Text);
                if (error is not null)
                {
                    var errorDialog = new ContentDialog()
                    {
                        Title = _translationService._("Error"),
                        Content = error,
                        CloseButtonText = _translationService._("OK"),
                        DefaultButton = ContentDialogButton.Close,
                        XamlRoot = XamlRoot,
                        RequestedTheme = ActualTheme
                    };
                    await errorDialog.ShowAsync();
                }
            }
        } while (error is not null);
    }

    private async void ClearCookiesFile(object? sender, RoutedEventArgs e)
    {
        LblCookiesFile.Text = _translationService._("No file selected");
        await ApplyChangesAsync();
    }

    private async void DeletePostprocessingArgument(object? sender, RoutedEventArgs e)
    {
        var tag = ((sender as Button)!.Tag as string)!;
        var confirmDialog = new ContentDialog()
        {
            Title = _translationService._("Delete Argument?"),
            Content = _translationService._("Are you sure you want to delete this post-processor argument? This action is irreversible"),
            PrimaryButtonText = _translationService._("Yes"),
            CloseButtonText = _translationService._("No"),
            DefaultButton = ContentDialogButton.Close,
            XamlRoot = XamlRoot,
            RequestedTheme = ActualTheme
        };
        if ((await confirmDialog.ShowAsync()) == ContentDialogResult.Primary)
        {
            await _controller.DeletePostprocessingArgumentAsync(tag);
        }
    }

    private async void EditPostprocessingArgument(object? sender, RoutedEventArgs e)
    {
        var tag = ((sender as Button)!.Tag as string)!;
        var argument = _controller.PostprocessingArguments.First(x => x.Name == tag);
        TxtPostprocessingArgumentName.Text = tag;
        TxtPostprocessingArgumentName.IsReadOnly = true;
        CmbPostprocessingArgumentPostProcessor.SelectedItem = _controller.PostProcessors.First(x => x.Value == argument.PostProcessor);
        CmbPostprocessingArgumentExecutable.SelectedItem = _controller.Executables.First(x => x.Value == argument.Executable);
        TxtPostprocessingArgumentArgs.Text = argument.Args;
        DlgPostprocessingArgument.PrimaryButtonText = _translationService._("Update");
        DlgPostprocessingArgument.XamlRoot = XamlRoot;
        DlgPostprocessingArgument.RequestedTheme = ActualTheme;
        string? error = null;
        do
        {
            if ((await DlgPostprocessingArgument.ShowAsync()) == ContentDialogResult.Primary)
            {
                error = await _controller.UpdatePostprocessingArgumentAsync(TxtPostprocessingArgumentName.Text,
                    (CmbPostprocessingArgumentPostProcessor.SelectedItem as BindableSelectionItem)!.ToSelectionItem<PostProcessor>()!,
                    (CmbPostprocessingArgumentExecutable.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Executable>()!,
                    TxtPostprocessingArgumentArgs.Text);
                if (error is not null)
                {
                    var errorDialog = new ContentDialog()
                    {
                        Title = _translationService._("Error"),
                        Content = error,
                        CloseButtonText = _translationService._("OK"),
                        DefaultButton = ContentDialogButton.Close,
                        XamlRoot = XamlRoot,
                        RequestedTheme = ActualTheme
                    };
                    await errorDialog.ShowAsync();
                }
            }
        } while (error is not null);
    }

    private async void SelectCookiesFile(object? sender, RoutedEventArgs e)
    {
        var filePicker = new FileOpenPicker(WindowId!.Value)
        {
            SuggestedStartLocation = PickerLocationId.DocumentsLibrary,
            FileTypeFilter = { ".txt" }
        };
        var file = await filePicker.PickSingleFileAsync();
        if (file is not null)
        {
            LblCookiesFile.Text = file.Path;
            await ApplyChangesAsync();
        }
    }

    private void SelectorBar_SelectionChanged(SelectorBar sender, SelectorBarSelectionChangedEventArgs args)
    {
        var index = sender.Items.IndexOf(sender.SelectedItem);
        ViewStack.SelectedIndex = index == -1 ? 0 : index;
    }

    private async void Cmb_SelectionChanged(object? sender, SelectionChangedEventArgs e) => await ApplyChangesAsync();

    private async void Num_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args) => await ApplyChangesAsync();

    private async void Tgl_Toggled(object? sender, RoutedEventArgs e) => await ApplyChangesAsync();

    private async void Txt_TextChanged(object? sender, TextChangedEventArgs e) => await ApplyChangesAsync();

    private async Task ApplyChangesAsync()
    {
        if (_constructing)
        {
            return;
        }
        _controller.Theme = (CmbTheme.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Theme>()!;
        _controller.TranslationLanguage = (CmbTranslationLanguage.SelectedItem as BindableSelectionItem)!.ToSelectionItem<string>()!;
        _controller.AllowPreviewUpdates = TglPreviewUpdates.IsOn;
        _controller.PreventSuspend = TglPreventSuspend.IsOn;
        _controller.HistoryLength = (CmbHistoryLength.SelectedItem as BindableSelectionItem)!.ToSelectionItem<HistoryLength>()!;
        _controller.MaxNumberOfActiveDownloads = (int)NumActiveDownloads.Value;
        _controller.OverwriteExistingFiles = TglOverwriteFiles.IsOn;
        _controller.IncludeMediaIdInTitle = TglIncludeMediaId.IsOn;
        _controller.IncludeAutoGeneratedSubtitles = TglIncludeAutoSubtitles.IsOn;
        _controller.IncludeSuperResolutions = TglIncludeSuperResolutions.IsOn;
        _controller.PreferredVideoCodec = (CmbPreferredVideoCodec.SelectedItem as BindableSelectionItem)!.ToSelectionItem<VideoCodec>()!;
        _controller.PreferredAudioCodec = (CmbPreferredAudioCodec.SelectedItem as BindableSelectionItem)!.ToSelectionItem<AudioCodec>()!;
        _controller.PreferredSubtitleFormat = (CmbPreferredSubtitleFormat.SelectedItem as BindableSelectionItem)!.ToSelectionItem<SubtitleFormat>()!;
        _controller.PreferredFrameRate = (CmbPreferredFrameRate.SelectedItem as BindableSelectionItem)!.ToSelectionItem<FrameRate>()!;
        _controller.UsePartFiles = TglUsePartFiles.IsOn;
        _controller.YouTubeSponsorBlock = TglUseSponsorBlock.IsOn;
        _controller.SpeedLimit = TglLimitSpeed.IsOn ? (int)NumSpeedLimit.Value : null;
        _controller.ProxyUrl = TxtProxyUrl.Text;
        _controller.CookiesPath = File.Exists(LblCookiesFile.Text) ? LblCookiesFile.Text : string.Empty;
        _controller.CookiesBrowser = (CmbCookiesBrowser.SelectedItem as BindableSelectionItem)!.ToSelectionItem<Browser>()!;
        _controller.UseAria = TglUseAria.IsOn;
        _controller.AriaMaxConnectionsPerServer = (int)NumMaxConnectionsPerServer.Value;
        _controller.AriaMinSplitSize = (int)NumMinimumSplitSize.Value;
        _controller.YtdlpDiscoveryArgs = TxtYtdlpDiscoveryArgs.Text;
        _controller.YtdlpDownloadArgs = TxtYtdlpDownloadArgs.Text;
        _controller.TranslateMetadataAndChapters = TglTranslateMetadataAndChapters.IsOn;
        _controller.EmbedMetadata = TglEmbedMetadata.IsOn;
        _controller.RemoveSourceData = TglRemoveSourceData.IsOn;
        _controller.EmbedThumbnails = TglEmbedThumbnails.IsOn;
        _controller.CropAudioThumbnails = TglCropAudioThumbnails.IsOn;
        _controller.EmbedChapters = TglEmbedChapters.IsOn;
        _controller.EmbedSubtitles = TglEmbedSubtitles.IsOn;
        _controller.PostprocessingThreads = (int)NumFfmpegThreads.Value;
        await _controller.SaveConfigurationAsync();
    }
}