using System.Collections.Concurrent;
using System.Diagnostics;
using Vanara.PInvoke;

namespace Nickvision.Parabolic.Shared.Helpers;

public static partial class WindowsProcessHelpers
{
    private static readonly ConcurrentDictionary<int, Kernel32.SafeHJOB> _jobObjects;

    static WindowsProcessHelpers()
    {
        _jobObjects = new ConcurrentDictionary<int, Kernel32.SafeHJOB>();
    }

    public static void SetAsParentProcess(Process p)
    {
        var pid = p.Id;
        var job = Kernel32.CreateJobObject(null, null);
        var info = new Kernel32.JOBOBJECT_EXTENDED_LIMIT_INFORMATION
        {
            BasicLimitInformation = new Kernel32.JOBOBJECT_BASIC_LIMIT_INFORMATION
            {
                LimitFlags = Kernel32.JOBOBJECT_LIMIT_FLAGS.JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | Kernel32.JOBOBJECT_LIMIT_FLAGS.JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION
            }
        };
        Kernel32.SetInformationJobObject(job, Kernel32.JOBOBJECTINFOCLASS.JobObjectExtendedLimitInformation, info);
        Kernel32.AssignProcessToJobObject(job, p.Handle);
        _jobObjects[pid] = job;
        p.Exited += (_, e) =>
        {
            job.Dispose();
            _jobObjects.TryRemove(pid, out var _);
        };
    }

    public static void Suspend(Process p)
    {
        foreach (var id in GetJobProcessIds(p))
        {
            try
            {
                using var proc = Process.GetProcessById((int)id.ToUInt32());
                foreach (ProcessThread thread in proc.Threads)
                {
                    using var handle = Kernel32.OpenThread(ACCESS_MASK.FromEnum(Kernel32.ThreadAccess.THREAD_SUSPEND_RESUME), false, (uint)thread.Id);
                    if (handle.IsInvalid)
                    {
                        continue;
                    }
                    Kernel32.SuspendThread(handle);
                }
            }
            catch { }
        }
    }

    public static void Resume(Process p)
    {
        foreach (var id in GetJobProcessIds(p))
        {
            try
            {
                using var proc = Process.GetProcessById((int)id.ToUInt32());
                foreach (ProcessThread thread in proc.Threads)
                {
                    using var handle = Kernel32.OpenThread(ACCESS_MASK.FromEnum(Kernel32.ThreadAccess.THREAD_SUSPEND_RESUME), false, (uint)thread.Id);
                    if (handle.IsInvalid)
                    {
                        continue;
                    }
                    Kernel32.ResumeThread(handle);
                }
            }
            catch { }
        }
    }

    private static nuint[] GetJobProcessIds(Process p)
    {
        if (!_jobObjects.TryGetValue(p.Id, out var job))
        {
            return [];
        }
        return Kernel32.QueryInformationJobObject<Kernel32.JOBOBJECT_BASIC_PROCESS_ID_LIST>(job, Kernel32.JOBOBJECTINFOCLASS.JobObjectBasicProcessIdList).ProcessIdList;
    }
}
