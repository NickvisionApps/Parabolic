using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadProgressChangedEventArgs : DownloadEventArgs
{
    private static readonly string[] Units;

    public ReadOnlyMemory<char> LogChunk { get; }
    public double Progress { get; }
    public double Speed { get; }
    public int Eta { get; }

    static DownloadProgressChangedEventArgs()
    {
        Units = ["B/s", "KB/s", "MB/s", "GB/s"];
    }

    public DownloadProgressChangedEventArgs(int id, ReadOnlyMemory<char> logChunk, double progress = 1.0, double speed = 0.0, int eta = 0) : base(id)
    {
        LogChunk = logChunk;
        Progress = progress;
        Speed = speed;
        Eta = eta;
    }

    public string EtaString
    {
        get
        {
            if (Eta < 0)
            {
                return string.Empty;
            }
            var timeSpan = TimeSpan.FromSeconds(Eta);
            if (timeSpan.TotalHours >= 1)
            {
                return $"{(int)timeSpan.TotalHours}:{timeSpan.Minutes:D2}:{timeSpan.Seconds:D2}";
            }
            return $"{timeSpan.Minutes}:{timeSpan.Seconds:D2}";
        }
    }

    public string SpeedString
    {
        get
        {
            var unitIndex = 0;
            var speed = Speed;
            while (speed >= 1024 && unitIndex < Units.Length - 1)
            {
                speed /= 1024;
                unitIndex++;
            }
            return $"{speed:F2} {Units[unitIndex]}";
        }
    }
}
