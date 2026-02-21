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
using System.Linq;
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
        RowPreviewUpdates.Description = _controller.Translator._("Update Parabolic and dependencies, such as yt-dlp, to beta versions");
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
        RowPreferredFrameRate.Header = _controller.Translator._("Preferred Frame Rate");
        RowPreferredFrameRate.Description = _controller.Translator._("Prefer this frame rate when parsing video formats to show available to download");
        CmbPreferredFrameRate.ItemsSource = _controller.FrameRates;
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
        RowCookiesFile.Description = _controller.Translator._("Upload a txt cookies file from unlisted browsers");
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
        LblAdvanced.Text = _controller.Translator._("Advanced");
        RowYtdlpDiscoveryArgs.Header = _controller.Translator._("yt-dlp Discovery Arguments");
        RowYtdlpDiscoveryArgs.Description = _controller.Translator._("Extra arguments to pass to yt-dlp when discovering media");
        TxtYtdlpDiscoveryArgs.PlaceholderText = _controller.Translator._("Enter args here");
        RowYtdlpDownloadArgs.Header = _controller.Translator._("yt-dlp Download Arguments");
        RowYtdlpDownloadArgs.Description = _controller.Translator._("Extra arguments to pass to yt-dlp when downloading media");
        TxtYtdlpDownloadArgs.PlaceholderText = _controller.Translator._("Enter args here");
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
        LblAddPostProcessorArgument.Text = _controller.Translator._("Add");
        DlgPostprocessingArgument.Title = _controller.Translator._("Argument");
        DlgPostprocessingArgument.CloseButtonText = _controller.Translator._("Cancel");
        TxtPostprocessingArgumentName.Header = _controller.Translator._("Name");
        TxtPostprocessingArgumentName.PlaceholderText = _controller.Translator._("Enter name here");
        CmbPostprocessingArgumentPostProcessor.Header = _controller.Translator._("Post-Processor");
        CmbPostprocessingArgumentExecutable.Header = _controller.Translator._("Executable");
        TxtPostprocessingArgumentArgs.Header = _controller.Translator._("Args");
        TxtPostprocessingArgumentArgs.PlaceholderText = _controller.Translator._("Enter args here");
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
        CmbPreferredVideoCodec.SelectSelectionItem();
        CmbPreferredAudioCodec.SelectSelectionItem();
        CmbPreferredSubtitleFormat.SelectSelectionItem();
        CmbPreferredFrameRate.SelectSelectionItem();
        TglUsePartFiles.IsOn = _controller.UsePartFiles;
        TglUseSponsorBlock.IsOn = _controller.YouTubeSponsorBlock;
        TglLimitSpeed.IsOn = _controller.SpeedLimit.HasValue;
        NumSpeedLimit.Value = _controller.SpeedLimit ?? 1024;
        TxtProxyUrl.Text = _controller.ProxyUrl;
        LblCookiesFile.Text = !File.Exists(_controller.CookiesPath) ? _controller.Translator._("No file selected") : _controller.CookiesPath;
        CmbCookiesBrowser.SelectSelectionItem();
        TglUseAria.IsOn = _controller.UseAria;
        NumMaxConnectionsPerServer.Value = _controller.AriaMaxConnectionsPerServer;
        NumMinimumSplitSize.Value = _controller.AriaMinSplitSize;
        TxtYtdlpDiscoveryArgs.Text = _controller.YtdlpDiscoveryArgs;
        TxtYtdlpDownloadArgs.Text = _controller.YtdlpDownloadArgs;
        TglEmbedMetadata.IsOn = _controller.EmbedMetadata;
        TglRemoveSourceData.IsOn = _controller.RemoveSourceData;
        TglEmbedThumbnails.IsOn = _controller.EmbedThumbnails;
        TglCropAudioThumbnails.IsOn = _controller.CropAudioThumbnails;
        TglEmbedChapters.IsOn = _controller.EmbedChapters;
        TglEmbedSubtitles.IsOn = _controller.EmbedSubtitles;
        NumFfmpegThreads.Value = _controller.PostprocessingThreads;
        RowPostProcessorArguments.ItemsSource = _controller.PostprocessingArguments;
        CmbPostprocessingArgumentPostProcessor.ItemsSource = _controller.PostProcessors;
        CmbPostprocessingArgumentExecutable.ItemsSource = _controller.Executables;
        _constructing = false;
    }

    private async void AddPostprocessingArgument(object? sender, RoutedEventArgs e)
    {
        TxtPostprocessingArgumentName.IsReadOnly = false;
        TxtPostprocessingArgumentName.Text = string.Empty;
        CmbPostprocessingArgumentPostProcessor.SelectSelectionItem();
        CmbPostprocessingArgumentExecutable.SelectSelectionItem();
        TxtPostprocessingArgumentArgs.Text = string.Empty;
        DlgPostprocessingArgument.PrimaryButtonText = _controller.Translator._("Add");
        DlgPostprocessingArgument.XamlRoot = XamlRoot;
        DlgPostprocessingArgument.RequestedTheme = ActualTheme;
        string? error = null;
        do
        {
            if ((await DlgPostprocessingArgument.ShowAsync()) == ContentDialogResult.Primary)
            {
                error = await _controller.AddPostprocessingArgumentAsync(TxtPostprocessingArgumentName.Text,
                    (CmbPostprocessingArgumentPostProcessor.SelectedItem as SelectionItem<PostProcessor>)!,
                    (CmbPostprocessingArgumentExecutable.SelectedItem as SelectionItem<Executable>)!,
                    TxtPostprocessingArgumentArgs.Text);
                if (error is not null)
                {
                    var errorDialog = new ContentDialog()
                    {
                        Title = _controller.Translator._("Error"),
                        Content = error,
                        CloseButtonText = _controller.Translator._("OK"),
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
        LblCookiesFile.Text = _controller.Translator._("No file selected");
        await ApplyChangesAsync();
    }

    private async void DeletePostprocessingArgument(object? sender, RoutedEventArgs e)
    {
        var tag = ((sender as Button)!.Tag as string)!;
        var confirmDialog = new ContentDialog()
        {
            Title = _controller.Translator._("Delete Argument?"),
            Content = _controller.Translator._("Are you sure you want to delete this post-processor argument? This action is irreversible"),
            PrimaryButtonText = _controller.Translator._("Yes"),
            CloseButtonText = _controller.Translator._("No"),
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
        DlgPostprocessingArgument.PrimaryButtonText = _controller.Translator._("Update");
        DlgPostprocessingArgument.XamlRoot = XamlRoot;
        DlgPostprocessingArgument.RequestedTheme = ActualTheme;
        string? error = null;
        do
        {
            if ((await DlgPostprocessingArgument.ShowAsync()) == ContentDialogResult.Primary)
            {
                error = await _controller.UpdatePostprocessingArgumentAsync(TxtPostprocessingArgumentName.Text,
                    (CmbPostprocessingArgumentPostProcessor.SelectedItem as SelectionItem<PostProcessor>)!,
                    (CmbPostprocessingArgumentExecutable.SelectedItem as SelectionItem<Executable>)!,
                    TxtPostprocessingArgumentArgs.Text);
                if (error is not null)
                {
                    var errorDialog = new ContentDialog()
                    {
                        Title = _controller.Translator._("Error"),
                        Content = error,
                        CloseButtonText = _controller.Translator._("OK"),
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
        _controller.Theme = (CmbTheme.SelectedItem as SelectionItem<Theme>)!;
        _controller.TranslationLanguage = (CmbTranslationLanguage.SelectedItem as SelectionItem<string>)!;
        _controller.AllowPreviewUpdates = TglPreviewUpdates.IsOn;
        _controller.PreventSuspend = TglPreventSuspend.IsOn;
        _controller.HistoryLength = (CmbHistoryLength.SelectedItem as SelectionItem<HistoryLength>)!;
        _controller.MaxNumberOfActiveDownloads = (int)NumActiveDownloads.Value;
        _controller.OverwriteExistingFiles = TglOverwriteFiles.IsOn;
        _controller.IncludeMediaIdInTitle = TglIncludeMediaId.IsOn;
        _controller.IncludeAutoGeneratedSubtitles = TglIncludeAutoSubtitles.IsOn;
        _controller.PreferredVideoCodec = (CmbPreferredVideoCodec.SelectedItem as SelectionItem<VideoCodec>)!;
        _controller.PreferredAudioCodec = (CmbPreferredAudioCodec.SelectedItem as SelectionItem<AudioCodec>)!;
        _controller.PreferredSubtitleFormat = (CmbPreferredSubtitleFormat.SelectedItem as SelectionItem<SubtitleFormat>)!;
        _controller.PreferredFrameRate = (CmbPreferredFrameRate.SelectedItem as SelectionItem<FrameRate>)!;
        _controller.UsePartFiles = TglUsePartFiles.IsOn;
        _controller.YouTubeSponsorBlock = TglUseSponsorBlock.IsOn;
        _controller.SpeedLimit = TglLimitSpeed.IsOn ? (int)NumSpeedLimit.Value : null;
        _controller.ProxyUrl = TxtProxyUrl.Text;
        _controller.CookiesPath = File.Exists(LblCookiesFile.Text) ? LblCookiesFile.Text : string.Empty;
        _controller.CookiesBrowser = (CmbCookiesBrowser.SelectedItem as SelectionItem<Browser>)!;
        _controller.UseAria = TglUseAria.IsOn;
        _controller.AriaMaxConnectionsPerServer = (int)NumMaxConnectionsPerServer.Value;
        _controller.AriaMinSplitSize = (int)NumMinimumSplitSize.Value;
        _controller.YtdlpDiscoveryArgs = TxtYtdlpDiscoveryArgs.Text;
        _controller.YtdlpDownloadArgs = TxtYtdlpDownloadArgs.Text;
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