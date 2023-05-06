﻿using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Linq;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A manager for downloads
/// </summary>
public class DownloadManager
{
    private Localizer _localizer;
    private int _maxNumberOfActiveDownloads;
    private Dictionary<Guid, Download> _downloading;
    private Dictionary<Guid, (Download Download, bool UseAria, bool EmbedMetadata)> _queued;
    private Dictionary<Guid, Download> _completed;
    private Dictionary<Guid, DownloadProgressState> _progressStates;

    /// <summary>
    /// Whether or not any downloads are running
    /// </summary>
    public bool AreDownloadsRunning => _downloading.Count > 0;
    /// <summary>
    /// The number of remaining downloads
    /// </summary>
    public int RemainingDownloadsCount => _downloading.Count + _queued.Count;

    /// <summary>
    /// Occurs when a download is added
    /// </summary>
    public EventHandler<(Guid Id, string Filename, string SaveFolder, bool IsDownloading)>? DownloadAdded;
    /// <summary>
    /// Occurs when a download's progress is changed
    /// </summary>
    public EventHandler<(Guid Id, DownloadProgressState State)>? DownloadProgressUpdated;
    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    public EventHandler<(Guid Id, bool Successful)>? DownloadCompleted;
    /// <summary>
    /// Occurs when a download is stopped
    /// </summary>
    public EventHandler<Guid>? DownloadStopped;
    /// <summary>
    /// Occurs when a download is retried
    /// </summary>
    public EventHandler<Guid>? DownloadRetried;
    /// <summary>
    /// Occurs when a download from the queue starts downloading
    /// </summary>
    public EventHandler<Guid>? DownloadStartedFromQueue;

    /// <summary>
    /// Constructs a DownloadManager
    /// </summary>
    /// <param name="maxNumberOfActiveDownloads">The maximum number of active downloads</param>
    /// <param name="localizer">The Localizer for strings</param>
    public DownloadManager(int maxNumberOfActiveDownloads, Localizer localizer)
    {
        _localizer = localizer;
        _downloading = new Dictionary<Guid, Download>();
        _queued = new Dictionary<Guid, (Download Download, bool UseAria, bool EmbedMetadata)>();
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
                firstPair.Value.Download.Start(firstPair.Value.UseAria, firstPair.Value.EmbedMetadata, _localizer);
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
                if(!pair.Value.IsSuccess)
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
                result += _progressStates[pair.Value.Id].Progress;
            }
            result /= (_downloading.Count + _queued.Count) > 0 ? (_downloading.Count + _queued.Count) : 1;
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
                totalSpeed += _progressStates[pair.Value.Id].Speed;
            }
            return totalSpeed.GetSpeedString(_localizer);
        }
    }

    /// <summary>
    /// The background activity report string
    /// </summary>
    public string BackgroundActivityReport
    {
        get
        {
            if ((_downloading.Count + _queued.Count) > 0)
            {
                return string.Format(_localizer["BackgroundActivityReport"], _downloading.Count + _queued.Count, TotalProgress * 100, TotalSpeedString);
            }
            else if (ErrorsCount > 0)
            {
                return _localizer["FinishedWithErrors"];
            }
            else
            {
                return _localizer["NoDownloadsRunning"];
            }
        }
    }

    /// <summary>
    /// Adds a download
    /// </summary>
    /// <param name="download"></param>
    /// <param name="useAria"></param>
    /// <param name="embedMetadata"></param>
    public void AddDownload(Download download, bool useAria, bool embedMetadata)
    {
        download.ProgressChanged += Download_ProgressChanged;
        download.Completed += Download_Compelted;
        if(_downloading.Count < MaxNumberOfActiveDownloads)
        {
            _downloading.Add(download.Id, download);
            DownloadAdded?.Invoke(this, (download.Id, download.Filename, download.SaveFolder, true));
            download.Start(useAria, embedMetadata, _localizer);
        }
        else
        {
            _queued.Add(download.Id, (download, useAria, embedMetadata));
            DownloadAdded?.Invoke(this, (download.Id, download.Filename, download.SaveFolder, false));
        }
    }

    /// <summary>
    /// Requests for a download to stop
    /// </summary>
    /// <param name="id">The id of the download</param>
    public void RequestStop(Guid id)
    {
        var stopped = false;
        if(_downloading.ContainsKey(id))
        {
            _downloading[id].Stop();
            _completed.Add(id, _downloading[id]);
            _downloading.Remove(id);
            stopped = true;
        }
        if(_queued.ContainsKey(id))
        {
            _completed.Add(id, _queued[id].Download);
            _queued.Remove(id);
            stopped = true;
        }
        if(stopped)
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
    public void RequestRetry(Guid id, bool useAria, bool embedMetadata)
    {
        if(_completed.ContainsKey(id))
        {
            var download = _completed[id];
            _completed.Remove(id);
            DownloadRetried?.Invoke(this, id);
            AddDownload(download, useAria, embedMetadata);
        }
    }

    /// <summary>
    /// Requests all downloads to be stopped
    /// </summary>
    public void StopAllDownloads()
    {
        foreach (var pair in _queued)
        {
            RequestStop(pair.Key);
        }
        foreach (var pair in _downloading)
        {
            RequestStop(pair.Key);
        }
    }

    /// <summary>
    /// Requests all failed downloads to be retried
    /// </summary>
    /// <param name="useAria">Whether or not to use aria2 downloader</param>
    /// <param name="embedMetadata">Whether or not to emebed metadata</param>
    public void RetryFailedDownloads(bool useAria, bool embedMetadata)
    {
        foreach (var pair in _completed)
        {
            RequestRetry(pair.Key, useAria, embedMetadata);
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
        _progressStates[download.Id] = e;
        DownloadProgressUpdated?.Invoke(this, (download.Id, e));
    }

    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="successful">Whether or not the download was successful</param>
    private void Download_Compelted(object? sender, bool successful)
    {
        var download = (Download)sender!;
        _completed.Add(download.Id, _downloading[download.Id]);
        _downloading.Remove(download.Id);
        DownloadCompleted?.Invoke(this, (download.Id, successful));
        if (_downloading.Count < MaxNumberOfActiveDownloads && _queued.Count > 0)
        {
            var firstPair = _queued.First();
            _downloading.Add(firstPair.Key, firstPair.Value.Download);
            _queued.Remove(firstPair.Key);
            DownloadStartedFromQueue?.Invoke(this, firstPair.Key);
            firstPair.Value.Download.Start(firstPair.Value.UseAria, firstPair.Value.EmbedMetadata, _localizer);
        }
    }
}
