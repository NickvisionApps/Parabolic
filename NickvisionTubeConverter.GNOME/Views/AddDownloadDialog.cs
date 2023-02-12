using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public class AddDownloadDialog
{
    private bool _constructing;
    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;
    private readonly Adw.MessageDialog _dialog;
    private readonly Adw.PreferencesGroup _preferencesGroup;
    private readonly Adw.EntryRow _rowVideoUrl;
    private readonly Adw.ComboRow _rowFileType;
    private readonly Adw.ComboRow _rowQuality;
    private readonly Adw.ComboRow _rowSubtitle;
    private readonly Gtk.Button _btnSelectSaveFolder;
    private readonly Adw.EntryRow _rowSaveFolder;
    private readonly Adw.EntryRow _rowNewFilename;

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
        _rowVideoUrl = Adw.EntryRow.New();
        _rowVideoUrl.SetSizeRequest(420, -1);
        _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl", "Field"]);
        _rowVideoUrl.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                if (!_constructing)
                {
                    Validate();
                }
            }
        };
        _preferencesGroup.Add(_rowVideoUrl);
        //File Type
        _rowFileType = Adw.ComboRow.New();
        _rowFileType.SetTitle(_controller.Localizer["FileType", "Field"]);
        _rowFileType.SetModel(Gtk.StringList.New(new string[] { "MP4", "WEBM", "MP3", "OPUS", "FLAC", "WAV" }));
        _rowFileType.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                _rowSubtitle!.SetSensitive(((MediaFileType)_rowFileType.GetSelected()).GetIsVideo());
                if (!_constructing)
                {
                    Validate();
                }
            }
        };
        _preferencesGroup.Add(_rowFileType);
        //Quality
        _rowQuality = Adw.ComboRow.New();
        _rowQuality.SetTitle(_controller.Localizer["Quality", "Field"]);
        _rowQuality.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["Quality", "Best"], _controller.Localizer["Quality", "Good"], _controller.Localizer["Quality", "Worst"] }));
        _rowQuality.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    Validate();
                }
            }
        };
        _preferencesGroup.Add(_rowQuality);
        //Subtitles
        _rowSubtitle = Adw.ComboRow.New();
        _rowSubtitle.SetTitle(_controller.Localizer["Subtitle", "Field"]);
        _rowSubtitle.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["Subtitle", "None"], "VTT", "SRT" }));
        _rowSubtitle.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                if (!_constructing)
                {
                    Validate();
                }
            }
        };
        _preferencesGroup.Add(_rowSubtitle);
        //Save Folder
        _btnSelectSaveFolder = Gtk.Button.New();
        _btnSelectSaveFolder.SetValign(Gtk.Align.Center);
        _btnSelectSaveFolder.AddCssClass("flat");
        _btnSelectSaveFolder.SetIconName("folder-open-symbolic");
        _btnSelectSaveFolder.SetTooltipText(_controller.Localizer["SelectSaveFolder"]);
        _btnSelectSaveFolder.OnClicked += SelectSaveFolder;
        _rowSaveFolder = Adw.EntryRow.New();
        _rowSaveFolder.SetSizeRequest(420, -1);
        _rowSaveFolder.SetTitle(_controller.Localizer["SaveFolder", "Field"]);
        _rowSaveFolder.AddSuffix(_btnSelectSaveFolder);
        _rowSaveFolder.SetEditable(false);
        _rowSaveFolder.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                if (!_constructing)
                {
                    Validate();
                }
            }
        };
        _preferencesGroup.Add(_rowSaveFolder);
        //New Filename
        _rowNewFilename = Adw.EntryRow.New();
        _rowNewFilename.SetSizeRequest(420, -1);
        _rowNewFilename.SetTitle(_controller.Localizer["NewFilename", "Field"]);
        _rowNewFilename.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                if (!_constructing)
                {
                    Validate();
                }
            }
        };
        _preferencesGroup.Add(_rowNewFilename);
        //Layout
        _dialog.SetExtraChild(_preferencesGroup);
        //Load Config
        _rowFileType.SetSelected((uint)_controller.PreviousMediaFileType);
        _rowSaveFolder.SetText(_controller.PreviousSaveFolder);
        Validate();
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
    private void Validate()
    {
        var checkStatus = _controller.UpdateDownload(_rowVideoUrl.GetText(), (MediaFileType)_rowFileType.GetSelected(), _rowSaveFolder.GetText(), _rowNewFilename.GetText(), (Quality)_rowQuality.GetSelected(), (Subtitle)_rowSubtitle.GetSelected());
        _rowVideoUrl.RemoveCssClass("error");
        _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl", "Field"]);
        _rowSaveFolder.RemoveCssClass("error");
        _rowSaveFolder.SetTitle(_controller.Localizer["SaveFolder", "Field"]);
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
                _rowSaveFolder.AddCssClass("error");
                _rowSaveFolder.SetTitle(_controller.Localizer["SaveFolder", "Invalid"]);
            }
            _dialog.SetResponseEnabled("ok", false);
        }
    }

    /// <summary>
    /// Occurs when the select save folder button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void SelectSaveFolder(Gtk.Button sender, EventArgs e)
    {
        var fileDialog = Gtk.FileChooserNative.New(_controller.Localizer["SelectSaveFolder"], _parent, Gtk.FileChooserAction.SelectFolder, _controller.Localizer["OK"], _controller.Localizer["Cancel"]);
        fileDialog.SetModal(true);
        fileDialog.OnResponse += (sender, e) =>
        {
            if (e.ResponseId == (int)Gtk.ResponseType.Accept)
            {
                var path = fileDialog.GetFile()!.GetPath() ?? "";
                _rowSaveFolder.SetText(path);
                Validate();
            }
        };
        fileDialog.Show();
    }
}