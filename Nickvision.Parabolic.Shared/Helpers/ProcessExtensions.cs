using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Nickvision.Parabolic.Shared.Helpers;

public static partial class ProcessExtensions
{
#if OS_WINDOWS
    [LibraryImport("ntdll.dll")]
    private static partial int NtSuspendProcess(nint processHandle);

    [LibraryImport("ntdll.dll")]
    private static partial int NtResumeProcess(nint processHandle);
#else
    [LibraryImport("libc")]
    private static partial int kill(int pid, int sig);
#endif

    extension(Process p)
    {

        public void Suspend(bool entireProcessTree = false)
        {
#if OS_WINDOWS
            NtSuspendProcess(p.Handle);
            if(entireProcessTree)
            {
                var searcher = new System.Management.ManagementObjectSearcher($"SELECT ProcessId FROM Win32_Process WHERE ParentProcessId={p.Id}");
                foreach (var obj in searcher.Get())
                {
                    try
                    {
                        using var childProcess = Process.GetProcessById(Convert.ToInt32(obj["ProcessId"]));
                        childProcess.Suspend(true);
                    }
                    catch { }
                }
            }
#else
            if(entireProcessTree)
            {
                kill(-p.Id, 19);
            }
            kill(p.Id, 19);
#endif
        }

        public void Resume(bool entireProcessTree = false)
        {
#if OS_WINDOWS
            NtResumeProcess(p.Handle);
            if(entireProcessTree)
            {
                var searcher = new System.Management.ManagementObjectSearcher($"SELECT ProcessId FROM Win32_Process WHERE ParentProcessId={p.Id}");
                foreach (var obj in searcher.Get())
                {
                    try
                    {
                        using var childProcess = Process.GetProcessById(Convert.ToInt32(obj["ProcessId"]));
                        childProcess.Resume(true);
                    }
                    catch { }
                }
            }
#else       
            if(entireProcessTree)
            {
                kill(-p.Id, 18);
            }
            kill(p.Id, 18);
#endif
        }
    }
}
