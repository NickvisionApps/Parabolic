using Nickvision.Parabolic.Shared.Models;
using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadAddedEventArgs : EventArgs
{
    public int Id { get; }
    public string Path { get; }
    public Uri Url { get; }
    public DownloadStatus Status { get; }

    public DownloadAddedEventArgs(int id, string path, Uri url, DownloadStatus status)
    {
        Id = id;
        Path = path;
        Url = url;
        Status = status;
    }
}
