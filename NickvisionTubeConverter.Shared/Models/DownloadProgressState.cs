namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Progress statuses for a download
/// </summary>
public enum DownloadProgressStatus
{
    Processing = 0,
    Downloading,
    Other
}

/// <summary>
/// The progress state of a download
/// </summary>
public class DownloadProgressState
{
    /// <summary>
    /// The status of the download
    /// </summary>
    public DownloadProgressStatus Status { get; set; }
    /// <summary>
    /// The progress of the download
    /// </summary>
    public double Progress { get; set; }
    /// <summary>
    /// The speed of the download
    /// </summary>
    public double Speed { get; set; }
    /// <summary>
    /// The current log of the download
    /// </summary>
    public string Log { get; set; }

    /// <summary>
    /// Constructs a DownloadProgressState
    /// </summary>
    public DownloadProgressState()
    {
        Status = DownloadProgressStatus.Processing;
        Progress = 0.0;
        Speed = 0.0;
        Log = "";
    }
}
