using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.Storage.Pickers;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.WinUI.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.IO;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class SettingsPage : Page
{
    private PreferencesViewController _controller;
    private readonly WindowId _windowId;
    private bool _constructing;

    public SettingsPage(PreferencesViewController controller, WindowId windowId)
    {
        InitializeComponent();
        _controller = controller;
        _windowId = windowId;
        _constructing = true;
        // Translations
        LblSettings.Text = _controller.Translator._("Settings");
        SelectorUI.Text = _controller.Translator._("User Interface");
        SelectorDownloads.Text = _controller.Translator._("Downloads");
        SelectorDownloader.Text = _controller.Translator._("Downloader");
        SelectorConverter.Text = _controller.Translator._("Converter");
        RowTheme.Header = _controller.Translator._("Theme");
        CmbTheme.ItemsSource = _controller.Themes;
        RowTranslationLanguage.Header = _controller.Translator._("Translation Language");
        RowTranslationLanguage.Description = _controller.Translator._("An application restart is required for a change to take effect");
        CmbTranslationLanguage.ItemsSource = _controller.AvailableTranslationLanguages;
        RowPreviewUpdates.Header = _controller.Translator._("Receive Preview Updates");
        TglPreviewUpdates.OnContent = _controller.Translator._("On");
        TglPreviewUpdates.OffContent = _controller.Translator._("Off");
        RowPreventSuspend.Header = _controller.Translator._("Prevent Suspend");
        RowPreventSuspend.Description = _controller.Translator._("Prevent the computer from sleeping while downloads are running");
        TglPreventSuspend.OnContent = _controller.Translator._("On");
        TglPreventSuspend.OffContent = _controller.Translator._("Off");
        RowHistoryLength.Header = _controller.Translator._("Download History Length");
        RowHistoryLength.Description = _controller.Translator._("The amount of time to keep past downloads in the app's history");
        CmbHistoryLength.ItemsSource = _controller.HistoryLengths;
        RowActiveDownloads.Header = _controller.Translator._("Max Number of Active Downloads");
        RowOverwriteFiles.Header = _controller.Translator._("Overwrite Existing Files");
        TglOverwriteFiles.OnContent = _controller.Translator._("On");
        TglOverwriteFiles.OffContent = _controller.Translator._("Off");
        RowIncludeMediaId.Header = _controller.Translator._("Include Media Id in Title");
        RowIncludeMediaId.Description = _controller.Translator._("Add the media's id to its default title");
        TglIncludeMediaId.OnContent = _controller.Translator._("On");
        TglIncludeMediaId.OffContent = _controller.Translator._("Off");
        RowIncludeAutoSubtitles.Header = _controller.Translator._("Include Auto-Generated Subtitles");
        RowIncludeAutoSubtitles.Description = _controller.Translator._("Show auto-generated subtitles to download in addition to available subtitles");
        TglIncludeAutoSubtitles.OnContent = _controller.Translator._("On");
        TglIncludeAutoSubtitles.OffContent = _controller.Translator._("Off");
        RowPreferredVideoCodec.Header = _controller.Translator._("Preferred Video Codec");
        RowPreferredVideoCodec.Description = _controller.Translator._("Prefer this codec when parsing video formats to show available to download");
        CmbPreferredVideoCodec.ItemsSource = _controller.VideoCodecs;
        RowPreferredAudioCodec.Header = _controller.Translator._("Preferred Audio Codec");
        RowPreferredAudioCodec.Description = _controller.Translator._("Prefer this codec when parsing audio formats to show available to download");
        CmbPreferredAudioCodec.ItemsSource = _controller.AudioCodecs;
        RowPreferredSubtitleFormat.Header = _controller.Translator._("Preferred Subtitle Format");
        RowPreferredSubtitleFormat.Description = _controller.Translator._("Prefer this subtitle file format when downloading");
        CmbPreferredSubtitleFormat.ItemsSource = _controller.SubtitleFormats;
        RowUsePartFiles.Header = _controller.Translator._("Use Part Files");
        RowUsePartFiles.Description = _controller.Translator._("Download media in separate .part files instead of directly into the output file");
        TglUsePartFiles.OnContent = _controller.Translator._("On");
        TglUsePartFiles.OffContent = _controller.Translator._("Off");
        RowUseSponsorBlock.Header = _controller.Translator._("Use SponsorBlock for YouTube");
        RowUseSponsorBlock.Description = _controller.Translator._("Try to remove sponsored segments from videos");
        TglUseSponsorBlock.OnContent = _controller.Translator._("On");
        TglUseSponsorBlock.OffContent = _controller.Translator._("Off");
        RowLimitSpeed.Header = _controller.Translator._("Limit Download Speed");
        TglLimitSpeed.OnContent = _controller.Translator._("On");
        TglLimitSpeed.OffContent = _controller.Translator._("Off");
        RowSpeedLimit.Header = _controller.Translator._("Speed Limit");
        RowProxyUrl.Header = _controller.Translator._("Proxy URL");
        TxtProxyUrl.PlaceholderText = _controller.Translator._("Enter proxy url here");
        RowCookiesFile.Header = _controller.Translator._("Cookies from File");
        LblCookiesFile.Text = _controller.Translator._("No file selected");
        ToolTipService.SetToolTip(BtnClearCookiesFile, _controller.Translator._("Clear Cookies File"));
        ToolTipService.SetToolTip(BtnSelectCookiesFile, _controller.Translator._("Select Cookies File"));
        RowCookiesBrowser.Header = _controller.Translator._("Cookies from Browser");
        CmbCookiesBrowser.ItemsSource = _controller.Browsers;
        LblAria.Text = _controller.Translator._("aria2c");
        RowUseAria.Header = _controller.Translator._("Use aria2c");
        RowUseAria.Description = _controller.Translator._("An alternative downloader that may be faster in some regions compared to yt-dlp's native downloader");
        TglUseAria.OnContent = _controller.Translator._("On");
        TglUseAria.OffContent = _controller.Translator._("Off");
        RowMaxConnectionsPerServer.Header = _controller.Translator._("Max Connections Per Server");
        RowMaxConnectionsPerServer.Description = _controller.Translator._("Corresponds to -x option");
        RowMinimumSplitSize.Header = _controller.Translator._("Minimum Split Size (MiB)");
        RowMinimumSplitSize.Description = _controller.Translator._("Corresponds to -k option");
        RowEmbedMetadata.Header = _controller.Translator._("Embed Metadata");
        TglEmbedMetadata.OnContent = _controller.Translator._("On");
        TglEmbedMetadata.OffContent = _controller.Translator._("Off");
        RowRemoveSourceData.Header = _controller.Translator._("Remove Source Data");
        RowRemoveSourceData.Description = _controller.Translator._("Clear metadata fields containing identifying download information");
        TglRemoveSourceData.OnContent = _controller.Translator._("On");
        TglRemoveSourceData.OffContent = _controller.Translator._("Off");
        RowEmbedThumbnails.Header = _controller.Translator._("Embed Thumbnails");
        RowEmbedThumbnails.Description = _controller.Translator._("If the file type does not support embedding, the thumbnail will be written to a separate image file");
        TglEmbedThumbnails.OnContent = _controller.Translator._("On");
        TglEmbedThumbnails.OffContent = _controller.Translator._("Off");
        RowCropAudioThumbnails.Header = _controller.Translator._("Crop Audio Thumbnails");
        RowCropAudioThumbnails.Description = _controller.Translator._("Crop thumbnails of audio files to squares");
        TglCropAudioThumbnails.OnContent = _controller.Translator._("On");
        TglCropAudioThumbnails.OffContent = _controller.Translator._("Off");
        RowEmbedChapters.Header = _controller.Translator._("Embed Chapters");
        TglEmbedChapters.OnContent = _controller.Translator._("On");
        TglEmbedChapters.OffContent = _controller.Translator._("Off");
        RowEmbedSubtitles.Header = _controller.Translator._("Embed Subtitles");
        RowEmbedSubtitles.Description = _controller.Translator._("If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files");
        TglEmbedSubtitles.OnContent = _controller.Translator._("On");
        TglEmbedSubtitles.OffContent = _controller.Translator._("Off");
        RowFfmpegThreads.Header = _controller.Translator._("FFmpeg Threads");
        RowFfmpegThreads.Description = _controller.Translator._("Limit the number of threads used by ffmpeg");
        NumFfmpegThreads.Maximum = Environment.ProcessorCount;
        RowPostProcessorArguments.Header = _controller.Translator._("Post-Processor Arguments");
        RowPostProcessorArguments.Description = _controller.Translator._("Arguments will be shown for selection in the add download dialog");
        LblManagePostProcessorArguments.Text = _controller.Translator._("Manage");
    }

    private void Page_Loaded(object sender, RoutedEventArgs e)
    {
        CmbTheme.SelectSelectionItem();
        CmbTranslationLanguage.SelectSelectionItem();
        TglPreviewUpdates.IsOn = _controller.AllowPreviewUpdates;
        TglPreventSuspend.IsOn = _controller.PreventSuspend;
        CmbHistoryLength.SelectSelectionItem();
        var downloader = _controller.DownloaderOptions;
        NumActiveDownloads.Value = downloader.MaxNumberOfActiveDownloads;
        TglOverwriteFiles.IsOn = downloader.OverwriteExistingFiles;
        TglIncludeMediaId.IsOn = downloader.IncludeMediaIdInTitle;
        TglIncludeAutoSubtitles.IsOn = downloader.IncludeAutoGeneratedSubtitles;
        CmbPreferredVideoCodec.SelectSelectionItem();
        CmbPreferredAudioCodec.SelectSelectionItem();
        CmbPreferredSubtitleFormat.SelectSelectionItem();
        TglUsePartFiles.IsOn = downloader.UsePartFiles;
        TglUseSponsorBlock.IsOn = downloader.YouTubeSponsorBlock;
        TglLimitSpeed.IsOn = downloader.SpeedLimit.HasValue;
        NumSpeedLimit.Value = downloader.SpeedLimit ?? 1024;
        TxtProxyUrl.Text = downloader.ProxyUrl;
        LblCookiesFile.Text = !File.Exists(downloader.CookiesPath) ? _controller.Translator._("No file selected") : downloader.CookiesPath;
        CmbCookiesBrowser.SelectSelectionItem();
        TglUseAria.IsOn = downloader.UseAria;
        NumMaxConnectionsPerServer.Value = downloader.AriaMaxConnectionsPerServer;
        NumMinimumSplitSize.Value = downloader.AriaMinSplitSize;
        TglEmbedMetadata.IsOn = downloader.EmbedMetadata;
        TglRemoveSourceData.IsOn = downloader.RemoveSourceData;
        TglEmbedThumbnails.IsOn = downloader.EmbedThumbnails;
        TglCropAudioThumbnails.IsOn = downloader.CropAudioThumbnails;
        TglEmbedChapters.IsOn = downloader.EmbedChapters;
        TglEmbedSubtitles.IsOn = downloader.EmbedSubtitles;
        NumFfmpegThreads.Value = downloader.PostprocessingThreads;
        _constructing = false;
    }

    private async void ManagePostProcessorArguments(object? sender, RoutedEventArgs e)
    {

    }

    private async void ClearCookiesFile(object? sender, RoutedEventArgs e)
    {
        LblCookiesFile.Text = _controller.Translator._("No file selected");
        await ApplyChangesAsync();
    }

    private async void SelectCookiesFile(object? sender, RoutedEventArgs e)
    {
        var filePicker = new FileOpenPicker(_windowId)
        {
            SuggestedStartLocation = PickerLocationId.DocumentsLibrary,
            FileTypeFilter = { ".txt" }
        };
        var file = await filePicker.PickSingleFileAsync();
        if (file is not null)
        {
            LblCookiesFile.Text = file.Path;
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
        var downloader = _controller.DownloaderOptions;
        _controller.Theme = (CmbTheme.SelectedItem as SelectionItem<Theme>)!;
        _controller.TranslationLanguage = (CmbTranslationLanguage.SelectedItem as SelectionItem<string>)!;
        _controller.AllowPreviewUpdates = TglPreviewUpdates.IsOn;
        _controller.PreventSuspend = TglPreventSuspend.IsOn;
        _controller.HistoryLength = (CmbHistoryLength.SelectedItem as SelectionItem<HistoryLength>)!;
        downloader.MaxNumberOfActiveDownloads = (int)NumActiveDownloads.Value;
        downloader.OverwriteExistingFiles = TglOverwriteFiles.IsOn;
        downloader.IncludeMediaIdInTitle = TglIncludeMediaId.IsOn;
        downloader.IncludeAutoGeneratedSubtitles = TglIncludeAutoSubtitles.IsOn;
        downloader.PreferredVideoCodec = (CmbPreferredVideoCodec.SelectedItem as SelectionItem<VideoCodec>)!.Value;
        downloader.PreferredAudioCodec = (CmbPreferredAudioCodec.SelectedItem as SelectionItem<AudioCodec>)!.Value;
        downloader.PreferredSubtitleFormat = (CmbPreferredSubtitleFormat.SelectedItem as SelectionItem<SubtitleFormat>)!.Value;
        downloader.UsePartFiles = TglUsePartFiles.IsOn;
        downloader.YouTubeSponsorBlock = TglUseSponsorBlock.IsOn;
        downloader.SpeedLimit = TglLimitSpeed.IsOn ? (int)NumSpeedLimit.Value : null;
        downloader.ProxyUrl = TxtProxyUrl.Text;
        downloader.CookiesPath = File.Exists(LblCookiesFile.Text) ? LblCookiesFile.Text : string.Empty;
        downloader.CookiesBrowser = (CmbCookiesBrowser.SelectedItem as SelectionItem<Browser>)!.Value;
        downloader.UseAria = TglUseAria.IsOn;
        downloader.AriaMaxConnectionsPerServer = (int)NumMaxConnectionsPerServer.Value;
        downloader.AriaMinSplitSize = (int)NumMinimumSplitSize.Value;
        downloader.EmbedMetadata = TglEmbedMetadata.IsOn;
        downloader.RemoveSourceData = TglRemoveSourceData.IsOn;
        downloader.EmbedThumbnails = TglEmbedThumbnails.IsOn;
        downloader.CropAudioThumbnails = TglCropAudioThumbnails.IsOn;
        downloader.EmbedChapters = TglEmbedChapters.IsOn;
        downloader.EmbedSubtitles = TglEmbedSubtitles.IsOn;
        downloader.PostprocessingThreads = (int)NumFfmpegThreads.Value;
        _controller.DownloaderOptions = downloader;
        await _controller.SaveConfigurationAsync();
    }
}
