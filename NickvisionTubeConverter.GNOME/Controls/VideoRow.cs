using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.GNOME.Controls;

public class VideoRow : Adw.ActionRow
{
    private VideoInfo _videoInfo;

    [Gtk.Connect] private readonly Gtk.CheckButton _downloadCheck;
    [Gtk.Connect] private readonly Gtk.Button _editButton;

    private VideoRow(Gtk.Builder builder, VideoInfo videoInfo) : base(builder.GetPointer("_root"), false)
    {
    }

    public VideoRow(VideoInfo videoInfo, Localizer localizer) : this(Builder.FromFile("video_row.ui", localizer), videoInfo)
    {

    }
}
