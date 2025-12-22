using Nickvision.Parabolic.Shared.Models;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadCompletedEventArgs
{
    public int Id { get; }
    public DownloadStatus Status { get; }
    public string Path { get; }
    public bool ShowNotification { get; }

    public DownloadCompletedEventArgs(int id, DownloadStatus status, string path, bool showNotification)
    {
        Id = id;
        Status = status;
        Path = path;
        ShowNotification = showNotification;
    }
}
