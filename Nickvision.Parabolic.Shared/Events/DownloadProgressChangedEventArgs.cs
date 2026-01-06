using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadProgressChangedEventArgs : DownloadEventArgs
{
    public ReadOnlyMemory<char> LogChunk { get; }
    public double Progress { get; }
    public double Speed { get; }
    public int Eta { get; }

    public DownloadProgressChangedEventArgs(int id, ReadOnlyMemory<char> logChunk, double progress = 1.0, double speed = 0.0, int eta = 0) : base(id)
    {
        LogChunk = logChunk;
        Progress = progress;
        Speed = speed;
        Eta = eta;
    }
}
