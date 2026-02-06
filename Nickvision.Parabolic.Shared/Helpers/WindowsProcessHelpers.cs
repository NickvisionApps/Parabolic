using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Nickvision.Parabolic.Shared.Helpers;

public static partial class WindowsProcessHelpers
{
    [LibraryImport("ntdll.dll")]
    private static partial int NtSuspendProcess(nint processHandle);

    [LibraryImport("ntdll.dll")]
    private static partial int NtResumeProcess(nint processHandle);

    public static void Suspend(Process p, bool entireProcessTree = false)
    {
        NtSuspendProcess(p.Handle);
        if (entireProcessTree)
        {
            var searcher = new System.Management.ManagementObjectSearcher($"SELECT ProcessId FROM Win32_Process WHERE ParentProcessId={p.Id}");
            foreach (var obj in searcher.Get())
            {
                try
                {
                    using var childProcess = Process.GetProcessById(Convert.ToInt32(obj["ProcessId"]));
                    Suspend(childProcess, true);
                }
                catch { }
            }
        }
    }

    public static void Resume(Process p, bool entireProcessTree = false)
    {
        NtResumeProcess(p.Handle);
        if (entireProcessTree)
        {
            var searcher = new System.Management.ManagementObjectSearcher($"SELECT ProcessId FROM Win32_Process WHERE ParentProcessId={p.Id}");
            foreach (var obj in searcher.Get())
            {
                try
                {
                    using var childProcess = Process.GetProcessById(Convert.ToInt32(obj["ProcessId"]));
                    Resume(childProcess, true);
                }
                catch { }
            }
        }
    }
}
