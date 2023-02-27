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

    private bool _constructing;
    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;

    [Gtk.Connect] private readonly Adw.PreferencesGroup _preferencesGroup;
    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Gtk.Spinner _urlSpinner;
    [Gtk.Connect] private readonly Adw.ComboRow _fileTypeRow;
    [Gtk.Connect] private readonly Adw.ComboRow _qualityRow;
    [Gtk.Connect] private readonly Adw.ComboRow _subtitleRow;
    [Gtk.Connect] private readonly Adw.EntryRow _savePathRow;
    [Gtk.Connect] private readonly Gtk.MenuButton _saveWarning;
    [Gtk.Connect] private readonly Gtk.Button _selectPathButton;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent)
    {
        _constructing = true;
        _parent = parent;
        _controller = controller;
        //Dialog Settings
        SetHeading(_controller.Localizer["AddDownload"]);
        SetTransientFor(parent);
        SetDefaultSize(420, -1);
        SetHideOnClose(true);
        SetModal(true);
        AddResponse("cancel", _controller.Localizer["Cancel"]);
        SetCloseResponse("cancel");
        AddResponse("ok", _controller.Localizer["Download"]);
        SetDefaultResponse("ok");
        SetResponseAppearance("ok", Adw.ResponseAppearance.Suggested);
        OnResponse += (sender, e) => _controller.Accepted = e.Response == "ok";
        //Build UI
        var builder = Builder.FromFile("add_download.ui", _controller.Localizer);
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
        //Layout
        SetExtraChild(_preferencesGroup);
        //Load
        _fileTypeRow.SetSelected((uint)_controller.PreviousMediaFileType);
        _constructing = false;
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
        var fileDialog = Gtk.FileChooserNative.New(_controller.Localizer["SelectSavePath"], _parent, Gtk.FileChooserAction.Save, _controller.Localizer["OK"], _controller.Localizer["Cancel"]);
        fileDialog.SetModal(true);
        var filter = Gtk.FileFilter.New();
        filter.SetName(((MediaFileType)_fileTypeRow.GetSelected()).GetDotExtension());
        filter.AddPattern($"*{((MediaFileType)_fileTypeRow.GetSelected()).GetDotExtension()}");
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