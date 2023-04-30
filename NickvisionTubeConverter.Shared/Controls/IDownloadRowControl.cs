using System;
using System.Threading.Tasks;

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
    /// The callback function to run when the download is completed
    /// </summary>
    public Func<IDownloadRowControl, Task>? DownloadCompletedAsyncCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is stopped
    /// </summary>
    public Action<IDownloadRowControl>? DownloadStoppedCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is retried
    /// </summary>
    public Func<IDownloadRowControl, Task>? DownloadRetriedAsyncCallback { get; set; }
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
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public Task RunAsync(bool embedMetadata);

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop();

    /// <summary>
    /// Retries the download if needed
    /// </summary>
    public Task RetryAsync();
}
