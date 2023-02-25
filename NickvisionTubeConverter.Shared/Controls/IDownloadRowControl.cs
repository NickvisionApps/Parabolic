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
    public Task StartAsync(bool embedMetadata);

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop();
}
