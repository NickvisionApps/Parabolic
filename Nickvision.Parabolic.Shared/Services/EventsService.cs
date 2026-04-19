using Nickvision.Desktop.Application;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Events;
using System;

namespace Nickvision.Parabolic.Shared.Services;

public class EventsService : IEventsService
{
    private readonly IConfigurationService _configurationService;
    private readonly IDownloadService _downloadService;
    private readonly INotificationService _notificationService;

    public event EventHandler<DownloadRequestedEventArgs>? DownloadRequested;

    public EventsService(IConfigurationService configurationService, IDownloadService downloadService, INotificationService notificationService)
    {
        _configurationService = configurationService;
        _downloadService = downloadService;
        _notificationService = notificationService;
    }

    public event EventHandler<AppNotificationSentEventArgs>? AppNotificationSent
    {
        add => _notificationService.AppNotificationSent += value;

        remove => _notificationService.AppNotificationSent -= value;
    }

    public event EventHandler<ConfigurationSavedEventArgs>? ConfigurationSaved
    {
        add => _configurationService.Saved += value;

        remove => _configurationService.Saved -= value;
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

    public void InvokeDownloadRequested(Uri url) => DownloadRequested?.Invoke(this, new DownloadRequestedEventArgs(url));
}
