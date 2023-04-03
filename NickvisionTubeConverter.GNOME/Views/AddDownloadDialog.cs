using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public partial class AddDownloadDialog : Adw.Window
{
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

    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Gtk.Button _validateUrlButton;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _downloadPage;
    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Adw.ComboRow _fileTypeRow;
    [Gtk.Connect] private readonly Adw.ComboRow _qualityRow;
    [Gtk.Connect] private readonly Adw.ComboRow _subtitleRow;
    [Gtk.Connect] private readonly Adw.EntryRow _saveFolderRow;
    [Gtk.Connect] private readonly Gtk.Button _selectSaveFolderButton;
    [Gtk.Connect] private readonly Gtk.Switch _overwriteSwitch;
    [Gtk.Connect] private readonly Gtk.ToggleButton _numberVideosButton;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _playlist;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _videosGroup;
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
        //Dialog Settings
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        _validateUrlButton.OnClicked += SearchUrl;
        _backButton.OnClicked += (sender, e) =>
        {
            _viewStack.SetVisibleChildName("pageUrl");
            _downloadPage.SetVisible(false);
            _urlRow.SetText("");
            _addDownloadButton.SetSensitive(false);
        };
        _fileTypeRow.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                _subtitleRow.SetSensitive(((MediaFileType)_fileTypeRow.GetSelected()).GetIsVideo());
            }
        };
        _selectSaveFolderButton.OnClicked += SelectSaveFolder;
        _numberVideosButton.OnClicked += ToggleNumberVideos;
        //Add Download Button
        _addDownloadButton.OnClicked += (sender, e) => {
            _controller.PopulateDownloads(_videoUrlInfo!, (MediaFileType)_fileTypeRow.GetSelected(), (Quality)_qualityRow.GetSelected(), (Subtitle)_subtitleRow.GetSelected(), _saveFolderRow.GetText(), _overwriteSwitch.GetActive());
            OnDownload?.Invoke(this, EventArgs.Empty);
        };
        _addDownloadButton.SetSensitive(false);
        //Load
        _viewStack.SetVisibleChildName("pageUrl");
        _fileTypeRow.SetSelected((uint)_controller.PreviousMediaFileType);
        _saveFolderRow.SetText(_controller.PreviousSaveFolder);
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
        var urlSpinner = Gtk.Spinner.New();
        _validateUrlButton.SetSensitive(false);
        _validateUrlButton.SetChild(urlSpinner);
        urlSpinner.Start();
        _videoUrlInfo = await _controller.SearchUrlAsync(_urlRow.GetText());
        urlSpinner.Stop();
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
                    if (_playlist.GetVadjustment().GetPageSize() >= _playlist.GetMaxContentHeight())
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
            row.UpdateTitle();
        }
    }
}