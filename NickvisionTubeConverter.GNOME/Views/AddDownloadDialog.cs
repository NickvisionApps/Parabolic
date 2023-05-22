using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public partial class AddDownloadDialog : Adw.Window
{
    private delegate bool GSourceFunc(nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_main_context_invoke(nint context, GSourceFunc function, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool gtk_file_chooser_set_current_folder(nint chooser, nint file, nint error);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    public static partial void gtk_file_chooser_set_current_name(nint chooser, string name);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial string g_file_get_path(nint file);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_new();

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_title(nint dialog, string title);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_filters(nint dialog, nint filters);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_initial_name(nint dialog, string name);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_initial_folder(nint dialog, nint folder);

    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_select_folder(nint dialog, nint parent, nint cancellable, GAsyncReadyCallback callback, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_select_folder_finish(nint dialog, nint result, nint error);

    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;
    private MediaUrlInfo? _mediaUrlInfo;
    private string _saveFolderString;
    private GAsyncReadyCallback? _saveCallback;
    private GSourceFunc _startSearchCallback;
    private GSourceFunc _finishSearchCallback;
    private readonly Gtk.ShortcutController _shortcutController;

    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Gtk.Button _validateUrlButton;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _downloadPage;
    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _scrolledWindow;
    [Gtk.Connect] private readonly Adw.ComboRow _fileTypeRow;
    [Gtk.Connect] private readonly Adw.ComboRow _qualityRow;
    [Gtk.Connect] private readonly Adw.ComboRow _subtitleRow;
    [Gtk.Connect] private readonly Adw.EntryRow _saveFolderRow;
    [Gtk.Connect] private readonly Gtk.Button _selectSaveFolderButton;
    [Gtk.Connect] private readonly Gtk.Switch _overwriteSwitch;
    [Gtk.Connect] private readonly Adw.ActionRow _speedLimitRow;
    [Gtk.Connect] private readonly Gtk.Switch _speedLimitSwitch;
    [Gtk.Connect] private readonly Adw.ActionRow _cropThumbnailRow;
    [Gtk.Connect] private readonly Gtk.Switch _cropThumbnailSwitch;
    [Gtk.Connect] private readonly Adw.ExpanderRow _downloadTimeframeRow;
    [Gtk.Connect] private readonly Adw.EntryRow _timeframeStartRow;
    [Gtk.Connect] private readonly Adw.EntryRow _timeframeEndRow;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _mediaGroup;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _openPlaylistGroup;
    [Gtk.Connect] private readonly Adw.ActionRow _openPlaylistRow;
    [Gtk.Connect] private readonly Gtk.Box _playlistPage;
    [Gtk.Connect] private readonly Gtk.ToggleButton _numberTitlesButton;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _playlist;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _playlistGroup;
    private Gtk.Spinner? _urlSpinner;
    private readonly List<MediaRow> _mediaRows;
    private readonly string[] _audioQualityArray;
    private List<string>? _videoQualityList;
    private bool _audioOnly;
    private double _singleMediaDuration;

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
        _mediaUrlInfo = null;
        _saveCallback = null;
        _mediaRows = new List<MediaRow>();
        _audioOnly = false;
        _singleMediaDuration = 0;
        _audioQualityArray = new string[] { _controller.Localizer["Quality", "Best"], _controller.Localizer["Quality", "Worst"] };
        _startSearchCallback = (x) =>
        {
            _urlSpinner = Gtk.Spinner.New();
            _validateUrlButton.SetSensitive(false);
            _validateUrlButton.SetChild(_urlSpinner);
            _urlSpinner.Start();
            return false;
        };
        _finishSearchCallback = (x) =>
        {
            _urlSpinner.Stop();
            _validateUrlButton.SetSensitive(true);
            _validateUrlButton.SetChild(null);
            _validateUrlButton.SetLabel(_controller.Localizer["ValidateUrl"]);
            if (_mediaUrlInfo == null)
            {
                _urlRow.AddCssClass("error");
                _urlRow.SetTitle(_controller.Localizer["MediaUrl", "Invalid"]);
            }
            else
            {
                _videoQualityList = new List<string> {};
                if (_mediaUrlInfo.VideoResolutions.Count == 0)
                {
                    _audioOnly = true;
                    _fileTypeRow.SetModel(Gtk.StringList.New(new string[] {"MP3", "OPUS", "FLAC", "WAV"}));
                    _fileTypeRow.SetSelected((uint)Math.Max((int)_controller.PreviousMediaFileType - 2, 0));
                }
                else
                {
                    foreach (var resolution in _mediaUrlInfo.VideoResolutions)
                    {
                        _videoQualityList.Add(resolution.ToString());
                    }
                    _fileTypeRow.SetSelected((uint)_controller.PreviousMediaFileType);
                }
                SetQualityRowModel(); // in case _fileTypeRow.SetSelected didn't invoke OnNotify
                _urlRow.RemoveCssClass("error");
                _urlRow.SetTitle(_controller.Localizer["MediaUrl", "Field"]);
                _downloadPage.SetVisible(true);
                _viewStack.SetVisibleChildName("pageDownload");
                SetDefaultWidget(_addDownloadButton);
                _addDownloadButton.SetSensitive(!string.IsNullOrEmpty(_saveFolderRow.GetText()));
                _numberTitlesButton.SetVisible(_mediaUrlInfo.MediaList.Count > 1 ? true : false);
                if (_mediaUrlInfo.MediaList.Count > 1)
                {
                    foreach (var mediaInfo in _mediaUrlInfo.MediaList)
                    {
                        var row = new MediaRow(mediaInfo, _controller.Localizer);
                        _mediaRows.Add(row);
                        row.OnSelectionChanged += PlaylistChanged;
                        _playlistGroup.Add(row);
                    }
                    _downloadTimeframeRow.SetVisible(false);
                    _openPlaylistGroup.SetVisible(true);
                    _openPlaylistRow.SetTitle(string.Format(_controller.Localizer["Playlist", "Count"], _mediaUrlInfo.MediaList.Count, _mediaUrlInfo.MediaList.Count));
                    _qualityRow.SetTitle(_controller.Localizer["MaxQuality", "Field"]);
                }
                else
                {
                    _singleMediaDuration = _mediaUrlInfo.MediaList[0].Duration;
                    var row = new MediaRow(_mediaUrlInfo.MediaList[0], _controller.Localizer);
                    _mediaRows.Add(row);
                    _mediaGroup.SetVisible(true);
                    _mediaGroup.Add(row);
                }
            }
            ValidateOptions();
            return false;
        };
        //Dialog Settings
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        _validateUrlButton.OnClicked += SearchUrl;
        _viewStack.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "visible-child")
            {
                var isPagePlaylist = _viewStack.GetVisibleChildName() == "pagePlaylist";
                _backButton.SetVisible(isPagePlaylist);
                _titleLabel.SetLabel(_controller.Localizer[isPagePlaylist ? "Playlist" : "AddDownload"]);
            }
        };
        _backButton.OnClicked += (sender, e) =>
        {
            _viewStack.SetVisibleChildName("pageDownload");
            SetDefaultWidget(_addDownloadButton);
        };
        var vadjustment = _scrolledWindow.GetVadjustment();
        vadjustment.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "upper")
            {
                if (vadjustment.GetPageSize() < vadjustment.GetUpper())
                {
                    _scrolledWindow.AddCssClass("scrolled-window");
                }
            }
        };
        _fileTypeRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                SetQualityRowModel();
            }
        };
        _selectSaveFolderButton.OnClicked += SelectSaveFolder;
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
        _cropThumbnailRow.SetVisible(_controller.EmbedMetadata);
        _downloadTimeframeRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "expanded")
            {
                if (_downloadTimeframeRow.GetExpanded())
                {
                    _speedLimitSwitch.SetActive(false);
                    _timeframeStartRow.SetText(TimeSpan.FromSeconds(0).ToString(@"hh\:mm\:ss"));
                    _timeframeEndRow.SetText(TimeSpan.FromSeconds(_singleMediaDuration).ToString(@"hh\:mm\:ss"));
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
        _openPlaylistRow.OnActivated += (sender, e) => _viewStack.SetVisibleChildName("pagePlaylist");
        _numberTitlesButton.OnClicked += ToggleNumberTitles;
        _playlist.GetVadjustment().OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "value")
            {
                if (_playlist.GetVadjustment().GetValue() > 0)
                {
                    _playlist.AddCssClass("playlist");
                }
                else
                {
                    _playlist.RemoveCssClass("playlist");
                }
            }
        };
        //Add Download Button
        _addDownloadButton.OnClicked += (sender, e) =>
        {
            Quality quality;
            VideoResolution? resolution;
            var fileType = (MediaFileType)_fileTypeRow.GetSelected();
            if (_audioOnly)
            {
                fileType += 2;
            }
            if (fileType.GetIsAudio())
            {
                quality = (Quality)_qualityRow.GetSelected();
                resolution = null;
            }
            else
            {
                quality = Quality.Resolution;
                resolution = _mediaUrlInfo.VideoResolutions[(int)_qualityRow.GetSelected()];
            }
            _controller.PopulateDownloads(_mediaUrlInfo!, fileType, quality, resolution, (Subtitle)_subtitleRow.GetSelected(), _saveFolderString, _overwriteSwitch.GetActive(), _speedLimitSwitch.GetActive(), _cropThumbnailSwitch.GetActive());
            OnDownload?.Invoke(this, EventArgs.Empty);
        };
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
            _saveFolderString = "";
        }
        _saveFolderRow.SetText(Path.GetFileName(_saveFolderString) ?? "");
        _speedLimitRow.SetSubtitle($"{string.Format(_controller.Localizer["Speed", "KiBps"], _controller.CurrentSpeedLimit)} ({_controller.Localizer["Configurable", "GTK"]})");
    }

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    /// <param name="controller">AddDownloadDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent) : this(Builder.FromFile("add_download_dialog.ui", controller.Localizer), controller, parent)
    {
    }

    /// <summary>
    /// Occurs when the media url is changed
    /// </summary>
    /// <param name="sender">Adw.EntryRow</param>
    /// <param name="e">EventArgs</param>
    private async void SearchUrl(Gtk.Button sender, EventArgs e)
    {
        g_main_context_invoke(0, _startSearchCallback, 0);
        await Task.Run(async () =>
        {
            try
            {
                _mediaUrlInfo = await _controller.SearchUrlAsync(_urlRow.GetText());
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.StackTrace);
            }
        });
        g_main_context_invoke(0, _finishSearchCallback, 0);
    }

    /// <summary>
    /// Validate download options
    /// </summary>
    private void ValidateOptions()
    {
        _saveFolderRow.RemoveCssClass("error");
        _saveFolderRow.SetTitle(_controller.Localizer["SaveFolder.Field"]);
        _timeframeStartRow.RemoveCssClass("error");
        _timeframeStartRow.SetTitle(_controller.Localizer["DownloadTimeframeStart.Field"]);
        _timeframeEndRow.RemoveCssClass("error");
        _timeframeEndRow.SetTitle(_controller.Localizer["DownloadTimeframeEnd.Field"]);
        _addDownloadButton.SetSensitive(false);
        var status = _controller.CheckDownloadOptions(_saveFolderString, _downloadTimeframeRow.GetExpanded(), _timeframeStartRow.GetText(), _timeframeEndRow.GetText(), _singleMediaDuration);
        if (status == DownloadOptionsCheckStatus.Valid)
        {
            _addDownloadButton.SetSensitive(true);
            return;
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidSaveFolder))
        {
            _saveFolderRow.SetTitle(_controller.Localizer["SaveFolder.Invalid"]);
            _saveFolderRow.AddCssClass("error");
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidTimeframeStart))
        {
            _timeframeStartRow.SetTitle(_controller.Localizer["DownloadTimeframeStart.Invalid"]);
            _timeframeStartRow.AddCssClass("error");
        }
        if (status.HasFlag(DownloadOptionsCheckStatus.InvalidTimeframeEnd))
        {
            _timeframeEndRow.SetTitle(_controller.Localizer["DownloadTimeframeEnd.Invalid"]);
            _timeframeEndRow.AddCssClass("error");
        }
    }

    /// <summary>
    /// Set _qualityRow model depending on file type
    /// </summary>
    private void SetQualityRowModel()
    {
        if (((MediaFileType)_fileTypeRow.GetSelected()).GetIsVideo() && !_audioOnly)
        {
            _qualityRow.SetModel(Gtk.StringList.New(_videoQualityList.ToArray()));
            _subtitleRow.SetSensitive(true);
        }
        else
        {
            _qualityRow.SetModel(Gtk.StringList.New(_audioQualityArray));
            _subtitleRow.SetSensitive(false);
        }
    }

    /// <summary>
    /// Occurs when the select save folder button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void SelectSaveFolder(Gtk.Button sender, EventArgs e)
    {
        var folderDialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(folderDialog, _controller.Localizer["SelectSaveFolder"]);
        if (Directory.Exists(_saveFolderString) && _saveFolderString != "/")
        {
            var folder = Gio.FileHelper.NewForPath(_saveFolderString);
            gtk_file_dialog_set_initial_folder(folderDialog, folder.Handle);
        }
        _saveCallback = (source, res, data) =>
        {
            var fileHandle = gtk_file_dialog_select_folder_finish(folderDialog, res, IntPtr.Zero);
            if (fileHandle != IntPtr.Zero)
            {
                _saveFolderString = g_file_get_path(fileHandle);
                _saveFolderRow.SetText(Path.GetFileName(_saveFolderString));
                _saveFolderRow.SetTitle(_controller.Localizer["SaveFolder.Field"]);
                _saveFolderRow.RemoveCssClass("error");
                _addDownloadButton.SetSensitive(true);
            }
            ValidateOptions();
        };
        gtk_file_dialog_select_folder(folderDialog, Handle, IntPtr.Zero, _saveCallback, IntPtr.Zero);
    }

    /// <summary>
    /// Occurs when the number of items selected to download has changed
    /// </summary>
    private void PlaylistChanged()
    {
        var downloadsCount = _mediaUrlInfo.MediaList.FindAll(x => x.ToDownload).Count;
         _openPlaylistRow.SetTitle(string.Format(_controller.Localizer["Playlist", "Count"], downloadsCount, _mediaUrlInfo.MediaList.Count));
    }

    /// <summary>
    /// Occurs when the number titles toggle button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void ToggleNumberTitles(Gtk.Button sender, EventArgs e)
    {
        _controller.ToggleNumberTitles(_mediaUrlInfo!, _numberTitlesButton.GetActive());
        foreach (var row in _mediaRows)
        {
            row.UpdateTitle(_numberTitlesButton.GetActive());
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
            SetDefaultWidget(_addDownloadButton);
            _playlistPage.SetVisible(false);
        }
        else
        {
            Close();
        }
        return true;
    }
}
