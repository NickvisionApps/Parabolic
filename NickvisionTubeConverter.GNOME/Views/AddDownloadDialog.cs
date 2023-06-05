using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public partial class AddDownloadDialog : Adw.Window
{
    private delegate bool GSourceFunc(nint data);
    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_main_context_invoke(nint context, GSourceFunc function, nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial string g_file_get_path(nint file);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_new();
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_title(nint dialog, string title);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_initial_folder(nint dialog, nint folder);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_select_folder(nint dialog, nint parent, nint cancellable, GAsyncReadyCallback callback, nint user_data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_select_folder_finish(nint dialog, nint result, nint error);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gdk_clipboard_read_text_async(nint clipboard, nint cancellable, GAsyncReadyCallback callback, nint user_data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial string gdk_clipboard_read_text_finish(nint clipboard, nint result, nint error);

    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;
    private MediaUrlInfo? _mediaUrlInfo;
    private string _saveFolderString;
    private GAsyncReadyCallback? _saveCallback;
    private GSourceFunc _startSearchCallback;
    private GSourceFunc _finishSearchCallback;
    private GAsyncReadyCallback _clipboardCallback;
    private readonly Gtk.ShortcutController _shortcutController;

    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
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
    [Gtk.Connect] private readonly Adw.ActionRow _openAdvancedRow;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _mediaGroup;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _openPlaylistGroup;
    [Gtk.Connect] private readonly Adw.ActionRow _openPlaylistRow;
    [Gtk.Connect] private readonly Gtk.Box _playlistPage;
    [Gtk.Connect] private readonly Gtk.ToggleButton _numberTitlesButton;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _playlist;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _playlistGroup;
    [Gtk.Connect] private readonly Gtk.Box _advancedPage;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _advanced;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _advancedGroup;
    [Gtk.Connect] private readonly Adw.ActionRow _speedLimitRow;
    [Gtk.Connect] private readonly Gtk.Switch _speedLimitSwitch;
    [Gtk.Connect] private readonly Adw.ActionRow _cropThumbnailRow;
    [Gtk.Connect] private readonly Gtk.Switch _cropThumbnailSwitch;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _authGroup;
    [Gtk.Connect] private readonly Adw.EntryRow _usernameRow;
    [Gtk.Connect] private readonly Adw.PasswordEntryRow _passwordRow;
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
        _audioQualityArray = new string[] { _("Best"), _("Worst") };
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
            _validateUrlButton.SetLabel(_("Validate"));
            if (_mediaUrlInfo == null)
            {
                _urlRow.AddCssClass("error");
                _urlRow.SetTitle(_("Media URL (Invalid)"));
            }
            else
            {
                _videoQualityList = new List<string> {};
                if (_mediaUrlInfo.VideoResolutions.Count == 0)
                {
                    _audioOnly = true;
                    if (_controller.DisallowConversions)
                    {
                        _fileTypeRow.SetModel(Gtk.StringList.New(new string[] { _("Audio") }));
                        _fileTypeRow.SetSelected(0);
                    }
                    else
                    {
                        _fileTypeRow.SetModel(Gtk.StringList.New(new string[] {"MP3", "OPUS", "FLAC", "WAV"}));
                        _fileTypeRow.SetSelected((uint)Math.Max((int)_controller.PreviousMediaFileType - 2, 0));
                    }
                }
                else
                {
                    foreach (var resolution in _mediaUrlInfo.VideoResolutions)
                    {
                        _videoQualityList.Add(resolution.ToString());
                    }
                    if(_controller.DisallowConversions)
                    {
                        _fileTypeRow.SetModel(Gtk.StringList.New(new string[] { _("Video"), _("Audio") }));
                        _fileTypeRow.SetSelected(0);
                    }
                    else
                    {
                        _fileTypeRow.SetSelected((uint)_controller.PreviousMediaFileType);
                    }
                }
                SetQualityRowModel(); // in case _fileTypeRow.SetSelected didn't invoke OnNotify
                _urlRow.RemoveCssClass("error");
                _urlRow.SetTitle(_("Media URL"));
                _downloadPage.SetVisible(true);
                _viewStack.SetVisibleChildName("pageDownload");
                SetDefaultWidget(_addDownloadButton);
                _addDownloadButton.SetSensitive(!string.IsNullOrEmpty(_saveFolderRow.GetText()));
                _numberTitlesButton.SetVisible(_mediaUrlInfo.MediaList.Count > 1 ? true : false);
                if (_mediaUrlInfo.MediaList.Count > 1)
                {
                    foreach (var mediaInfo in _mediaUrlInfo.MediaList)
                    {
                        var row = new MediaRow(mediaInfo);
                        _mediaRows.Add(row);
                        row.OnSelectionChanged += PlaylistChanged;
                        _playlistGroup.Add(row);
                    }
                    _openPlaylistGroup.SetVisible(true);
                    _openPlaylistRow.SetTitle(_n("{0} of {1} items", "{0} of {1} items", _mediaUrlInfo.MediaList.Count, _mediaUrlInfo.MediaList.Count, _mediaUrlInfo.MediaList.Count));
                    _qualityRow.SetTitle(_("Maximum Quality"));
                }
                else
                {
                    _singleMediaDuration = _mediaUrlInfo.MediaList[0].Duration;
                    var row = new MediaRow(_mediaUrlInfo.MediaList[0]);
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
        _backButton.OnClicked += (sender, e) =>
        {
            _viewStack.SetVisibleChildName("pageDownload");
            SetDefaultWidget(_addDownloadButton);
        };
        var vadjustment = _scrolledWindow.GetVadjustment();
        vadjustment.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "page-size")
            {
                if (vadjustment.GetPageSize() < vadjustment.GetUpper())
                {
                    _scrolledWindow.AddCssClass("scrolled-window");
                }
                else
                {
                    _scrolledWindow.RemoveCssClass("scrolled-window");
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
        _cropThumbnailRow.SetVisible(_controller.EmbedMetadata);
        _openAdvancedRow.OnActivated += (sender, e) => _viewStack.SetVisibleChildName("pageAdvanced");
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
            MediaFileType fileType;
            if (_controller.DisallowConversions)
            {
                fileType = _fileTypeRow.GetSelected() == 0 && !_audioOnly ? MediaFileType.Video : MediaFileType.Audio;
            }
            else
            {
                fileType = (MediaFileType)_fileTypeRow.GetSelected();
                if (_audioOnly)
                {
                    fileType += 2;
                }
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
            _controller.PopulateDownloads(_mediaUrlInfo!, fileType, quality, resolution, (Subtitle)_subtitleRow.GetSelected(), _saveFolderString, _speedLimitSwitch.GetActive(), _cropThumbnailSwitch.GetActive(), _usernameRow.GetText(), _passwordRow.GetText());
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
    /// Presents the dialog
    /// </summary>
    /// <param name="url">A url to validate at startup</param>
    public async Task PresentAsync(string? url = null)
    {
        base.Present();
        //Validated from startup
        if (!string.IsNullOrEmpty(url))
        {
            await SearchUrlAsync(url);
        }
        else
        {
            //Validate Clipboard
            var clipboard = Gdk.Display.GetDefault()!.GetClipboard();
            _clipboardCallback = (source, res, data) =>
            {
                var clipboardText = gdk_clipboard_read_text_finish(clipboard.Handle, res, IntPtr.Zero);
                if(!string.IsNullOrEmpty(clipboardText))
                {
                    var result = Uri.TryCreate(clipboardText, UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
                    if (result)
                    {
                        _urlRow.SetText(clipboardText);
                    }
                }
            };
            gdk_clipboard_read_text_async(clipboard.Handle, IntPtr.Zero, _clipboardCallback, IntPtr.Zero);
        }
    }

    /// <summary>
    /// Searches for information about a URL in the dialog
    /// </summary>
    /// <param name="url">The URL to search</param>
    private async Task SearchUrlAsync(string url)
    {
        g_main_context_invoke(0, _startSearchCallback, 0);
        await Task.Run(async () =>
        {
            try
            {
                _urlRow.SetText(url);
                _mediaUrlInfo = await _controller.SearchUrlAsync(url);
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
        _saveFolderRow.SetTitle(_("Save Folder"));
        _addDownloadButton.SetSensitive(false);
        var status = _controller.CheckDownloadOptions(_saveFolderString);
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
    }

    /// <summary>
    /// Set _qualityRow model depending on file type
    /// </summary>
    private void SetQualityRowModel()
    {
        var isVideo = false;
        if (_controller.DisallowConversions)
        {
            isVideo = _fileTypeRow.GetSelected() == 0;
        }
        else
        {
            isVideo = ((MediaFileType)_fileTypeRow.GetSelected()).GetIsVideo();
        }
        if (isVideo && !_audioOnly)
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
        gtk_file_dialog_set_title(folderDialog, _("Select Save Folder"));
        if (Directory.Exists(_saveFolderString) && _saveFolderString != "/" && string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")))
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
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void PlaylistChanged(object? sender, EventArgs e)
    {
        var downloadsCount = _mediaUrlInfo.MediaList.FindAll(x => x.ToDownload).Count;
        _openPlaylistRow.SetTitle(_n("{0} of {1} items", "{0} of {1} items", _mediaUrlInfo.MediaList.Count, downloadsCount, _mediaUrlInfo.MediaList.Count));
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
