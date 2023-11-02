using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Nickvision.Aura;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage.Pickers;
using Windows.System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public sealed partial class AddDownloadDialog : ContentDialog
{
    private readonly AddDownloadDialogController _controller;
    private readonly Action<object> _initializeWithWindow;
    private readonly List<string> _audioQualities;
    private string _saveFolderString;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    /// <param name="controller">AddDownloadDialogController</param>
    /// <param name="initializeWithWindow">Action</param>
    public AddDownloadDialog(AddDownloadDialogController controller, Action<object> initializeWithWindow)
    {
        InitializeComponent();
        _controller = controller;
        _initializeWithWindow = initializeWithWindow;
        _audioQualities = new List<string>() { _("Best"), _("Worst") };
        //Localize Strings
        Title = _("Add Download");
        CloseButtonText = _("Close");
        LblBtnBack.Text = _("Back");
        CardUrl.Header = _("Media URL");
        TxtUrl.PlaceholderText = _("Ender media url here");
        CardAuthenticate.Header = _("Authenticate");
        TglAuthenticate.OnContent = _("On");
        TglAuthenticate.OffContent = _("Off");
        CardKeyringCredentials.Header = _("Keyring Credential");
        CardUsername.Header = _("Username");
        TxtUsername.PlaceholderText = _("Enter user name here");
        CardPassword.Header = _("Password");
        TxtPassword.PlaceholderText = _("Enter password here");
        BtnValidate.Content = _("Validate");
        CardFileType.Header = _("File Type");
        CardQuality.Header = _("Quality");
        CardAudioLanguage.Header = _("Audio Language");
        CardSubtitle.Header = _("Download Subtitle");
        TglSubtitle.OnContent = _("On");
        TglSubtitle.OffContent = _("Off");
        CardSaveFolder.Header = _("Save Folder");
        ToolTipService.SetToolTip(BtnSelectSaveFolder, _("Select Save Folder"));
        CardAdvancedOptions.Header = _("Advanced Options");
        CardOpenPlaylist.Description = _("Select items to download or change file names.");
        CardNumberTitles.Header = _("Number Titles");
        TglNumberTitles.OnContent = _("On");
        TglNumberTitles.OffContent = _("Off");
        CardNumberTitles2.Header = _("Number Titles");
        TglNumberTitles2.OnContent = _("On");
        TglNumberTitles2.OffContent = _("Off");
        LblBtnSelectAll.Text = _("Select All");
        LblBtnDeselectAll.Text = _("Deselect All");
        CardSpeedLimit.Header = _("Speed Limit");
        TglSpeedLimit.OnContent = _("On");
        TglSpeedLimit.OffContent = _("Off");
        CardSplitChapters.Header = _("Split Chapters");
        CardSplitChapters.Description = _("Splits the video into multiple smaller ones based on its chapters.");
        TglSplitChapters.OnContent = _("On");
        TglSplitChapters.OffContent = _("Off");
        CardCropThumbnail.Header = _("Crop Thumbnail");
        CardCropThumbnail.Description = _("Make thumbnail square, useful when downloading music.");
        TglCropThumbnail.OnContent = _("On");
        TglCropThumbnail.OffContent = _("Off");
        CardDownloadTimeframe.Header = _("Download Specific Timeframe");
        CardDownloadTimeframe.Description = _("Media can possibly be cut inaccurately.\nEnabling this option will disable the use of aria2 as the downloader if it is enabled.");
        TglDownloadTimeframe.OnContent = _("On");
        TglDownloadTimeframe.OffContent = _("Off");
        CardTimeframeStart.Header = _("Start Time");
        CardTimeframeStart.Description = _("Leave empty to download from start.");
        TxtTimeframeStart.PlaceholderText = _("Enter start timeframe here");
        CardTimeframeEnd.Header = _("End Time");
        CardTimeframeEnd.Description = _("Leave empty to download from start.");
        TxtTimeframeEnd.PlaceholderText = _("Enter end timeframe here");
        //Load
        ViewStack.CurrentPageName = "Url";
        if (Directory.Exists(_controller.PreviousSaveFolder))
        {
            _saveFolderString = _controller.PreviousSaveFolder;
        }
        else
        {
            _saveFolderString = UserDirectories.Downloads;
        }
        LblSaveFolder.Text = Path.GetFileName(_saveFolderString);
        TglSubtitle.IsOn = _controller.PreviousSubtitleState;
        CardSpeedLimit.Description = $"{_("{0:f1} KiB/s", _controller.CurrentSpeedLimit)} {_("(Configurable in preferences)")}";
        CardCropThumbnail.IsEnabled = _controller.EmbedMetadata;
    }

    /// <summary>
    /// The MediaFileType object representing the selected file type
    /// </summary>
    private MediaFileType SelectedMediaFileType
    {
        get
        {
            MediaFileType fileType;
            if (_controller.DisallowConversions)
            {
                fileType = (CmbFileType.SelectedIndex == 0 && _controller.HasVideoResolutions) ? MediaFileType.Video : MediaFileType.Audio;
            }
            else
            {
                fileType = (MediaFileType)CmbFileType.SelectedIndex;
                if (!_controller.HasVideoResolutions)
                {
                    fileType += 2;
                }
            }
            return fileType;
        }
    }

    /// <summary>
    /// Show the dialog
    /// </summary>
    /// <returns>ContentDialogResult</returns>
    public new async Task<ContentDialogResult> ShowAsync() => await ShowAsync(null);

    /// <summary>
    /// Show the dialog
    /// </summary>
    /// <param name="url">A url to validate on startup</param>
    /// <returns>ContentDialogResult</returns>
    public async Task<ContentDialogResult> ShowAsync(string? url)
    {
        //Validated from startup
        if (!string.IsNullOrEmpty(url))
        {
            TxtUrl.Text = url;
            await SearchUrlAsync(url);
        }
        else
        {
            //Validate Clipboard
            try
            {
                var clipboardText = await Clipboard.GetContent().GetTextAsync();
                if (!string.IsNullOrEmpty(clipboardText))
                {
                    var result = Uri.TryCreate(clipboardText, UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
                    if (result)
                    {
                        TxtUrl.Text = clipboardText;
                        TxtUrl.Select(clipboardText.Length, 0);
                        BtnValidate.IsEnabled = true;
                    }
                }
            }
            catch { }
        }
        //Keyring
        var names = await _controller.GetKeyringCredentialNamesAsync();
        if (names.Count > 0)
        {
            CmbKeyringCredentials.ItemsSource = names;
            CmbKeyringCredentials.SelectedIndex = names.Count > 1 ? 1 : 0;
        }
        else
        {
            CardKeyringCredentials.Visibility = Visibility.Collapsed;
            CardUsername.Visibility = Visibility.Visible;
            CardPassword.Visibility = Visibility.Visible;
        }
        var res = await base.ShowAsync();
        if (res == ContentDialogResult.Primary)
        {
            Quality quality;
            int? resolutionIndex;
            if (SelectedMediaFileType.GetIsAudio())
            {
                quality = (Quality)CmbQuality.SelectedIndex;
                resolutionIndex = null;
            }
            else
            {
                quality = Quality.Resolution;
                resolutionIndex = (int)CmbQuality.SelectedIndex;
            }
            string? audioLanguage = null;
            if (_controller.AudioLanguages.Count > 1)
            {
                audioLanguage = _controller.AudioLanguages[CmbAudioLanguage.SelectedIndex];
            }
            Timeframe? timeframe = null;
            if (TglDownloadTimeframe.IsOn)
            {
                try
                {
                    timeframe = Timeframe.Parse(TxtTimeframeStart.Text, TxtTimeframeEnd.Text, _controller.MediaList[0].Duration);
                }
                catch { }
            }
            if (CmbKeyringCredentials.SelectedIndex == 0 || !TglAuthenticate.IsOn)
            {
                _controller.PopulateDownloads(SelectedMediaFileType, quality, resolutionIndex, audioLanguage,
                    TglSubtitle.IsOn, _saveFolderString, TglSpeedLimit.IsOn, TglSplitChapters.IsOn,
                    TglCropThumbnail.IsOn, timeframe, TxtUsername.Text, TxtPassword.Password);
            }
            else
            {
                await _controller.PopulateDownloadsAsync(SelectedMediaFileType, quality, resolutionIndex, audioLanguage,
                    TglSubtitle.IsOn, _saveFolderString, TglSpeedLimit.IsOn, TglSplitChapters.IsOn,
                    TglCropThumbnail.IsOn, timeframe, CmbKeyringCredentials.SelectedIndex - 1);
            }
        }
        return res;
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => ViewStack.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the back button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Back(object sender, RoutedEventArgs e) => ViewStack.CurrentPageName = "Download";

    /// <summary>
    /// Occurs when the ViewStack's page is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">PageChangedEventArgs</param>
    private void ViewStack_PageChanged(object sender, PageChangedEventArgs e)
    {
        BtnBack.Visibility = ViewStack.CurrentPageName == "Playlist" || ViewStack.CurrentPageName == "Advanced" ? Visibility.Visible : Visibility.Collapsed;
        Title = ViewStack.CurrentPageName switch
        {
            "Playlist" => _("Playlist"),
            "Advanced" => _("Advanced Options"),
            _ => _("Add Download")
        };
    }

    /// <summary>
    /// Occurs when TxtUrl's text is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">TextChangedEventArgs</param>
    private void TxtUrl_TextChanged(object sender, TextChangedEventArgs e)
    {
        if (!string.IsNullOrEmpty(TxtUrl.Text))
        {
            var result = Uri.TryCreate(TxtUrl.Text, UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
            BtnValidate.IsEnabled = result;
        }
    }

    /// <summary>
    /// Occurs when TxtUrl's key is pressed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">KeyRoutedEventArgs</param>
    private async void TxtUrl_KeyDown(object sender, KeyRoutedEventArgs e)
    {
        if (e.Key == VirtualKey.Enter)
        {
            await SearchUrlAsync(TxtUrl.Text);
        }
    }

    /// <summary>
    /// Occurs when TglAuthenticate is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void TglAuthenticate_Toggled(object sender, RoutedEventArgs e)
    {
        TxtUsername.Text = "";
        TxtPassword.Password = "";
    }

    /// <summary>
    /// Occurs when CmbKeyringCredentials' selection is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private void CmbKeyringCredentials_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (CmbKeyringCredentials.SelectedIndex == 0)
        {
            CardUsername.Visibility = Visibility.Visible;
            CardPassword.Visibility = Visibility.Visible;
        }
        else
        {
            CardUsername.Visibility = Visibility.Collapsed;
            CardPassword.Visibility = Visibility.Collapsed;
        }
    }

    /// <summary>
    /// Occurs when the validate button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Validate(object sender, RoutedEventArgs e) => await SearchUrlAsync(TxtUrl.Text);

    /// <summary>
    /// Occurs when CmbFileType's selection is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private void CmbFileType_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (SelectedMediaFileType.GetIsVideo())
        {
            CmbQuality.ItemsSource = _controller.VideoResolutions;
            var findPrevious = _controller.PreviousVideoResolutionIndex;
            CmbQuality.SelectedIndex = findPrevious != -1 ? findPrevious : 0;
            CardSubtitle.IsEnabled = true;
        }
        else
        {
            CmbQuality.ItemsSource = _audioQualities;
            CmbQuality.SelectedIndex = 0;
            CardSubtitle.IsEnabled = false;
        }
        if (_controller.CropAudioThumbnails)
        {
            TglCropThumbnail.IsOn = SelectedMediaFileType.GetIsAudio();
        }
    }

    /// <summary>
    /// Occurs when the SelectSaveFolder button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void SelectSaveFolder(object sender, RoutedEventArgs e)
    {
        var folderPicker = new FolderPicker();
        _initializeWithWindow(folderPicker);
        folderPicker.SuggestedStartLocation = PickerLocationId.Downloads;
        folderPicker.FileTypeFilter.Add("*");
        var folder = await folderPicker.PickSingleFolderAsync();
        if (folder != null)
        {
            _saveFolderString = folder.Path;
            LblSaveFolder.Text = folder.Name;
        }
        ValidateOptions();
    }

    /// <summary>
    /// Occurs when the OpenAdvancedOptions card is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void OpenAdvancedOptions(object sender, RoutedEventArgs e) => ViewStack.CurrentPageName = "Advanced";

    /// <summary>
    /// Occurs when the OpenPlaylist card is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void OpenPlaylist(object sender, RoutedEventArgs e) => ViewStack.CurrentPageName = "Playlist";

    /// <summary>
    /// Occurs when the TglNumberTitles is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void TglNumberTitles_Toggled(object sender, RoutedEventArgs e)
    {
        if (_controller.ToggleNumberTitles(TglNumberTitles.IsOn))
        {
            foreach (MediaRow row in ListPlaylist.Children)
            {
                row.UpdateTitle(TglNumberTitles.IsOn);
            }
        }
    }

    /// <summary>
    /// Occurs when the SelectAll button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void SelectAll(object sender, RoutedEventArgs e)
    {
        foreach (MediaRow row in ListPlaylist.Children)
        {
            row.IsChecked = true;
        }
    }

    /// <summary>
    /// Occurs when the DeselectAll button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void DeselectAll(object sender, RoutedEventArgs e)
    {
        foreach (MediaRow row in ListPlaylist.Children)
        {
            row.IsChecked = false;
        }
    }

    /// <summary>
    /// Occurs when the TglSpeedLimit is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void TglSpeedLimit_Toggled(object sender, RoutedEventArgs e)
    {
        if (TglSpeedLimit.IsOn)
        {
            CardDownloadTimeframe.IsExpanded = false;
        }
        CardDownloadTimeframe.IsEnabled = !TglSpeedLimit.IsOn && _controller.MediaList.Count == 1 && _controller.MediaList[0].Duration > 0;
    }

    /// <summary>
    /// Occurs when the TglDownloadTimeframe is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void TglDownloadTimeframe_Toggled(object sender, RoutedEventArgs e)
    {
        if (TglDownloadTimeframe.IsOn)
        {
            TglSpeedLimit.IsOn = false;
            TxtTimeframeStart.Text = TimeSpan.FromSeconds(0).ToString(@"hh\:mm\:ss");
            TxtTimeframeEnd.Text = TimeSpan.FromSeconds(_controller.MediaList[0].Duration).ToString(@"hh\:mm\:ss");
        }
        CardSpeedLimit.IsEnabled = !TglDownloadTimeframe.IsOn;
        ValidateOptions();
    }

    private void TxtTimeframe_TextChanged(object sender, TextChangedEventArgs e) => ValidateOptions();

    /// <summary>
    /// Occurs when the number of items selected to download has changed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void PlaylistChanged(object? sender, EventArgs e)
    {
        var downloadsCount = _controller.MediaList.FindAll(x => x.ToDownload).Count;
        CardOpenPlaylist.Header = _n("{0} of {1} items", "{0} of {1} items", _controller.MediaList.Count, downloadsCount, _controller.MediaList.Count);
        IsPrimaryButtonEnabled = downloadsCount > 0;
    }

    /// <summary>
    /// Searches for information about a URL in the dialog
    /// </summary>
    /// <param name="url">The URL to search</param>
    private async Task SearchUrlAsync(string url)
    {
        CardUrl.Header = _("Media URL");
        ProgRingUrl.Visibility = Visibility.Visible;
        BtnValidate.IsEnabled = false;
        try
        {
            if (CmbKeyringCredentials.SelectedIndex == 0 || CmbKeyringCredentials.SelectedIndex == -1 || !TglAuthenticate.IsOn)
            {
                await _controller.SearchUrlAsync(url, TxtUsername.Text, TxtPassword.Password);
            }
            else
            {
                await _controller.SearchUrlAsync(url, CmbKeyringCredentials.SelectedIndex - 1);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
            Console.WriteLine(ex.StackTrace);
        }
        ProgRingUrl.Visibility = Visibility.Collapsed;
        BtnValidate.IsEnabled = true;
        if (!_controller.HasMediaInfo)
        {
            CardUrl.Header = _("Media URL (Invalid)");
            if (TglAuthenticate.IsOn)
            {
                InfoBar.Message = _("Ensure credentials are correct.");
                InfoBar.Severity = InfoBarSeverity.Warning;
                InfoBar.IsOpen = true;
            }
        }
        else
        {
            if (!_controller.HasVideoResolutions)
            {
                if (_controller.DisallowConversions)
                {
                    CmbFileType.ItemsSource = new string[] { _("Audio") };
                    CmbFileType.SelectedIndex = 0;
                }
                else
                {
                    CmbFileType.ItemsSource = new string[] { "MP3", "OPUS", "FLAC", "WAV" };
                    CmbFileType.SelectedIndex = Math.Max((int)_controller.PreviousMediaFileType - 2, 0);
                }
            }
            else
            {
                if (_controller.DisallowConversions)
                {
                    CmbFileType.ItemsSource = new string[] { _("Video"), _("Audio") };
                    CmbFileType.SelectedIndex = 0;
                }
                else
                {
                    CmbFileType.SelectedIndex = (int)_controller.PreviousMediaFileType;
                }
            }
            if (_controller.AudioLanguages.Count > 1)
            {
                CardAudioLanguage.IsEnabled = true;
                CmbAudioLanguage.ItemsSource = _controller.AudioLanguages;
            }
            ViewStack.CurrentPageName = "Download";
            PrimaryButtonText = _("Download");
            IsPrimaryButtonEnabled = Directory.Exists(_saveFolderString);
            DefaultButton = ContentDialogButton.Primary;
            if (_controller.MediaList.Count > 1)
            {
                CardQuality.Header = _("Maximum Quality");
                OpenPlaylistGroup.Visibility = Visibility.Visible;
                CardOpenPlaylist.Header = _n("{0} of {1} items", "{0} of {1} items", _controller.MediaList.Count, _controller.MediaList.Count, _controller.MediaList.Count);
                CardDownloadTimeframe.IsEnabled = false;
                if (_controller.NumberTitles)
                {
                    TglNumberTitles.IsOn = true;
                }
                foreach (var mediaInfo in _controller.MediaList)
                {
                    var row = new MediaRow(mediaInfo);
                    row.OnSelectionChanged += PlaylistChanged;
                    ListPlaylist.Children.Add(row);
                }
            }
            else
            {
                CardDownloadTimeframe.IsEnabled = _controller.MediaList[0].Duration > 0;
                StackDownload.Children.Insert(StackDownload.Children.IndexOf(CardAdvancedOptions) + 1, new MediaRow(_controller.MediaList[0])
                {
                    Margin = new Thickness(0, 12, 0, 0)
                });
            }
            ValidateOptions();
        }
    }

    /// <summary>
    /// Validate download options
    /// </summary>
    private void ValidateOptions()
    {
        CardSaveFolder.Header = _("Save Folder");
        CardTimeframeStart.Header = _("Start Time");
        CardTimeframeEnd.Header = _("End Time");
        IsPrimaryButtonEnabled = false;
        var status = _controller.ValidateDownloadOptions(_saveFolderString, TglDownloadTimeframe.IsOn, TxtTimeframeStart.Text, TxtTimeframeEnd.Text, _controller.MediaList[0].Duration);
        if (status == DownloadOptionsCheckStatus.Valid)
        {
            IsPrimaryButtonEnabled = true;
            return;
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidSaveFolder))
        {
            CardSaveFolder.Header = _("Save Folder (Invalid)");
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidTimeframeStart))
        {
            CardTimeframeStart.Header = _("Start Time (Invalid)");
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidTimeframeEnd))
        {
            CardTimeframeEnd.Header = _("End Time (Invalid)");
        }
    }
}
