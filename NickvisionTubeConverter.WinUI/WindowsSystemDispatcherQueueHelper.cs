using System.Runtime.InteropServices;
using Windows.System;

namespace NickvisionTubeConverter.WinUI;

/// <summary>
/// A helper classed used for checking if a WinUI dispatcher queue controller is available 
/// </summary>
public static class WindowsSystemDispatcherQueueHelper
{
    [StructLayout(LayoutKind.Sequential)]
    private struct DispatcherQueueOptions
    {
        internal int dwSize;
        internal int threadType;
        internal int apartmentType;
    }

    private static object? _dispatcherQueueController;

    [DllImport("CoreMessaging.dll")]
    private static extern int CreateDispatcherQueueController([In] DispatcherQueueOptions options, [In, Out, MarshalAs(UnmanagedType.IUnknown)] ref object dispatcherQueueController);

    /// <summary>
    /// Ensures that a WinUI dispatcher queue controller is available  
    /// </summary>
    public static void EnsureWindowsSystemDispatcherQueueController()
    {
        if (DispatcherQueue.GetForCurrentThread() == null && _dispatcherQueueController == null)
        {
            DispatcherQueueOptions options;
            options.dwSize = Marshal.SizeOf(typeof(DispatcherQueueOptions));
            options.threadType = 2;    // DQTYPE_THREAD_CURRENT
            options.apartmentType = 2; // DQTAT_COM_STA
            CreateDispatcherQueueController(options, ref _dispatcherQueueController);
        }
    }
}
