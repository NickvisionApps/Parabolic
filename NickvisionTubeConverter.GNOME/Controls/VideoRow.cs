using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.GNOME.Controls;

public class VideoRow : Adw.ActionRow
{
    private VideoInfo _videoInfo;

    [Gtk.Connect] private readonly Gtk.CheckButton _downloadCheck;
    [Gtk.Connect] private readonly Gtk.Button _editButton;
    [Gtk.Connect] private readonly Gtk.Entry _titleEntry;

    private VideoRow(Gtk.Builder builder, VideoInfo videoInfo) : base(builder.GetPointer("_root"), false)
    {
        _videoInfo = videoInfo;
        //Build UI
        builder.Connect(this);
        SetTitle(_videoInfo.Title);
        SetSubtitle(_videoInfo.Url);
        _downloadCheck.SetSensitive(_videoInfo.IsPartOfPlaylist);
        _downloadCheck.SetActive(_videoInfo.ToDownload);
        _downloadCheck.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "active")
            {
                _videoInfo.ToDownload = _downloadCheck.GetActive();
            }
        };
        _titleEntry.SetText(_videoInfo.Title);
        _titleEntry.SetPlaceholderText(_videoInfo.OriginalTitle);
        _titleEntry.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                _videoInfo.Title = _titleEntry.GetText();
                SetTitle(_videoInfo.Title);
            }
        };
    }

    public VideoRow(VideoInfo videoInfo, Localizer localizer) : this(Builder.FromFile("video_row.ui", localizer), videoInfo)
    {

    }

    public void UpdateTitle()
    {
        SetTitle(_videoInfo.Title);
        _titleEntry.SetText(_videoInfo.Title);
    }
}
