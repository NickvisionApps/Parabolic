using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

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
    private static partial uint gtk_get_minor_version();

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
    private static partial void gtk_file_dialog_save(nint dialog, nint parent, nint cancellable, GAsyncReadyCallback callback, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_save_finish(nint dialog, nint result, nint error);

    private GAsyncReadyCallback _saveCallback { get; set; }

    private bool _constructing;
    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;

    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Gtk.Spinner _urlSpinner;
    [Gtk.Connect] private readonly Adw.ComboRow _fileTypeRow;
    [Gtk.Connect] private readonly Adw.ComboRow _qualityRow;
    [Gtk.Connect] private readonly Adw.ComboRow _subtitleRow;
    [Gtk.Connect] private readonly Adw.EntryRow _savePathRow;
    [Gtk.Connect] private readonly Gtk.MenuButton _saveWarning;
    [Gtk.Connect] private readonly Gtk.Button _selectPathButton;

    private AddDownloadDialog(Gtk.Builder builder, AddDownloadDialogController controller, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        _constructing = true;
        _parent = parent;
        _controller = controller;
        //Dialog Settings
        SetTransientFor(parent);
        AddResponse("cancel", controller.Localizer["Cancel"]);
        SetCloseResponse("cancel");
        AddResponse("ok", controller.Localizer["Download"]);
        SetDefaultResponse("ok");
        SetResponseAppearance("ok", Adw.ResponseAppearance.Suggested);
        OnResponse += (sender, e) => controller.Accepted = e.Response == "ok";
        //Build UI
        builder.Connect(this);
        _urlRow.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _fileTypeRow.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _qualityRow.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _subtitleRow.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _selectPathButton.OnClicked += SelectSavePath;
        _savePathRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                _saveWarning.SetVisible(Regex.Match(_savePathRow.GetText(), @"^\/run\/user\/.*\/doc\/.*").Success);
            }
        };
        //Load
        _fileTypeRow.SetSelected((uint)_controller.PreviousMediaFileType);
        _constructing = false;
    }

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent) : this(Builder.FromFile("add_download_dialog.ui", controller.Localizer), controller, parent)
    {
    }

    /// <summary>
    /// Shows the dialog
    /// </summary>
    public async Task ShowAsync()
    {
        await ValidateAsync();
        Show();
    }

    /// <summary>
    /// Validates the dialog's input
    /// </summary>
    private async Task ValidateAsync()
    {
        _urlSpinner.Start();
        _urlRow.SetSensitive(false);
        _fileTypeRow.SetSensitive(false);
        _qualityRow.SetSensitive(false);
        _subtitleRow.SetSensitive(false);
        _savePathRow.SetSensitive(false);
        var checkStatus = await _controller.UpdateDownloadAsync(_urlRow.GetText(), (MediaFileType)_fileTypeRow.GetSelected(), _savePathRow.GetText(), (Quality)_qualityRow.GetSelected(), (Subtitle)_subtitleRow.GetSelected());
        _urlSpinner.Stop();
        _urlRow.SetSensitive(true);
        _urlRow.RemoveCssClass("error");
        _urlRow.SetTitle(_controller.Localizer["VideoUrl", "Field"]);
        _savePathRow.RemoveCssClass("error");
        _savePathRow.SetTitle(_controller.Localizer["SavePath", "Field"]);
        _savePathRow.SetText(_controller.SavePath);
        if (checkStatus == DownloadCheckStatus.Valid)
        {
            _fileTypeRow.SetSensitive(true);
            _qualityRow.SetSensitive(true);
            _subtitleRow.SetSensitive(((MediaFileType)_fileTypeRow.GetSelected()).GetIsVideo());
            _savePathRow.SetSensitive(true);
            SetResponseEnabled("ok", true);
        }
        else
        {
            if (checkStatus.HasFlag(DownloadCheckStatus.EmptyVideoUrl))
            {
                _urlRow.AddCssClass("error");
                _urlRow.SetTitle(_controller.Localizer["VideoUrl", "Empty"]);
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.InvalidVideoUrl))
            {
                _urlRow.AddCssClass("error");
                _urlRow.SetTitle(_controller.Localizer["VideoUrl", "Invalid"]);
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.PlaylistNotSupported))
            {
                _urlRow.AddCssClass("error");
                _urlRow.SetTitle(_controller.Localizer["VideoUrl", "PlaylistNotSupported"]);
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.InvalidSaveFolder))
            {
                _savePathRow.AddCssClass("error");
                _savePathRow.SetTitle(_controller.Localizer["SavePath", "Invalid"]);
                _fileTypeRow.SetSensitive(true);
                _qualityRow.SetSensitive(true);
                _subtitleRow.SetSensitive(((MediaFileType)_fileTypeRow.GetSelected()).GetIsVideo());
                _savePathRow.SetSensitive(true);
            }
            SetResponseEnabled("ok", false);
        }
    }

    /// <summary>
    /// Occurs when the select save path button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void SelectSavePath(Gtk.Button sender, EventArgs e)
    {
        var filter = Gtk.FileFilter.New();
        filter.SetName(((MediaFileType)_fileTypeRow.GetSelected()).GetDotExtension());
        filter.AddPattern($"*{((MediaFileType)_fileTypeRow.GetSelected()).GetDotExtension()}");
        if (Gtk.Functions.GetMinorVersion() >= 9)
        {
            var saveFileDialog = gtk_file_dialog_new();
            gtk_file_dialog_set_title(saveFileDialog, _controller.Localizer["SelectSavePath"]);
            var filters = Gio.ListStore.New(Gtk.FileFilter.GetGType());
            filters.Append(filter);
            gtk_file_dialog_set_filters(saveFileDialog, filters.Handle);
            if (_savePathRow.GetText().Length > 0 && Directory.Exists(Path.GetDirectoryName(_savePathRow.GetText())) && _savePathRow.GetText() != "/")
            {
                var folder = Gio.FileHelper.NewForPath(Path.GetDirectoryName(_savePathRow.GetText()));
                gtk_file_dialog_set_initial_folder(saveFileDialog, folder.Handle);
                gtk_file_dialog_set_initial_name(saveFileDialog, Path.GetFileName(_savePathRow.GetText()));
            }
            _saveCallback = async (source, res, data) =>
            {
                var fileHandle = gtk_file_dialog_save_finish(saveFileDialog, res, IntPtr.Zero);
                if (fileHandle != IntPtr.Zero)
                {
                    var path = g_file_get_path(fileHandle);
                    _savePathRow.SetText(path);
                    await ValidateAsync();
                }
            };
            gtk_file_dialog_save(saveFileDialog, Handle, IntPtr.Zero, _saveCallback, IntPtr.Zero);
        }
        else
        {
            var fileDialog = Gtk.FileChooserNative.New(_controller.Localizer["SelectSavePath"], _parent, Gtk.FileChooserAction.Save, _controller.Localizer["OK"], _controller.Localizer["Cancel"]);
            fileDialog.SetModal(true);
            fileDialog.SetFilter(filter);
            if (_savePathRow.GetText().Length > 0 && Directory.Exists(Path.GetDirectoryName(_savePathRow.GetText())) && _savePathRow.GetText() != "/")
            {
                var folder = Gio.FileHelper.NewForPath(Path.GetDirectoryName(_savePathRow.GetText()));
                gtk_file_chooser_set_current_folder(fileDialog.Handle, folder.Handle, IntPtr.Zero);
                gtk_file_chooser_set_current_name(fileDialog.Handle, Path.GetFileName(_savePathRow.GetText()));
            }
            fileDialog.OnResponse += async (sender, e) =>
            {
                if (e.ResponseId == (int)Gtk.ResponseType.Accept)
                {
                    var path = fileDialog.GetFile()!.GetPath() ?? "";
                    _savePathRow.SetText(path);
                    await ValidateAsync();
                }
            };
            fileDialog.Show();
        }
    }
}