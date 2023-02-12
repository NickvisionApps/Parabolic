using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Runtime.InteropServices;
using YoutubeDLSharp;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public partial class DownloadRow : Adw.ActionRow
{
    private delegate bool GSourceFunc(nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_idle_add(GSourceFunc function, nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_timeout_add(uint interval, GSourceFunc function, nint data);

    private readonly Localizer _localizer;
    private readonly Download _download;
    private readonly Gtk.ProgressBar _progBar;
    private readonly Gtk.Label _progLabel;
    private readonly Gtk.Image _imgStatus;
    private readonly Gtk.LevelBar _levelBar;
    private readonly Adw.ViewStack _viewStack;
    private DownloadProgress _lastProgress;

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="download">The download displayed by the row</param>
    public DownloadRow(Localizer localizer, Download download)
    {
        _localizer = localizer;
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
        var boxProgress = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        boxProgress.SetValign(Gtk.Align.Center);
        //Progress Bar
        _progBar = Gtk.ProgressBar.New();
        _progBar.SetValign(Gtk.Align.Center);
        _progBar.SetSizeRequest(300, -1);
        boxProgress.Append(_progBar);
        //Progress Label
        _progLabel = Gtk.Label.New(localizer["DownloadState.Preparing"]);
        _progLabel.SetValign(Gtk.Align.Center);
        _progLabel.AddCssClass("caption");
        boxProgress.Append(_progLabel);
        boxDownloading.Append(boxProgress);
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
        g_timeout_add(30, d => {
            _progBar.Pulse();
            return _lastProgress == null;
        }, 0);
    }

    /// <summary>
    /// Starts the download
    /// </summary>
    public async void Start()
    {
        var success = await _download.RunAsync(true, new Progress<DownloadProgress>(p => {
            _lastProgress = p;
            switch (p.State)
            {
                case DownloadState.PreProcessing:
                case DownloadState.PostProcessing:
                    g_timeout_add(30, d => {
                        _progBar.Pulse();
                        return _lastProgress.State == DownloadState.PreProcessing || _lastProgress.State == DownloadState.PostProcessing;
                    }, 0);
                    break;
                case DownloadState.Downloading:
                    g_idle_add(d => {
                        _progBar.SetFraction(p.Progress);
                        return false;
                    }, 0);
                    break;
            }
            g_idle_add(d => {
                _progLabel.SetText(_localizer["DownloadState." + Enum.GetName(typeof(DownloadState), p.State)]);
                return false;
            }, 0);
        }));
        g_idle_add(d => {
            _imgStatus.RemoveCssClass("accent");
            _imgStatus.AddCssClass(success ? "success" : "error");
            _viewStack.SetVisibleChildName("done");
            _levelBar.SetValue(success ? 1 : 0);
            return false;
        }, 0);
    }
}