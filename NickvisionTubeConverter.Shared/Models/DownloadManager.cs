using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Linq;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A manager for downloads
/// </summary>
public class DownloadManager
{
    private int _maxNumberOfActiveDownloads;
    private Dictionary<Guid, Download> _downloading;
    private Dictionary<Guid, (Download Download, bool UseAria, bool EmbedMetadata, string? CookiesPath, int AriaMaxConnectionsPerServer, int AriaMinSplitSize)> _queued;
    private Dictionary<Guid, Download> _completed;
    private Dictionary<Guid, DownloadProgressState> _progressStates;

    /// <summary>
    /// Whether or not any downloads are running
    /// </summary>
    public bool AreDownloadsRunning => _downloading.Count > 0;
    /// <summary>
    /// Whether or not any downloads are in the queue
    /// </summary>
    public bool AreDownloadsQueued => _queued.Count > 0;
    /// <summary>
    /// Whether or not any downloads are completed
    /// </summary>
    public bool AreDownloadsCompleted => _completed.Count > 0;
    /// <summary>
    /// The number of remaining downloads
    /// </summary>
    public int RemainingDownloadsCount => _downloading.Count + _queued.Count;

    /// <summary>
    /// Occurs when a download is added
    /// </summary>
    public event EventHandler<(Guid Id, string Filename, string SaveFolder, bool IsDownloading)>? DownloadAdded;
    /// <summary>
    /// Occurs when a download's progress is changed
    /// </summary>
    public event EventHandler<(Guid Id, DownloadProgressState State)>? DownloadProgressUpdated;
    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    public event EventHandler<(Guid Id, bool Successful)>? DownloadCompleted;
    /// <summary>
    /// Occurs when a download is stopped
    /// </summary>
    public event EventHandler<Guid>? DownloadStopped;
    /// <summary>
    /// Occurs when a download is retried
    /// </summary>
    public event EventHandler<Guid>? DownloadRetried;
    /// <summary>
    /// Occurs when a download from the queue starts downloading
    /// </summary>
    public event EventHandler<Guid>? DownloadStartedFromQueue;

    /// <summary>
    /// Constructs a DownloadManager
    /// </summary>
    /// <param name="maxNumberOfActiveDownloads">The maximum number of active downloads</param>
    public DownloadManager(int maxNumberOfActiveDownloads)
    {
        _downloading = new Dictionary<Guid, Download>();
        _queued = new Dictionary<Guid, (Download Download, bool UseAria, bool EmbedMetadata, string? CookiesPath, int AriaMaxConnectionsPerServer, int AriaMinSplitSize)>();
        _completed = new Dictionary<Guid, Download>();
        _progressStates = new Dictionary<Guid, DownloadProgressState>();
        _maxNumberOfActiveDownloads = maxNumberOfActiveDownloads;
    }

    /// <summary>
    /// The maximum number of active downloads
    /// </summary>
    public int MaxNumberOfActiveDownloads
    {
        get => _maxNumberOfActiveDownloads;

        set
        {
            _maxNumberOfActiveDownloads = value;
            while (_downloading.Count < MaxNumberOfActiveDownloads && _queued.Count > 0)
            {
                var firstPair = _queued.First();
                _downloading.Add(firstPair.Key, firstPair.Value.Download);
                _queued.Remove(firstPair.Key);
                DownloadStartedFromQueue?.Invoke(this, firstPair.Key);
                firstPair.Value.Download.Start(firstPair.Value.UseAria, firstPair.Value.EmbedMetadata, firstPair.Value.CookiesPath, firstPair.Value.AriaMaxConnectionsPerServer, firstPair.Value.AriaMinSplitSize);
            }
        }
    }

    /// <summary>
    /// Downloading errors count
    /// </summary>
    public uint ErrorsCount
    {
        get
        {
            var result = 0u;
            foreach (var pair in _completed)
            {
                if (!pair.Value.IsSuccess)
                {
                    result++;
                }
            }
            return result;
        }
    }

    /// <summary>
    /// The total download progress
    /// </summary>
    public double TotalProgress
    {
        get
        {
            var result = 0.0;
            foreach (var pair in _downloading)
            {
                if (_progressStates.ContainsKey(pair.Value.Id))
                {
                    result += _progressStates[pair.Value.Id].Progress;
                }
            }
            result /= (RemainingDownloadsCount) > 0 ? (RemainingDownloadsCount) : 1;
            return result;
        }
    }

    /// <summary>
    /// The total download speed string
    /// </summary>
    public string TotalSpeedString
    {
        get
        {
            var totalSpeed = 0.0;
            foreach (var pair in _downloading)
            {
                if (_progressStates.ContainsKey(pair.Value.Id))
                {
                    totalSpeed += _progressStates[pair.Value.Id].Speed;
                }
            }
            return totalSpeed.GetSpeedString();
        }
    }

    /// <summary>
    /// The background activity report string
    /// </summary>
    public string BackgroundActivityReport
    {
        get
        {
            if (RemainingDownloadsCount > 0)
            {
                return _n("{0} download — {1:f1}% ({2})", "{0} downloads — {1:f1}% ({2})", RemainingDownloadsCount, RemainingDownloadsCount, TotalProgress * 100, TotalSpeedString);
            }
            else if (ErrorsCount > 0)
            {
                return _("Some downloads finished with errors!");
            }
            else
            {
                return _("No downloads running");
            }
        }
    }

    /// <summary>
    /// Adds a download
    /// </summary>
    /// <param name="download">The Download model</param>
    /// <param name="useAria">Whether or not to use aria2 for the download</param>
    /// <param name="embedMetadata">Whether or not to embed media metadata in the downloaded file</param>
    /// <param name="cookiesPath">The path to the cookies file to use for yt-dlp</param>
    /// <param name="ariaMaxConnectionsPerServer">The maximum number of connections to one server for each download (-x)</param>
    /// <param name="ariaMinSplitSize">The minimum size of which to split a file (-k)</param>
    public void AddDownload(Download download, bool useAria, bool embedMetadata, string? cookiesPath, int ariaMaxConnectionsPerServer, int ariaMinSplitSize)
    {
        download.ProgressChanged += Download_ProgressChanged;
        download.Completed += Download_Completed;
        if (_downloading.Count < MaxNumberOfActiveDownloads)
        {
            _downloading.Add(download.Id, download);
            DownloadAdded?.Invoke(this, (download.Id, download.Filename, download.SaveFolder, true));
            download.Start(useAria, embedMetadata, cookiesPath, ariaMaxConnectionsPerServer, ariaMinSplitSize);
        }
        else
        {
            _queued.Add(download.Id, (download, useAria, embedMetadata, cookiesPath, ariaMaxConnectionsPerServer, ariaMinSplitSize));
            DownloadAdded?.Invoke(this, (download.Id, download.Filename, download.SaveFolder, false));
        }
    }

    /// <summary>
    /// Requests for a download to stop
    /// </summary>
    /// <param name="id">The id of the download</param>
    /// <param name="updateUI">Whether or not to update the UI when download stopped</param>
    public void RequestStop(Guid id, bool updateUI = true)
    {
        var stopped = false;
        if (_downloading.ContainsKey(id))
        {
            _downloading[id].Stop();
            _completed.Add(id, _downloading[id]);
            _downloading.Remove(id);
            stopped = true;
        }
        if (_queued.ContainsKey(id))
        {
            _completed.Add(id, _queued[id].Download);
            _queued.Remove(id);
            stopped = true;
        }
        if (stopped && updateUI)
        {
            DownloadStopped?.Invoke(this, id);
        }
    }

    /// <summary>
    /// Requests for a download to be retried
    /// </summary>
    /// <param name="id">The id of the download</param>
    /// <param name="useAria">Whether or not to use aria2 downloader</param>
    /// <param name="embedMetadata">Whether or not to emebed metadata</param>
    /// <param name="cookiesPath">The path to the cookies file to use for yt-dlp</param>
    /// <param name="ariaMaxConnectionsPerServer">The maximum number of connections to one server for each download (-x)</param>
    /// <param name="ariaMinSplitSize">The minimum size of which to split a file (-k)</param>
    public void RequestRetry(Guid id, bool useAria, bool embedMetadata, string? cookiesPath, int ariaMaxConnectionsPerServer, int ariaMinSplitSize)
    {
        if (_completed.ContainsKey(id))
        {
            var download = _completed[id];
            _completed.Remove(id);
            DownloadRetried?.Invoke(this, id);
            AddDownload(download, useAria, embedMetadata, cookiesPath, ariaMaxConnectionsPerServer, ariaMinSplitSize);
        }
    }

    /// <summary>
    /// Requests all downloads to be stopped
    /// </summary>
    /// <param name="updateUI">Whether or not to update the UI when downloads are stopped</param>
    public void StopAllDownloads(bool updateUI)
    {
        foreach (var pair in _queued)
        {
            RequestStop(pair.Key, updateUI);
        }
        foreach (var pair in _downloading)
        {
            RequestStop(pair.Key, updateUI);
        }
    }

    /// <summary>
    /// Requests all failed downloads to be retried
    /// </summary>
    /// <param name="useAria">Whether or not to use aria2 downloader</param>
    /// <param name="embedMetadata">Whether or not to emebed metadata</param>
    /// <param name="cookiesPath">The path to the cookies file to use for yt-dlp</param>
    /// <param name="ariaMaxConnectionsPerServer">The maximum number of connections to one server for each download (-x)</param>
    /// <param name="ariaMinSplitSize">The minimum size of which to split a file (-k)</param>
    public void RetryFailedDownloads(bool useAria, bool embedMetadata, string? cookiesPath, int ariaMaxConnectionsPerServer, int ariaMinSplitSize)
    {
        foreach (var pair in _completed)
        {
            if (!pair.Value.IsSuccess)
            {
                RequestRetry(pair.Key, useAria, embedMetadata, cookiesPath, ariaMaxConnectionsPerServer, ariaMinSplitSize);
            }
        }
    }

    /// <summary>
    /// Clears all downloads from queue
    /// </summary>
    public void ClearQueuedDownloads() => _queued.Clear();

    /// <summary>
    /// Occurs when a download's progress is changed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">DownloadProgressState</param>
    private void Download_ProgressChanged(object? sender, DownloadProgressState e)
    {
        var download = (Download)sender!;
        if (!download.WasStopped)
        {
            _progressStates[download.Id] = e;
            DownloadProgressUpdated?.Invoke(this, (download.Id, e));
        }
    }

    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="successful">Whether or not the download was successful</param>
    private void Download_Completed(object? sender, bool successful)
    {
        var download = (Download)sender!;
        if (!download.WasStopped && _downloading.ContainsKey(download.Id))
        {
            _completed.Add(download.Id, _downloading[download.Id]);
            _downloading.Remove(download.Id);
            DownloadCompleted?.Invoke(this, (download.Id, successful));
        }
        if (_downloading.Count < MaxNumberOfActiveDownloads && _queued.Count > 0)
        {
            var firstPair = _queued.First();
            _downloading.Add(firstPair.Key, firstPair.Value.Download);
            _queued.Remove(firstPair.Key);
            DownloadStartedFromQueue?.Invoke(this, firstPair.Key);
            firstPair.Value.Download.Start(firstPair.Value.UseAria, firstPair.Value.EmbedMetadata, firstPair.Value.CookiesPath, firstPair.Value.AriaMaxConnectionsPerServer, firstPair.Value.AriaMinSplitSize);
        }
    }
}
