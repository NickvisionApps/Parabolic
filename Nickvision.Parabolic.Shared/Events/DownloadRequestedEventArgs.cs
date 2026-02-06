using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadRequestedEventArgs : EventArgs
{
    public Uri Url { get; }

    public DownloadRequestedEventArgs(Uri url)
    {
        Url = url;
    }
}
