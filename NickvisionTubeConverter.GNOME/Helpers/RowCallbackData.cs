using NickvisionTubeConverter.GNOME.Controls;
using System;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.GNOME.Helpers;

/// <summary>
/// An object to pass to move/delete row callbacks in main window
/// </summary>
public class RowCallbackData : IDisposable
{
    private bool _disposed;

    /// <summary>
    /// Row to move/delete
    /// </summary>
    public DownloadRow Row { get; init; }
    /// <summary>
    /// Box to modify
    /// </summary>
    public Gtk.Box Box { get; init; }
    /// <summary>
    /// GCHandle to pass to unmanaged code
    /// </summary>
    public GCHandle Handle { get; init; }

    /// <summary>
    /// Constructs RowCallbackData
    /// </summary>
    /// <param name="row">Row to move/delete</param>
    /// <param name="box">Box to modify</param>
    public RowCallbackData(DownloadRow row, Gtk.Box box)
    {
        _disposed = false;
        Row = row;
        Box = box;
        Handle = GCHandle.Alloc(this);
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
            Handle.Free();
        }
        _disposed = true;
    }
}