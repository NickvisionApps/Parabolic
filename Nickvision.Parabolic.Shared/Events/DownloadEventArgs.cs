using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadEventArgs : EventArgs
{
    public int Id { get; }

    public DownloadEventArgs(int id)
    {
        Id = id;
    }
}
