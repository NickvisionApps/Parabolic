using Nickvision.Parabolic.Shared.Models;
using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadCompletedEventArgs : DownloadEventArgs
{
    public DownloadStatus Status { get; }
    public string Path { get; }
    public ReadOnlyMemory<char> Log { get; }
    public bool ShowNotification { get; }

    public DownloadCompletedEventArgs(int id, DownloadStatus status, string path, ReadOnlyMemory<char> log, bool showNotification) : base(id)
    {
        Status = status;
        Path = path;
        Log = log;
        ShowNotification = showNotification;
    }
}
