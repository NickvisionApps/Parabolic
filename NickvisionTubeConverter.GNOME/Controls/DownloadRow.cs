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
public partial class DownloadRow : Gtk.ListBoxRow, IDownloadRowControl
{
    private delegate bool GSourceFunc(nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_idle_add(GSourceFunc function, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_timeout_add(uint interval, GSourceFunc function, nint data);

    private readonly Localizer _localizer;
    private readonly Download _download;
    private readonly Gtk.Image _imgStatus;
    private readonly Gtk.Label _lblFilename;
    private readonly Adw.ViewStack _viewStackState;
    private readonly Gtk.Label _progLabel;
    private readonly Gtk.ProgressBar _progBar;
    private readonly Gtk.LevelBar _levelBar;
    private readonly Gtk.Label _doneLabel;
    private readonly Adw.ViewStack _viewStackAction;
    private DownloadProgress? _lastProgress;
    private GSourceFunc? _processingCallback;
    private GSourceFunc? _downloadingCallback;

    /// <summary>
    /// Whether or not the download is done
    /// </summary>
    public bool IsDone => _download.IsDone;
    /// <summary>
    /// Whether or not the download finished successfully
    /// </summary>
    public bool _success;

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="download">The download displayed by the row</param>
    public DownloadRow(Localizer localizer, Download download)
    {
        _localizer = localizer;
        _download = download;
        var box = Gtk.Box.New(Gtk.Orientation.Horizontal, 6);
        //Status Image
        _imgStatus = Gtk.Image.NewFromIconName("folder-download-symbolic");
        _imgStatus.SetPixelSize(20);
        box.Append(_imgStatus);
        //Info Box
        var boxInfo = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        box.Append(boxInfo);
        //Download Filename
        _lblFilename = Gtk.Label.New(download.Filename);
        _lblFilename.SetHalign(Gtk.Align.Start);
        _lblFilename.SetWrap(true);
        boxInfo.Append(_lblFilename);
        //Download Url
        var lblUrl = Gtk.Label.New(download.VideoUrl);
        lblUrl.SetHalign(Gtk.Align.Start);
        lblUrl.SetWrap(true);
        lblUrl.AddCssClass("caption");
        boxInfo.Append(lblUrl);
        //State View Stack
        _viewStackState = Adw.ViewStack.New();
        boxInfo.Append(_viewStackState);
        //Download View
        var boxDownload = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        _viewStackState.AddNamed(boxDownload, "downloading");
        //Download Progress Bar
        _progBar = Gtk.ProgressBar.New();
        _progBar.SetHexpand(true);
        boxDownload.Append(_progBar);
        //Download Progress Label
        _progLabel = Gtk.Label.New(_localizer["DownloadState", "Preparing"]);
        _progLabel.SetHalign(Gtk.Align.Start);
        boxDownload.Append(_progLabel);
        //Done View
        var boxDone = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        _viewStackState.AddNamed(boxDone, "done");
        //Done Level Bar
        _levelBar = Gtk.LevelBar.New();
        boxDone.Append(_levelBar);
        //Done Label
        _doneLabel = Gtk.Label.New(null);
        _doneLabel.SetHalign(Gtk.Align.Start);
        boxDone.Append(_doneLabel);
        //Action Button
        _viewStackAction = Adw.ViewStack.New();
        box.Append(_viewStackAction);
        //Cancel Button
        var btnCancel = Gtk.Button.New();
        btnCancel.SetValign(Gtk.Align.Center);
        btnCancel.SetIconName("media-playback-stop-symbolic");
        btnCancel.SetTooltipText(_localizer["StopDownload"]);
        btnCancel.AddCssClass("flat");
        btnCancel.OnClicked += (sender, e) => Stop();
        _viewStackAction.AddNamed(btnCancel, "cancel");
        //Open Folder Button
        var btnOpenFolder = Gtk.Button.New();
        btnOpenFolder.SetValign(Gtk.Align.Center);
        btnOpenFolder.SetIconName("folder-symbolic");
        btnOpenFolder.SetTooltipText(_localizer["OpenSaveFolder"]);
        btnOpenFolder.AddCssClass("flat");
        btnOpenFolder.OnClicked += (sender, e) => Gtk.Functions.ShowUri(null, "file://" + _download.SaveFolder, 0);
        _viewStackAction.AddNamed(btnOpenFolder, "open-folder");
        //Retry Button
        var btnRetry = Gtk.Button.New();
        btnRetry.SetValign(Gtk.Align.Center);
        btnRetry.SetIconName("view-refresh-symbolic");
        btnRetry.SetTooltipText(_localizer["RetryDownload"]);
        btnRetry.AddCssClass("flat");
        btnRetry.OnClicked += (sender, e) => StartAsync(true);
        _viewStackAction.AddNamed(btnRetry, "retry");
        SetChild(box);
    }

    /// <summary>
    /// Starts the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public async Task StartAsync(bool embedMetadata)
    {
        _imgStatus.AddCssClass("accent");
        _imgStatus.RemoveCssClass("error");
        _imgStatus.SetFromIconName("folder-download-symbolic");
        _viewStackState.SetVisibleChildName("downloading");
        _progLabel.SetText(_localizer["DownloadState", "Preparing"]);
        _success = await _download.RunAsync(embedMetadata, new Progress<DownloadProgress>((x) =>
        {
            _lastProgress = x;
            _lblFilename.SetText(_download.Filename);
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
        _imgStatus.AddCssClass(_success ? "success" : "error");
        _imgStatus.SetFromIconName(_success ? "emblem-ok-symbolic" : "process-stop-symbolic");
        _viewStackState.SetVisibleChildName("done");
        _levelBar.SetValue(_success ? 1 : 0);
        _doneLabel.SetText(_success ? _localizer["Success"] : _localizer["Error"]);
        _viewStackAction.SetVisibleChildName(_success ? "open-folder" : "retry");
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop()
    {
        _success = false;
        _download.Stop();
        _progBar.SetFraction(1.0);
        _imgStatus.RemoveCssClass("accent");
        _imgStatus.AddCssClass("error");
        _viewStackState.SetVisibleChildName("done");
        _levelBar.SetValue(0);
        _doneLabel.SetText(_localizer["Stopped"]);
        _viewStackAction.SetVisibleChildName("retry");
    }
}