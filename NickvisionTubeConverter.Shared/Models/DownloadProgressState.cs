namespace NickvisionTubeConverter.Shared.Models;

public enum ProgressStatus
{
    Processing = 0,
    Downloading,
    Other
}

public class DownloadProgressState
{
    public ProgressStatus Status { get; set; }
    public double Progress { get; set; }
    public double Speed { get; set; }

    public DownloadProgressState()
    {
        Status = ProgressStatus.Processing;
        Progress = 0.0;
        Speed = 0.0;
    }
}
