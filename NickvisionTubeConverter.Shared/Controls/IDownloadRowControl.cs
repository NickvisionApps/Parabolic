using System;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Controls;

/// <summary>
/// A contract for a download row control
/// </summary>
public interface IDownloadRowControl
{
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
    /// Starts the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public Task StartAsync(bool embedMetadata);

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop();
}
