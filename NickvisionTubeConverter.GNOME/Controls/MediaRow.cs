using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.Text.RegularExpressions;

namespace NickvisionTubeConverter.GNOME.Controls;

public class MediaRow : Adw.EntryRow
{
    private MediaInfo _mediaInfo;
    private string _numberString;
    private readonly Gtk.EventControllerKey _titleKeyController;

    [Gtk.Connect] private readonly Gtk.CheckButton _downloadCheck;
    [Gtk.Connect] private readonly Gtk.Button _undoButton;

    private MediaRow(Gtk.Builder builder, MediaInfo mediaInfo) : base(builder.GetPointer("_root"), false)
    {
        _mediaInfo = mediaInfo;
        _numberString = "";
        //Build UI
        builder.Connect(this);
        SetText(_mediaInfo.Title);
        SetTitle(_mediaInfo.Url);
        _downloadCheck.SetSensitive(_mediaInfo.IsPartOfPlaylist);
        _downloadCheck.SetActive(_mediaInfo.ToDownload);
        _downloadCheck.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "active")
            {
                _mediaInfo.ToDownload = _downloadCheck.GetActive();
            }
        };
        OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                _mediaInfo.Title = GetText();
            }
        };
        _titleKeyController = Gtk.EventControllerKey.New();
        _titleKeyController.SetPropagationPhase(Gtk.PropagationPhase.Capture);
        _titleKeyController.OnKeyPressed += OnKeyPressed;
        AddController(_titleKeyController);
        _undoButton.OnClicked += (sender, e) =>
        {
            SetText(_numberString + _mediaInfo.OriginalTitle);
        };
    }

    public MediaRow(MediaInfo mediaInfo, Localizer localizer) : this(Builder.FromFile("media_row.ui", localizer), mediaInfo)
    {

    }

    public void UpdateTitle(bool numbered)
    {
        SetText(_mediaInfo.Title);
        if (numbered)
        {
            var numberedRegex = new Regex(@"[0-9]+ - ", RegexOptions.None);
            _numberString = numberedRegex.Match(_mediaInfo.Title).Value;
        }
        else
        {
            _numberString = "";
        }
    }

    private bool OnKeyPressed(Gtk.EventControllerKey sender, Gtk.EventControllerKey.KeyPressedSignalArgs e)
    {
        return e.Keyval == 47; // Disallow "/"
    }
}
