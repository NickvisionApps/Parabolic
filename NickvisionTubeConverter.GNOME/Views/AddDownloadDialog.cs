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
    private VideoUrlInfo? _videoUrlInfo;
    private GAsyncReadyCallback? _saveCallback;
    private GSourceFunc _startSearchCallback;
    private GSourceFunc _finishSearchCallback;
    private readonly Gtk.ShortcutController _shortcutController;

    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Gtk.Button _validateUrlButton;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _downloadPage;
    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Gtk.DropDown _fileTypeDropDown;
    [Gtk.Connect] private readonly Gtk.DropDown _qualityDropDown;
    [Gtk.Connect] private readonly Gtk.DropDown _subtitlesDropDown;
    [Gtk.Connect] private readonly Adw.EntryRow _saveFolderRow;
    [Gtk.Connect] private readonly Gtk.Button _selectSaveFolderButton;
    [Gtk.Connect] private readonly Gtk.Switch _overwriteSwitch;
    [Gtk.Connect] private readonly Adw.ActionRow _speedLimitRow;
    [Gtk.Connect] private readonly Gtk.Switch _speedLimitSwitch;
    [Gtk.Connect] private readonly Gtk.ToggleButton _numberVideosButton;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _playlist;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _videosGroup;
    private Gtk.Spinner? _urlSpinner;
    private readonly List<VideoRow> _videoRows;

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
        _videoUrlInfo = null;
        _saveCallback = null;
        _videoRows = new List<VideoRow>();
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
            if (_videoUrlInfo == null)
            {
                _urlRow.AddCssClass("error");
                _urlRow.SetTitle(_controller.Localizer["VideoUrl", "Invalid"]);
            }
            else
            {
                _urlRow.RemoveCssClass("error");
                _urlRow.SetTitle(_controller.Localizer["VideoUrl", "Field"]);
                _downloadPage.SetVisible(true);
                _viewStack.SetVisibleChildName("pageDownload");
                SetDefaultWidget(_addDownloadButton);
                _addDownloadButton.SetSensitive(!string.IsNullOrEmpty(_saveFolderRow.GetText()));
                _numberVideosButton.SetVisible(_videoUrlInfo.Videos.Count > 1 ? true : false);
                foreach (var row in _videoRows)
                {
                    _videosGroup.Remove(row);
                }
                foreach (var videoInfo in _videoUrlInfo.Videos)
                {
                    var row = new VideoRow(videoInfo, _controller.Localizer);
                    _videoRows.Add(row);
                    _videosGroup.Add(row);
                }
                _playlist.GetVadjustment().OnNotify += (sender, e) =>
                {
                    if (e.Pspec.GetName() == "page-size")
                    {
                        if (_playlist.GetVadjustment().GetPageSize() < _playlist.GetVadjustment().GetUpper())
                        {
                            _playlist.AddCssClass("playlist");
                        }
                        else
                        {
                            _playlist.RemoveCssClass("playlist");
                        }
                    }
                };
            }
            return false;
        };
        //Dialog Settings
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        _validateUrlButton.OnClicked += SearchUrl;
        _backButton.OnClicked += (sender, e) =>
        {
            _viewStack.SetVisibleChildName("pageUrl");
            SetDefaultWidget(_validateUrlButton);
            _downloadPage.SetVisible(false);
            _urlRow.SetText("");
            _addDownloadButton.SetSensitive(false);
        };
        _fileTypeDropDown.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                _subtitlesDropDown.SetSensitive(((MediaFileType)_fileTypeDropDown.GetSelected()).GetIsVideo());
            }
        };
        _selectSaveFolderButton.OnClicked += SelectSaveFolder;
        _numberVideosButton.OnClicked += ToggleNumberVideos;
        //Add Download Button
        _addDownloadButton.OnClicked += (sender, e) =>
        {
            _controller.PopulateDownloads(_videoUrlInfo!, (MediaFileType)_fileTypeDropDown.GetSelected(), (Quality)_qualityDropDown.GetSelected(), (Subtitle)_subtitlesDropDown.GetSelected(), _saveFolderRow.GetText(), _overwriteSwitch.GetActive(), _speedLimitSwitch.GetActive());
            OnDownload?.Invoke(this, EventArgs.Empty);
        };
        _addDownloadButton.SetSensitive(false);
        //Shotcut Controller
        _shortcutController = Gtk.ShortcutController.New();
        _shortcutController.SetScope(Gtk.ShortcutScope.Managed);
        _shortcutController.AddShortcut(Gtk.Shortcut.New(Gtk.ShortcutTrigger.ParseString("Escape"), Gtk.CallbackAction.New(OnEscapeKey)));
        AddController(_shortcutController);
        //Load
        _viewStack.SetVisibleChildName("pageUrl");
        SetDefaultWidget(_validateUrlButton);
        _fileTypeDropDown.SetSelected((uint)_controller.PreviousMediaFileType);
        _saveFolderRow.SetText(_controller.PreviousSaveFolder);
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
    /// Occurs when the video url is changed
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
                _videoUrlInfo = await _controller.SearchUrlAsync(_urlRow.GetText());
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
    /// Occurs when the select save folder button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void SelectSaveFolder(Gtk.Button sender, EventArgs e)
    {
        var folderDialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(folderDialog, _controller.Localizer["SelectSaveFolder"]);
        if (Directory.Exists(_saveFolderRow.GetText()) && _saveFolderRow.GetText() != "/")
        {
            var folder = Gio.FileHelper.NewForPath(_saveFolderRow.GetText());
            gtk_file_dialog_set_initial_folder(folderDialog, folder.Handle);
        }
        _saveCallback = (source, res, data) =>
        {
            var fileHandle = gtk_file_dialog_select_folder_finish(folderDialog, res, IntPtr.Zero);
            if (fileHandle != IntPtr.Zero)
            {
                var path = g_file_get_path(fileHandle);
                _saveFolderRow.SetText(path);
                _addDownloadButton.SetSensitive(true);
            }
        };
        gtk_file_dialog_select_folder(folderDialog, Handle, IntPtr.Zero, _saveCallback, IntPtr.Zero);
    }

    /// <summary>
    /// Occurs when the number videos toggle button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void ToggleNumberVideos(Gtk.Button sender, EventArgs e)
    {
        _controller.ToggleNumberVideos(_videoUrlInfo!, _numberVideosButton.GetActive());
        foreach (var row in _videoRows)
        {
            row.UpdateTitle(_numberVideosButton.GetActive());
        }
    }

    /// <summary>
    /// Occurs when the escape key is pressed on the window
    /// </summary>
    /// <param name="sender">Gtk.Widget</param>
    /// <param name="e">GLib.Variant</param>
    private bool OnEscapeKey(Gtk.Widget sender, GLib.Variant e)
    {
        if (_viewStack.GetVisibleChildName() == "pageUrl")
        {
            Close();
        }
        else if (_viewStack.GetVisibleChildName() == "pageDownload")
        {
            _viewStack.SetVisibleChildName("pageUrl");
            SetDefaultWidget(_validateUrlButton);
            _downloadPage.SetVisible(false);
            _urlRow.SetText("");
            _addDownloadButton.SetSensitive(false);
        }
        return true;
    }
}