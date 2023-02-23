namespace NickvisionTubeConverter.Shared.Models;

public enum DownloadProgressStatus
{
    Processing = 0,
    Downloading,
    Other
}

public class DownloadProgressState
{
    public DownloadProgressStatus Status { get; set; }
    public double Progress { get; set; }
    public double Speed { get; set; }

    public DownloadProgressState()
    {
        Status = DownloadProgressStatus.Processing;
        Progress = 0.0;
        Speed = 0.0;
    }
}
