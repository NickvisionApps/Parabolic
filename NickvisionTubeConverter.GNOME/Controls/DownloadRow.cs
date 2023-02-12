using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using YoutubeDLSharp;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public partial class DownloadRow : Adw.ActionRow, IDownloadRowControl
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
    private readonly Gtk.Label _doneLabel;
    private readonly Adw.ViewStack _viewStack;
    private DownloadProgress? _lastProgress;
    private GSourceFunc? _processingCallback;
    private GSourceFunc? _downloadingCallback;

    /// <summary>
    /// Whether or not the download is done
    /// </summary>
    public bool IsDone => _download.IsDone;

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="download">The download displayed by the row</param>
    public DownloadRow(Localizer localizer, Download download)
    {
        _localizer = localizer;
        _download = download;
        //Row Settings
        SetUseMarkup(false);
        SetTitle(_download.Filename);
        SetSubtitle(_download.VideoUrl);
        SetTitleLines(1);
        SetSubtitleLines(1);
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
        _progLabel = Gtk.Label.New(_localizer["DownloadState", "Preparing"]);
        _progLabel.SetValign(Gtk.Align.Center);
        _progLabel.AddCssClass("caption");
        boxProgress.Append(_progLabel);
        boxDownloading.Append(boxProgress);
        //Stop Button
        var btnStop = Gtk.Button.New();
        btnStop.SetValign(Gtk.Align.Center);
        btnStop.AddCssClass("flat");
        btnStop.SetIconName("media-playback-stop-symbolic");
        btnStop.SetTooltipText(_localizer["StopDownload"]);
        btnStop.OnClicked += (sender, e) => Stop();
        boxDownloading.Append(btnStop);
        //Box Done
        var boxDone = Gtk.Box.New(Gtk.Orientation.Horizontal, 6);
        var boxLevel = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        boxLevel.SetValign(Gtk.Align.Center);
        //Level Bar
        _levelBar = Gtk.LevelBar.New();
        _levelBar.SetValign(Gtk.Align.Center);
        _levelBar.SetSizeRequest(300, -1);
        boxLevel.Append(_levelBar);
        //Done Label
        _doneLabel = Gtk.Label.New(null);
        _doneLabel.SetValign(Gtk.Align.Center);
        _doneLabel.AddCssClass("caption");
        boxLevel.Append(_doneLabel);
        boxDone.Append(boxLevel);
        //Open Save Folder Button
        var btnOpenSaveFolder = Gtk.Button.New();
        btnOpenSaveFolder.SetValign(Gtk.Align.Center);
        btnOpenSaveFolder.AddCssClass("flat");
        btnOpenSaveFolder.SetIconName("folder-symbolic");
        btnOpenSaveFolder.SetTooltipText(localizer["OpenSaveFolder"]);
        btnOpenSaveFolder.OnClicked += OnOpenSaveFolder;
        boxDone.Append(btnOpenSaveFolder);
        //View Stack
        _viewStack = Adw.ViewStack.New();
        _viewStack.AddNamed(boxDownloading, "downloading");
        _viewStack.AddNamed(boxDone, "done");
        AddSuffix(_viewStack);
    }

    /// <summary>
    /// Starts the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public async Task StartAsync(bool embedMetadata)
    {
        var success = await _download.RunAsync(embedMetadata, new Progress<DownloadProgress>((x) => 
        {
            _lastProgress = x;
            SetTitle(_download.Filename);
            switch (x.State)
            {
                case DownloadState.PreProcessing:
                case DownloadState.PostProcessing:
                    _processingCallback = (d) =>
                    {
                        _progBar.Pulse();
                        _progLabel.SetText(_localizer["DownloadState", "Processing"]);
                        return _lastProgress.State == DownloadState.PreProcessing || _lastProgress.State == DownloadState.PostProcessing;
                    };
                    g_timeout_add(30, _processingCallback, 0);
                    break;
                case DownloadState.Downloading:
                    _downloadingCallback = (d) =>
                    {
                        _progBar.SetFraction(x.Progress);
                        _progLabel.SetText(string.Format(_localizer["DownloadState", "Downloading"], x.Progress * 100, x.DownloadSpeed));
                        return false;
                    };
                    g_idle_add(_downloadingCallback, 0);
                    break;
            }
        }));
        _imgStatus.RemoveCssClass("accent");
        _imgStatus.AddCssClass(success ? "success" : "error");
        _imgStatus.SetFromIconName(success ? "emblem-ok-symbolic" : "process-stop-symbolic");
        _viewStack.SetVisibleChildName("done");
        _levelBar.SetValue(success ? 1 : 0);
        _doneLabel.SetText(success ? _localizer["Success"] : _localizer["Error"]);
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop()
    {
        _download.Stop();
        _progBar.SetFraction(1.0);
        _imgStatus.RemoveCssClass("accent");
        _imgStatus.AddCssClass("error");
        _viewStack.SetVisibleChildName("done");
        _levelBar.SetValue(0);
        _doneLabel.SetText(_localizer["Stopped"]);
    }

    /// <summary>
    /// Occurs when the open save folder button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void OnOpenSaveFolder(Gtk.Button sender, EventArgs e) => Gtk.Functions.ShowUri(null, "file://" + _download.SaveFolder, 0);
}