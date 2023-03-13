using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public partial class AddDownloadDialog : Adw.MessageDialog
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
    [Gtk.Connect] private readonly Gtk.Spinner _urlSpinner;
    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ComboRow _fileTypeRow;
    [Gtk.Connect] private readonly Adw.ComboRow _qualityRow;
    [Gtk.Connect] private readonly Adw.ComboRow _subtitleRow;
    [Gtk.Connect] private readonly Adw.EntryRow _saveFolderRow;
    [Gtk.Connect] private readonly Gtk.MenuButton _saveWarning;
    [Gtk.Connect] private readonly Gtk.Button _selectSaveFolderButton;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _videosGroup;
    private readonly List<Adw.ActionRow> _videoRows;

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
        _videoRows = new List<Adw.ActionRow>();
        //Dialog Settings
        SetTransientFor(parent);
        AddResponse("cancel", controller.Localizer["Cancel"]);
        SetCloseResponse("cancel");
        AddResponse("ok", controller.Localizer["Download"]);
        SetDefaultResponse("ok");
        SetResponseAppearance("ok", Adw.ResponseAppearance.Suggested);
        OnResponse += Response;
        //Build UI
        builder.Connect(this);
        _urlRow.OnApply += SearchUrl;
        _backButton.OnClicked += (sender, e) =>
        {
            _viewStack.SetVisibleChildName("pageUrl");
            _urlRow.SetText("");
            SetResponseEnabled("ok", false);
        };
        _fileTypeRow.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                _subtitleRow.SetSensitive(((MediaFileType)_fileTypeRow.GetSelected()).GetIsVideo());
            }
        };
        _selectSaveFolderButton.OnClicked += SelectSaveFolder;
        //Load
        _viewStack.SetVisibleChildName("pageUrl");
        SetResponseEnabled("ok", false);
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
    /// Occurs when the dialog is closed
    /// </summary>
    /// <param name="sender">Adw.MessageDialog</param>
    /// <param name="e">ResponseSignalArgs</param>
    private void Response(Adw.MessageDialog sender, ResponseSignalArgs e)
    {
        _controller.Accepted = e.Response == "ok";
        if (_controller.Accepted)
        {
            _controller.PopulateDownloads(_videoUrlInfo!, (MediaFileType)_fileTypeRow.GetSelected(), (Quality)_qualityRow.GetSelected(), (Subtitle)_subtitleRow.GetSelected(), _saveFolderRow.GetText());
        }
    }

    /// <summary>
    /// Occurs when the video url is changed
    /// </summary>
    /// <param name="sender">Adw.EntryRow</param>
    /// <param name="e">EventArgs</param>
    private async void SearchUrl(Adw.EntryRow sender, EventArgs e)
    {
        _urlSpinner.Start();
        SetResponseEnabled("ok", false);
        _videoUrlInfo = await _controller.SearchUrlAsync(_urlRow.GetText());
        _urlSpinner.Stop();
        if (_videoUrlInfo == null)
        {
            _urlRow.AddCssClass("error");
            _urlRow.SetTitle(_controller.Localizer["VideoUrl", "Invalid"]);
        }
        else
        {
            _urlRow.RemoveCssClass("error");
            _urlRow.SetTitle(_controller.Localizer["VideoUrl", "Field"]);
            _viewStack.SetVisibleChildName("pageDownload");
            SetResponseEnabled("ok", !string.IsNullOrEmpty(_saveFolderRow.GetText()));
            _titleLabel.SetText(_videoUrlInfo.Videos.Count > 1 ? _videoUrlInfo.PlaylistTitle! : _videoUrlInfo.Videos[0].Title);
            foreach (var row in _videoRows)
            {
                _videosGroup.Remove(row);
            }
            foreach (var videoInfo in _videoUrlInfo.Videos)
            {
                var row = new Adw.ActionRow();
                row.SetTitle(videoInfo.Title);
                row.SetSubtitle(videoInfo.Url);
                _videoRows.Add(row);
                _videosGroup.Add(row);
            }
        }
    }

    /// <summary>
    /// Occurs when the select save folder button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void SelectSaveFolder(Gtk.Button sender, EventArgs e)
    {
        if (Gtk.Functions.GetMinorVersion() >= 9)
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
                    SetResponseEnabled("ok", true);
                }
            };
            gtk_file_dialog_select_folder(folderDialog, Handle, IntPtr.Zero, _saveCallback, IntPtr.Zero);
        }
        else
        {
            var folderDialog = Gtk.FileChooserNative.New(_controller.Localizer["SelectSaveFolder"], _parent, Gtk.FileChooserAction.SelectFolder, _controller.Localizer["OK"], _controller.Localizer["Cancel"]);
            folderDialog.SetModal(true);
            if (Directory.Exists(_saveFolderRow.GetText()) && _saveFolderRow.GetText() != "/")
            {
                var folder = Gio.FileHelper.NewForPath(_saveFolderRow.GetText());
                gtk_file_chooser_set_current_folder(folderDialog.Handle, folder.Handle, IntPtr.Zero);
            }
            folderDialog.OnResponse += (sender, e) =>
            {
                if (e.ResponseId == (int)Gtk.ResponseType.Accept)
                {
                    var path = folderDialog.GetFile()!.GetPath() ?? "";
                    _saveFolderRow.SetText(path);
                    SetResponseEnabled("ok", true);
                }
            };
            folderDialog.Show();
        }
    }
}