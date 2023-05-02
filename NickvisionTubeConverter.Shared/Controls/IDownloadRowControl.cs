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
    public Action<IDownloadRowControl>? DownloadCompletedCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is stopped
    /// </summary>
    public Action<IDownloadRowControl>? DownloadStoppedCallback { get; set; }
    /// <summary>
    /// The callback function to run when the download is retried
    /// </summary>
    public Action<IDownloadRowControl>? DownloadRetriedCallback { get; set; }
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
    /// Starts the download
    /// </summary>
    /// <param name="useAria">Whether or not to use aria2 downloader</param>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    public void Start(bool useAria, bool embedMetadata);

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop();

    /// <summary>
    /// Retries the download if needed
    /// </summary>
    public void Retry();
}
