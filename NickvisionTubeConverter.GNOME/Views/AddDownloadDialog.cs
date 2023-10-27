using Nickvision.Aura;
using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;
using static Nickvision.GirExt.GdkExt;
using static Nickvision.GirExt.GtkExt;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public partial class AddDownloadDialog : Adw.Window
{
    private const uint GTK_INVALID_LIST_POSITION = 4294967295;

    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;
    private readonly List<MediaRow> _mediaRows;
    private readonly string[] _audioQualities;
    private string _saveFolderString;

    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Adw.ExpanderRow _authRow;
    [Gtk.Connect] private readonly Adw.ComboRow _keyringRow;
    [Gtk.Connect] private readonly Adw.EntryRow _usernameRow;
    [Gtk.Connect] private readonly Adw.PasswordEntryRow _passwordRow;
    [Gtk.Connect] private readonly Gtk.Button _validateUrlButton;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _downloadPage;
    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Adw.ComboRow _fileTypeRow;
    [Gtk.Connect] private readonly Adw.ComboRow _qualityRow;
    [Gtk.Connect] private readonly Adw.ComboRow _audioLanguageRow;
    [Gtk.Connect] private readonly Adw.ActionRow _subtitleRow;
    [Gtk.Connect] private readonly Gtk.Switch _subtitleSwitch;
    [Gtk.Connect] private readonly Adw.EntryRow _saveFolderRow;
    [Gtk.Connect] private readonly Gtk.Button _selectSaveFolderButton;
    [Gtk.Connect] private readonly Adw.ActionRow _openAdvancedRow;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _mediaGroup;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _openPlaylistGroup;
    [Gtk.Connect] private readonly Adw.ActionRow _openPlaylistRow;
    [Gtk.Connect] private readonly Adw.ActionRow _numberTitlesRow;
    [Gtk.Connect] private readonly Gtk.Switch _numberTitlesSwitch;
    [Gtk.Connect] private readonly Gtk.Button _selectAllButton;
    [Gtk.Connect] private readonly Gtk.Button _deselectAllButton;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _playlistGroup;
    [Gtk.Connect] private readonly Adw.ActionRow _speedLimitRow;
    [Gtk.Connect] private readonly Gtk.Switch _speedLimitSwitch;
    [Gtk.Connect] private readonly Gtk.Switch _splitChaptersSwitch;
    [Gtk.Connect] private readonly Adw.ActionRow _cropThumbnailRow;
    [Gtk.Connect] private readonly Gtk.Switch _cropThumbnailSwitch;
    [Gtk.Connect] private readonly Adw.ExpanderRow _downloadTimeframeRow;
    [Gtk.Connect] private readonly Adw.EntryRow _timeframeStartRow;
    [Gtk.Connect] private readonly Adw.EntryRow _timeframeEndRow;
    private Gtk.Spinner? _urlSpinner;
    private readonly Gtk.ShortcutController _shortcutController;

    public event EventHandler? OnDownload;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    /// <param name="builder">Gtk.Builder</param>
    /// <param name="controller">AddDownloadDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    private AddDownloadDialog(Gtk.Builder builder, AddDownloadDialogController controller, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        _parent = parent;
        _controller = controller;
        _mediaRows = new List<MediaRow>();
        _audioQualities = new string[] { _("Best"), _("Worst") };
        //Dialog Settings
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        _validateUrlButton.OnClicked += async (sender, e) => await SearchUrlAsync(_urlRow.GetText());
        _viewStack.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "visible-child")
            {
                _backButton.SetVisible(_viewStack.GetVisibleChildName() == "pagePlaylist" || _viewStack.GetVisibleChildName() == "pageAdvanced");
                _titleLabel.SetLabel(_viewStack.GetVisibleChildName() switch
                {
                    "pagePlaylist" => _("Playlist"),
                    "pageAdvanced" => _("Advanced Options"),
                    _ => _("Add Download")
                });
            }
        };
        _backButton.OnClicked += (sender, e) => _viewStack.SetVisibleChildName("pageDownload");
        _urlRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                if (!string.IsNullOrEmpty(_urlRow.GetText()))
                {
                    var result = Uri.TryCreate(_urlRow.GetText(), UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
                    _validateUrlButton.SetSensitive(result);
                }
            }
        };
        _authRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "enable-expansion")
            {
                _usernameRow.SetText("");
                _passwordRow.SetText("");
            }
        };
        _keyringRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected")
            {
                if (_keyringRow.GetSelected() == 0)
                {
                    _usernameRow.SetVisible(true);
                    _passwordRow.SetVisible(true);
                }
                else
                {
                    _usernameRow.SetVisible(false);
                    _passwordRow.SetVisible(false);
                }
            }
        };
        _fileTypeRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                SetQualityRowModel();
                if (_controller.CropAudioThumbnails)
                {
                    _cropThumbnailSwitch.SetActive(SelectedMediaFileType.GetIsAudio());
                }
            }
        };
        _selectSaveFolderButton.OnClicked += async (sender, e) => await SelectSaveFolderAsync();
        _cropThumbnailRow.SetVisible(_controller.EmbedMetadata);
        _speedLimitSwitch.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "active")
            {
                if (_speedLimitSwitch.GetActive())
                {
                    _downloadTimeframeRow.SetExpanded(false);
                }
                _downloadTimeframeRow.SetSensitive(!_speedLimitSwitch.GetActive());
            }
        };
        _downloadTimeframeRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "expanded")
            {
                if (_downloadTimeframeRow.GetExpanded())
                {
                    _speedLimitSwitch.SetActive(false);
                    _timeframeStartRow.SetText(TimeSpan.FromSeconds(0).ToString(@"hh\:mm\:ss"));
                    _timeframeEndRow.SetText(TimeSpan.FromSeconds(_controller.MediaList[0].Duration).ToString(@"hh\:mm\:ss"));
                }
                _speedLimitRow.SetSensitive(!_downloadTimeframeRow.GetExpanded());
                ValidateOptions();
            }
        };
        _timeframeStartRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                ValidateOptions();
            }
        };
        _timeframeEndRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                ValidateOptions();
            }
        };
        _openAdvancedRow.OnActivated += (sender, e) => _viewStack.SetVisibleChildName("pageAdvanced");
        _openPlaylistRow.OnActivated += (sender, e) => _viewStack.SetVisibleChildName("pagePlaylist");
        _numberTitlesSwitch.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "active")
            {
                ToggleNumberTitles();
            }
        };
        _selectAllButton.OnClicked += (sender, e) =>
        {
            foreach (var row in _mediaRows)
            {
                row.Active = true;
            }
        };
        _deselectAllButton.OnClicked += (sender, e) =>
        {
            foreach (var row in _mediaRows)
            {
                row.Active = false;
            }
        };
        //Add Download Button
        _addDownloadButton.OnClicked += AddDownload;
        _addDownloadButton.SetSensitive(false);
        //Shortcut Controller
        _shortcutController = Gtk.ShortcutController.New();
        _shortcutController.SetScope(Gtk.ShortcutScope.Managed);
        _shortcutController.AddShortcut(Gtk.Shortcut.New(Gtk.ShortcutTrigger.ParseString("Escape"), Gtk.CallbackAction.New(OnEscapeKey)));
        AddController(_shortcutController);
        //Load
        _viewStack.SetVisibleChildName("pageUrl");
        SetDefaultWidget(_validateUrlButton);
        if (Directory.Exists(_controller.PreviousSaveFolder))
        {
            _saveFolderString = _controller.PreviousSaveFolder;
        }
        else
        {
            _saveFolderString = UserDirectories.Downloads;
        }
        _saveFolderRow.SetText(Path.GetFileName(_saveFolderString) ?? "");
        _subtitleSwitch.SetActive(_controller.PreviousSubtitleState);
        _speedLimitRow.SetSubtitle($"{_("{0:f1} KiB/s", _controller.CurrentSpeedLimit)} {_("(Configurable in preferences)")}");
    }

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    /// <param name="controller">AddDownloadDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent) : this(Builder.FromFile("add_download_dialog.ui"), controller, parent)
    {
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
                fileType = (_fileTypeRow.GetSelected() == 0 && _controller.HasVideoResolutions) ? MediaFileType.Video : MediaFileType.Audio;
            }
            else
            {
                fileType = (MediaFileType)_fileTypeRow.GetSelected();
                if (!_controller.HasVideoResolutions)
                {
                    fileType += 2;
                }
            }
            return fileType;
        }
    }

    /// <summary>
    /// Presents the dialog
    /// </summary>
    /// <param name="url">A url to validate at startup</param>
    public async Task PresentAsync(string? url = null)
    {
        Present();
        //Validated from startup
        if (!string.IsNullOrEmpty(url))
        {
            _urlRow.SetText(url);
            await SearchUrlAsync(url);
        }
        else
        {
            //Validate Clipboard
            var clipboard = Gdk.Display.GetDefault()!.GetClipboard();
            try
            {
                var clipboardText = await clipboard.ReadTextAsync();
                if (!string.IsNullOrEmpty(clipboardText))
                {
                    var result = Uri.TryCreate(clipboardText, UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
                    if (result)
                    {
                        _urlRow.SetText(clipboardText);
                        _urlRow.SelectRegion(0, -1);
                    }
                }
            }
            catch { }
        }
        //Keyring
        var names = await _controller.GetKeyringCredentialNamesAsync();
        if (names.Count > 0)
        {
            _keyringRow.SetModel(Gtk.StringList.New(names.ToArray()));
            _keyringRow.SetSelected(names.Count > 1 ? 1u : 0u);
        }
        else
        {
            _keyringRow.SetVisible(false);
            _usernameRow.SetVisible(true);
            _passwordRow.SetVisible(true);
        }
    }

    /// <summary>
    /// Occurs when the escape key is pressed on the window
    /// </summary>
    /// <param name="sender">Gtk.Widget</param>
    /// <param name="e">GLib.Variant</param>
    private bool OnEscapeKey(Gtk.Widget sender, GLib.Variant e)
    {
        if (_viewStack.GetVisibleChildName() == "pagePlaylist")
        {
            _viewStack.SetVisibleChildName("pageDownload");
        }
        else
        {
            Close();
        }
        return true;
    }

    /// <summary>
    /// Searches for information about a URL in the dialog
    /// </summary>
    /// <param name="url">The URL to search</param>
    private async Task SearchUrlAsync(string url)
    {
        _urlSpinner = Gtk.Spinner.New();
        _validateUrlButton.SetSensitive(false);
        _validateUrlButton.SetChild(_urlSpinner);
        _urlSpinner.Start();
        await Task.Run(async () =>
        {
            try
            {
                if (_keyringRow.GetSelected() == 0 || _keyringRow.GetSelected() == GTK_INVALID_LIST_POSITION || !_authRow.GetEnableExpansion())
                {
                    await _controller.SearchUrlAsync(url, _usernameRow.GetText(), _passwordRow.GetText());
                }
                else
                {
                    await _controller.SearchUrlAsync(url, ((int)_keyringRow.GetSelected()) - 1);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.StackTrace);
            }
        });
        _urlSpinner.Stop();
        _urlRow.RemoveCssClass("error");
        _urlRow.SetTitle(_("Media URL"));
        _validateUrlButton.SetSensitive(true);
        _validateUrlButton.SetChild(null);
        _validateUrlButton.SetLabel(_("Validate"));
        if (!_controller.HasMediaInfo)
        {
            _urlRow.AddCssClass("error");
            _urlRow.SetTitle(_("Media URL (Invalid)"));
            if (_authRow.GetEnableExpansion())
            {
                _toastOverlay.AddToast(Adw.Toast.New(_("Ensure credentials are correct.")));
            }
        }
        else
        {
            if (!_controller.HasVideoResolutions)
            {
                if (_controller.DisallowConversions)
                {
                    _fileTypeRow.SetModel(Gtk.StringList.New(new string[] { _("Audio") }));
                    _fileTypeRow.SetSelected(0);
                }
                else
                {
                    _fileTypeRow.SetModel(Gtk.StringList.New(new string[] { "MP3", "OPUS", "FLAC", "WAV" }));
                    _fileTypeRow.SetSelected((uint)Math.Max((int)_controller.PreviousMediaFileType - 2, 0));
                }
            }
            else
            {
                if (_controller.DisallowConversions)
                {
                    _fileTypeRow.SetModel(Gtk.StringList.New(new string[] { _("Video"), _("Audio") }));
                    _fileTypeRow.SetSelected(0);
                }
                else
                {
                    _fileTypeRow.SetSelected((uint)_controller.PreviousMediaFileType);
                }
            }
            if (_controller.AudioLanguages.Count > 1)
            {
                _audioLanguageRow.SetVisible(true);
                _audioLanguageRow.SetModel(Gtk.StringList.New(_controller.AudioLanguages.ToArray()));
            }
            SetQualityRowModel(); // in case _fileTypeRow.SetSelected didn't invoke OnNotify
            _downloadPage.SetVisible(true);
            _viewStack.SetVisibleChildName("pageDownload");
            SetDefaultWidget(_addDownloadButton);
            _addDownloadButton.SetSensitive(Directory.Exists(_saveFolderString));
            _numberTitlesRow.SetVisible(_controller.MediaList.Count > 1 ? true : false);
            if (_controller.MediaList.Count > 1)
            {
                foreach (var mediaInfo in _controller.MediaList)
                {
                    var row = new MediaRow(mediaInfo, _controller.LimitCharacters);
                    _mediaRows.Add(row);
                    row.OnSelectionChanged += PlaylistChanged;
                    _playlistGroup.Add(row);
                }
                _downloadTimeframeRow.SetVisible(false);
                _openPlaylistGroup.SetVisible(true);
                _openPlaylistRow.SetTitle(_n("{0} of {1} items", "{0} of {1} items", _controller.MediaList.Count, _controller.MediaList.Count, _controller.MediaList.Count));
                _qualityRow.SetTitle(_("Maximum Quality"));
                if (_controller.NumberTitles)
                {
                    _numberTitlesSwitch.SetActive(true);
                }
            }
            else
            {
                var row = new MediaRow(_controller.MediaList[0], _controller.LimitCharacters);
                row.SetActivatesDefault(true);
                _mediaRows.Add(row);
                _mediaGroup.SetVisible(true);
                _mediaGroup.Add(row);
            }
            ValidateOptions();
        }
    }

    /// <summary>
    /// Validate download options
    /// </summary>
    private void ValidateOptions()
    {
        _saveFolderRow.RemoveCssClass("error");
        _saveFolderRow.SetTitle(_("Save Folder"));
        _timeframeStartRow.RemoveCssClass("error");
        _timeframeStartRow.SetTitle(_("Start Time"));
        _timeframeEndRow.RemoveCssClass("error");
        _timeframeEndRow.SetTitle(_("End Time"));
        _addDownloadButton.SetSensitive(false);
        var status = _controller.ValidateDownloadOptions(_saveFolderString, _downloadTimeframeRow.GetExpanded(), _timeframeStartRow.GetText(), _timeframeEndRow.GetText(), _controller.MediaList[0].Duration);
        if (status == DownloadOptionsCheckStatus.Valid)
        {
            _addDownloadButton.SetSensitive(true);
            return;
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidSaveFolder))
        {
            _saveFolderRow.SetTitle(_("Save Folder (Invalid)"));
            _saveFolderRow.AddCssClass("error");
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidTimeframeStart))
        {
            _timeframeStartRow.SetTitle(_("Start Time (Invalid)"));
            _timeframeStartRow.AddCssClass("error");
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidTimeframeEnd))
        {
            _timeframeEndRow.SetTitle(_("End Time (Invalid)"));
            _timeframeEndRow.AddCssClass("error");
        }
    }

    /// <summary>
    /// Sets _qualityRow model depending on file type
    /// </summary>
    private void SetQualityRowModel()
    {
        if (SelectedMediaFileType.GetIsVideo())
        {
            _qualityRow.SetModel(Gtk.StringList.New(_controller.VideoResolutions.ToArray()));
            var findPrevious = _controller.PreviousVideoResolutionIndex;
            if (findPrevious != -1)
            {
                _qualityRow.SetSelected((uint)findPrevious);
            }
            _subtitleRow.SetSensitive(true);
        }
        else
        {
            _qualityRow.SetModel(Gtk.StringList.New(_audioQualities));
            _subtitleRow.SetSensitive(false);
        }
    }

    /// <summary>
    /// Occurs when the select save folder button is clicked
    /// </summary>
    private async Task SelectSaveFolderAsync()
    {
        var folderDialog = Gtk.FileDialog.New();
        folderDialog.SetTitle(_("Select Save Folder"));
        if (Directory.Exists(_saveFolderString) && _saveFolderString != "/" && string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")))
        {
            var folder = Gio.FileHelper.NewForPath(_saveFolderString);
            folderDialog.SetInitialFolder(folder);
        }
        try
        {
            var file = await folderDialog.SelectFolderAsync(this);
            _saveFolderString = file.GetPath();
            _saveFolderRow.SetText(Path.GetFileName(_saveFolderString));
            _saveFolderRow.RemoveCssClass("error");
            _addDownloadButton.SetSensitive(true);
        }
        catch { }
        ValidateOptions();
    }

    /// <summary>
    /// Occurs when the number of items selected to download has changed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void PlaylistChanged(object? sender, EventArgs e)
    {
        var downloadsCount = _controller.MediaList.FindAll(x => x.ToDownload).Count;
        _openPlaylistRow.SetTitle(_n("{0} of {1} items", "{0} of {1} items", _controller.MediaList.Count, downloadsCount, _controller.MediaList.Count));
        _addDownloadButton.SetSensitive(downloadsCount > 0 && Directory.Exists(_saveFolderString));
    }

    /// <summary>
    /// Occurs when the number titles toggle button is clicked
    /// </summary>
    private void ToggleNumberTitles()
    {
        if (_controller.ToggleNumberTitles(_numberTitlesSwitch.GetActive()))
        {
            foreach (var row in _mediaRows)
            {
                row.UpdateTitle(_numberTitlesSwitch.GetActive());
            }
        }
    }

    /// <summary>
    /// Occurs when the add download button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private async void AddDownload(Gtk.Button sender, EventArgs e)
    {
        Quality quality;
        int? resolutionIndex;
        if (SelectedMediaFileType.GetIsAudio())
        {
            quality = (Quality)_qualityRow.GetSelected();
            resolutionIndex = null;
        }
        else
        {
            quality = Quality.Resolution;
            resolutionIndex = (int)_qualityRow.GetSelected();
        }
        string? audioLanguage = null;
        if (_controller.AudioLanguages.Count > 1)
        {
            audioLanguage = _controller.AudioLanguages[(int)_audioLanguageRow.GetSelected()];
        }
        Timeframe? timeframe = null;
        if (_downloadTimeframeRow.GetExpanded())
        {
            try
            {
                timeframe = Timeframe.Parse(_timeframeStartRow.GetText(), _timeframeEndRow.GetText(), _controller.MediaList[0].Duration);
            }
            catch { }
        }
        if (_keyringRow.GetSelected() == 0 || _keyringRow.GetSelected() == GTK_INVALID_LIST_POSITION || !_authRow.GetEnableExpansion())
        {
            _controller.PopulateDownloads(SelectedMediaFileType, quality, resolutionIndex, audioLanguage,
                _subtitleSwitch.GetActive(), _saveFolderString, _speedLimitSwitch.GetActive(), _splitChaptersSwitch.GetActive(),
                _cropThumbnailSwitch.GetActive(), timeframe, _usernameRow.GetText(), _passwordRow.GetText());
        }
        else
        {
            await _controller.PopulateDownloadsAsync(SelectedMediaFileType, quality, resolutionIndex, audioLanguage,
                _subtitleSwitch.GetActive(), _saveFolderString, _speedLimitSwitch.GetActive(), _splitChaptersSwitch.GetActive(),
                _cropThumbnailSwitch.GetActive(), timeframe, ((int)_keyringRow.GetSelected()) - 1);
        }
        OnDownload?.Invoke(this, EventArgs.Empty);
    }
}
