using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadProgressChangedEventArgs : EventArgs
{
    public int Id { get; }
    public string Log { get; }
    public double Progress { get; }
    public double Speed { get; }
    public int Eta { get; }

    public DownloadProgressChangedEventArgs(int id, string log, double progress = 1.0, double speed = 0.0, int eta = 0)
    {
        Id = id;
        Log = log;
        Progress = progress;
        Speed = speed;
        Eta = eta;
    }
}
