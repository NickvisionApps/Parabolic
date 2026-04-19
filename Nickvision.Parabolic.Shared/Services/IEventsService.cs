using Nickvision.Desktop.Application;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Events;
using System;

namespace Nickvision.Parabolic.Shared.Services;

public interface IEventsService
{
    event EventHandler<AppNotificationSentEventArgs>? AppNotificationSent;
    event EventHandler<ConfigurationSavedEventArgs>? ConfigurationSaved;
    event EventHandler<DownloadAddedEventArgs> DownloadAdded;
    event EventHandler<DownloadCompletedEventArgs> DownloadCompleted;
    event EventHandler<DownloadCredentialRequiredEventArgs> DownloadCredentialRequired;
    event EventHandler<DownloadProgressChangedEventArgs> DownloadProgressChanged;
    event EventHandler<DownloadRequestedEventArgs>? DownloadRequested;
    event EventHandler<DownloadEventArgs> DownloadRetired;
    event EventHandler<DownloadEventArgs> DownloadStartedFromQueue;
    event EventHandler<DownloadEventArgs> DownloadStopped;

    void InvokeDownloadRequested(Uri url);
}
