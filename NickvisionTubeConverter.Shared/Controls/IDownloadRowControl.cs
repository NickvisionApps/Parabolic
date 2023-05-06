using System;

namespace NickvisionTubeConverter.Shared.Controls;

/// <summary>
/// A contract for a download row control
/// </summary>
public interface IDownloadRowControl
{
    /// <summary>
    /// The filename of the download
    /// </summary>
    public string Filename { get; }
    /// <summary>
    /// Whether or not the download is done
    /// </summary>
    public bool IsDone { get; }
    /// <summary>
    /// Download progress
    /// </summary>
    public double Progress { get; set; }
    /// <summary>
    /// Download speed (in bytes per second)
    /// </summary>
    public double Speed { get; set; }
    /// <summary>
    /// Whether or not download was finished with error
    /// </summary>
    public bool FinishedWithError { get; set; }

    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    public event EventHandler<EventArgs>? DownloadCompleted;
    /// <summary>
    /// Occurs when a download is stopped
    /// </summary>
    public event EventHandler<EventArgs>? DownloadStopped;
    /// <summary>
    /// Occurs when a download is retried
    /// </summary>
    public event EventHandler<EventArgs>? DownloadRetried;

    /// <summary>
    /// Starts the download
    /// </summary>
    /// <param name="useAria">Whether or not to use aria2 downloader</param>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    /// <param name="isRetry">Whether or not this download is being retried</param>
    public void Start(bool useAria, bool embedMetadata, bool isRetry);
    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop();
    /// <summary>
    /// Retries the download if needed
    /// </summary>
    public void Retry();
}
