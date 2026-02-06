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
    event EventHandler<DownloadCredentialRequiredEventArgs>? DownloadCredentialRequired;
    event EventHandler<DownloadProgressChangedEventArgs>? DownloadProgressChanged;
    event EventHandler<DownloadEventArgs>? DownloadRetired;
    event EventHandler<DownloadEventArgs>? DownloadStartedFromQueue;
    event EventHandler<DownloadEventArgs>? DownloadStopped;

    int DownloadingCount { get; }
    int QueuedCount { get; }
    int CompletedCount { get; }

    int RemainingCount => DownloadingCount + QueuedCount;

    Task AddAsync(DownloadOptions options, bool excludeFromHistory);
    Task AddAsync(IReadOnlyList<DownloadOptions> options, bool excludeFromHistory);
    IReadOnlyList<int> ClearCompleted();
    IReadOnlyList<int> ClearQueued();
    bool Pause(int id);
    Task RecoverAllAsync();
    bool Resume(int id);
    Task<bool> RetryAsync(int id);
    Task RetryFailedAsync();
    Task<bool> StopAsync(int id);
    Task StopAllAsync();
}
