using ATL;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DownloadService : IDisposable, IDownloadService
{
    private readonly IJsonFileService _jsonFileService;
    private readonly ITranslationService _translationService;
    private readonly IYtdlpExecutableService _ytdlpService;
    private readonly IHistoryService _historyService;
    private readonly IRecoveryService _recoveryService;
    private readonly Dictionary<int, Download> _downloading;
    private readonly Dictionary<int, Download> _queued;
    private readonly Dictionary<int, Download> _completed;

    public event EventHandler<DownloadAddedEventArgs>? DownloadAdded;
    public event EventHandler<DownloadCompletedEventArgs>? DownloadCompleted;
    public event EventHandler<DownloadCredentialRequiredEventArgs>? DownloadCredentialRequired;
    public event EventHandler<DownloadProgressChangedEventArgs>? DownloadProgressChanged;
    public event EventHandler<DownloadEventArgs>? DownloadRetired;
    public event EventHandler<DownloadEventArgs>? DownloadStartedFromQueue;
    public event EventHandler<DownloadEventArgs>? DownloadStopped;

    public int DownloadingCount => _downloading.Count;
    public int QueuedCount => _queued.Count;
    public int CompletedCount => _completed.Count;

    public DownloadService(IJsonFileService jsonFileService, ITranslationService translationService, IYtdlpExecutableService ytdlpService, IHistoryService historyService, IRecoveryService recoveryService)
    {
        _jsonFileService = jsonFileService;
        _translationService = translationService;
        _ytdlpService = ytdlpService;
        _historyService = historyService;
        _recoveryService = recoveryService;
        _downloading = new Dictionary<int, Download>();
        _queued = new Dictionary<int, Download>();
        _completed = new Dictionary<int, Download>();
    }

    ~DownloadService()
    {
        Dispose(false);
    }

    public async Task AddAsync(DownloadOptions options, bool excludeFromHistory)
    {
        var downloaderOptions = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
        var download = new Download(options, _translationService);
        download.Completed += Download_Completed;
        download.ProgressChanged += Download_ProgressChanged;
        await _recoveryService.AddAsync(new RecoverableDownload(download.Id, download.Options));
        if (!excludeFromHistory)
        {
            await _historyService.AddAsync(new HistoricDownload(download.Options.Url)
            {
                Title = Path.GetFileNameWithoutExtension(download.FilePath),
                Path = download.FilePath
            });
        }
        if (_downloading.Count < downloaderOptions.MaxNumberOfActiveDownloads)
        {
            _downloading.Add(download.Id, download);
            DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download.Id, download.FilePath, download.Options.Url, DownloadStatus.Running));
            download.Start(_ytdlpService.ExecutablePath ?? "yt-dlp", downloaderOptions);
        }
        else
        {
            _queued.Add(download.Id, download);
            DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download));
        }
    }

    public async Task AddAsync(IReadOnlyList<DownloadOptions> options, bool excludeFromHistory)
    {
        var downloaderOptions = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
        var ytdlpExecutablePath = _ytdlpService.ExecutablePath ?? "yt-dlp";
        var recoverableDownloads = new List<RecoverableDownload>();
        var historicDownloads = new List<HistoricDownload>();
        var downloadsToStart = new List<Download>();
        foreach (var option in options)
        {
            var download = new Download(option, _translationService);
            download.Completed += Download_Completed;
            download.ProgressChanged += Download_ProgressChanged;
            recoverableDownloads.Add(new RecoverableDownload(download.Id, download.Options));
            if (!excludeFromHistory)
            {
                historicDownloads.Add(new HistoricDownload(download.Options.Url)
                {
                    Title = Path.GetFileNameWithoutExtension(download.FilePath),
                    Path = download.FilePath
                });
            }
            if (_downloading.Count < downloaderOptions.MaxNumberOfActiveDownloads)
            {
                _downloading.Add(download.Id, download);
                DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download.Id, download.FilePath, download.Options.Url, DownloadStatus.Running));
                downloadsToStart.Add(download);
            }
            else
            {
                _queued.Add(download.Id, download);
                DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download));
            }
        }
        await _recoveryService.AddAsync(recoverableDownloads);
        await _historyService.AddAsync(historicDownloads);
        foreach (var download in downloadsToStart)
        {
            download.Start(ytdlpExecutablePath, downloaderOptions);
        }
    }

    public IReadOnlyList<int> ClearCompleted()
    {
        var ids = new List<int>(_completed.Keys);
        foreach (var pair in _completed)
        {
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
        }
        _completed.Clear();
        return ids;
    }

    public IReadOnlyList<int> ClearQueued()
    {
        var ids = new List<int>(_queued.Keys);
        foreach (var pair in _queued)
        {
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
        }
        _queued.Clear();
        return ids;
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    public bool Pause(int id)
    {
        if (_downloading.TryGetValue(id, out var download))
        {
            download.Pause();
            return true;
        }
        return false;
    }

    public async Task RecoverAllAsync()
    {
        var downloads = await _recoveryService.GetAllAsync();
        foreach (var recoverableDownload in downloads)
        {
            if (recoverableDownload.CredentialRequired)
            {
                var args = new DownloadCredentialRequiredEventArgs(recoverableDownload.Options.SaveFilename, recoverableDownload.Options.Url);
                DownloadCredentialRequired?.Invoke(this, args);
                if (string.IsNullOrEmpty(args.Credential.Username) && string.IsNullOrEmpty(args.Credential.Password))
                {
                    continue;
                }
                recoverableDownload.Options.Credential = args.Credential;
            }
        }
        await _recoveryService.ClearAsync();
        await AddAsync(downloads.Select(x => x.Options).ToList(), false);
    }

    public bool Resume(int id)
    {
        if (_downloading.TryGetValue(id, out var download))
        {
            download.Resume();
            return true;
        }
        return false;
    }

    public async Task<bool> RetryAsync(int id)
    {
        if (_completed.TryGetValue(id, out var download))
        {
            DownloadRetired?.Invoke(this, new DownloadEventArgs(id));
            await AddAsync(download.Options, true);
            download.Completed -= Download_Completed;
            download.ProgressChanged -= Download_ProgressChanged;
            download.Dispose();
            _completed.Remove(id);
            return true;
        }
        return false;
    }

    public async Task RetryFailedAsync()
    {
        var retryDownloadOptions = new List<DownloadOptions>();
        foreach (var pair in _completed.Where(pair => pair.Value.Status == DownloadStatus.Error).ToList())
        {
            retryDownloadOptions.Add(pair.Value.Options);
            DownloadRetired?.Invoke(this, new DownloadEventArgs(pair.Key));
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
            _completed.Remove(pair.Key);
        }
        await AddAsync(retryDownloadOptions, true);
    }

    public async Task<bool> StopAsync(int id)
    {
        Download? download = null;
        if (_downloading.TryGetValue(id, out download) || _queued.TryGetValue(id, out download))
        {
            download.Stop();
            download.Completed -= Download_Completed;
            download.ProgressChanged -= Download_ProgressChanged;
            download.Dispose();
            _downloading.Remove(id);
            _queued.Remove(id);
            _completed.Add(id, download);
            await _recoveryService.RemoveAsync(id);
            DownloadStopped?.Invoke(this, new DownloadEventArgs(id));
            return true;
        }
        return false;
    }

    public async Task StopAllAsync()
    {
        var ids = new List<int>(_downloading.Keys.Concat(_queued.Keys));
        foreach (var pair in _downloading)
        {
            pair.Value.Stop();
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
            _completed.Add(pair.Key, pair.Value);
            DownloadStopped?.Invoke(this, new DownloadEventArgs(pair.Key));
        }
        foreach (var pair in _queued)
        {
            pair.Value.Stop();
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
            _completed.Add(pair.Key, pair.Value);
            DownloadStopped?.Invoke(this, new DownloadEventArgs(pair.Key));
        }
        _downloading.Clear();
        _queued.Clear();
        await _recoveryService.RemoveAsync(ids);
    }

    private void Dispose(bool disposing)
    {
        if (!disposing)
        {
            return;
        }
        foreach (var pair in _downloading)
        {
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
        }
        foreach (var pair in _queued)
        {
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
        }
        foreach (var pair in _completed)
        {
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
        }
    }

    private async void Download_Completed(object? sender, DownloadCompletedEventArgs e)
    {
        var downloaderOptions = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
        if (!_downloading.TryGetValue(e.Id, out var download) || download.Status == DownloadStatus.Stopped)
        {
            return;
        }
        if (downloaderOptions.RemoveSourceData && File.Exists(e.Path))
        {
            var track = new Track(e.Path);
            track.Comment = string.Empty;
            track.Description = string.Empty;
            track.EncodedBy = string.Empty;
            track.Encoder = string.Empty;
            if (track.AdditionalFields.ContainsKey("purl"))
            {
                track.AdditionalFields.Remove("purl");
            }
            if (track.AdditionalFields.ContainsKey("synopsis"))
            {
                track.AdditionalFields.Remove("synopsis");
            }
            if (track.AdditionalFields.ContainsKey("url"))
            {
                track.AdditionalFields.Remove("url");
            }
            await track.SaveAsync();
        }
        _completed.Add(e.Id, download);
        _downloading.Remove(e.Id);
        await _recoveryService.RemoveAsync(e.Id);
        DownloadCompleted?.Invoke(this, e);
        if (_queued.Count > 0 && _downloading.Count < downloaderOptions.MaxNumberOfActiveDownloads)
        {
            var firstDownload = _queued.First().Value;
            _downloading.Add(firstDownload.Id, firstDownload);
            _queued.Remove(firstDownload.Id);
            DownloadStartedFromQueue?.Invoke(this, new DownloadEventArgs(firstDownload.Id));
            firstDownload.Start(_ytdlpService.ExecutablePath ?? "yt-dlp", downloaderOptions);
        }
    }

    private void Download_ProgressChanged(object? sender, DownloadProgressChangedEventArgs e)
    {
        if (!_downloading.TryGetValue(e.Id, out var download) || download.Status == DownloadStatus.Stopped)
        {
            return;
        }
        DownloadProgressChanged?.Invoke(this, e);
    }
}
