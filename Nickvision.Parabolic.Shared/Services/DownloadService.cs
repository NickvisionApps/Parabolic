using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DownloadService : IDisposable, IDownloadService
{
    private readonly ILogger<DownloadService> _logger;
    private readonly IDenoExecutableService _denoExecutableService;
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

    public DownloadService(ILogger<DownloadService> logger, IDenoExecutableService denoExecutableService, IJsonFileService jsonFileService, ITranslationService translationService, IYtdlpExecutableService ytdlpService, IHistoryService historyService, IRecoveryService recoveryService)
    {
        _logger = logger;
        _denoExecutableService = denoExecutableService;
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
        var config = await _jsonFileService.LoadAsync(ApplicationJsonContext.Default.Configuration, Configuration.Key);
        var downloaderOptions = config.DownloaderOptions;
        var download = new Download(options, _denoExecutableService, _translationService);
        _logger.LogInformation($"Adding download ({download.Id}): {JsonSerializer.Serialize(options, ApplicationJsonContext.Default.DownloadOptions)}");
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
            _logger.LogInformation($"Starting download ({download.Id}): {JsonSerializer.Serialize(downloaderOptions, ApplicationJsonContext.Default.DownloaderOptions)}");
            _downloading.Add(download.Id, download);
            DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download.Id, download.FilePath, download.Options.Url, DownloadStatus.Running));
            download.Start(_ytdlpService.ExecutablePath ?? "yt-dlp", downloaderOptions, config.TranslationLanguage);
        }
        else
        {
            _logger.LogInformation($"Queueing download ({download.Id})...");
            _queued.Add(download.Id, download);
            DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download));
        }
    }

    public async Task AddAsync(IReadOnlyList<DownloadOptions> options, bool excludeFromHistory)
    {
        var config = await _jsonFileService.LoadAsync(ApplicationJsonContext.Default.Configuration, Configuration.Key);
        var downloaderOptions = config.DownloaderOptions;
        var ytdlpExecutablePath = _ytdlpService.ExecutablePath ?? "yt-dlp";
        var recoverableDownloads = new List<RecoverableDownload>();
        var historicDownloads = new List<HistoricDownload>();
        var downloadsToStart = new List<Download>();
        foreach (var option in options)
        {
            var download = new Download(option, _denoExecutableService, _translationService);
            _logger.LogInformation($"Adding download ({download.Id}): {JsonSerializer.Serialize(option, ApplicationJsonContext.Default.DownloadOptions)}");
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
                _logger.LogInformation($"Starting download ({download.Id}): {JsonSerializer.Serialize(downloaderOptions, ApplicationJsonContext.Default.DownloaderOptions)}");
                _downloading.Add(download.Id, download);
                DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download.Id, download.FilePath, download.Options.Url, DownloadStatus.Running));
                downloadsToStart.Add(download);
            }
            else
            {
                _logger.LogInformation($"Queueing download ({download.Id})...");
                _queued.Add(download.Id, download);
                DownloadAdded?.Invoke(this, new DownloadAddedEventArgs(download));
            }
        }
        await _recoveryService.AddAsync(recoverableDownloads);
        await _historyService.AddAsync(historicDownloads);
        foreach (var download in downloadsToStart)
        {
            download.Start(ytdlpExecutablePath, downloaderOptions, config.TranslationLanguage);
        }
    }

    public IReadOnlyList<int> ClearCompleted()
    {
        _logger.LogInformation($"Clearing completed downloads...");
        var ids = new List<int>(_completed.Keys);
        foreach (var pair in _completed)
        {
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
        }
        _completed.Clear();
        _logger.LogInformation($"Cleared {ids.Count} completed download(s).");
        return ids;
    }

    public IReadOnlyList<int> ClearQueued()
    {
        _logger.LogInformation($"Clearing queued downloads...");
        var ids = new List<int>(_queued.Keys);
        foreach (var pair in _queued)
        {
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
        }
        _queued.Clear();
        _logger.LogInformation($"Cleared {ids.Count} queued download(s).");
        return ids;
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    public bool Pause(int id)
    {
        _logger.LogInformation($"Pausing download ({id})...");
        if (_downloading.TryGetValue(id, out var download))
        {
            download.Pause();
            _logger.LogInformation($"Paused download ({id}).");
            return true;
        }
        _logger.LogWarning($"Unable to pause download ({id}), not found.");
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
        _logger.LogInformation($"Resuming download ({id})...");
        if (_downloading.TryGetValue(id, out var download))
        {
            download.Resume();
            _logger.LogInformation($"Resumed download ({id}).");
            return true;
        }
        _logger.LogWarning($"Unable to resume download ({id}), not found.");
        return false;
    }

    public async Task<bool> RetryAsync(int id)
    {
        _logger.LogInformation($"Retrying download ({id})...");
        if (_completed.TryGetValue(id, out var download))
        {
            _logger.LogInformation($"Retried download ({id}).");
            DownloadRetired?.Invoke(this, new DownloadEventArgs(id));
            await AddAsync(download.Options, true);
            download.Completed -= Download_Completed;
            download.ProgressChanged -= Download_ProgressChanged;
            download.Dispose();
            _completed.Remove(id);
            return true;
        }
        _logger.LogWarning($"Unable to retry download ({id}), not found.");
        return false;
    }

    public async Task RetryFailedAsync()
    {
        _logger.LogInformation($"Retrying failed downloads...");
        var retryDownloadOptions = new List<DownloadOptions>();
        foreach (var pair in _completed.Where(pair => pair.Value.Status == DownloadStatus.Error).ToList())
        {
            retryDownloadOptions.Add(pair.Value.Options);
            DownloadRetired?.Invoke(this, new DownloadEventArgs(pair.Key));
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
            _completed.Remove(pair.Key);
            _logger.LogInformation($"Retried download ({pair.Key}).");
        }
        _logger.LogInformation($"Retried {retryDownloadOptions.Count} failed download(s).");
        await AddAsync(retryDownloadOptions, true);
    }

    public async Task<bool> StopAsync(int id)
    {
        _logger.LogInformation($"Stopping download ({id})...");
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
            _logger.LogInformation($"Stopped download ({id}).");
            DownloadStopped?.Invoke(this, new DownloadEventArgs(id));
            return true;
        }
        _logger.LogWarning($"Unable to stop download ({id}), not found.");
        return false;
    }

    public async Task StopAllAsync()
    {
        _logger.LogInformation($"Stopping all downloads...");
        var ids = new List<int>(_downloading.Keys.Concat(_queued.Keys));
        foreach (var pair in _downloading)
        {
            pair.Value.Stop();
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
            _completed.Add(pair.Key, pair.Value);
            _logger.LogInformation($"Stopped download ({pair.Key}).");
            DownloadStopped?.Invoke(this, new DownloadEventArgs(pair.Key));
        }
        foreach (var pair in _queued)
        {
            pair.Value.Stop();
            pair.Value.Completed -= Download_Completed;
            pair.Value.ProgressChanged -= Download_ProgressChanged;
            pair.Value.Dispose();
            _completed.Add(pair.Key, pair.Value);
            _logger.LogInformation($"Stopped download ({pair.Key}).");
            DownloadStopped?.Invoke(this, new DownloadEventArgs(pair.Key));
        }
        _downloading.Clear();
        _queued.Clear();
        await _recoveryService.RemoveAsync(ids);

        _logger.LogInformation($"Stopped {ids.Count} download(s).");
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
        var config = await _jsonFileService.LoadAsync(ApplicationJsonContext.Default.Configuration, Configuration.Key);
        var downloaderOptions = config.DownloaderOptions;
        if (!_downloading.TryGetValue(e.Id, out var download) || download.Status == DownloadStatus.Stopped)
        {
            return;
        }
        _completed.Add(e.Id, download);
        _downloading.Remove(e.Id);
        await _recoveryService.RemoveAsync(e.Id);
        if (e.Status == DownloadStatus.Error)
        {
            _logger.LogError($"Download failed ({e.Id}): {download.Log}");
        }
        else if (e.Status == DownloadStatus.Success)
        {
            _logger.LogInformation($"Download completed ({e.Id}): {download.Log}");
        }
        else if (e.Status == DownloadStatus.Stopped)
        {
            _logger.LogInformation($"Download stopped ({e.Id}): {download.Log}");
        }
        DownloadCompleted?.Invoke(this, e);
        if (_queued.Count > 0 && _downloading.Count < downloaderOptions.MaxNumberOfActiveDownloads)
        {
            var firstDownload = _queued.First().Value;
            _downloading.Add(firstDownload.Id, firstDownload);
            _queued.Remove(firstDownload.Id);
            _logger.LogInformation($"Starting download from queue ({firstDownload.Id}): {JsonSerializer.Serialize(downloaderOptions, ApplicationJsonContext.Default.DownloaderOptions)}");
            DownloadStartedFromQueue?.Invoke(this, new DownloadEventArgs(firstDownload.Id));
            firstDownload.Start(_ytdlpService.ExecutablePath ?? "yt-dlp", downloaderOptions, config.TranslationLanguage);
        }
    }

    private void Download_ProgressChanged(object? sender, DownloadProgressChangedEventArgs e)
    {
        if (!_downloading.TryGetValue(e.Id, out var download) || download.Status != DownloadStatus.Running)
        {
            return;
        }
        DownloadProgressChanged?.Invoke(this, e);
    }
}
