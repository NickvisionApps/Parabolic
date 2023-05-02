using System;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Progress statuses for a download
/// </summary>
public enum DownloadProgressStatus
{
    Processing = 0,
    Downloading,
    DownloadingAria,
    Other
}

/// <summary>
/// The progress state of a download
/// </summary>
public class DownloadProgressState : IDisposable
{
    private bool _disposed;

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
    /// GCHandle to pass to unmanaged code
    /// </summary>
    public GCHandle? Handle { get; init; }

    /// <summary>
    /// Constructs a DownloadProgressState
    /// </summary>
    public DownloadProgressState()
    {
        _disposed = false;
        Status = DownloadProgressStatus.Processing;
        Progress = 0.0;
        Speed = 0.0;
        Log = "";
        if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            Handle = GCHandle.Alloc(this);
        }
    }

    /// <summary>
    /// Frees resources used by the DownloadProgressState object
    /// </summary>
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Frees resources used by the DownloadProgressState object
    /// </summary>
    protected virtual void Dispose(bool disposing)
    {
        if (_disposed)
        {
            return;
        }
        if (disposing)
        {
            Handle?.Free();
        }
        _disposed = true;
    }
}
