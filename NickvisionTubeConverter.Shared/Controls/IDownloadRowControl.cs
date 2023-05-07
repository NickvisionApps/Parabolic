using NickvisionTubeConverter.Shared.Models;
using System;

namespace NickvisionTubeConverter.Shared.Controls;

/// <summary>
/// An interface for download rows
/// </summary>
public interface IDownloadRowControl
{
    /// <summary>
    /// The Id of the download
    /// </summary>
    public Guid Id { get; }
    /// <summary>
    /// The filename of the download
    /// </summary>
    public string Filename { get; }

    /// <summary>
    /// Occurs when the download is requested to stop
    /// </summary>
    public event EventHandler<Guid>? StopRequested;
    /// <summary>
    /// Occurs when the download is requested to be retried
    /// </summary>
    public event EventHandler<Guid>? RetryRequested;

    /// <summary>
    /// Sets the row to the waiting state
    /// </summary>
    public void SetWaitingState();
    /// <summary>
    /// Sets the row to the preparing state
    /// </summary>
    public void SetPreparingState();
    /// <summary>
    /// Sets the row to the progress state
    /// </summary>
    /// <param name="state">The DownloadProgressState</param>
    public void SetProgressState(DownloadProgressState state);
    /// <summary>
    /// Sets the row to the completed state
    /// </summary>
    /// <param name="success">Whether or not the download was successful</param>
    public void SetCompletedState(bool success);
    /// <summary>
    /// Sets the row to the stop state
    /// </summary>
    public void SetStopState();
}
