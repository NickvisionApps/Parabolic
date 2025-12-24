using Nickvision.Desktop;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IDownloadService : IService
{
    event EventHandler<DownloadAddedEventArgs>? DownloadAdded;
    event EventHandler<DownloadCompletedEventArgs>? DownloadCompleted;
    event EventHandler<DownloadProgressChangedEventArgs>? DownloadProgressChanged;
    event EventHandler<DownloadEventArgs>? DownloadRetired;
    event EventHandler<DownloadEventArgs>? DownloadStartedFromQueue;
    event EventHandler<DownloadEventArgs>? DownloadStopped;

    int DownloadingCount { get; }
    int QueuedCount { get; }
    int CompletedCount { get; }

    Task AddAsync(DownloadOptions options, bool excludeFromHistory);
    Task AddAsync(IEnumerable<DownloadOptions> options, bool excludeFromHistory);
    IEnumerable<int> ClearCompleted();
    IEnumerable<int> ClearQueued();
    bool Pause(int id);
    bool Resume(int id);
    Task<bool> RetryAsync(int id);
    Task RetryFailedAsync();
    Task<bool> StopAsync(int id);
    Task StopAllAsync();
}
