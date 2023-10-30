using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.AppLifecycle;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Threading.Tasks;
using Windows.Storage.Pickers;
using Windows.System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// A dialog for managing application settings
/// </summary>
public sealed partial class SettingsDialog : ContentDialog
{
    private readonly PreferencesViewController _controller;
    private readonly Action<object> _initializeWithWindow;

    /// <summary>
    /// Constructs a SettingsDialog
    /// </summary>
    /// <param name="controller">PreferencesViewController</param>
    /// <param name="initializeWithWindow">Action</param>
    public SettingsDialog(PreferencesViewController controller, Action<object> initializeWithWindow)
    {
        InitializeComponent();
        _controller = controller;
        _initializeWithWindow = initializeWithWindow;
        //Localize Strings
        Title = _("Settings");
        PrimaryButtonText = _("Apply");
        CloseButtonText = _("Cancel");
        LblUserInterface.Text = _("User Interface");
        CardTheme.Header = _("Theme");
        CardTheme.Description = _("An application restart is required to change the theme.");
        CmbTheme.Items.Add(_p("Theme", "Light"));
        CmbTheme.Items.Add(_p("Theme", "Dark"));
        CmbTheme.Items.Add(_p("Theme", "System"));
        CardAutomaticallyCheckForUpdates.Header = _("Automatically Check for Updates");
        TglAutomaticallyCheckForUpdates.OnContent = _("On");
        TglAutomaticallyCheckForUpdates.OffContent = _("Off");
        CardCompletedNotification.Header = _("Completed Notification Trigger");
        CardCompletedNotification.Description = _("In combination with the trigger, completed notifications will only be shown if the window is not focused.");
        CmbCompletedNotification.Items.Add(_p("CompletedNotification", "For each download"));
        CmbCompletedNotification.Items.Add(_p("CompletedNotification", "When all downloads finish"));
        CmbCompletedNotification.Items.Add(_p("CompletedNotification", "Never"));
        CardSuspend.Header = _("Prevent Suspend");
        TglSuspend.OnContent = _("On");
        TglSuspend.OffContent = _("Off");
        LblDownloader.Text = _("Downloader");
        CardMaxNumberOfActiveDownloads.Header = _("Max Number of Active Downloads");
        CardOverwrite.Header = _("Overwrite Existing Files");
        TglOverwrite.OnContent = _("On");
        TglOverwrite.OffContent = _("Off");
        CardSpeedLimit.Header = _("Speed Limit");
        CardSpeedLimit.Description = _("This limit (in KiB/s) is applied to downloads that have speed limit enabled. Changing the value doesn't affect already running downloads.");
        CardUseAria.Header = _("Use aria2");
        CardUseAria.Description = _("Enable to use aria2 downloader. It can be faster, but you will not see download progress.");
        TglUseAria.OnContent = _("On");
        TglUseAria.OffContent = _("Off");
        CardAriaMaxConnectionsPerServer.Header = _("Max Connections Per Server (-x)");
        ToolTipService.SetToolTip(BtnAriaMaxConnectionsPerServerReset, _("Reset to default"));
        CardAriaMinSplitSize.Header = _("Minimum Split Size (-k)");
        CardAriaMinSplitSize.Description = _("The minimum size of which to split a file (in MiB).");
        ToolTipService.SetToolTip(BtnAriaMinSplitSizeReset, _("Reset to default"));
        CardSponsorBlock.Header = _("Use SponsorBlock for YouTube");
        TglSponsorBlock.OnContent = _("On");
        TglSponsorBlock.OffContent = _("Off");
        ToolTipService.SetToolTip(BtnSponsorBlockInfo, _("SponsorBlock Information (opens in a web browser)"));
        CardSubtitleLangs.Header = _("Subtitle Languages");
        CardSubtitleLangs.Description = _("Comma separated list");
        ToolTipService.SetToolTip(BtnSubtitleLangs, _("Language Code Information"));
        LblSubtitleLangs.Text = _("Different sites can use different language code formats for the same language. You may encounter one of the following three types: two-letter, two-letter with region, or three-letter. For example, \"en\", \"en-US\" and \"eng\" are all used for English.\nPlease specify all valid codes for your languages for the best results. Auto-generated subtitles will also be downloaded if available.\n\nYou can specify \"all\" instead to download all subtitles, but without auto-generated ones.");
        CardProxy.Header = _("Proxy URL");
        TxtProxy.PlaceholderText = _("Enter proxy url here");
        CardCookiesFile.Header = _("Cookies File");
        CardCookiesFile.Description = _("A cookies file can be provided to yt-dlp to allow downloading media that requires a login.");
        ToolTipService.SetToolTip(BtnSelectCookiesFile, _("Select Cookies File"));
        ToolTipService.SetToolTip(BtnUnsetCookiesFile, _("Unset Cookies File"));
        ToolTipService.SetToolTip(BtnCookiesInformation, _("Cookies File Information"));
        LblCookiesInformation.Text = _("Cookies can be passed to yt-dlp in the form of TXT files. Export cookies from your browser using the following extensions (use at your own risk):");
        BtnCookiesChrome.Content = _("Chrome/Edge");
        BtnCookiesFirefox.Content = _("Firefox");
        LblConverter.Text = _("Converter");
        CardDisallowConversions.Header = _("Disallow Conversions");
        CardDisallowConversions.Description = _("If enabled, Parabolic will download the appropriate video/audio format for the selected quality without converting to other formats. (You will be unable to select a file format)");
        TglDisallowConversions.OnContent = _("On");
        TglDisallowConversions.OffContent = _("Off");
        CardEmbedMetadata.Header = _("Embed Metadata");
        TglEmbedMetadata.OnContent = _("On");
        TglEmbedMetadata.OffContent = _("Off");
        CardCropAudioThumbnail.Header = _("Crop Audio Thumbnails");
        CardCropAudioThumbnail.Description = _("If enabled, Parabolic will automatically turn on the crop thumbnail advanced option for audio downloads");
        TglCropAudioThumbnail.OnContent = _("On");
        TglCropAudioThumbnail.OffContent = _("Off");
        CardRemoveSourceData.Header = _("Remove Source Data");
        CardRemoveSourceData.Description = _("Clear metadata fields containing the URL and other identifying information of the media source");
        TglRemoveSourceData.OnContent = _("On");
        TglRemoveSourceData.OffContent = _("Off");
        CardEmbedChapters.Header = _("Embed Chapters");
        TglEmbedChapters.OnContent = _("On");
        TglEmbedChapters.OffContent = _("Off");
        CardEmbedSubtitle.Header = _("Embed Subtitle");
        CardEmbedSubtitle.Description = _("If disabled or if embedding fails, downloaded subtitle will be saved to a separate file instead");
        TglEmbedSubtitle.OnContent = _("On");
        TglEmbedSubtitle.OffContent = _("Off");
        //Load Config
        CmbTheme.SelectedIndex = (int)_controller.Theme;
        TglAutomaticallyCheckForUpdates.IsOn = _controller.AutomaticallyCheckForUpdates;
        CmbCompletedNotification.SelectedIndex = (int)_controller.CompletedNotificationPreference;
        TglSuspend.IsOn = _controller.PreventSuspendWhenDownloading;
        TxtMaxNumberOfActiveDownloads.Value = _controller.MaxNumberOfActiveDownloads;
        TglOverwrite.IsOn = _controller.OverwriteExistingFiles;
        TxtSpeedLimit.Value = _controller.SpeedLimit;
        TglUseAria.IsOn = _controller.UseAria;
        TxtAriaMaxConnectionsPerServer.Value = _controller.AriaMaxConnectionsPerServer;
        TxtAriaMinSplitSize.Value = _controller.AriaMinSplitSize;
        TglSponsorBlock.IsOn = _controller.YouTubeSponsorBlock;
        TxtSubtitleLangs.Text = _controller.SubtitleLangs;
        TxtProxy.Text = _controller.ProxyUrl;
        LblCookiesFile.Text = File.Exists(_controller.CookiesPath) ? _controller.CookiesPath : _("No File Selected");
        TglDisallowConversions.IsOn = _controller.DisallowConversions;
        TglEmbedMetadata.IsOn = _controller.EmbedMetadata;
        TglCropAudioThumbnail.IsOn = _controller.CropAudioThumbnails;
        TglRemoveSourceData.IsOn = _controller.RemoveSourceData;
        TglEmbedChapters.IsOn = _controller.EmbedChapters;
        TglEmbedSubtitle.IsOn = _controller.EmbedSubtitle;
    }

    /// <summary>
    /// Shows the dialog
    /// </summary>
    /// <returns>ContentDialogResult</returns>
    public new async Task<ContentDialogResult> ShowAsync()
    {
        var result = await base.ShowAsync();
        if (result == ContentDialogResult.Primary)
        {
            var needsRestart = false;
            if (_controller.Theme != (Theme)CmbTheme.SelectedIndex)
            {
                _controller.Theme = (Theme)CmbTheme.SelectedIndex;
                needsRestart = true;
            }
            _controller.AutomaticallyCheckForUpdates = TglAutomaticallyCheckForUpdates.IsOn;
            _controller.CompletedNotificationPreference = (NotificationPreference)CmbCompletedNotification.SelectedIndex;
            _controller.PreventSuspendWhenDownloading = TglSuspend.IsOn;
            _controller.MaxNumberOfActiveDownloads = (int)TxtMaxNumberOfActiveDownloads.Value;
            _controller.OverwriteExistingFiles = TglOverwrite.IsOn;
            _controller.SpeedLimit = (uint)TxtSpeedLimit.Value;
            _controller.UseAria = TglUseAria.IsOn;
            _controller.AriaMaxConnectionsPerServer = (int)TxtAriaMaxConnectionsPerServer.Value;
            _controller.AriaMinSplitSize = (int)TxtAriaMinSplitSize.Value;
            _controller.YouTubeSponsorBlock = TglSponsorBlock.IsOn;
            _controller.ProxyUrl = TxtProxy.Text;
            _controller.DisallowConversions = TglDisallowConversions.IsOn;
            _controller.EmbedMetadata = TglEmbedMetadata.IsOn;
            _controller.CropAudioThumbnails = TglCropAudioThumbnail.IsOn;
            _controller.RemoveSourceData = TglRemoveSourceData.IsOn;
            _controller.EmbedChapters = TglEmbedChapters.IsOn;
            _controller.EmbedSubtitle = TglEmbedSubtitle.IsOn;
            _controller.SaveConfiguration();
            if (needsRestart)
            {
                var restartDialog = new ContentDialog()
                {
                    Title = _("Restart To Apply Theme?"),
                    Content = _("Would you like to restart {0} to apply the new theme?\nAny unsaved work will be lost.", _controller.AppInfo.ShortName),
                    PrimaryButtonText = _("Yes"),
                    CloseButtonText = _("No"),
                    DefaultButton = ContentDialogButton.Primary,
                    XamlRoot = XamlRoot
                };
                var resultRestart = await restartDialog.ShowAsync();
                if (resultRestart == ContentDialogResult.Primary)
                {
                    AppInstance.Restart("Apply new theme");
                }
            }
        }
        return result;
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => StackPanel.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the AriaMaxConnectionsPerServerReset button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void AriaMaxConnectionsPerServerReset(object sender, RoutedEventArgs e) => TxtAriaMaxConnectionsPerServer.Value = 16;

    /// <summary>
    /// Occurs when the AriaMinSplitSizeReset button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void AriaMinSplitSizeReset(object sender, RoutedEventArgs e) => TxtAriaMinSplitSize.Value = 20;

    /// <summary>
    /// Occurs when the SponsorBlockInfo button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void SponsorBlockInfo(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri(_controller.SponsorBlockInfoUrl));

    /// <summary>
    /// Occurs when the TxtSubtitleLangs' text is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">TextChangedEventArgs</param>
    private void TxtSubtitleLangs_TextChanged(object sender, TextChangedEventArgs e)
    {
        CardSubtitleLangs.Header = _("Subtitle Languages");
        var valid = _controller.ValidateSubtitleLangs(TxtSubtitleLangs.Text);
        if (valid)
        {
            _controller.SubtitleLangs = TxtSubtitleLangs.Text;
            IsPrimaryButtonEnabled = true;
        }
        else
        {
            CardSubtitleLangs.Header = _("Subtitle Languages (Invalid)");
            IsPrimaryButtonEnabled = false;
        }
    }

    /// <summary>
    /// Occurs when the SelectCookiesFile button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void SelectCookiesFile(object sender, RoutedEventArgs e)
    {
        var filePicker = new FileOpenPicker();
        _initializeWithWindow(filePicker);
        filePicker.SuggestedStartLocation = PickerLocationId.MusicLibrary;
        filePicker.FileTypeFilter.Add(".txt");
        filePicker.FileTypeFilter.Add(".TXT");
        var file = await filePicker.PickSingleFileAsync();
        if (file != null)
        {
            _controller.CookiesPath = file.Path;
            LblCookiesFile.Text = file.Path;
        }
    }

    /// <summary>
    /// Occurs when the UnsetCookiesFile button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void UnsetCookiesFile(object sender, RoutedEventArgs e)
    {
        _controller.CookiesPath = "";
        LblCookiesFile.Text = _("No File Selected");
    }

    /// <summary>
    /// Occurs when the CookiesChrome button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void CookiesChrome(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri(_controller.ChromeCookiesExtensionUrl));

    /// <summary>
    /// Occurs when the CookiesFirefox button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void CookiesFirefox(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri(_controller.FirefoxCookiesExtensionUrl));
}
