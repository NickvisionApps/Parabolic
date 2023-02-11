using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Helpers;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public class AddDownloadDialog
{
    private bool _constructing;
    private readonly AddDownloadDialogController _controller;
    private readonly Adw.MessageDialog _dialog;
    private readonly Adw.PreferencesGroup _preferencesGroup;
    private readonly Adw.EntryRow _rowVideoUrl;
    private readonly Adw.ComboRow _rowFileType;
    private readonly Adw.ComboRow _rowQuality;
    private readonly Adw.ComboRow _rowSubtitles;
    private readonly Adw.EntryRow _rowNewFilename;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent)
    {
        _constructing = true;
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
        _preferencesGroup.Add(_rowVideoUrl);
        //File Type
        _rowFileType = Adw.ComboRow.New();
        _rowFileType.SetTitle(_controller.Localizer["FileType.Field"]);
        _rowFileType.SetModel(Gtk.StringList.New(new string[] { "MP4", "WEBM", "MP3", "OPUS", "FLAC", "WAV" }));
        _preferencesGroup.Add(_rowFileType);
        //TODO: Handle response
        //Quality
        _rowQuality = Adw.ComboRow.New();
        _rowQuality.SetTitle(_controller.Localizer["Quality.Field"]);
        _rowQuality.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["Best"], _controller.Localizer["Good"], _controller.Localizer["Worst"] }));
        _preferencesGroup.Add(_rowQuality);
        //TODO: Handle response
        //Subtitles
        _rowSubtitles = Adw.ComboRow.New();
        _rowSubtitles.SetTitle(_controller.Localizer["Subtitles.Field"]);
        _rowSubtitles.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["None"], "VTT", "SRT" }));
        _preferencesGroup.Add(_rowSubtitles);
        //TODO: Handle response
        //Save Folder
        //TODO
        //New Filename
        _rowNewFilename = Adw.EntryRow.New();
        _rowNewFilename.SetSizeRequest(420, -1);
        _rowNewFilename.SetTitle(_controller.Localizer["NewFilename.Field"]);
        _preferencesGroup.Add(_rowNewFilename);
        //TODO: Handle response
        //Layout
        _dialog.SetExtraChild(_preferencesGroup);
        //Load Config
        //TODO
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
}