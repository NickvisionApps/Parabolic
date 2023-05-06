using NickvisionTubeConverter.Shared.Models;
using System;

namespace NickvisionTubeConverter.Shared.Controls;

/// <summary>
/// An interface for download rows
/// </summary>
public interface IDownloadRowControl
{
    /// <summary>
    /// Occurs when the download is requested to stop
    /// </summary>
    event EventHandler<Guid>? StopRequested;
    /// <summary>
    /// Occurs when the download is requested to be retried
    /// </summary>
    event EventHandler<Guid>? RetryRequested;

    /// <summary>
    /// Sets the row to the waiting state
    /// </summary>
    void SetWaitingState();
    /// <summary>
    /// Sets the row to the preparing state
    /// </summary>
    void SetPreparingState();
    /// <summary>
    /// Sets the row to the progress state
    /// </summary>
    /// <param name="state">The DownloadProgressState</param>
    void SetProgressState(DownloadProgressState state);
    /// <summary>
    /// Sets the row to the completed state
    /// </summary>
    /// <param name="success">Whether or not the download was successful</param>
    void SetCompletedState(bool success);
    /// <summary>
    /// Sets the row to the stop state
    /// </summary>
    void SetStopState();
    // <summary>
    /// Sets the row to the retry state
    /// </summary>
    void SetRetryState();
}
