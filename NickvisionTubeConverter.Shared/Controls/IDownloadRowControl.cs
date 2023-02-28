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
    /// Starts the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata</param>
    /// <param name="completedCallback">The callback function to run when the download is completed</param>
    public Task StartAsync(bool embedMetadata, Func<IDownloadRowControl, Task>? completedCallback);

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop();
}
