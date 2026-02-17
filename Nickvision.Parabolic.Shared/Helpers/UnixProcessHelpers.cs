using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Nickvision.Parabolic.Shared.Helpers;

public static partial class UnixProcessHelpers
{
    [LibraryImport("libc")]
    private static partial int kill(int pid, int sig);
    [LibraryImport("libc")]
    private static partial int setpgid(int pid, int pgid);

    public static void SetAsParentProcess(Process p) => setpgid(p.Id, p.Id);

    public static void Suspend(Process p)
    {
        kill(-p.Id, 19);
        kill(p.Id, 19);
    }

    public static void Resume(Process p)
    {
        kill(-p.Id, 18);
        kill(p.Id, 18);
    }
}
