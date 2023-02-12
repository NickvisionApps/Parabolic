using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using YoutubeDLSharp;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public class DownloadRow : Adw.ActionRow
{
    private readonly Download _download;
    private readonly Gtk.ProgressBar _progBar;
    private readonly Gtk.Image _imgStatus;
    private readonly Gtk.LevelBar _levelBar;
    private readonly Adw.ViewStack _viewStack;

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
        _imgStatus = Gtk.Image.NewFromIconName("folder-download-symbolic");
        _imgStatus.SetPixelSize(20);
        _imgStatus.AddCssClass("accent");
        AddPrefix(_imgStatus);
        //Box Downloading
        var boxDownloading = Gtk.Box.New(Gtk.Orientation.Horizontal, 6);
        //Progress Bar
        _progBar = Gtk.ProgressBar.New();
        _progBar.SetValign(Gtk.Align.Center);
        _progBar.SetSizeRequest(300, -1);
        boxDownloading.Append(_progBar);
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
        _levelBar = Gtk.LevelBar.New();
        _levelBar.SetValign(Gtk.Align.Center);
        _levelBar.SetSizeRequest(300, -1);
        boxDone.Append(_levelBar);
        //View Stack
        _viewStack = Adw.ViewStack.New();
        _viewStack.AddNamed(boxDownloading, "downloading");
        _viewStack.AddNamed(boxDone, "done");
        AddSuffix(_viewStack);
    }

    /// <summary>
    /// Starts the download
    /// </summary>
    public async void Start()
    {
        var success = await _download.RunAsync(true, new Progress<DownloadProgress>(p => {
          // _progBar.SetFraction(p.Progress); //TODO: Causes issues with multiple downloads...
        }));
        _imgStatus.RemoveCssClass("accent");
        _imgStatus.AddCssClass(success ? "success" : "error");
        //_viewStack.SetVisibleChildName("done"); //TODO: Causes issues with multiple downloads...
        _levelBar.SetValue(success ? 1 : 0);
    }
}