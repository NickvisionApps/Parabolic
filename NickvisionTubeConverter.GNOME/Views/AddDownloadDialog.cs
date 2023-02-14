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
public partial class AddDownloadDialog
{
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool gtk_file_chooser_set_current_folder(nint chooser, nint file, nint error);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    public static partial void gtk_file_chooser_set_current_name(nint chooser, string name);

    private bool _constructing;
    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;
    private readonly Adw.MessageDialog _dialog;
    private readonly Adw.PreferencesGroup _preferencesGroup;
    private readonly Adw.EntryRow _rowVideoUrl;
    private readonly Gtk.Spinner _spinnerVideoUrl;
    private readonly Adw.ComboRow _rowFileType;
    private readonly Adw.ComboRow _rowQuality;
    private readonly Adw.ComboRow _rowSubtitle;
    private readonly Gtk.Label _lblSaveWarning;
    private readonly Adw.Clamp _clampSaveWarning;
    private readonly Gtk.Popover _popoverSaveWarning;
    private readonly Gtk.MenuButton _btnSaveWarning;
    private readonly Gtk.Button _btnSelectSavePath;
    private readonly Adw.EntryRow _rowSavePath;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent)
    {
        _constructing = true;
        _parent = parent;
        _controller = controller;
        //Dialog Settings
        _dialog = Adw.MessageDialog.New(parent, _controller.Localizer["AddDownload"], "");
        _dialog.SetDefaultSize(420, -1);
        _dialog.SetHideOnClose(true);
        _dialog.SetModal(true);
        _dialog.AddResponse("cancel", _controller.Localizer["Cancel"]);
        _dialog.SetCloseResponse("cancel");
        _dialog.AddResponse("ok", _controller.Localizer["OK"]);
        _dialog.SetDefaultResponse("ok");
        _dialog.SetResponseAppearance("ok", Adw.ResponseAppearance.Suggested);
        _dialog.OnResponse += (sender, e) => _controller.Accepted = e.Response == "ok";
        //Preference Group
        _preferencesGroup = Adw.PreferencesGroup.New();
        //Video Url
        _spinnerVideoUrl = Gtk.Spinner.New();
        _spinnerVideoUrl.SetValign(Gtk.Align.Center);
        _rowVideoUrl = Adw.EntryRow.New();
        _rowVideoUrl.SetSizeRequest(420, -1);
        _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl", "Field"]);
        _rowVideoUrl.AddSuffix(_spinnerVideoUrl);
        _rowVideoUrl.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _preferencesGroup.Add(_rowVideoUrl);
        //File Type
        _rowFileType = Adw.ComboRow.New();
        _rowFileType.SetTitle(_controller.Localizer["FileType", "Field"]);
        _rowFileType.SetModel(Gtk.StringList.New(new string[] { "MP4", "WEBM", "MP3", "OPUS", "FLAC", "WAV" }));
        _rowFileType.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _preferencesGroup.Add(_rowFileType);
        //Quality
        _rowQuality = Adw.ComboRow.New();
        _rowQuality.SetTitle(_controller.Localizer["Quality", "Field"]);
        _rowQuality.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["Quality", "Best"], _controller.Localizer["Quality", "Good"], _controller.Localizer["Quality", "Worst"] }));
        _rowQuality.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _preferencesGroup.Add(_rowQuality);
        //Subtitles
        _rowSubtitle = Adw.ComboRow.New();
        _rowSubtitle.SetTitle(_controller.Localizer["Subtitle", "Field"]);
        _rowSubtitle.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["Subtitle", "None"], "VTT", "SRT" }));
        _rowSubtitle.OnNotify += async (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    await ValidateAsync();
                }
            }
        };
        _preferencesGroup.Add(_rowSubtitle);
        //Save Path
        _lblSaveWarning = Gtk.Label.New(_controller.Localizer["SaveWarning", "GTK"]);
        _lblSaveWarning.SetWrap(true);
        _lblSaveWarning.SetJustify(Gtk.Justification.Center);
        _clampSaveWarning = Adw.Clamp.New();
        _clampSaveWarning.SetMaximumSize(300);
        _clampSaveWarning.SetChild(_lblSaveWarning);
        _popoverSaveWarning = Gtk.Popover.New();
        _popoverSaveWarning.SetChild(_clampSaveWarning);
        _btnSaveWarning = Gtk.MenuButton.New();
        _btnSaveWarning.SetIconName("dialog-warning-symbolic");
        _btnSaveWarning.SetValign(Gtk.Align.Center);
        _btnSaveWarning.AddCssClass("flat");
        _btnSaveWarning.AddCssClass("warning");
        _btnSaveWarning.SetPopover(_popoverSaveWarning);
        _btnSaveWarning.SetVisible(false);
        _btnSelectSavePath = Gtk.Button.New();
        _btnSelectSavePath.SetValign(Gtk.Align.Center);
        _btnSelectSavePath.AddCssClass("flat");
        _btnSelectSavePath.SetIconName("folder-open-symbolic");
        _btnSelectSavePath.SetTooltipText(_controller.Localizer["SelectSavePath"]);
        _btnSelectSavePath.OnClicked += SelectSavePath;
        _rowSavePath = Adw.EntryRow.New();
        _rowSavePath.SetSizeRequest(420, -1);
        _rowSavePath.SetTitle(_controller.Localizer["SavePath", "Field"]);
        _rowSavePath.AddSuffix(_btnSaveWarning);
        _rowSavePath.AddSuffix(_btnSelectSavePath);
        _rowSavePath.SetEditable(false);
        _rowSavePath.OnNotify += (sender, e) => {
            if (e.Pspec.GetName() == "text")
            {
                _btnSaveWarning.SetVisible(Regex.Match(_rowSavePath.GetText(), @"^\/run\/user\/.*\/doc\/.*").Success);
            }
        };
        _preferencesGroup.Add(_rowSavePath);
        //Layout
        _dialog.SetExtraChild(_preferencesGroup);
        //Load
        _rowVideoUrl.AddCssClass("error");
        _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl", "Empty"]);
        _rowFileType.SetSelected((uint)_controller.PreviousMediaFileType);
        if(string.IsNullOrEmpty(_rowSavePath.GetText()))
        {
            _rowSavePath.AddCssClass("error");
            _rowSavePath.SetTitle(_controller.Localizer["SavePath", "Invalid"]);
        }
        _dialog.SetResponseEnabled("ok", false);
        _constructing = false;
    }

    public event GObject.SignalHandler<Adw.MessageDialog, Adw.MessageDialog.ResponseSignalArgs> OnResponse
    {
        add
        {
            _dialog.OnResponse += value;
        }
        remove
        {
            _dialog.OnResponse -= value;
        }
    }

    /// <summary>
    /// Shows the dialog
    /// </summary>
    public void Show() => _dialog.Show();

    /// <summary>
    /// Destroys the dialog
    /// </summary>
    public void Destroy() => _dialog.Destroy();

    /// <summary>
    /// Validates the dialog's input
    /// </summary>
    private async Task ValidateAsync()
    {
        _spinnerVideoUrl.Start();
        _rowFileType.SetSensitive(false);
        _rowSavePath.SetSensitive(false);
        _rowQuality.SetSensitive(false);
        _rowSubtitle.SetSensitive(false);
        var checkStatus = await _controller.UpdateDownloadAsync(_rowVideoUrl.GetText(), (MediaFileType)_rowFileType.GetSelected(), _rowSavePath.GetText(), (Quality)_rowQuality.GetSelected(), (Subtitle)_rowSubtitle.GetSelected());
        _spinnerVideoUrl.Stop();
        _rowFileType.SetSensitive(true);
        _rowSavePath.SetSensitive(true);
        _rowQuality.SetSensitive(true);
        _rowSubtitle.SetSensitive(((MediaFileType)_rowFileType.GetSelected()).GetIsVideo());
        _rowVideoUrl.RemoveCssClass("error");
        _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl", "Field"]);
        _rowSavePath.RemoveCssClass("error");
        _rowSavePath.SetTitle(_controller.Localizer["SavePath", "Field"]);
        _rowSavePath.SetText(_controller.GetSavePath());
        if (checkStatus == DownloadCheckStatus.Valid)
        {
            _dialog.SetResponseEnabled("ok", true);
        }
        else
        {
            if(checkStatus.HasFlag(DownloadCheckStatus.EmptyVideoUrl))
            {
                _rowVideoUrl.AddCssClass("error");
                _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl", "Empty"]);
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.InvalidVideoUrl))
            {
                _rowVideoUrl.AddCssClass("error");
                _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl", "Invalid"]);
            }
            if (checkStatus.HasFlag(DownloadCheckStatus.InvalidSaveFolder))
            {
                _rowSavePath.AddCssClass("error");
                _rowSavePath.SetTitle(_controller.Localizer["SavePath", "Invalid"]);
            }
            _dialog.SetResponseEnabled("ok", false);
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
        filter.SetName(((MediaFileType)_rowFileType.GetSelected()).GetDotExtension());
        filter.AddPattern($"*{((MediaFileType)_rowFileType.GetSelected()).GetDotExtension()}");
        fileDialog.SetFilter(filter);
        if (_rowSavePath.GetText().Length > 0)
        {
            var folder = Gio.FileHelper.NewForPath(Path.GetDirectoryName(_rowSavePath.GetText()));
            gtk_file_chooser_set_current_folder(fileDialog.Handle, folder.Handle, IntPtr.Zero);
            gtk_file_chooser_set_current_name(fileDialog.Handle, Path.GetFileName(_rowSavePath.GetText()));
        }
        fileDialog.OnResponse += async (sender, e) =>
        {
            if (e.ResponseId == (int)Gtk.ResponseType.Accept)
            {
                var path = fileDialog.GetFile()!.GetPath() ?? "";
                _rowSavePath.SetText(path);
                await ValidateAsync();
            }
        };
        fileDialog.Show();
    }
}