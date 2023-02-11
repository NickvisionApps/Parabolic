using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;

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
    private readonly Adw.ComboRow _rowSubtitles;
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
        _dialog.SetHideOnClose(true);
        _dialog.SetModal(true);
        _dialog.AddResponse("cancel", _controller.Localizer["Cancel"]);
        _dialog.SetCloseResponse("cancel");
        _dialog.AddResponse("ok", _controller.Localizer["OK"]);
        _dialog.SetDefaultResponse("ok");
        _dialog.SetResponseAppearance("ok", Adw.ResponseAppearance.Suggested);
        //TODO: Handle response
        //Preference Group
        _preferencesGroup = Adw.PreferencesGroup.New();
        //Video Url
        _rowVideoUrl = Adw.EntryRow.New();
        _rowVideoUrl.SetSizeRequest(420, -1);
        _rowVideoUrl.SetTitle(_controller.Localizer["VideoUrl.Field"]);
        _rowVideoUrl.OnNotify += (sender, e) => {
            if (e.Pspec.GetName() == "text")
            {
                Validate();
            }
        };
        _preferencesGroup.Add(_rowVideoUrl);
        //File Type
        _rowFileType = Adw.ComboRow.New();
        _rowFileType.SetTitle(_controller.Localizer["FileType.Field"]);
        _rowFileType.SetModel(Gtk.StringList.New(new string[] { "MP4", "WEBM", "MP3", "OPUS", "FLAC", "WAV" }));
        _rowFileType.OnNotify += (sender, e) => {
            if (e.Pspec.GetName() == "selected-item")
            {
                Validate();
            }
        };
        _preferencesGroup.Add(_rowFileType);
        //Quality
        _rowQuality = Adw.ComboRow.New();
        _rowQuality.SetTitle(_controller.Localizer["Quality.Field"]);
        _rowQuality.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["Best"], _controller.Localizer["Good"], _controller.Localizer["Worst"] }));
        _rowQuality.OnNotify += (sender, e) => {
            if (e.Pspec.GetName() == "selected-item")
            {
                Validate();
            }
        };
        _preferencesGroup.Add(_rowQuality);
        //Subtitles
        _rowSubtitles = Adw.ComboRow.New();
        _rowSubtitles.SetTitle(_controller.Localizer["Subtitles.Field"]);
        _rowSubtitles.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["None"], "VTT", "SRT" }));
        _rowSubtitles.OnNotify += (sender, e) => {
            if (e.Pspec.GetName() == "selected-item")
            {
                Validate();
            }
        };
        _preferencesGroup.Add(_rowSubtitles);
        //Save Folder
        _btnSelectSaveFolder = Gtk.Button.New();
        _btnSelectSaveFolder.SetValign(Gtk.Align.Center);
        _btnSelectSaveFolder.AddCssClass("flat");
        _btnSelectSaveFolder.SetIconName("folder-open-symbolic");
        _btnSelectSaveFolder.SetTooltipText(_controller.Localizer["SelectSaveFolder", "Tooltip"]);
        _btnSelectSaveFolder.OnClicked += SelectSaveFolder;
        _rowSaveFolder = Adw.EntryRow.New();
        _rowSaveFolder.SetSizeRequest(420, -1);
        _rowSaveFolder.SetTitle(_controller.Localizer["SelectSaveFolder"]);
        _rowSaveFolder.AddSuffix(_btnSelectSaveFolder);
        _rowSaveFolder.SetEditable(false);
        _preferencesGroup.Add(_rowSaveFolder);
        //New Filename
        _rowNewFilename = Adw.EntryRow.New();
        _rowNewFilename.SetSizeRequest(420, -1);
        _rowNewFilename.SetTitle(_controller.Localizer["NewFilename.Field"]);
        _preferencesGroup.Add(_rowNewFilename);
        _rowNewFilename.OnNotify += (sender, e) => {
            if (e.Pspec.GetName() == "text")
            {
                Validate();
            }
        };
        //Layout
        _dialog.SetExtraChild(_preferencesGroup);
        //Load Config
        _rowFileType.SetSelected((uint)_controller.GetPreviousFileTypeAsInt());
        _rowSaveFolder.SetText(_controller.GetPreviousSaveFolder());
        _constructing = false;

        //TODO: Remove this
        Show();
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
    /// Occurs when the select save folder button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void SelectSaveFolder(Gtk.Button sender, EventArgs e)
    {
        var fileDialog = Gtk.FileChooserNative.New(_controller.Localizer["SelectSaveFolder"], _parent, Gtk.FileChooserAction.SelectFolder, _controller.Localizer["OK"], _controller.Localizer["Cancel"]);
        fileDialog.SetModal(true);
        fileDialog.OnResponse += (sender, e) => {
            if (e.ResponseId == (int)Gtk.ResponseType.Accept)
            {
                var path = fileDialog.GetFile()!.GetPath();
                _rowSaveFolder.SetText(path);
            }
        };
        fileDialog.Show();
    }

    /// <summary>
    /// Validates the dialog's input
    /// </summary>
    private void Validate()
    {
        if (_constructing)
        {
            return;
        }
        _rowSubtitles.SetSensitive(MediaFileTypeHelpers.GetIsVideo((MediaFileType)_rowFileType.GetSelected()));
    }
}