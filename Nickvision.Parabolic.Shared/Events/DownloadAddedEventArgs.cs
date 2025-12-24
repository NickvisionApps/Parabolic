using Nickvision.Parabolic.Shared.Models;
using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadAddedEventArgs : DownloadEventArgs
{
    public string Path { get; }
    public Uri Url { get; }
    public DownloadStatus Status { get; }

    public DownloadAddedEventArgs(int id, string path, Uri url, DownloadStatus status) : base(id)
    {
        Path = path;
        Url = url;
        Status = status;
    }

    public DownloadAddedEventArgs(Download download) : this(download.Id, download.FilePath, download.Options.Url, download.Status)
    {

    }
}
