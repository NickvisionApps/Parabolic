using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.GNOME.Controls;

public class VideoRow : Adw.EntryRow
{
    private VideoInfo _videoInfo;
    private readonly Gtk.EventControllerKey _titleKeyController;

    [Gtk.Connect] private readonly Gtk.CheckButton _downloadCheck;

    private VideoRow(Gtk.Builder builder, VideoInfo videoInfo) : base(builder.GetPointer("_root"), false)
    {
        _videoInfo = videoInfo;
        //Build UI
        builder.Connect(this);
        SetTitle(_videoInfo.Url);
        SetText(_videoInfo.Title);
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
                if(GetText() != _videoInfo.Title)
                {
                    SetText(_videoInfo.Title);
                }
            }
        };
        _titleKeyController = Gtk.EventControllerKey.New();
        _titleKeyController.SetPropagationPhase(Gtk.PropagationPhase.Capture);
        _titleKeyController.OnKeyPressed += OnKeyPressed;
        AddController(_titleKeyController);
    }

    public VideoRow(VideoInfo videoInfo, Localizer localizer) : this(Builder.FromFile("video_row.ui", localizer), videoInfo)
    {

    }

    public void UpdateTitle() => SetText(_videoInfo.Title);

    private bool OnKeyPressed(Gtk.EventControllerKey sender, Gtk.EventControllerKey.KeyPressedSignalArgs e)
    {
        return e.Keyval == 47; // Disallow "/"
    }
}
