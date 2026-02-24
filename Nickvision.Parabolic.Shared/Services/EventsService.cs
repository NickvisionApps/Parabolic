using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Events;
using System;

namespace Nickvision.Parabolic.Shared.Services;

public class EventsService : IEventsService
{
    private readonly IDownloadService _downloadService;
    private readonly IHistoryService _historyService;
    private readonly IJsonFileService _jsonFileService;
    private readonly INotificationService _notificationService;

    public event EventHandler<DownloadRequestedEventArgs>? DownloadRequested;

    public EventsService(IDownloadService downloadService, IHistoryService historyService, IJsonFileService jsonFileService, INotificationService notificationService)
    {
        _downloadService = downloadService;
        _historyService = historyService;
        _jsonFileService = jsonFileService;
        _notificationService = notificationService;
    }

    public event EventHandler<AppNotificationSentEventArgs>? AppNotificationSent
    {
        add => _notificationService.AppNotificationSent += value;

        remove => _notificationService.AppNotificationSent -= value;
    }

    public event EventHandler<DownloadAddedEventArgs> DownloadAdded
    {
        add => _downloadService.DownloadAdded += value;

        remove => _downloadService.DownloadAdded -= value;
    }

    public event EventHandler<DownloadCompletedEventArgs> DownloadCompleted
    {
        add => _downloadService.DownloadCompleted += value;

        remove => _downloadService.DownloadCompleted -= value;
    }

    public event EventHandler<DownloadCredentialRequiredEventArgs> DownloadCredentialRequired
    {
        add => _downloadService.DownloadCredentialRequired += value;

        remove => _downloadService.DownloadCredentialRequired -= value;
    }

    public event EventHandler<DownloadProgressChangedEventArgs> DownloadProgressChanged
    {
        add => _downloadService.DownloadProgressChanged += value;

        remove => _downloadService.DownloadProgressChanged -= value;
    }

    public event EventHandler<DownloadEventArgs> DownloadRetired
    {
        add => _downloadService.DownloadRetired += value;

        remove => _downloadService.DownloadRetired -= value;
    }

    public event EventHandler<DownloadEventArgs> DownloadStartedFromQueue
    {
        add => _downloadService.DownloadStartedFromQueue += value;

        remove => _downloadService.DownloadStartedFromQueue -= value;
    }

    public event EventHandler<DownloadEventArgs> DownloadStopped
    {
        add => _downloadService.DownloadStopped += value;

        remove => _downloadService.DownloadStopped -= value;
    }

    public event EventHandler<JsonFileSavedEventArgs>? JsonFileSaved
    {
        add => _jsonFileService.Saved += value;

        remove => _jsonFileService.Saved -= value;
    }

    public void InvokeDownloadRequested(Uri url) => DownloadRequested?.Invoke(this, new DownloadRequestedEventArgs(url));
}
