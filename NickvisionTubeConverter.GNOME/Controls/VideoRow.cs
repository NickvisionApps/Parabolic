using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.Text.RegularExpressions;

namespace NickvisionTubeConverter.GNOME.Controls;

public class VideoRow : Adw.EntryRow
{
    private VideoInfo _videoInfo;
    private string _numberString;
    private readonly Gtk.EventControllerKey _titleKeyController;

    [Gtk.Connect] private readonly Gtk.CheckButton _downloadCheck;
    [Gtk.Connect] private readonly Gtk.Button _undoButton;

    private VideoRow(Gtk.Builder builder, VideoInfo videoInfo) : base(builder.GetPointer("_root"), false)
    {
        _videoInfo = videoInfo;
        _numberString = "";
        //Build UI
        builder.Connect(this);SetText(_videoInfo.Title);
        SetTitle(_videoInfo.Url);
        _downloadCheck.SetSensitive(_videoInfo.IsPartOfPlaylist);
        _downloadCheck.SetActive(_videoInfo.ToDownload);
        _downloadCheck.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "active")
            {
                _videoInfo.ToDownload = _downloadCheck.GetActive();
            }
        };
        OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                _videoInfo.Title = GetText();
            }
        };
        _titleKeyController = Gtk.EventControllerKey.New();
        _titleKeyController.SetPropagationPhase(Gtk.PropagationPhase.Capture);
        _titleKeyController.OnKeyPressed += OnKeyPressed;
        AddController(_titleKeyController);
        _undoButton.OnClicked += (sender, e) =>
        {
            SetText(_numberString + _videoInfo.OriginalTitle);
        };
    }

    public VideoRow(VideoInfo videoInfo, Localizer localizer) : this(Builder.FromFile("video_row.ui", localizer), videoInfo)
    {

    }

    public void UpdateTitle(bool numbered)
    {
        SetText(_videoInfo.Title);
        if (numbered)
        {
            var numberedRegex = new Regex(@"[0-9]+ - ", RegexOptions.None);
            _numberString = numberedRegex.Match(_videoInfo.Title).Value;
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
