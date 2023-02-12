using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.GNOME.Controls;


/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public class DownloadRow : Adw.ActionRow
{
    private readonly Download _download;

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="download">The download displayed by the row</param>
    public DownloadRow(Localizer localizer, Download download)
    {
        _download = download;
        //Row Settings
        SetTitle(download.Path);
        SetSubtitle(download.VideoUrl);
        //Status Image
        var imgStatus = Gtk.Image.NewFromIconName("folder-download-symbolic");
        imgStatus.SetPixelSize(20);
        AddPrefix(imgStatus);
        //Box Downloading
        var boxDownloading = Gtk.Box.New(Gtk.Orientation.Horizontal, 6);
        //Progress Bar
        var progBar = Gtk.ProgressBar.New();
        progBar.SetValign(Gtk.Align.Center);
        progBar.SetSizeRequest(300, -1);
        boxDownloading.Append(progBar);
        //Stop Button
        var btnStop = Gtk.Button.New();
        btnStop.SetValign(Gtk.Align.Center);
        btnStop.AddCssClass("flat");
        btnStop.SetIconName("media-playback-stop-symbolic");
        btnStop.SetTooltipText(localizer["StopDownload"]);
        btnStop.OnClicked += (sender, args) => _download.Stop();
        boxDownloading.Append(btnStop);
        //Box Done
        var boxDone = Gtk.Box.New(Gtk.Orientation.Horizontal, 6);
        boxDone.SetValign(Gtk.Align.Center);
        //Level Bar
        var levelBar = Gtk.LevelBar.New();
        levelBar.SetValign(Gtk.Align.Center);
        levelBar.SetSizeRequest(300, -1);
        boxDone.Append(levelBar);
        //View Stack
        var viewStack = Gtk.Stack.New();
        viewStack.AddNamed(boxDownloading, "downloading");
        viewStack.AddNamed(boxDone, "done");
        AddSuffix(viewStack);
    }
}   