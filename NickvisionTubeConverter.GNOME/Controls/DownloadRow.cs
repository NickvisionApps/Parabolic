using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using YoutubeDLSharp;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A DownloadRow for the downloads page
/// </summary>
public partial class DownloadRow : Adw.Bin, IDownloadRowControl
{
    private delegate bool GSourceFunc(nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_idle_add(GSourceFunc function, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_timeout_add(uint interval, GSourceFunc function, nint data);

    private readonly Localizer _localizer;
    private readonly Download _download;
    private bool? _previousEmbedMetadata;
    private readonly Gtk.Box _boxMain;
    private readonly Gtk.Image _imgStatus;
    private readonly Gtk.Box _boxInfo;
    private readonly Gtk.Label _lblFilename;
    private readonly Gtk.Label _lblUrl;
    private readonly Adw.ViewStack _viewStackState;
    private readonly Gtk.Box _boxDownload;
    private readonly Gtk.Box _boxDone;
    private readonly Gtk.Label _progLabel;
    private readonly Gtk.ProgressBar _progBar;
    private readonly Gtk.LevelBar _levelBar;
    private readonly Gtk.Label _doneLabel;
    private readonly Adw.ViewStack _viewStackAction;
    private readonly Gtk.Button _btnCancel;
    private readonly Gtk.Button _btnOpenFolder;
    private readonly Gtk.Button _btnRetry;
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
        _previousEmbedMetadata = null;
        _boxMain = Gtk.Box.New(Gtk.Orientation.Horizontal, 6);
        //Status Image
        _imgStatus = Gtk.Image.NewFromIconName("folder-download-symbolic");
        _imgStatus.SetPixelSize(24);
        _imgStatus.SetMarginStart(10);
        _imgStatus.SetMarginEnd(5);
        _boxMain.Append(_imgStatus);
        //Info Box
        _boxInfo = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        _boxInfo.SetMarginTop(8);
        _boxInfo.SetMarginBottom(8);
        _boxMain.Append(_boxInfo);
        //Download Filename
        _lblFilename = Gtk.Label.New(download.Filename);
        _lblFilename.SetHalign(Gtk.Align.Start);
        _lblFilename.SetEllipsize(Pango.EllipsizeMode.End);
        _lblFilename.SetLines(1);
        _boxInfo.Append(_lblFilename);
        //Download Url
        _lblUrl = Gtk.Label.New(download.VideoUrl);
        _lblUrl.SetHalign(Gtk.Align.Start);
        _lblUrl.SetEllipsize(Pango.EllipsizeMode.End);
        _lblUrl.SetLines(1);
        _lblUrl.AddCssClass("caption");
        _lblUrl.AddCssClass("dim-label");
        _boxInfo.Append(_lblUrl);
        //State View Stack
        _viewStackState = Adw.ViewStack.New();
        _viewStackState.SetMarginTop(8);
        _boxInfo.Append(_viewStackState);
        //Download View
        _boxDownload = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        _viewStackState.AddNamed(_boxDownload, "downloading");
        //Download Progress Bar
        _progBar = Gtk.ProgressBar.New();
        _progBar.SetHexpand(true);
        _boxDownload.Append(_progBar);
        //Download Progress Label
        _progLabel = Gtk.Label.New(_localizer["DownloadState", "Preparing"]);
        _progLabel.SetHalign(Gtk.Align.Start);
        _progLabel.AddCssClass("caption");
        _boxDownload.Append(_progLabel);
        //Done View
        _boxDone = Gtk.Box.New(Gtk.Orientation.Vertical, 3);
        _viewStackState.AddNamed(_boxDone, "done");
        //Done Level Bar
        _levelBar = Gtk.LevelBar.New();
        _boxDone.Append(_levelBar);
        //Done Label
        _doneLabel = Gtk.Label.New(null);
        _doneLabel.SetHalign(Gtk.Align.Start);
        _doneLabel.AddCssClass("caption");
        _boxDone.Append(_doneLabel);
        //Action Button
        _viewStackAction = Adw.ViewStack.New();
        _viewStackAction.SetMarginStart(5);
        _viewStackAction.SetMarginEnd(10);
        _boxMain.Append(_viewStackAction);
        //Cancel Button
        _btnCancel = Gtk.Button.New();
        _btnCancel.SetValign(Gtk.Align.Center);
        _btnCancel.SetIconName("media-playback-stop-symbolic");
        _btnCancel.SetTooltipText(_localizer["StopDownload"]);
        _btnCancel.AddCssClass("circular");
        _btnCancel.OnClicked += (sender, e) => Stop();
        _viewStackAction.AddNamed(_btnCancel, "cancel");
        //Open Folder Button
        _btnOpenFolder = Gtk.Button.New();
        _btnOpenFolder.SetValign(Gtk.Align.Center);
        _btnOpenFolder.SetIconName("folder-symbolic");
        _btnOpenFolder.SetTooltipText(_localizer["OpenSaveFolder"]);
        _btnOpenFolder.AddCssClass("circular");
        _btnOpenFolder.OnClicked += (sender, e) => Gtk.Functions.ShowUri(null, "file://" + _download.SaveFolder, 0);
        _viewStackAction.AddNamed(_btnOpenFolder, "open-folder");
        //Retry Button
        _btnRetry = Gtk.Button.New();
        _btnRetry.SetValign(Gtk.Align.Center);
        _btnRetry.SetIconName("view-refresh-symbolic");
        _btnRetry.SetTooltipText(_localizer["RetryDownload"]);
        _btnRetry.AddCssClass("circular");
        _btnRetry.OnClicked += async (sender, e) => await StartAsync(_previousEmbedMetadata ?? false);
        _viewStackAction.AddNamed(_btnRetry, "retry");
        SetChild(_boxMain);
    }

    /// <summary>
    /// Starts the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public async Task StartAsync(bool embedMetadata)
    {
        if(_previousEmbedMetadata == null)
        {
            _previousEmbedMetadata = embedMetadata;
        }
        _imgStatus.AddCssClass("accent");
        _imgStatus.RemoveCssClass("error");
        _imgStatus.SetFromIconName("folder-download-symbolic");
        _viewStackState.SetVisibleChildName("downloading");
        _progLabel.SetText(_localizer["DownloadState", "Preparing"]);
        _lblFilename.SetText(_download.Filename);
        var success = await _download.RunAsync(embedMetadata, (state) =>
        {
            switch (state.Status)
            {
                case ProgressStatus.Downloading:
                    _downloadingCallback = (d) =>
                    {
                        _progBar.SetFraction(state.Progress);
                        _progLabel.SetText(string.Format(_localizer["DownloadState", "Downloading"], state.Progress * 100, state.Speed));
                        return false;
                    };
                    g_idle_add(_downloadingCallback, 0);
                    break;
                default:
                    _processingCallback = (d) =>
                    {
                        _progBar.Pulse();
                        _progLabel.SetText(_localizer["DownloadState", "Processing"]);
                        return state.Status == ProgressStatus.Processing;
                    };
                    g_timeout_add(30, _processingCallback, 0);
                    break;
            }
        });
        _imgStatus.RemoveCssClass("accent");
        _imgStatus.AddCssClass(success ? "success" : "error");
        _imgStatus.SetFromIconName(success ? "emblem-ok-symbolic" : "process-stop-symbolic");
        _viewStackState.SetVisibleChildName("done");
        _levelBar.SetValue(success ? 1 : 0);
        _doneLabel.SetText(success ? _localizer["Success"] : _localizer["Error"]);
        _viewStackAction.SetVisibleChildName(success ? "open-folder" : "retry");
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
        _viewStackState.SetVisibleChildName("done");
        _levelBar.SetValue(0);
        _doneLabel.SetText(_localizer["Stopped"]);
        _viewStackAction.SetVisibleChildName("retry");
    }
}