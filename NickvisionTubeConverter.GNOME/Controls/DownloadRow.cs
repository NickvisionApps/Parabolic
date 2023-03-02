using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

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

    private bool _disposed;
    private readonly Localizer _localizer;
    private readonly Download _download;
    private bool? _previousEmbedMetadata;
    private bool _wasStopped;
    private DownloadProgressStatus _progressStatus;
    private GSourceFunc? _processingCallback;
    private GSourceFunc? _downloadingCallback;
    private string _logMessage;

    [Gtk.Connect] private readonly Gtk.Image _statusIcon;
    [Gtk.Connect] private readonly Gtk.Label _filenameLabel;
    [Gtk.Connect] private readonly Gtk.Label _urlLabel;
    [Gtk.Connect] private readonly Adw.ViewStack _stateViewStack;
    [Gtk.Connect] private readonly Gtk.Label _progressLabel;
    [Gtk.Connect] private readonly Gtk.ProgressBar _progressBar;
    [Gtk.Connect] private readonly Gtk.LevelBar _levelBar;
    [Gtk.Connect] private readonly Gtk.Label _doneLabel;
    [Gtk.Connect] private readonly Adw.ViewStack _actionViewStack;
    [Gtk.Connect] private readonly Gtk.Button _stopButton;
    [Gtk.Connect] private readonly Gtk.Button _openFolderButton;
    [Gtk.Connect] private readonly Gtk.Button _retryButton;
    [Gtk.Connect] private readonly Gtk.ToggleButton _viewLogToggleBtn;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _scrollLog;
    [Gtk.Connect] private readonly Gtk.Label _lblLog;

    /// <summary>
    /// The callback function to run when the download is completed
    /// </summary>
    public Func<IDownloadRowControl, Task>? DownloadCompletedAsyncCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is stopped
    /// </summary>
    public Action<IDownloadRowControl>? DownloadStoppedCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is retried
    /// </summary>
    public Func<IDownloadRowControl, Task>? DownloadRetriedAsyncCallback { get; set; }

    /// <summary>
    /// Whether or not the download is done
    /// </summary>
    public bool IsDone => _download.IsDone;

    private DownloadRow(Gtk.Builder builder, Localizer localizer, Download download) : base(builder.GetPointer("_root"), false)
    {
        _disposed = false;
        _localizer = localizer;
        _download = download;
        _previousEmbedMetadata = null;
        _wasStopped = false;
        _logMessage = "";
        //Build UI
        builder.Connect(this);
        _filenameLabel.SetLabel(download.Filename);
        _urlLabel.SetLabel(download.VideoUrl);
        _stopButton.OnClicked += (sender, e) => Stop();
        _openFolderButton.OnClicked += (sender, e) => Gtk.Functions.ShowUri(null, "file://" + _download.SaveFolder, 0);
        _retryButton.OnClicked += async (sender, e) =>
        {
            if (DownloadRetriedAsyncCallback != null)
            {
                await DownloadRetriedAsyncCallback(this);
            }
        };
        _viewLogToggleBtn.OnClicked += (sender, e) => _scrollLog.SetVisible(!_scrollLog.GetVisible());
    }

    /// <summary>
    /// Constructs a DownloadRow
    /// </summary>
    /// <param name="download">The download displayed by the row</param>
    public DownloadRow(Localizer localizer, Download download) : this(Builder.FromFile("download_row.ui", localizer), localizer, download)
    {

    }

    /// <summary>
    /// Frees resources used by the DownloadRow object
    /// </summary>
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Frees resources used by the DownloadRow object
    /// </summary>
    protected virtual void Dispose(bool disposing)
    {
        if (_disposed)
        {
            return;
        }
        if (disposing)
        {
            _download.Dispose();
        }
        _disposed = true;
    }

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public async Task RunAsync(bool embedMetadata)
    {
        if (_previousEmbedMetadata == null)
        {
            _previousEmbedMetadata = embedMetadata;
        }
        _wasStopped = false;
        _statusIcon.AddCssClass("accent");
        _statusIcon.RemoveCssClass("error");
        _statusIcon.SetFromIconName("folder-download-symbolic");
        _stateViewStack.SetVisibleChildName("downloading");
        _progressLabel.SetText(_localizer["DownloadState", "Preparing"]);
        _filenameLabel.SetText(_download.Filename);
        _actionViewStack.SetVisibleChildName("cancel");
        _progressBar.SetFraction(0);
        var success = await _download.RunAsync(embedMetadata, (state) =>
        {
            _progressStatus = state.Status;
            _logMessage = state.Log + "\n";
            switch (state.Status)
            {
                case DownloadProgressStatus.Downloading:
                    _downloadingCallback = (d) =>
                    {
                        _progressBar.SetFraction(state.Progress);
                        _progressLabel.SetText(string.Format(_localizer["DownloadState", "Downloading"], state.Progress * 100, state.Speed));
                        _lblLog.SetLabel(_logMessage);
                        var vadjustment = _scrollLog.GetVadjustment();
                        vadjustment.SetValue(vadjustment.GetUpper() - vadjustment.GetPageSize());
                        return false;
                    };
                    g_idle_add(_downloadingCallback, 0);
                    break;
                case DownloadProgressStatus.Processing:
                    _progressLabel.SetText(_localizer["DownloadState", "Processing"]);
                    if (_processingCallback == null)
                    {
                        _processingCallback = (d) =>
                        {
                            _progressBar.Pulse();
                            _lblLog.SetLabel(_logMessage);
                            var vadjustment = _scrollLog.GetVadjustment();
                            vadjustment.SetValue(vadjustment.GetUpper() - vadjustment.GetPageSize());
                            if (_progressStatus != DownloadProgressStatus.Processing || IsDone)
                            {
                                _processingCallback = null;
                                return false;
                            }
                            return true;
                        };
                        g_timeout_add(30, _processingCallback, 0);
                    }
                    break;
            }
        });
        if (!_wasStopped)
        {
            _statusIcon.RemoveCssClass("accent");
            _statusIcon.AddCssClass(success ? "success" : "error");
            _statusIcon.SetFromIconName(success ? "emblem-ok-symbolic" : "process-stop-symbolic");
            _stateViewStack.SetVisibleChildName("done");
            _levelBar.SetValue(success ? 1 : 0);
            _doneLabel.SetText(success ? _localizer["Success"] : _localizer["Error"]);
            _actionViewStack.SetVisibleChildName(success ? "open-folder" : "retry");
        }
        if (DownloadCompletedAsyncCallback != null)
        {
            await DownloadCompletedAsyncCallback(this);
        }
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop()
    {
        _wasStopped = true;
        _download.Stop();
        _progressBar.SetFraction(1.0);
        _statusIcon.RemoveCssClass("accent");
        _statusIcon.AddCssClass("error");
        _statusIcon.SetFromIconName("process-stop-symbolic");
        _stateViewStack.SetVisibleChildName("done");
        _levelBar.SetValue(0);
        _doneLabel.SetText(_localizer["Stopped"]);
        _actionViewStack.SetVisibleChildName("retry");
        if (DownloadStoppedCallback != null)
        {
            DownloadStoppedCallback(this);
        }
    }
}