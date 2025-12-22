using Nickvision.Parabolic.Shared.Models;
using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadAddedEventArgs : EventArgs
{
    public int Id { get; init; }
    public string Path { get; init; }
    public Uri Url { get; init; }
    public DownloadStatus Status { get; init; }

    public DownloadAddedEventArgs(int id, string path, Uri url, DownloadStatus status)
    {
        Id = id;
        Path = path;
        Url = url;
        Status = status;
    }
}
